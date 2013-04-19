
#include <cassert>
#include <stdio.h>

#include "source/data/data.h"
#include "source/models/physical_model/device_models/modulator_driver.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    ModulatorDriver::ModulatorDriver(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel( MODULATOR_DRIVER, device_floorplan_map, device_definition_file)
        , _bit_period               (0)
        , _bit_one_time_const       (0)
        , _bit_zero_time_const      (0)
        , _current_bit              (false)
        , _current_out_voltage      (0)
        , _current_delta_voltage    (0)
        , _current_target_voltage   (0)
        , _current_time_const       (0)
        , _current_bit_elapsed_time (0)
    {}

    ModulatorDriver::~ModulatorDriver()
    {}

    void ModulatorDriver::deviceParameterCheck()
    {
        if( getParameter("bit_period") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Bit period cannot be negative.\n");

        if( getParameter("bit_one_transition_time") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Transition time cannot be negative.\n");

        if( getParameter("bit_zero_transition_time") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Transition time cannot be negative.\n");
    }

    void ModulatorDriver::initializeDevice()
    {
        // parameter sanity check
        deviceParameterCheck();
        assert(_instance_name != NO_INSTANCE_NAME);
        
        // bit period
        _bit_period = getParameter("bit_period");
        
        // 2.197 converts the 10%-90% transition time to time constant 
        _bit_one_time_const =   2.197 * getParameter("bit_one_transition_time");
        _bit_zero_time_const =  2.197 * getParameter("bit_zero_transition_time");

        // preset the driver to bit 0
        _current_target_voltage     = getParameter("bit_zero_voltage");
        _current_time_const         = _bit_zero_time_const;
        _current_delta_voltage      = 0;
        _current_bit                = false;
        _current_bit_elapsed_time   = 0;
        _current_out_voltage        = getParameter("bit_zero_voltage");
        
        // set output
        getPortForModification("out")->setPortPropertySize("voltage", 1);
        getPortForModification("out")->setPortPropertyValueByIndex("voltage", 0, _current_out_voltage);

    } // initializeDevice

    void ModulatorDriver::updateDeviceProperties(Time time_elapsed_since_last_update)
    {
        // update current bit elapsed time
        _current_bit_elapsed_time += time_elapsed_since_last_update;

        // if reaches another bit period, get the next bit from bit sequence
        if(_current_bit_elapsed_time >= _bit_period)
        {
            // the output voltage supposed to be reached at the end of last period
            double output_voltage_at_end_of_last_period = _current_target_voltage + _current_delta_voltage * exp(-_bit_period/_current_time_const);

            // get the next bit from the data structure
            _current_bit = Data::getSingleton()->getBitSequenceData(_instance_name)->getNextBit();

            if(_current_bit)
            {
                _current_target_voltage = getParameter("bit_one_voltage");
                _current_time_const     = _bit_one_time_const;
            }
            else
            {
                _current_target_voltage = getParameter("bit_zero_voltage");
                _current_time_const     = _bit_zero_time_const;
            }
            
            // new delta voltage for this period
            _current_delta_voltage = output_voltage_at_end_of_last_period - _current_target_voltage;
            
            _current_bit_elapsed_time -= _bit_period;
        }
        
        // update output voltage
        _current_out_voltage = _current_target_voltage + _current_delta_voltage * exp(-_current_bit_elapsed_time/_current_time_const);

        // update port property
        getPortForModification("out")->setPortPropertyValueByIndex("voltage", 0, _current_out_voltage);
        
        // TODO: update energy consumption if mapped to floorplan
        if(isMappedInFloorplan())
        {

        }
    }

    void ModulatorDriver::printDefinition(FILE* device_list_file)
    {
        DeviceModel::printDefinition(device_list_file);

        fprintf(device_list_file, "    [port]\n");
        fprintf(device_list_file, "        Name: out, Type: %s, Property: voltage(%d)", 
                ((int)getPort("out")->getPortType()==0)?"INPUT":"OUTPUT", getPort("out")->getPortPropertySize("voltage"));
        fprintf(device_list_file, "\n");

    }

} // namespace Thermal


