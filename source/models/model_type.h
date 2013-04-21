
#ifndef __THERMAL_MODEL_TYPE_H__
#define __THERMAL_MODEL_TYPE_H__

namespace Thermal
{
    // Model types, listed in prioritized order
    // i.e. The execution order if scheduled at 
    // the same time.
    enum ModelType
    {   
        // performance model
        PERFORMANCE_MODEL,
        //LINK_ACTIVITY_TRACE_MANAGER,

        // physical model
        DEVICE_MANAGER,
        POWER_TRACE_MANAGER,

        // thermal model
        THERMAL_MODEL,

        NUM_MODEL_TYPES
    };

}


#endif // __THERMAL_MODEL_TYPE_H__
