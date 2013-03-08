
#ifndef __THERMAL_EVENT_SCHEDULER_H__
#define __THERMAL_EVENT_SCHEDULER_H__

#include <vector>

#include "source/models/model_type.h"

namespace Thermal
{
    class Model;

    class EventScheduler
    {
    public:
        static void allocate();
        static void release();
        static EventScheduler* getSingleton();
        
        void setModel(int model_type, Model* model);
        void setSimClock(double* sim_clock);

        void advaceSimClock(double time_interval);

    protected:
        EventScheduler();
        ~EventScheduler();

    private:
        static EventScheduler* _event_scheduler_singleton; 
        
        std::vector< Model* > _model;
        double* _sim_clock;

    }; // class EventScheduler

} // namespace Thermal

#endif // __THERMAL_EVENT_SCHEDULER_H__
