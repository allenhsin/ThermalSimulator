
#include <cassert>
#include <stddef.h>
#include <stdio.h>

#include "source/misc/misc.h"
#include "source/data/data.h"
#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_models/device_definition_parser.h"
#include "source/models/physical_model/physical_constants.h"
#include "libutil/LibUtil.h"

// device models
#include "source/models/physical_model/device_models/current_integrating_receiver.h"
#include "source/models/physical_model/device_models/resonant_ring_depletion_modulator.h"
#include "source/models/physical_model/device_models/resonant_ring.h"
#include "source/models/physical_model/device_models/laser_source_off_chip.h"
#include "source/models/physical_model/device_models/lossy_optical_net.h"
#include "source/models/physical_model/device_models/modulator_driver.h"
#include "source/models/physical_model/device_models/thermal_tuner.h"
#include "source/models/physical_model/device_models/photodetector.h"

using std::string;
using std::map;
using std::vector;
using LibUtil::String;

namespace Thermal
{
    DeviceModel::DeviceModel(DeviceFloorplanMap* device_floorplan_map, string device_definition_file)
        : _device_definition_file   (device_definition_file)
        , _device_floorplan_map     (device_floorplan_map)
        , _mapped_on_floorplan      (false)
        , _traversed_in_bfs         (false)
        , _target_parameter_name    ("")
        , _target_port_name         ("")
    {
        _instance_name = NO_INSTANCE_NAME;
        _floorplan_unit_name = NO_MAPPED_NAME;
        _device_power = 0;

        _device_ports.clear();
        _device_parameters.clear();
        _monitored_device_ports.clear();
        
        // initilize the device's name/ports/parameters
        DeviceDefinitionParser device_def_parser(this, _device_ports, _device_parameters, _device_type_name);
        device_def_parser.loadDeviceDefinitionFile(_device_definition_file);

        // check if the device if a root device, i.e. no inputs
        _is_root = true;
        for( map<string, Port*>::iterator it=_device_ports.begin(); it!=_device_ports.end(); ++it)
        {
            if( it->second->getPortType() == INPUT_PORT ) 
            { _is_root = false; break; }
        }
    }

    DeviceModel::DeviceModel( const DeviceModel& cloned_device )
        : _instance_name            (cloned_device._instance_name)
        , _floorplan_unit_name      (cloned_device._floorplan_unit_name)
        , _device_power             (cloned_device._device_power)
        , _device_parameters        (cloned_device._device_parameters)
        , _device_type_name         (cloned_device._device_type_name)
        , _device_definition_file   (cloned_device._device_definition_file)
        , _device_floorplan_map     (cloned_device._device_floorplan_map)
        , _is_root                  (cloned_device._is_root)
        , _mapped_on_floorplan      (cloned_device._mapped_on_floorplan)
        , _traversed_in_bfs         (cloned_device._traversed_in_bfs)
        , _target_parameter_name    (cloned_device._target_parameter_name)
        , _target_port_name         (cloned_device._target_port_name)

    {
        _device_ports.clear();
        for ( map<string, Port*>::const_iterator it=cloned_device._device_ports.begin(); it!=cloned_device._device_ports.end(); ++it )
            _device_ports[it->first] = it->second->clone(this);

        _monitored_device_ports.clear();
    }


    DeviceModel::~DeviceModel()
    {
        for(map<string, Port*>::iterator it=_device_ports.begin(); it!=_device_ports.end(); ++it)
        { delete (it->second); }

        for(map<string, FILE*>::iterator it=_monitored_device_ports.begin(); it!=_monitored_device_ports.end(); ++it)
        { fclose(it->second); }
    }

    DeviceModel*
    DeviceModel::createDevice(DeviceType device_type, ConfigParser* physical_config, DeviceFloorplanMap* device_floorplan_map)
    {
        DeviceModel* device_model = NULL;

        switch(device_type)
        {
        case RESONANT_RING:
            device_model = new ResonantRing( device_floorplan_map, physical_config->getString("device/resonant_ring/def_file") );
            break;

        case RESONANT_RING_DEPLETION_MODULATOR:
            device_model = new ResonantRingDepletionModulator( device_floorplan_map, physical_config->getString("device/resonant_ring_depletion_modulator/def_file") );
            break;

        case MODULATOR_DRIVER:
            device_model = new ModulatorDriver( device_floorplan_map, physical_config->getString("device/modulator_driver/def_file") );
            break;

        case LOSSY_OPTICAL_NET:
            device_model = new LossyOpticalNet( device_floorplan_map, physical_config->getString("device/lossy_optical_net/def_file") );
            break;

        case LASER_SOURCE_OFF_CHIP:
            device_model = new LaserSourceOffChip( device_floorplan_map, physical_config->getString("device/laser_source_off_chip/def_file") );
            break;

        case THERMAL_TUNER:
            device_model = new ThermalTuner( device_floorplan_map, physical_config->getString("device/thermal_tuner/def_file") );
            break;

        case PHOTODETECTOR:
            device_model = new Photodetector( device_floorplan_map, physical_config->getString("device/photodetector/def_file") );
            break;

        case CURRENT_INTEGRATING_RECEIVER:
            device_model = new CurrentIntegratingReceiver( device_floorplan_map, physical_config->getString("device/current_integrating_receiver/def_file") );
            break;

        default:
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized Device Type: " + (String) device_type + ".\n");
            return NULL;
        }

        return device_model;
    }

    // Device Name ------------------------------------------------------------
    void DeviceModel::setDeviceName(string instance_name)
    {
        string floorplan_unit_name;

        _instance_name          = instance_name;
        floorplan_unit_name     = _device_floorplan_map->getFloorplanUnitNameFromInstanceName(instance_name);

        if (floorplan_unit_name != NO_MAPPED_NAME)
        {
            _floorplan_unit_name = floorplan_unit_name;
            _mapped_on_floorplan = true;
        }
    }
    // ------------------------------------------------------------------------


    // Device Parameter -------------------------------------------------------
    bool DeviceModel::hasParameter(string parameter_name)
    { return _device_parameters.count(parameter_name); }

    double DeviceModel::getParameter(string parameter_name)
    {
        if(!hasParameter(parameter_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized parameter: " + (String) parameter_name + ".\n");
        return _device_parameters[parameter_name];
    }

    void DeviceModel::setTargetParameterName(string parameter_name)
    { 
        if(!hasParameter(parameter_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized parameter: " + (String) parameter_name + ".\n");
        _target_parameter_name = parameter_name; 
    }

    void DeviceModel::setTargetParameterValue(double parameter_value)
    {
        assert(_target_parameter_name != "");
        _device_parameters[_target_parameter_name] = parameter_value;
    }
    // ------------------------------------------------------------------------


    // Device Port ------------------------------------------------------------
    bool DeviceModel::hasPort(string port_name)
    { return _device_ports.count(port_name); }

    Port* DeviceModel::getPortForModification(string port_name)
    {
        if(!hasPort(port_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized port when getPort: " + (String) port_name + ".\n");
        
        return _device_ports.find(port_name)->second;
    }

    const Port* DeviceModel::getPort(string port_name)
    {
        if(!hasPort(port_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized port when getPort: " + (String) port_name + ".\n");
        
        return _device_ports.find(port_name)->second;
    }

    void DeviceModel::setTargetPortName(string port_name)
    { 
        if(!hasPort(port_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized port when setTargetPortName: " + (String) port_name + ".\n");
        _target_port_name = port_name; 
    }

    void DeviceModel::setTargetPortConnectedPort(const Port* port)
    {
        assert(_target_port_name != "");
        _device_ports[_target_port_name]->setConnectedPort(port);
    }
    // ------------------------------------------------------------------------

    
    // Device Neighbor --------------------------------------------------------
    void DeviceModel::getParentDevices(vector<DeviceModel*>& parent_devices)
    {
        PortType port_type = NULL_PORT;
        parent_devices.clear();

        for( map<string, Port*>::iterator it=_device_ports.begin(); it!=_device_ports.end(); ++it)
        {
            port_type = it->second->getPortType();
            if( (port_type == INPUT_PORT) && it->second->getConnectedPort() )
                parent_devices.push_back(it->second->getConnectedPort()->getDevice());
        }
    }

    void DeviceModel::getChildDevices(vector<DeviceModel*>& child_devices)
    {
        PortType port_type = NULL_PORT;
        child_devices.clear();

        for( map<string, Port*>::iterator it=_device_ports.begin(); it!=_device_ports.end(); ++it)
        {
            port_type = it->second->getPortType();
            if( (port_type == OUTPUT_PORT) && it->second->getConnectedPort() )
                child_devices.push_back(it->second->getConnectedPort()->getDevice());
        }
    }
    // ------------------------------------------------------------------------

    // Monitoring -------------------------------------------------------------
    void DeviceModel::addMonitoredPort(string port_name, string output_dir)
    {
        if(!hasPort(port_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized port when setMonitoredPort: " + (String) port_name + ".\n");
        if(_monitored_device_ports.count(port_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port \"" + (String) port_name + "\" is already being monitored.\n");
        
        string file_name = output_dir + "/" + getInstanceName() + "_" + port_name + ".out";
        _monitored_device_ports[port_name] = fopen(file_name.c_str(), "w");
    }

    void DeviceModel::printSeparation()
    {
        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
            fprintf(it->second, "\n\n");
    }
    // ------------------------------------------------------------------------

    // Debug ------------------------------------------------------------------
    void DeviceModel::printDefinition(FILE* device_list_file)
    {
        fprintf(device_list_file, "\n");
        fprintf(device_list_file, "Device Type: %s\n", _device_type_name.c_str());
        fprintf(device_list_file, "    Instance Name: %s\n", _instance_name.c_str());
        fprintf(device_list_file, "    Floorplan Unit Name: %s\n", _floorplan_unit_name.c_str());
        fprintf(device_list_file, "    [parameter]\n");
        for(map<string, double>::iterator it=_device_parameters.begin(); it!=_device_parameters.end(); ++it)
            fprintf(device_list_file, "        %s -> %e\n", (it->first).c_str(), it->second );
    }
    // ------------------------------------------------------------------------

} // namespace Thermal

