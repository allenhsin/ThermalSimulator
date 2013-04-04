
#ifndef __THERMAL_LOSSY_OPTICAL_NET_H__
#define __THERMAL_LOSSY_OPTICAL_NET_H__

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"

namespace Thermal
{
    class LossyOpticalNet : public DeviceModel
    {
    public:
        LossyOpticalNet(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        ~LossyOpticalNet();

        // update device properties
        virtual void updateDeviceProperties();


    protected:

    private:


    };

} // namespace Thermal

#endif // __THERMAL_LOSSY_OPTICAL_NET_H__

