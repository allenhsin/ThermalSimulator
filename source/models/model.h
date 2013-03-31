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
        virtual ~Model();

        static Model* createModel(int model_type);

        virtual void startup() = 0;
        virtual void execute(double scheduled_time) = 0;

    protected:
        // Child classes must call this constructor
        Model();

        double _last_execute_time;

    private:


    }; // class Model

} // namespace Thermal

#endif // __THERMAL_MODEL_H__
