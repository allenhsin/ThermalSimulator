
#include <cassert>
#include <stdio.h>
#include <string>
#include <math.h>

#include "source/models/physical_model/device_models/thermal_tuner.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "libutil/LibUtil.h"

using std::string;

namespace Thermal
{
    ThermalTuner::ThermalTuner(DeviceFloorplanMap* device_floorplan_map, string device_definition_file)
        : DeviceModel( THERMAL_TUNER, device_floorplan_map, device_definition_file)
        , _heater_power                         (0)
        , _clock_period                         (0)
        , _current_clock_period_elapsed_time    (0)
        , _pdm_threshold                        (0)
        , _pdm_input                            (0)
        , _pdm_error                            (0)
        , _pdm_output                           (false)
    {}

    ThermalTuner::ThermalTuner( const ThermalTuner& cloned_device)
        : DeviceModel(cloned_device)
        , _heater_power                         (cloned_device._heater_power)
        , _clock_period                         (cloned_device._clock_period)
        , _current_clock_period_elapsed_time    (cloned_device._current_clock_period_elapsed_time)
        , _pdm_threshold                        (cloned_device._pdm_threshold)
        , _pdm_input                            (cloned_device._pdm_input)
        , _pdm_error                            (cloned_device._pdm_error)
        , _pdm_output                           (cloned_device._pdm_output)
    {}

    ThermalTuner::~ThermalTuner()
    {}

    ThermalTuner* ThermalTuner::clone() const
    { return new ThermalTuner(*this); }
    
    void ThermalTuner::deviceParameterCheck()
    {
        if( getParameter("heater_power") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Heater power cannot be negative.\n");

        if( getParameter("bit_width") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: PDM bit width cannot be negative.\n");

        if( getParameter("clock_period") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Clock period cannot be negative.\n");

        if( getParameter("pdm_pattern") < 0 || getParameter("pdm_pattern") >= pow(2.00, getParameter("bit_width")) )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Bit period cannot be negative.\n");

        if( ((int)getParameter("heater_init") != 0) && ((int)getParameter("heater_init") != 1) )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Heater initial state can only be 1(ON) or 0(OFF).\n");

    }

    void ThermalTuner::initializeDevice()
    {
        // parameter sanity check
        deviceParameterCheck();
        
        // load the parameters
        _heater_power   = getParameter("heater_power");
        _clock_period   = getParameter("clock_period");
        _current_clock_period_elapsed_time = 0;
        _pdm_threshold  = ((int)pow(2.00, getParameter("bit_width"))) - 1;
        _pdm_input      = getParameter("pdm_pattern");
        _pdm_error      = 0;
        _pdm_output     = ( ((int)getParameter("heater_init")) == 1 );

        // set output
        getPortForModification("heater")->setPortPropertySize("power", 1);
        getPortForModification("heater")->setPortPropertyValueByIndex("power", 0, _pdm_output? _heater_power : 0 );
    }

    void ThermalTuner::updateDeviceProperties(Time time_elapsed_since_last_update)
    {
        _current_clock_period_elapsed_time += time_elapsed_since_last_update;
        if(_current_clock_period_elapsed_time >= _clock_period)
        {
            _pdm_error += _pdm_input;
            if(_pdm_error >= _pdm_threshold)
            {
                _pdm_error -= _pdm_threshold;
                _pdm_output = true;
            }
            else
                _pdm_output = false;
            
            _current_clock_period_elapsed_time -= _clock_period;
        }

        getPortForModification("heater")->setPortPropertyValueByIndex("power", 0, _pdm_output? _heater_power : 0 );
    }
    
    void ThermalTuner::initializeMonitoring()
    {}

    void ThermalTuner::printMonitoredResult()
    {
        double value = 0;
        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if(it->first == "heater")
                value = getPort("heater")->getPortPropertyValueByIndex("power", 0);
            fprintf(it->second, "%.2f\n", value);
        }
    }

    void ThermalTuner::printDefinition(FILE* device_list_file)
    {
        DeviceModel::printDefinition(device_list_file);

        fprintf(device_list_file, "    [port]\n");
        fprintf(device_list_file, "        Name: heater, Type: %s, Property: power(%d)\n", 
                ((int)getPort("heater")->getPortType()==0)?"INPUT":"OUTPUT", getPort("heater")->getPortPropertySize("power"));
        fprintf(device_list_file, "\n");

    }

} // namespace Thermal

