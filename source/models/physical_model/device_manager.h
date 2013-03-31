
#ifndef __THERMAL_DEVICE_MANAGER_H__
#define __THERMAL_DEVICE_MANAGER_H__

#include <vector>
#include <string>

#include "source/models/physical_model/device_model/device_model.h"

namespace Thermal
{
    class DeviceManager
    {
    public:
        DeviceManager();
        ~DeviceManager();

        void parseNetlist(std::string netlist_file);

    protected:

    private:
        DeviceFloorplanMap*         _device_floorplan_map;
        std::vector<DeviceModel*>   _device_instances;

    }; // calss DeviceManager

} // namespace Thermal

#endif // __THERMAL_DEVICE_MANAGER_H__

