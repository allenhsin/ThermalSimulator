
#ifndef __THERMAL_DEVICE_MODEL_H__
#define __THERMAL_DEVICE_MODEL_H__

#include <map>
#include <string>
#include <vector>

#include "source/models/physical_model/device_models/device_type.h"

namespace Thermal
{
    class DeviceModel
    {
    public:
        static DeviceModel* createDevice(int device_type);

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

    protected:
        // derived child classes must call this constructor
        DeviceModel();
        virtual ~DeviceModel();
        
        // parameter_name -> parameter_value
        std::map<std::string, double>       _device_parameters;
        // port_name -> connected_device_pointer
        srd::map<std::string, DeviceModel*> _device_port_connections;

    private:
        std::string _target_parameter_name;
        std::string _target_port_name;

    }; // class DeviceModel

} // namespace Thermal

#endif // __THERMAL_DEVICE_MODEL_H__

