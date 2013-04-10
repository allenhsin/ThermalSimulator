
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

        // initialize device
        virtual void initializeDevice();

        // update device properties
        // Right now once the laser is setup (initialized)
        // then nothing will change during the operation
        // but laser power fluctuation model could be added
        // in the future
        virtual void updateDeviceProperties();

        void setLaserWavelength(double wavelength)
        { _wavelength = wavelength; }

    protected:

    private:
        double _wavelength;

    };

} // namespace Thermal

#endif // __THERMAL_LASER_SOURCE_OFF_CHIP_H__

