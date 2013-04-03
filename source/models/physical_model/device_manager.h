
#ifndef __THERMAL_DEVICE_MANAGER_H__
#define __THERMAL_DEVICE_MANAGER_H__

#include <vector>
#include <string>

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "config.hpp"

namespace Thermal
{
    class DeviceManager
    {
    public:
        DeviceManager();
        ~DeviceManager();

        void startup();
        void execute();

        void loadNetlist(std::string netlist_file);

        void setPhysicalConfig(config::Config* physical_config) 
        { _physical_config = physical_config; }

        config::Config* getPhysicalConfig(){ return _physical_config; }

    protected:

    private:
        config::Config*             _physical_config;
        DeviceFloorplanMap*         _device_floorplan_map;

        // _device_instances and _device_root_instances are
        // data placeholders resposible for device instance
        // construction and destruction. The creation of the 
        // device instance is when loading the netlist.
        std::vector<DeviceModel*>   _device_instances;
        std::vector<DeviceModel*>   _device_root_instances;
        
        // after loading the netlist, device manager will
        // trace down the device instances connections to
        // figure out the link sequence and store it
        
    }; // calss DeviceManager

} // namespace Thermal

#endif // __THERMAL_DEVICE_MANAGER_H__

