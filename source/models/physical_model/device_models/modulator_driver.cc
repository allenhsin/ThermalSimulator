
#include <cassert>
#include <stdio.h>

#include "source/data/data.h"
#include "source/models/physical_model/device_models/modulator_driver.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    ModulatorDriver::ModulatorDriver(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel( MODULATOR_DRIVER, device_floorplan_map, device_definition_file)
        , _bit_one_time_const       (0)
        , _bit_zero_time_const      (0)
        , _current_bit              (false)
        , _current_out_voltage      (0)
        , _current_delta_voltage    (0)
        , _current_target_voltage   (0)
        , _current_time_const       (0)
        , _transition_elapsed_time  (0)
    {}

    ModulatorDriver::~ModulatorDriver()
    {}

    void ModulatorDriver::deviceParameterCheck()
    {
        if( getParameter("bit_one_transition_time") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Transition time cannot be negative.\n");

        if( getParameter("bit_zero_transition_time") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Transition time cannot be negative.\n");
    }

    void ModulatorDriver::initializeDevice()
    {
        // parameter sanity check
        deviceParameterCheck();

        _bit_one_time_const = 2.197 * getParameter("bit_one_transition_time");
        _bit_zero_time_const = 2.197 * getParameter("bit_zero_transition_time");

        // preset the driver to bit 0
        _current_target_voltage     = getParameter("bit_zero_voltage");
        _current_time_const         = _bit_zero_time_const;
        _current_delta_voltage      = 0;
        _current_bit                = false;
        _transition_elapsed_time    = 0;
        _current_out_voltage        = getParameter("bit_zero_voltage");
        
        // set output
        getPortForModification("out")->setPortPropertySize("voltage", 1);
    }

    void ModulatorDriver::updateDeviceProperties(double time_elapsed_since_last_update)
    {
        // check the current bit from the data structure;
        bool new_bit = false;
        if( Data::getSingleton()->getData(MODULATOR_DRIVER_BITS_DATA, _instance_name) > 0 )
            new_bit = true;

        // check if bit changes
        if( new_bit != _current_bit)
        {   
            if(new_bit)
            {
                _current_target_voltage = getParameter("bit_one_voltage");
                _current_time_const     = _bit_one_time_const;
            }
            else
            {
                _current_target_voltage = getParameter("bit_zero_voltage");
                _current_time_const     = _bit_zero_time_const;
            }
            
            _current_delta_voltage = _current_out_voltage - _current_target_voltage;
            _current_bit = new_bit;
            _transition_elapsed_time = 0;
        }

        // update output voltage
        _transition_elapsed_time += time_elapsed_since_last_update;
        _current_out_voltage = _current_target_voltage + _current_delta_voltage * exp(-_transition_elapsed_time/_current_time_const);

        // update port property
        getPortForModification("out")->setPortPropertyValueByIndex("voltage", 0, _current_out_voltage);
        
        // TODO: update energy consumption if mapped to floorplan
        if(isMappedInFloorplan())
        {

        }
    }

} // namespace Thermal











