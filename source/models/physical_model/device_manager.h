
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

        void loadNetlist(std::string netlist_file);

        void setPhysicalConfig(config::Config* physical_config) 
        { _physical_config = physical_config; }

        config::Config* getPhysicalConfig(){ return _physical_config; }

    protected:

    private:
        config::Config*             _physical_config;
        DeviceFloorplanMap*         _device_floorplan_map;
        std::vector<DeviceModel*>   _device_instances;

    }; // calss DeviceManager

} // namespace Thermal

#endif // __THERMAL_DEVICE_MANAGER_H__

