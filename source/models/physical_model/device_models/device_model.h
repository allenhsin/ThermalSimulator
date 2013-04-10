
#ifndef __THERMAL_DEVICE_MODEL_H__
#define __THERMAL_DEVICE_MODEL_H__

#include <map>
#include <string>
#include <vector>
#include <math.h>

#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_models/port.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "config.hpp"

namespace Thermal
{
    class DeviceModel
    {
    public:
        virtual ~DeviceModel();

        static DeviceModel* createDevice(DeviceType device_type, config::Config* physical_config, DeviceFloorplanMap* device_floorplan_map);

    // ------------------------------------------------------------------------
    // derived classes must implement these functions
    // ------------------------------------------------------------------------

        // initialize device
        virtual void initializeDevice() = 0;
        
        // update device properties
        virtual void updateDeviceProperties() = 0;

    // ------------------------------------------------------------------------

        bool hasParameter(std::string parameter_name);
        // setTargetParameterName is to tell the device which loaded
        // parameter is going to change its value. Have to separate
        // this with the set value function because when parsing the 
        // netlist it reads the name first and then the value
        void setTargetParameterName(std::string parameter_name);
        // when setting the value it will also verify the 
        // existance of the parameter for the device
        // i.e. can only set for the loaded parameters
        void setTargetParameterValue(double parameter_value);
        double getParameter(std::string parameter_name);
        
        bool hasPort(std::string port_name);
        // similar to set parameters
        void setTargetPortName(std::string port_name);
        void setTargetPortConnectedPort(const Port* port);
        const Port* getPort(std::string port_name);
        
        void getParentDevices(std::vector<DeviceModel*>& parent_devices);
        void getChildDevices(std::vector<DeviceModel*>& child_devices);

        void setDeviceName(std::string instance_name);
        std::string getInstanceName() { return _instance_name; }
        std::string getFloorplanUnitName() { return _floorplan_unit_name; }

        void setTraversedInBFS()
        { _traversed_in_bfs = true; }

        bool isTraversedInBFS()
        { return _traversed_in_bfs; }

        bool isMappedInFloorplan()
        { return _mapped_in_floorplan; }
        
        // just for debug
        void printDefinition();

    protected:
        // derived child classes must call this constructor
        DeviceModel(DeviceType device_type, DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        
        Port* getPortForModification(std::string port_name);

        DeviceType  _device_type;
        std::string _instance_name;
        std::string _floorplan_unit_name;
        
        // port_name -> (port_type, connected_port)
        std::map<std::string, Port*>    _device_ports;
        // parameter_name -> parameter_value
        std::map<std::string, double>   _device_parameters;

    private:
        std::string         _device_definition_file;
        DeviceFloorplanMap* _device_floorplan_map;
        
        bool _mapped_in_floorplan;
        bool _traversed_in_bfs;

        std::string _target_parameter_name;
        std::string _target_port_name;

    }; // class DeviceModel

} // namespace Thermal

#endif // __THERMAL_DEVICE_MODEL_H__

