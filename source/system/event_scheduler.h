
#ifndef __THERMAL_EVENT_SCHEDULER_H__
#define __THERMAL_EVENT_SCHEDULER_H__

#include <vector>
#include <queue>

#include "source/models/model_type.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class Model;

    class EventScheduler
    {
    private:
        struct Event
        {
            Time        _scheduled_time;
            ModelType   _model_type;
        }; // struct Event

    public:
        static void allocate();
        static void release();
        static EventScheduler* getSingleton();
        
        void setModel(ModelType model_type, Model* model);

        void  setSimClock(Time* sim_clock);
        Time  getSimClock(){ return (*_sim_clock); }

        void enqueueEvent(Time schduled_time, ModelType model_type);

        void startScheduler();

        void finish(){ _finished = true; }

    protected:
        EventScheduler();
        ~EventScheduler();

        Event dequeueEvent();

        void advanceSimClockByInterval(Time time_interval);
        void advanceSimClockToAbsTime(Time abs_time);

    private:
        static EventScheduler* _event_scheduler_singleton; 
        
        bool _finished;

        std::vector< Model* > _model;
        Time* _sim_clock;

        class SortEvent
        {
        public:
            bool operator()(const Event& event_lhs, const Event& event_rhs)
            {
                if (event_rhs._scheduled_time == event_lhs._scheduled_time)
                    return (static_cast<int>(event_rhs._model_type) < static_cast<int>(event_lhs._model_type));
                else
                    return (event_rhs._scheduled_time < event_lhs._scheduled_time); 
            }
        }; // class SortEvent
    
        std::priority_queue<Event, std::vector<Event>, SortEvent> _event_queue;

    }; // class EventScheduler

} // namespace Thermal

#endif // __THERMAL_EVENT_SCHEDULER_H__

