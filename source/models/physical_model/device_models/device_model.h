
#ifndef __THERMAL_DEVICE_MODEL_H__
#define __THERMAL_DEVICE_MODEL_H__

#include <map>
#include <string>
#include <vector>

#include "source/models/physical_model/device_floorplan_map.h"
#include "config.hpp"

namespace Thermal
{
    class DeviceModel
    {
    public:
        virtual ~DeviceModel();

        static DeviceModel* createDevice(int device_type, config::Config* physical_config, DeviceFloorplanMap* device_floorplan_map);

    // ------------------------------------------------------------------------
    // derived classes must implement these functions
    // ------------------------------------------------------------------------

        // update device properties
        virtual void updateDeviceProperties() = 0;

        // get device power consumption based on current properties and parameters
        virtual void getDevicePower() = 0;

    // ------------------------------------------------------------------------

        void setDeviceDefinitionFile(std::string def_file);
        // load port list/default parameters/properties for the specific device
        void loadDeviceDefinition();

        bool hasParameter(std::string parameter_name);
        // have to separate the setting of name and value because
        // when parsing the netlist it reads the name first and 
        // then the value
        void setTargetParameterName(std::string parameter_name);
        // when setting the value it will also verify the 
        // existance of the parameter for the device
        // i.e. can only set for the loaded parameters
        void setTargetParameterValue(double parameter_value);
        
        bool hasPort(std::string port_name);
        // similar to set parameters
        void setTargetPortName(std::string port_name);
        void setTargetPortConnectedDevice(DeviceModel* device);
        
        void setDeviceInstanceAndFloorplanUnitName(std::string instance_name);
        std::string getInstanceName(){ return _instance_name; }
        std::string getFloorplanUnitName(){ return _floorplan_unit_name; }

    protected:
        // derived child classes must call this constructor
        DeviceModel(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);

        std::string _instance_name;
        std::string _floorplan_unit_name;
        
        // port_name -> connected_device_pointer
        std::map<std::string, DeviceModel*> _device_port_connections;
        // parameter_name -> parameter_value
        std::map<std::string, double>       _device_parameters;
        // property_name -> property_value
        std::map<std::string, double>       _device_properties;

    private:
        std::string         _device_definition_file;
        DeviceFloorplanMap* _device_floorplan_map;

        std::string _target_parameter_name;
        std::string _target_port_name;

    }; // class DeviceModel

} // namespace Thermal

#endif // __THERMAL_DEVICE_MODEL_H__

