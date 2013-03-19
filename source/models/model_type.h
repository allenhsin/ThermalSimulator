
#ifndef __THERMAL_MODEL_TYPE_H__
#define __THERMAL_MODEL_TYPE_H__

namespace Thermal
{
    // Model types, listed in prioritized order
    // i.e. The order to run when scheduled to be
    // executed at the same time.
    enum ModelType
    {   
        //PERFORMANCE_MODEL = 0,
        PHYSICAL_MODEL,
        THERMAL_MODEL,
        //EVALUATION_MODEL,
        NUM_MODEL_TYPES
    };

}


#endif // __THERMAL_MODEL_TYPE_H__
