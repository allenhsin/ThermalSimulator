
#include <cassert>
#include <stdio.h>
#include <string>
#include <map>

#include "source/data/data.h"
#include "source/models/physical_model/device_models/optical_data_generator.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "libutil/LibUtil.h"

using std::map;
using std::string;

namespace Thermal
{
    OpticalDataGenerator::OpticalDataGenerator(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel(device_floorplan_map, device_definition_file)
        , _bit_period                       (0)
        , _bit_zero_optical_power_in_watt   (0)
        , _bit_one_optical_power_in_watt    (0)
        , _bit_one_time_const               (0)
        , _bit_zero_time_const              (0)
        , _current_bit                      (false)
        , _current_out_optical_power        (0)
        , _current_delta_optical_power      (0)
        , _current_target_optical_power     (0)
        , _current_time_const               (0)
        , _current_bit_elapsed_time         (0)
    {}

    OpticalDataGenerator::OpticalDataGenerator( const OpticalDataGenerator& cloned_device)
        : DeviceModel(cloned_device)
        , _bit_period                       (cloned_device._bit_period)
        , _bit_zero_optical_power_in_watt   (cloned_device._bit_zero_optical_power_in_watt)
        , _bit_one_optical_power_in_watt    (cloned_device._bit_one_optical_power_in_watt)
        , _bit_one_time_const               (cloned_device._bit_one_time_const)
        , _bit_zero_time_const              (cloned_device._bit_zero_time_const)
        , _current_bit                      (cloned_device._current_bit)
        , _current_out_optical_power        (cloned_device._current_out_optical_power)
        , _current_delta_optical_power      (cloned_device._current_delta_optical_power)
        , _current_target_optical_power     (cloned_device._current_target_optical_power)
        , _current_time_const               (cloned_device._current_time_const)
        , _current_bit_elapsed_time         (cloned_device._current_bit_elapsed_time)
    {}

    OpticalDataGenerator::~OpticalDataGenerator()
    {}

    OpticalDataGenerator* OpticalDataGenerator::clone() const
    { return new OpticalDataGenerator(*this); }

    void OpticalDataGenerator::deviceParameterCheck()
    {   
        if( getParameter("wavelength") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wavelength cannot be negative.\n");

        if( getParameter("bit_period") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Bit period cannot be negative.\n");

        if( getParameter("bit_one_transition_time") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Transition time cannot be negative.\n");

        if( getParameter("bit_zero_transition_time") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Transition time cannot be negative.\n");
    }

    void OpticalDataGenerator::initializeDevice()
    {
        // parameter sanity check
        deviceParameterCheck();
        assert(_instance_name != NO_INSTANCE_NAME);
        
        // bit period
        _bit_period = getParameter("bit_period");
        // wavelength
        double wavelength = getParameter("wavelength");
        // optical power
        _bit_zero_optical_power_in_watt     = pow( 10.00, (getParameter("bit_zero_optical_power")/10 - 3) );;
        _bit_one_optical_power_in_watt      = pow( 10.00, (getParameter("bit_one_optical_power")/10 - 3) );;

        // 2.197 converts the 10%-90% transition time to time constant 
        _bit_one_time_const     = getParameter("bit_one_transition_time")/2.197;
        _bit_zero_time_const    = getParameter("bit_zero_transition_time")/2.197;

        // preset the driver to bit 0
        _current_target_optical_power       = _bit_zero_optical_power_in_watt;
        _current_time_const                 = _bit_zero_time_const;
        _current_delta_optical_power        = 0;
        _current_bit                        = false;
        _current_bit_elapsed_time           = 0;
        _current_out_optical_power          = _bit_zero_optical_power_in_watt;
        
        // set output
        getPortForModification("out")->setPortPropertySize("wavelength", 1);
        getPortForModification("out")->setPortPropertySize("power", 1);
        getPortForModification("out")->setPortPropertyValueByIndex("wavelength", 0, wavelength);
        getPortForModification("out")->setPortPropertyValueByIndex("power", 0, _current_out_optical_power);

        getPortForModification("ref")->setPortPropertySize("bit", 1);
        getPortForModification("ref")->setPortPropertyValueByIndex("bit", 0, (_current_bit? 1:0) );

    } // initializeDevice

    void OpticalDataGenerator::updateDeviceProperties(Time time_elapsed_since_last_update)
    {
        double output_optical_power_at_end_of_last_period = 0;

        // update current bit elapsed time
        _current_bit_elapsed_time += time_elapsed_since_last_update;

        // if reaching another bit period, get the next bit from bit sequence
        if(_current_bit_elapsed_time >= _bit_period)
        {
            // the output optical power supposed to be reached at the end of last period
            output_optical_power_at_end_of_last_period = _current_target_optical_power + _current_delta_optical_power * exp(-_bit_period/_current_time_const);

            // get the next bit from the data structure
            _current_bit = Data::getSingleton()->getBitSequenceData(_instance_name)->getNextBit();
            
            if(_current_bit)
            {
                _current_target_optical_power = _bit_one_optical_power_in_watt;
                _current_time_const     = _bit_one_time_const;
            }
            else
            {
                _current_target_optical_power = _bit_zero_optical_power_in_watt;
                _current_time_const     = _bit_zero_time_const;
            }
            
            // new delta optical_power for this period
            _current_delta_optical_power = output_optical_power_at_end_of_last_period - _current_target_optical_power;
            
            _current_bit_elapsed_time -= _bit_period;
        }
        
        // update output optical_power
        _current_out_optical_power = _current_target_optical_power + _current_delta_optical_power * exp(-_current_bit_elapsed_time/_current_time_const);

        // update port property
        getPortForModification("out")->setPortPropertyValueByIndex("power", 0, _current_out_optical_power);
        getPortForModification("ref")->setPortPropertyValueByIndex("bit", 0, (_current_bit? 1:0) );
    }
    
    void OpticalDataGenerator::initializeMonitoring()
    {
        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if( it->first=="out" )
            {
                fprintf(it->second, "%.12f ", getPort(it->first)->getPortPropertyValueByIndex("wavelength", 0) );
                fprintf(it->second, "\n\n\n");
            }
        }
    }

    void OpticalDataGenerator::printMonitoredResult()
    {
        double value = 0;
        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if(it->first == "out")
                value = getPort("out")->getPortPropertyValueByIndex("power", 0);
            else if(it->first == "ref")
                value = getPort("ref")->getPortPropertyValueByIndex("bit", 0);
            
            fprintf(it->second, "%.2f\n", value);
        }
    }

    void OpticalDataGenerator::printDefinition(FILE* device_list_file)
    {
        DeviceModel::printDefinition(device_list_file);

        fprintf(device_list_file, "    [port]\n");
        fprintf(device_list_file, "        Name: out, Type: %s, Property: power(%d), wavlength(%d)\n", 
                ((int)getPort("out")->getPortType()==0)?"INPUT":"OUTPUT", getPort("out")->getPortPropertySize("power"), getPort("out")->getPortPropertySize("wavelength"));
        fprintf(device_list_file, "        Name: ref, Type: %s, Property: bit(%d)\n", 
                ((int)getPort("ref")->getPortType()==0)?"INPUT":"OUTPUT", getPort("ref")->getPortPropertySize("bit"));
        fprintf(device_list_file, "\n");

    }

} // namespace Thermal


