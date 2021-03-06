
#ifndef __THERMAL_DEVICE_TYPE_H__
#define __THERMAL_DEVICE_TYPE_H__

namespace Thermal
{
    // primitive device types that are
    // used to build optical links
    enum DeviceType
    {   
        RESONANT_RING,
        RESONANT_RING_DEPLETION_MODULATOR,
        MODULATOR_DRIVER,
        LOSSY_OPTICAL_NET,
        LASER_SOURCE_OFF_CHIP,
        PHOTODETECTOR,
        CURRENT_INTEGRATING_RECEIVER,
        THERMAL_TUNER,
        OPTICAL_DATA_GENERATOR,
        NUM_DEVICE_TYPES
    };

}


#endif // __THERMAL_DEVICE_TYPE_H__

