
#include <cassert>
#include <stddef.h>

#include "source/misc/misc.h"
#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_models/device_definition_parser.h"
#include "source/models/physical_model/physical_constants.h"
#include "libutil/LibUtil.h"

#include "source/models/physical_model/device_models/resonant_ring_modulator.h"
#include "source/models/physical_model/device_models/laser_source_off_chip.h"
#include "source/models/physical_model/device_models/lossy_optical_net.h"

using std::string;
using std::map;
using std::vector;
using LibUtil::String;

namespace Thermal
{
    DeviceModel::DeviceModel(DeviceType device_type, DeviceFloorplanMap* device_floorplan_map, string device_definition_file)
        : _device_type              (device_type)
        , _instance_name            ("")
        , _floorplan_unit_name      ("")
        
        , _device_definition_file   (device_definition_file)
        , _device_floorplan_map     (device_floorplan_map)
        , _traversed_in_bfs         (false)
        , _target_parameter_name    ("")
        , _target_port_name         ("")
    {
        _device_ports.clear();
        _device_parameters.clear();
        
        // initilize the device's ports/parameters/properties
        DeviceDefinitionParser device_def_parser(this, _device_ports, _device_parameters);
        device_def_parser.loadDeviceDefinitionFile(_device_definition_file);
    }

    DeviceModel::~DeviceModel()
    {
        _device_floorplan_map   = NULL;
        
        for(map<std::string, Port*>::iterator it=_device_ports.begin(); it!=_device_ports.end(); ++it)
        { delete (it->second); }
    }

    DeviceModel*
    DeviceModel::createDevice(DeviceType device_type, config::Config* physical_config, DeviceFloorplanMap* device_floorplan_map)
    {
        DeviceModel* device_model = NULL;

        switch(device_type)
        {
        case RESONANT_RING:
            //device_model = new ResonantRing( device_floorplan_map, physical_config->getString("device/resonant_ring/def_file") );
            break;

        case RESONANT_RING_MODULATOR:
            device_model = new ResonantRingModulator( device_floorplan_map, physical_config->getString("device/resonant_ring_modulator/def_file") );
            break;

        case LOSSY_OPTICAL_NET:
            device_model = new LossyOpticalNet( device_floorplan_map, physical_config->getString("device/lossy_optical_net/def_file") );
            break;

        case LASER_SOURCE_OFF_CHIP:
            device_model = new LaserSourceOffChip( device_floorplan_map, physical_config->getString("device/laser_source_off_chip/def_file") );
            break;

        case LASER_SOURCE_ON_CHIP:
            //device_model = new LaserSourceOnChip( device_floorplan_map, physical_config->getString("device/laser_source_on_chip/def_file") );
            break;

        case MODULATOR_DRIVER:
            //device_model = new ModulatorDriver( device_floorplan_map, physical_config->getString("device/modulator_driver/def_file") );
            break;

        case PHOTODETECTOR:
            //device_model = new Photodetector( device_floorplan_map, physical_config->getString("device/photodetector/def_file") );
            break;

        case RECEIVER:
            //device_model = new Receiver( device_floorplan_map, physical_config->getString("device/receiver/def_file") );
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
        _instance_name          = instance_name;
        _floorplan_unit_name    = _device_floorplan_map->getFloorplanUnitNameFromInstanceName(instance_name);
    }
    // ------------------------------------------------------------------------


    // Device Parameter -------------------------------------------------------
    bool DeviceModel::hasParameter(string parameter_name)
    { return _device_parameters.count(parameter_name); }

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

        for( map<std::string, Port*>::iterator it=_device_ports.begin(); it!=_device_ports.end(); ++it)
        {
            port_type = it->second->getPortType();
            if( (port_type == INPUT_PORT || port_type == INOUT_PORT) && it->second->getConnectedPort() )
                parent_devices.push_back(it->second->getConnectedPort()->getDevice());
        }
    }

    void DeviceModel::getChildDevices(vector<DeviceModel*>& child_devices)
    {
        PortType port_type = NULL_PORT;
        child_devices.clear();

        for( map<std::string, Port*>::iterator it=_device_ports.begin(); it!=_device_ports.end(); ++it)
        {
            port_type = it->second->getPortType();
            if( (port_type == OUTPUT_PORT || port_type == INOUT_PORT) && it->second->getConnectedPort() )
                child_devices.push_back(it->second->getConnectedPort()->getDevice());
        }
    }
    // ------------------------------------------------------------------------


    // Debug ------------------------------------------------------------------
    void DeviceModel::printDefinition()
    {
        printf("\n\n");
        printf("Device Type: %d\n", _device_type);
        printf("Device Instance Name: %s\n", _instance_name.c_str());
        printf("Device Floorplan Unit Name: %s\n", _floorplan_unit_name.c_str());
        printf("\n");
        printf("[port]\n");
        for(map<std::string, Port*>::iterator it=_device_ports.begin(); it!=_device_ports.end(); ++it)
        {
            printf("Name: %s, Type: %d, ", (it->first).c_str(), (int) it->second->getPortType());
            printf("Properties: power: %s, voltage: %s\n",  it->second->hasPortProperty("power")?"Yes":"No", 
                                                            it->second->hasPortProperty("voltage")?"Yes":"No" );
        }
        printf("\n");
        printf("[parameter]\n");
        for(map<std::string, double>::iterator it=_device_parameters.begin(); it!=_device_parameters.end(); ++it)
            printf("%s -> %e\n", (it->first).c_str(), it->second );
    }
    // ------------------------------------------------------------------------

} // namespace Thermal

