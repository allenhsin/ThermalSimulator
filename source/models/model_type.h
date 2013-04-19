
#ifndef __THERMAL_MODEL_TYPE_H__
#define __THERMAL_MODEL_TYPE_H__

namespace Thermal
{
    // Model types, listed in prioritized order
    // i.e. The execution order if scheduled at 
    // the same time.
    enum ModelType
    {   
        THERMAL_MODEL,
        PERFORMANCE_MODEL,
        PHYSICAL_MODEL,
        NUM_MODEL_TYPES
    };

}


#endif // __THERMAL_MODEL_TYPE_H__
