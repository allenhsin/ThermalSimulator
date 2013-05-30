
#include <cassert>
#include <stdio.h>
#include <vector>
#include <iterator>

#include "source/data/data.h"
#include "source/models/physical_model/device_models/resonant_ring.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/physical_constants.h"
#include "libutil/LibUtil.h"

using std::set;
using std::vector;
using std::distance;

namespace Thermal
{
    ResonantRing::ResonantRing(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel   ( RESONANT_RING, device_floorplan_map, device_definition_file)
        , _number_wavelength(0)
        , _theta_partial    (0)
        , _alpha            (0)
        , _t2_sq_alpha_sq_plus_t1_sq    (0)
        , _t1_sq_alpha_sq_plus_t2_sq    (0)
        , _two_t1_t2_alpha              (0)
        , _one_plus_sq_of_t1_t2_alpha   (0)
        , _one_t1_t2_alpha              (0)
        , _last_temperature (0)
        , _n_eff            (0)
    {}

    ResonantRing::ResonantRing( const ResonantRing& cloned_device)
        : DeviceModel(cloned_device)
        , _number_wavelength            (cloned_device._number_wavelength)
        , _theta_partial                (cloned_device._theta_partial)
        , _alpha                        (cloned_device._alpha)
        , _t2_sq_alpha_sq_plus_t1_sq    (cloned_device._t2_sq_alpha_sq_plus_t1_sq)
        , _t1_sq_alpha_sq_plus_t2_sq    (cloned_device._t1_sq_alpha_sq_plus_t2_sq)
        , _two_t1_t2_alpha              (cloned_device._two_t1_t2_alpha)
        , _one_plus_sq_of_t1_t2_alpha   (cloned_device._one_plus_sq_of_t1_t2_alpha)
        , _one_t1_t2_alpha              (cloned_device._one_t1_t2_alpha)
        , _last_temperature             (cloned_device._last_temperature)
        , _n_eff                        (cloned_device._n_eff)
    {}

    ResonantRing::~ResonantRing()
    {}

    ResonantRing* ResonantRing::clone() const
    { return new ResonantRing(*this); }
    
    void ResonantRing::deviceParameterCheck()
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
    }

    void ResonantRing::initializeDevice()
    {
        if(!isMappedOnFloorplan())
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
            getPortForModification("thru")->setPortPropertyValueByIndex("power", i, 0);
            getPortForModification("drop")->setPortPropertyValueByIndex("power", i, 0);
            i += 1;
        }

        // precompute some partial properties

        // 1. theta_partial: ring phase shift 
        _theta_partial =    4*PI*PI*getParameter("ring_radius");

        // 2. ring loss
        _alpha =            sqrt( pow(10, (2*PI*getParameter("ring_radius")*(-getParameter("waveguide_loss"))/10)) );

        // 3. other stuff for power ratio
        _t2_sq_alpha_sq_plus_t1_sq =    (getParameter("t2")*getParameter("t2")*_alpha*_alpha) + (getParameter("t1") * getParameter("t1"));
        _t1_sq_alpha_sq_plus_t2_sq =    (getParameter("t1")*getParameter("t1")*_alpha*_alpha) + (getParameter("t2") * getParameter("t2"));
        _two_t1_t2_alpha =              2 * getParameter("t1") * getParameter("t2") * _alpha;
        _one_plus_sq_of_t1_t2_alpha =   1 + (getParameter("t2") * getParameter("t2") * getParameter("t1") * getParameter("t1") * _alpha * _alpha);
        _one_t1_t2_alpha =              (1 - (getParameter("t1")*getParameter("t1"))) * (1 - (getParameter("t2")*getParameter("t2"))) * _alpha;

    } // initializeDevice

    void ResonantRing::updateDeviceProperties(Time time_elapsed_since_last_update)
    {
        (void)time_elapsed_since_last_update;

        // get temperature from data structure
        double current_temperature = Data::getSingleton()->getTemperatureData(_floorplan_unit_name);

        // update device properties
        // only update when temperature changes
        if(current_temperature != _last_temperature)
            _n_eff = 3.38 + 13.182e-5 * current_temperature;

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
        
            assert(thru_to_in_power_ratio <= 1);
            assert(drop_to_in_power_ratio <= 1);

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
        
            assert(drop_to_add_power_ratio <= 1);
            assert(thru_to_add_power_ratio <= 1);
            
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
        _device_power = getPort("heater")->getPortPropertyValueByIndex("power", 0) + accumulated_dissipated_power;
        _last_temperature = current_temperature;
    } // updateDeviceProperties

    void ResonantRing::initializeMonitoring()
    {
        unsigned int number_wavelength = 0;
        unsigned int i = 0;

        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if( (it->first=="in") || (it->first=="thru") || (it->first=="add") || (it->first=="drop") )
            {
                number_wavelength = getPort(it->first)->getPortPropertySize("wavelength");
            
                for(i=0; i<number_wavelength; ++i)
                    fprintf(it->second, "%.12f ", getPort(it->first)->getPortPropertyValueByIndex("wavelength", i) );
                fprintf(it->second, "\n\n\n");
            }
        }
    }

    void ResonantRing::printMonitoredResult()
    {
        unsigned int number_wavelength = 0;
        unsigned int i = 0;

        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if ( it->first == "heater" )
                fprintf(it->second, "%.9f\n", getPort("heater")->getPortPropertyValueByIndex("power", 0) );
            else // other optical ports
            {
                number_wavelength = getPort(it->first)->getPortPropertySize("power");
                
                for(i=0; i<number_wavelength; ++i)
                    fprintf(it->second, "%.9f ", getPort(it->first)->getPortPropertyValueByIndex("power", i) );
                fprintf(it->second, "\n");
            }
        }
    }

    void ResonantRing::printDefinition(FILE* device_list_file)
    {
        DeviceModel::printDefinition(device_list_file);

        fprintf(device_list_file, "    [port]\n");
        fprintf(device_list_file, "        Name: in, Type: %s, Property: power(%d), wavlength(%d)\n", 
                ((int)getPort("in")->getPortType()==0)?"INPUT":"OUTPUT", getPort("in")->getPortPropertySize("power"), getPort("in")->getPortPropertySize("wavelength"));
        fprintf(device_list_file, "        Name: add, Type: %s, Property: power(%d), wavlength(%d)\n", 
                ((int)getPort("add")->getPortType()==0)?"INPUT":"OUTPUT", getPort("add")->getPortPropertySize("power"), getPort("add")->getPortPropertySize("wavelength"));
        fprintf(device_list_file, "        Name: heater, Type: %s, Property: power(%d)\n", 
                ((int)getPort("heater")->getPortType()==0)?"INPUT":"OUTPUT", getPort("heater")->getPortPropertySize("power"));
        fprintf(device_list_file, "        Name: thru, Type: %s, Property: power(%d), wavlength(%d)\n", 
                ((int)getPort("thru")->getPortType()==0)?"INPUT":"OUTPUT", getPort("thru")->getPortPropertySize("power"), getPort("thru")->getPortPropertySize("wavelength"));
        fprintf(device_list_file, "        Name: drop, Type: %s, Property: power(%d), wavlength(%d)\n", 
                ((int)getPort("drop")->getPortType()==0)?"INPUT":"OUTPUT", getPort("drop")->getPortPropertySize("power"), getPort("drop")->getPortPropertySize("wavelength"));
        fprintf(device_list_file, "\n");

    }

} // namespace Thermal

