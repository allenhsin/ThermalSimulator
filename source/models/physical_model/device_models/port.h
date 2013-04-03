
#ifndef __THERMAL_PORT_H__
#define __THERMAL_PORT_H__

#include <map>
#include <string>

namespace Thermal
{
    class DeviceModel;

    enum PortType
    {
        INPUT_PORT,
        OUTPUT_PORT,
        INOUT_PORT,
        NULL_PORT
    }; // enum PortType

    class Port
    {
    public:
        Port(DeviceModel* device, PortType port_type);
        ~Port();

        PortType getPortType()
        { return _port_type; }

        DeviceModel* getDevice()
        { return _device; }

        void setConnectedPort(Port* connected_port)
        { _connected_port = connected_port; }

        Port* getConnectedPort()
        { return _connected_port; }

        void addPortProperty(std::string property_name, double default_property_value = 0);
        void setPortProperty(std::string property_name, double property_value);
        bool   hasPortProperty(std::string property_name);
        double getPortProperty(std::string property_name);

    protected:

    private:
        const PortType                  _port_type;
        DeviceModel* const              _device;
        Port*                           _connected_port;

        std::map<std::string, double>   _port_properties;

    }; // class Port

} // namespace Thermal

#endif // __THERMAL_PORT_H__

