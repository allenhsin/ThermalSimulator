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
        // Child classes must call this constructor
        Model(EventScheduler* event_scheduler, Data* data);
        virtual ~Model();

        static Model* createModel(int model_type, EventScheduler* event_scheduler, Data* data);
        void setEventScheduler(EventScheduler* event_scheduler);
        void setData(Data* data);

        Data* getData(){ return _data; }
        EventScheduler* getEventScheduler(){ return _event_scheduler; }

        virtual void startup() = 0;
        virtual void execute(double scheduled_time) = 0;

    protected:
        Data* _data;
        EventScheduler* _event_scheduler;

    private:


    }; // class Model

} // namespace Thermal

#endif // __THERMAL_MODEL_H__
