
#ifndef __THERMAL_DEVICE_DEFINITION_PARSER_H__
#define __THERMAL_DEVICE_DEFINITION_PARSER_H__

#include <map>
#include <string>

#include "source/models/physical_model/device_models/port.h"
#include "source/models/physical_model/device_models/device_model.h"

namespace Thermal
{
    typedef std::map<std::string, Port*> port_name_map;
    typedef std::map<std::string, double> param_name_map;

    class DeviceDefinitionParser
    {
    public:
        DeviceDefinitionParser( DeviceModel* device_model, port_name_map& device_ports, 
                                param_name_map& device_parameters, std::string& device_type_name);
        ~DeviceDefinitionParser(){}

        void loadDeviceDefinitionFile(string device_difinition_file);

    private:
        DeviceModel*    _device_model;
        // port_name -> (port_type, connected_port)
        port_name_map&  _device_ports;
        // parameter_name -> parameter_value
        param_name_map& _device_parameters;
        // name of device type
        std::string&    _device_type_name;

    }; // class DeviceDefinitionParser

}// namespace Thermal

#endif // __THERMAL_DEVICE_DEFINITION_PARSER_H__

