
#ifndef __THERMAL_DEVICE_MANAGER_H__
#define __THERMAL_DEVICE_MANAGER_H__

#include <vector>
#include <string>
#include <stdio.h>

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "source/misc/common_types.h"
#include "config.hpp"

namespace Thermal
{
    class DeviceManager
    {
    public:
        DeviceManager();
        ~DeviceManager();

        void startup();
        void execute(Time scheduled_time);

        void setPhysicalConfig(config::Config* physical_config) 
        { _physical_config = physical_config; }

    protected:
        config::Config* getPhysicalConfig(){ return _physical_config; }

        // Breadth-First Search to build device sequence
        void buildDeviceSequence();

    private:
        config::Config*             _physical_config;
        DeviceFloorplanMap*         _device_floorplan_map;

        Time                        _sub_bit_sampling_intvl;
        Time                        _last_execute_time;

        bool                        _ready_to_execute;
        
        // _device_instances are data placeholders 
        // resposible for device instance construction 
        // and destruction. The creation of the device 
        // instance is when loading the netlist.
        std::vector<DeviceModel*>   _device_instances;
        
        // after loading the netlist, device manager will
        // trace down the device instances connections to
        // figure out the device sequence along the link
        // and store it in _device_sequence.
        std::vector<DeviceModel*>   _device_sequence;

    }; // calss DeviceManager

} // namespace Thermal

#endif // __THERMAL_DEVICE_MANAGER_H__

