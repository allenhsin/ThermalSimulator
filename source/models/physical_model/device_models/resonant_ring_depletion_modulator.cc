
#include <cassert>
#include <stdio.h>
#include <map>

#include "source/data/data.h"
#include "source/models/physical_model/device_models/resonant_ring_depletion_modulator.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/physical_constants.h"
#include "libutil/LibUtil.h"

using std::map;

namespace Thermal
{
    ResonantRingDepletionModulator::ResonantRingDepletionModulator(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel   ( RESONANT_RING_DEPLETION_MODULATOR, device_floorplan_map, device_definition_file)
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
    {}

    ResonantRingDepletionModulator::~ResonantRingDepletionModulator()
    {}

    void ResonantRingDepletionModulator::initializeDevice()
    {
        if(!isMappedInFloorplan())
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: A ring must be mapped on the floorplan.\n");

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

        // initialize device energy consumption to zero
        map<string, double>& data_energy = Data::getSingleton()->getAccumulatedEnergyConsumption(); 
        data_energy[_floorplan_unit_name] = 0;
    }

    void ResonantRingDepletionModulator::updateDeviceProperties()
    {
        // shortcuts to data structure
        map<string, double>& data_temperature   = Data::getSingleton()->getTemperature(); 
        map<string, double>& data_energy        = Data::getSingleton()->getAccumulatedEnergyConsumption(); 

        // get temperature from data structure
        double current_temperature = data_temperature[_floorplan_unit_name];

        // update input ports properties
        getPortForModification("in")->updatePortPropertyFromConnectedPort("power");
        getPortForModification("in")->updatePortPropertyFromConnectedPort("wavelength");
        getPortForModification("add")->updatePortPropertyFromConnectedPort("power");
        getPortForModification("add")->updatePortPropertyFromConnectedPort("wavelength");
        getPortForModification("heater")->updatePortPropertyFromConnectedPort("power");
        getPortForModification("mod_driver")->updatePortPropertyFromConnectedPort("voltage");
        
        // check validity
        double wavelength = getPort("in")->getPortProperty("wavelength");
        if(wavelength != getPort("add")->getPortProperty("wavelength"))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: \"in\" port wavelength does not match \"add\" port wavelength.\n");

        // update device properties
        // silicon intrinsic carrier concentration
        double n_i      =   sqrt( SILICON_INT_B * (current_temperature*current_temperature*current_temperature) * 
                            exp(-SILICON_INT_BG_E/(BOLTZMANN_CONST*current_temperature)) );
        // built-in voltage
        double v_bi     =   _v_bi_partial * current_temperature / n_i;
        // width of depletion region
        double w_dep    =   sqrt(_w_dep_partial * v_bi);
        // open-cricuit depletion region capacitance
        double c_j0     =   _c_j0_partial/w_dep;
        // change of charge in the depletion region of the p-n junction
        double delta_Q  =   2 * v_bi * c_j0 * (sqrt(1 + (getPort("mod_driver")->getPortProperty("voltage")/v_bi)) - 1);
        // change of free-carrier density (electrons and holes are equal) 
        double delta_N  =   delta_Q / _delta_N_partial;

        // change of effective refractive index due to electrical modulation (change of voltage)
        // This is from Soref's paper. We take out the minus sign since this is a depletion mod.
        double delta_n_eff = (getParameter("fcpd_n_fe") * delta_N + getParameter("fcpd_n_fh") * pow(delta_N, 0.8)) * getParameter("confinement_factor");
        
        // effective index of refraction
        double n_eff = 3.38 + 13.182e-5 * current_temperature + delta_n_eff;

        // ring one-round phase shift
        double theta = _theta_partial * n_eff / wavelength;
        
        // port power ratio
        double _two_t1_t2_alpha_cos     = _two_t1_t2_alpha * cos(theta);
        double power_ratio_denominator  = _one_plus_sq_of_t1_t2_alpha - _two_t1_t2_alpha_cos;

        double thru_to_in_power_ratio   = (_t2_sq_alpha_sq_plus_t1_sq - _two_t1_t2_alpha_cos) / power_ratio_denominator;
        double drop_to_in_power_ratio   = _one_t1_t2_alpha / power_ratio_denominator;

        double drop_to_add_power_ratio  = (_t1_sq_alpha_sq_plus_t2_sq - _two_t1_t2_alpha_cos) / power_ratio_denominator;
        double thru_to_add_power_ratio  = drop_to_in_power_ratio;
        
        double in_power =   getPort("in")->getPortProperty("power");
        double add_power =  getPort("add")->getPortProperty("power");

        double thru_power =    (in_power * thru_to_in_power_ratio) + (add_power * thru_to_add_power_ratio);
        double drop_power =    (in_power * drop_to_in_power_ratio) + (add_power * drop_to_add_power_ratio);

        // update output ports
        getPortForModification("thru")->setPortProperty("power", thru_power);
        getPortForModification("thru")->setPortProperty("wavelength", wavelength);
        getPortForModification("drop")->setPortProperty("power", drop_power);
        getPortForModification("drop")->setPortProperty("wavelength", wavelength);
    }
    

}



