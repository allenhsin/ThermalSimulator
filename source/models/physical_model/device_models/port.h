
#ifndef __THERMAL_PORT_H__
#define __THERMAL_PORT_H__

#include <map>
#include <vector>
#include <string>

namespace Thermal
{
    class DeviceModel;

    enum PortType
    {
        INPUT_PORT,
        OUTPUT_PORT,
        NULL_PORT
    }; // enum PortType

    class Port
    {
    public:
        Port(DeviceModel* device, PortType port_type);
        Port( const Port& cloned_port);
        ~Port();

        PortType getPortType() const
        { return _port_type; }

        DeviceModel* getDevice() const
        { return _device; }

        // also hook up the port properties of the two ports
        void setConnectedPort(const Port* connected_port);
        const Port* getConnectedPort() const
        { return _connected_port; }

        bool hasPortProperty(std::string property_name) const;

        void addPortProperty(std::string property_name);

        void setPortPropertySize(std::string property_name, unsigned int size);
        void setPortPropertyValueByIndex(std::string property_name, unsigned int index, double property_value);
        
        unsigned int    getPortPropertySize(std::string property_name) const;
        double          getPortPropertyValueByIndex(std::string property_name, unsigned int index) const;

    protected:
        std::vector<double>* getPortPropertyPointer(std::string property_name) const;
        void linkPortPropertyPointer(std::string property_name, std::vector<double>* property_pointer);

    private:
        const PortType      _port_type;
        DeviceModel* const  _device;
        const Port*         _connected_port;

        std::map< std::string, std::vector<double>* > _port_properties;

    }; // class Port

} // namespace Thermal

#endif // __THERMAL_PORT_H__

