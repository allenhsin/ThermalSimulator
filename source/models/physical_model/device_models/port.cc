
#include <cassert>
#include <stddef.h>

#include "source/models/physical_model/device_models/port.h"
#include "libutil/LibUtil.h"

using std::map;
using std::string;

namespace Thermal
{
    Port::Port(DeviceModel* device, PortType port_type)
        : _port_type        (port_type)
        , _device           (device)
        , _connected_port   (NULL)
    {
        assert(_device);
        assert(_port_type != NULL_PORT);
        _port_properties.clear();
    }

    Port::~Port()
    {}

    bool Port::hasPortProperty(string property_name)
    {   return (_port_properties.count(property_name)); }

    void Port::addPortProperty(string property_name, double default_property_value)
    {
        if(hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" already exists.\n");

        _port_properties[property_name] = default_property_value;
    }

    void Port::setPortProperty(string property_name, double property_value)
    {
        if(!hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" already does not exist.\n");

        _port_properties[property_name] = property_value;
    }

    double Port::getPortProperty(string property_name)
    {
        if(!hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" already does not exist.\n");

        return _port_properties[property_name];
    }

} // namespace Thermal

