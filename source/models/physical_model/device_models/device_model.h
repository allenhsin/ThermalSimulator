
#ifndef __THERMAL_DEVICE_MODEL_H__
#define __THERMAL_DEVICE_MODEL_H__

#include <map>
#include <string>
#include <vector>

#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_floorplan_map.h"

namespace Thermal
{
    class DeviceModel
    {
    public:
        virtual ~DeviceModel();

        static DeviceModel* createDevice(int device_type, DeviceFloorplanMap* device_floorplan_map);

        void setDeviceFloorplanMap(DeviceFloorplanMap* device_floorplan_map)
        { _device_floorplan_map = device_floorplan_map; }

        // load parameters into _device_parameters for specific device
        virtual void loadDeviceParameters() = 0;

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
        void setTargetPortName(std::string port_name);
        void setTargetPortConnectedDevice(DeviceModel* device);
        
        void setDeviceInstanceAndFloorplanUnitName(std::string instance_name);
        std::string getInstanceName(){ return _instance_name; }
        std::string getFloorplanUnitName(){ return _floorplan_unit_name; }

    protected:
        // derived child classes must call this constructor
        DeviceModel(DeviceFloorplanMap* device_floorplan_map);

        std::string _instance_name;
        std::string _floorplan_unit_name;
        
        // parameter_name -> parameter_value
        std::map<std::string, double>       _device_parameters;
        // port_name -> connected_device_pointer
        std::map<std::string, DeviceModel*> _device_port_connections;

    private:
        DeviceFloorplanMap* _device_floorplan_map;

        std::string _target_parameter_name;
        std::string _target_port_name;

    }; // class DeviceModel

} // namespace Thermal

#endif // __THERMAL_DEVICE_MODEL_H__

