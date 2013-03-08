#ifndef __THERMAL_MODEL_H__
#define __THERMAL_MODEL_H__

#include "source/data/data.h"
#include "source/system/event_scheduler.h"
#include "source/models/model_type.h"

namespace Thermal
{
    class Model
    {
    public:
        Model();
        ~Model();

        Model* createModel(ModelType model_type);

    protected:
        Data* _data;
        EventScheduler* _event_scheduler;

    private:


    }; // class Model

} // namespace Thermal

#endif // __THERMAL_MODEL_H__
