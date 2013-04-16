
#include <cassert>
#include <stdio.h>
#include <vector>
#include <iterator>

#include "source/data/data.h"
#include "source/models/physical_model/device_models/resonant_ring_depletion_modulator.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/physical_constants.h"
#include "libutil/LibUtil.h"

using std::set;
using std::vector;
using std::distance;

namespace Thermal
{
    ResonantRingDepletionModulator::ResonantRingDepletionModulator(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel   ( RESONANT_RING_DEPLETION_MODULATOR, device_floorplan_map, device_definition_file)
        , _number_wavelength(0)
        , _v_bi_partial     (0)
        , _w_dep_partial    (0)
        , _c_j0_partial     (0)
        , _delta_N_partial  (0)
        , _theta_partial    (0)
        , _alpha            (0)
        , _t2_sq_alpha_sq_plus_t1_sq    (0)
        , _t1_sq_alpha_sq_plus_t2_sq    (0)
        , _two_t1_t2_alpha              (0)
        , _one_plus_sq_of_t1_t2_alpha   (0)
        , _one_t1_t2_alpha              (0)
        , _last_temperature (0)
        , _last_voltage     (0)
        , _v_bi             (0)
        , _c_j0             (0)
        , _n_eff            (0)
    {}

    ResonantRingDepletionModulator::~ResonantRingDepletionModulator()
    {}
    
    void ResonantRingDepletionModulator::deviceParameterCheck()
    {
        if( getParameter("t1") > 1 || getParameter("t2") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Coupling coefficients cannot be larger than 1.\n");

        if( getParameter("t1") < 0 || getParameter("t2") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Coupling coefficients cannot be negative.\n");

        if( getParameter("waveguide_loss") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Waveguide loss cannot be negative.\n");

        if( getParameter("ring_radius") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Ring radius cannot be negative.\n");
        
        if( getParameter("ring_height") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Ring height cannot be negative.\n");
        
        if( getParameter("ring_width") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Ring width cannot be negative.\n");
        
        if( getParameter("confinement_factor") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Ring confinement factor cannot be larger than 1.\n");
        
        if( getParameter("Na") < 0 || getParameter("Nd") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Ring doping concentration cannot be negative.\n");
        
        if( getParameter("number_junctions") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Number of ring junctions cannot be negative.\n");
    }

    void ResonantRingDepletionModulator::initializeDevice()
    {
        if(!isMappedInFloorplan())
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: A ring must be mapped on the floorplan.\n");

        // parameter sanity check
        deviceParameterCheck();
        
        // construct the output wavelengths
        unsigned int i = 0;
        _output_wavelengths.clear();
        // in-port wavelengths
        for (i=0; i<getPort("in")->getPortPropertySize("wavelength"); ++i)
            _output_wavelengths.insert(getPort("in")->getPortPropertyValueByIndex("wavelength", i));
        // add-port wavelengths
        for (i=0; i<getPort("add")->getPortPropertySize("wavelength"); ++i)
            _output_wavelengths.insert(getPort("add")->getPortPropertyValueByIndex("wavelength", i));
        _number_wavelength = _output_wavelengths.size();
        
        // construct output port wavelengths
        getPortForModification("thru")->setPortPropertySize("wavelength", _number_wavelength);
        getPortForModification("thru")->setPortPropertySize("power", _number_wavelength);
        getPortForModification("drop")->setPortPropertySize("wavelength", _number_wavelength);
        getPortForModification("drop")->setPortPropertySize("power", _number_wavelength);
        i = 0;
        for(set<double>::iterator it = _output_wavelengths.begin(); it != _output_wavelengths.end(); ++it)
        {
            getPortForModification("thru")->setPortPropertyValueByIndex("wavelength", i, (*it));
            getPortForModification("drop")->setPortPropertyValueByIndex("wavelength", i, (*it));
            i += 1;
        }

        // precompute some partial properties

        // 1. built-in voltage V_bi: since temperature will only be 
        //    known at each execution stage, we precompute the rest
        //    part first when deriving V_bi
        _v_bi_partial = BOLTZMANN_CONST / ELECTRON_CHARGE * log( getParameter("Na") * getParameter("Nd") );

        // 2. width of depletion region W_dep: depends on V_bi
        _w_dep_partial =    2 * VACUUM_PERMITTIVITY * SILICON_REL_PERMIT / ELECTRON_CHARGE * 
                            ( (1/getParameter("Na")) + (1/getParameter("Nd")) );
    
        // 3. open-circuit depletion region cap C_j0: depends on W_dep
        _c_j0_partial =     getParameter("number_junctions") * VACUUM_PERMITTIVITY * SILICON_REL_PERMIT * 
                            getParameter("ring_height") * getParameter("ring_width");

        // 4. delta_N_partial: ring total volume * charge of electron
        _delta_N_partial =  2*PI * getParameter("ring_radius") * getParameter("ring_width") * getParameter("ring_height") * ELECTRON_CHARGE;

        // 5. theta_partial: ring phase shift 
        _theta_partial =    4*PI*PI*getParameter("ring_radius");

        // 6. ring loss
        _alpha =            sqrt( pow(10, (2*PI*getParameter("ring_radius")*(-getParameter("waveguide_loss"))/10)) );

        // 7. other stuff for power ratio
        _t2_sq_alpha_sq_plus_t1_sq =    (getParameter("t2")*getParameter("t2")*_alpha*_alpha) + (getParameter("t1") * getParameter("t1"));
        _t1_sq_alpha_sq_plus_t2_sq =    (getParameter("t1")*getParameter("t1")*_alpha*_alpha) + (getParameter("t2") * getParameter("t2"));
        _two_t1_t2_alpha =              2 * getParameter("t1") * getParameter("t2") * _alpha;
        _one_plus_sq_of_t1_t2_alpha =   1 + (getParameter("t2") * getParameter("t2") * getParameter("t1") * getParameter("t1") * _alpha * _alpha);
        _one_t1_t2_alpha =              (1 - (getParameter("t1")*getParameter("t1"))) * (1 - (getParameter("t2")*getParameter("t2"))) * _alpha;
    }

    void ResonantRingDepletionModulator::updateDeviceProperties(double time_elapsed_since_last_update)
    {
        // get temperature from data structure
        double current_temperature = Data::getSingleton()->getTemperatureData(_floorplan_unit_name);

        // update device properties
        // only update when temperature changes
        if(current_temperature != _last_temperature)
        {
            // silicon intrinsic carrier concentration
            double n_i      =   sqrt( SILICON_INT_B * (current_temperature*current_temperature*current_temperature) * 
                            exp(-SILICON_INT_BG_E/(BOLTZMANN_CONST*current_temperature)) );
            // built-in voltage
            _v_bi           =   _v_bi_partial * current_temperature / n_i;
            
            // width of the open-circuit depletion region
            double w_dep    =   sqrt(_w_dep_partial * _v_bi);
            // open-cricuit depletion region capacitance
            _c_j0     =   _c_j0_partial/w_dep;
        }
        
        // note that voltage here is the forward biasing voltage
        // you cannot sweep in more free carrier into the depletion 
        // region than v_bi, so the max. voltage would be equal to v_bi
        double current_voltage = getPort("mod_driver")->getPortPropertyValueByIndex("voltage", 0);
        if(current_voltage > _v_bi)
            current_voltage = _v_bi;

        if( (current_temperature != _last_temperature) || (current_voltage != _last_voltage) )
        {
            // check if you reach the maximum depletion width
            double max_w_dep = sqrt(_w_dep_partial * (_v_bi - current_voltage));
            if(max_w_dep*getParameter("number_junctions") > 2*PI*getParameter("ring_radius"))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Reverse-biased voltage too high.\n");

            // change of charge in the depletion region of the p-n junction
            double delta_Q  =   2 * _v_bi * _c_j0 * (sqrt(1 + (-current_voltage/_v_bi)) - 1);
            // change of free-carrier density (electrons and holes are equal) 
            double delta_N  =   delta_Q / _delta_N_partial;

            double sign_delta_N = (delta_N >= 0)? 1 : -1 ;
            double abs_delta_N  = fabs(delta_N);

            // change of effective refractive index due to electrical modulation (change of voltage)
            // This is from Soref's paper. We take out the minus sign since this is a depletion mod.
            double delta_n_eff = sign_delta_N * (getParameter("fcpd_n_fe") * abs_delta_N + getParameter("fcpd_n_fh") * pow(abs_delta_N, 0.8)) * getParameter("confinement_factor");
        
            // effective index of refraction
            _n_eff = 3.38 + 13.182e-5 * current_temperature + delta_n_eff;
        }

        vector<double> accumulated_thru_power;
        vector<double> accumulated_drop_power;
        accumulated_thru_power.clear();
        accumulated_drop_power.clear();
        accumulated_thru_power.resize(_number_wavelength);
        accumulated_drop_power.resize(_number_wavelength);
        
        unsigned int i = 0;
        double theta_partial = _theta_partial * _n_eff;
        double theta;
        double wavelength;
        unsigned int wavelength_index;
        double in_power, add_power;
        double thru_power, drop_power;
        double accumulated_dissipated_power = 0;
        double dissipated_power;
        double two_t1_t2_alpha_cos, power_ratio_denominator;
        double thru_to_in_power_ratio, drop_to_in_power_ratio;
        double drop_to_add_power_ratio, thru_to_add_power_ratio;

        // accumulate "in" port power to "drop" and "thru" ports
        for (i=0; i<getPort("in")->getPortPropertySize("wavelength"); ++i)
        {
            // current wavelength
            wavelength = getPort("in")->getPortPropertyValueByIndex("wavelength", i);
            // corresponding power
            in_power = getPort("in")->getPortPropertyValueByIndex("power", i);
            
            // the index of the current wavelength in the output wavelength list
            wavelength_index = distance(_output_wavelengths.begin(), _output_wavelengths.find(wavelength));

            // ring one-round phase shift
            theta = theta_partial / wavelength;

            // port power ratio
            two_t1_t2_alpha_cos      = _two_t1_t2_alpha * cos(theta);
            power_ratio_denominator  = _one_plus_sq_of_t1_t2_alpha - two_t1_t2_alpha_cos;
            thru_to_in_power_ratio   = (_t2_sq_alpha_sq_plus_t1_sq - two_t1_t2_alpha_cos) / power_ratio_denominator;
            drop_to_in_power_ratio   = _one_t1_t2_alpha / power_ratio_denominator;
        
            assert(thru_to_in_power_ratio < 1);
            assert(drop_to_in_power_ratio < 1);

            // compute thru and drop power
            thru_power = (in_power * thru_to_in_power_ratio);
            drop_power = (in_power * drop_to_in_power_ratio);
            accumulated_thru_power[wavelength_index] += thru_power;
            accumulated_drop_power[wavelength_index] += drop_power;
            
            // compute dissipated power
            dissipated_power = (in_power - thru_power - drop_power);
            assert(dissipated_power>=0);
            accumulated_dissipated_power += dissipated_power;
        }
        // accumulate add port power to drop and thru ports
        for (i=0; i<getPort("add")->getPortPropertySize("wavelength"); ++i)
        {
            // current wavelength
            wavelength = getPort("add")->getPortPropertyValueByIndex("wavelength", i);
            // corresponding power
            add_power = getPort("add")->getPortPropertyValueByIndex("power", i);
            
            // the index of the current wavelength in the output wavelength list
            wavelength_index = distance(_output_wavelengths.begin(), _output_wavelengths.find(wavelength));

            // ring one-round phase shift
            theta = theta_partial / wavelength;

            // port power ratio
            two_t1_t2_alpha_cos      = _two_t1_t2_alpha * cos(theta);
            power_ratio_denominator  = _one_plus_sq_of_t1_t2_alpha - two_t1_t2_alpha_cos;
            drop_to_add_power_ratio  = (_t1_sq_alpha_sq_plus_t2_sq - two_t1_t2_alpha_cos) / power_ratio_denominator;
            thru_to_add_power_ratio  = _one_t1_t2_alpha / power_ratio_denominator;
        
            assert(drop_to_add_power_ratio < 1);
            assert(thru_to_add_power_ratio < 1);
            
            // compute thru and drop power
            thru_power = (add_power * thru_to_add_power_ratio);
            drop_power = (add_power * drop_to_add_power_ratio);
            accumulated_thru_power[wavelength_index] += thru_power;
            accumulated_drop_power[wavelength_index] += drop_power;
            
            // compute dissipated power
            dissipated_power = (add_power - thru_power - drop_power);
            assert(dissipated_power>=0);
            accumulated_dissipated_power += dissipated_power;
        }
        
        // update the output ports
        for (i=0; i<_number_wavelength; ++i)
        {
            getPortForModification("thru")->setPortPropertyValueByIndex("power", i, accumulated_thru_power[i]);
            getPortForModification("drop")->setPortPropertyValueByIndex("power", i, accumulated_drop_power[i]);
        }

        // update energy data structure for the consumed energy
        double total_power_consumption = getPort("heater")->getPortPropertyValueByIndex("power", 0) + accumulated_dissipated_power;
        double previous_accumulated_energy = Data::getSingleton()->getEnergyData(_floorplan_unit_name);
        Data::getSingleton()->setEnergyData (   _floorplan_unit_name, 
                                                (previous_accumulated_energy+(time_elapsed_since_last_update*total_power_consumption))
                                            );

        _last_temperature = current_temperature;
        _last_voltage = current_voltage;
    } // updateDeviceProperties

} // namespace Thermal

