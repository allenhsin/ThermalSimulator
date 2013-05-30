
#include <cassert>
#include <stddef.h>

#include "source/models/physical_model/device_models/port.h"
#include "libutil/LibUtil.h"

using std::map;
using std::vector;
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

    Port::Port( const Port& cloned_port)
        : _port_type        (cloned_port._port_type)
        , _device           (NULL)
        , _connected_port   (NULL)
    {
        _port_properties.clear();
        if(_port_type == INPUT_PORT)
            for (map< string, vector<double>* >::const_iterator it = cloned_port._port_properties.begin(); it != cloned_port._port_properties.end(); ++it)
                _port_properties[it->first] = NULL;
        else // OUTPUT_PORT
            for (map< string, vector<double>* >::const_iterator it = cloned_port._port_properties.begin(); it != cloned_port._port_properties.end(); ++it)
                _port_properties[it->first] = new vector<double>();
    }

    Port::~Port()
    {
        if(_port_type == OUTPUT_PORT)
            for (map< string, vector<double>* >::iterator it = _port_properties.begin(); it != _port_properties.end(); ++it)
            { if(it->second) delete it->second; }
    }

    Port* Port::clone(DeviceModel* device) const
    { 
        Port* port = new Port(*this); 
        port->_device = device;

        return port;
    }
    
    void Port::setConnectedPort(const Port* connected_port)
    { 
        _connected_port = connected_port; 
        
        // only link the properties of INPUT ports to its connected ports
        // so if it's an OUTPUT port, do nothing
        if(_port_type == OUTPUT_PORT)
            return;

        for (map< string, vector<double>* >::iterator it = _port_properties.begin(); it != _port_properties.end(); ++it)
            linkPortPropertyPointer(it->first, _connected_port->getPortPropertyPointer(it->first));
    }
    
    bool Port::hasPortProperty(string property_name) const
    {   return (_port_properties.count(property_name)); }

    void Port::addPortProperty(string property_name)
    {
        if(hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" already exists.\n");
        
        if(_port_type == INPUT_PORT)
            _port_properties[property_name] = NULL;
        else // OUTPUT_PORT
            _port_properties[property_name] = new vector<double>();
    }

    void Port::setPortPropertySize(string property_name, unsigned int size)
    {
        if(!hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" does not exist.\n");
        
        _port_properties[property_name]->resize(size);
    }

    void Port::setPortPropertyValueByIndex(string property_name, unsigned int index, double property_value)
    {
        if(!hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" does not exist.\n");

        (*_port_properties[property_name])[index] = property_value;
    }

    unsigned int Port::getPortPropertySize(string property_name) const
    {
        if(!hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" does not exist.\n");
        
        if(!_port_properties.find(property_name)->second)
            return 0;
        else
            return _port_properties.find(property_name)->second->size();
    }

    double Port::getPortPropertyValueByIndex(string property_name, unsigned int index) const
    {
        if(!hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" does not exist.\n");

        if(!_port_properties.find(property_name)->second)
            return 0;
        else
            return (*_port_properties.find(property_name)->second)[index];
    }



    void Port::linkPortPropertyPointer(string property_name, vector<double>* property_pointer)
    {
        if(!hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" does not exist.\n");

        _port_properties[property_name] = property_pointer;
    }

    vector<double>* Port::getPortPropertyPointer(string property_name) const
    {
        if(!hasPortProperty(property_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Port Property \"" + property_name + "\" does not exist.\n");

        return _port_properties.find(property_name)->second;
    }

} // namespace Thermal

