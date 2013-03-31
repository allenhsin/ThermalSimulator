
#ifndef __THERMAL_DEVICE_TYPE_H__
#define __THERMAL_DEVICE_TYPE_H__

namespace Thermal
{
    // primitive device types that are
    // used to build optical links
    enum DeviceType
    {   
        RING_MODULATOR,
        RING_RESONATOR,
        LASER_SOURCE,
        LOSSY_OPTICAL_NET,
        NUM_DEVICE_TYPES
    };

}


#endif // __THERMAL_DEVICE_TYPE_H__

