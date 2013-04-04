
#ifndef __THERMAL_LASER_SOURCE_OFF_CHIP_H__
#define __THERMAL_LASER_SOURCE_OFF_CHIP_H__

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"

namespace Thermal
{
    class LaserSourceOffChip : public DeviceModel
    {
    public:
        LaserSourceOffChip(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        ~LaserSourceOffChip();

        // update device properties
        virtual void updateDeviceProperties();


    protected:

    private:


    };

} // namespace Thermal

#endif // __THERMAL_LASER_SOURCE_OFF_CHIP_H__

