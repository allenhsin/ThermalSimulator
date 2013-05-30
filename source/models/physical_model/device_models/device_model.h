
#ifndef __THERMAL_DEVICE_MODEL_H__
#define __THERMAL_DEVICE_MODEL_H__

#include <map>
#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>

#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_models/port.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "source/misc/common_types.h"
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
        virtual void updateDeviceProperties(Time time_elapsed_since_last_update) = 0;

        // just for debug
        virtual void printDefinition(FILE* device_list_file);
        
        // initialize the monitoring of the device
        virtual void initializeMonitoring() = 0;
        
        // get the monitoring results at the time instant
        virtual void printMonitoredResult() = 0;

        // clone this device
        virtual DeviceModel* clone() const = 0;

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
        
        // Get map of ports and parameters
        const std::map<std::string, Port*>& getPorts() const { return _device_ports; }
        const std::map<std::string, double>& getParameters() const { return _device_parameters; }

        void addMonitoredPort(std::string port_name, std::string output_dir);
        void printSeparation();
        
        void getParentDevices(std::vector<DeviceModel*>& parent_devices);
        void getChildDevices(std::vector<DeviceModel*>& child_devices);

        void setDeviceName(std::string instance_name);
        std::string getInstanceName() { return _instance_name; }
        std::string getFloorplanUnitName() { return _floorplan_unit_name; }
        
        std::string getDeviceTypeName() const
        { return _device_type_name; }

        bool isRootDevice() const
        { return _is_root; }

        void setTraversedInBFS()
        { _traversed_in_bfs = true; }

        bool isTraversedInBFS() const
        { return _traversed_in_bfs; }

        bool isMappedOnFloorplan() const
        { return _mapped_on_floorplan; }

        double getDevicePower() const
        { return _device_power; }
        
    protected:
        // derived child classes must call this constructor
        DeviceModel(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);

        // copy constructor
        DeviceModel( const DeviceModel& cloned_device );

    // ------------------------------------------------------------------------
    // derived classes must implement these functions
    // ------------------------------------------------------------------------
        
        // check the validity of device parameters
        virtual void deviceParameterCheck() = 0;
    
    // ------------------------------------------------------------------------
        
        Port* getPortForModification(std::string port_name);

        std::string _instance_name;
        std::string _floorplan_unit_name;
        double      _device_power;
        
        // port_name -> (port_type, connected_port)
        std::map<std::string, Port*>    _device_ports;
        // parameter_name -> parameter_value
        std::map<std::string, double>   _device_parameters;

        // port name
        std::map<std::string, FILE*>    _monitored_device_ports;

    private:
        std::string         _device_type_name;
        std::string         _device_definition_file;
        DeviceFloorplanMap* _device_floorplan_map;

        bool _is_root;
        bool _mapped_on_floorplan;
        bool _traversed_in_bfs;

        std::string _target_parameter_name;
        std::string _target_port_name;

    }; // class DeviceModel

} // namespace Thermal

#endif // __THERMAL_DEVICE_MODEL_H__

