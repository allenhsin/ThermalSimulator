
#include <cassert>

#include "source/models/physical_model/device_models/device_model.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    DeviceModel::DeviceModel()
    {
        _device_parameters.clear();
        _device_port_connections.clear();
        _target_parameter_name = NULL;
        _target_port_name = NULL;
    }

    DeviceModel::~DeviceModel()
    {}

    DeviceModel*
    DeviceModel::createDevice(int device_type)
    {
        switch(device_type)
        {
        case RING_MODULATOR:
            return NULL;

        case RING_RESONATOR:
            return NULL;

        case LASER_SOURCE:
            return NULL;

        case LOSSY_OPTICAL_NET:
            return NULL;

        default:
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized Device Type: " + (String) device_type);
            return NULL;
        }
    }

    bool DeviceModel::hasParameter(std::string parameter_name)
    { return _device_parameters.count(parameter_name); }

    void DeviceModel::setTargetParameterName(std::string parameter_name)
    { 
        if(!hasParameter(parameter_name))
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized parameter: " + (String) parameter_name);
        _target_parameter_name = parameter_name; 
    }

    void DeviceModel::setTargetParameterValue(double parameter_value)
    {
        assert(_target_parameter_name);
        _device_parameters[_target_parameter_name] = parameter_value;
    }

    bool DeviceModel::hasPort(std::string port_name)
    { return _device_port_connections.count(port_name); }

    void DeviceModel::setTargetPortName(std::string port_name)
    { 
        if(!hasParameter(port_name))
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized port: " + (String) port_name);
        _target_port_name = port_name; 
    }

    void DeviceModel::setTargetPortConnectedDevice(DeviceModel* device)
    {
        assert(_target_port_name);
        _device_port_connections[_target_port_name] = device;
    }


} // namespace Thermal

