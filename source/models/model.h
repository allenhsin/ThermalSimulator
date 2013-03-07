#ifndef __THERMAL_MODEL_H__
#define __THERMAL_MODEL_H__

#include "source/data/data.h"
#include "source/system/event_scheduler.h"

namespace Thermal
{
    class Model
    {
    public:
        Model();
        ~Model();

    protected:
        Data* _data;
        EventScheduler* _event_scheduler;

    private:


    }; // class Model

} // namespace Thermal

#endif // __THERMAL_MODEL_H__
