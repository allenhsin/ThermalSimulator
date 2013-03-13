
#ifndef __THERMAL_EVENT_SCHEDULER_H__
#define __THERMAL_EVENT_SCHEDULER_H__

#include <vector>
#include <queue>

#include "source/models/model_type.h"

namespace Thermal
{
    class Model;

    class EventScheduler
    {
    private:
        struct Event
        {
            double  _scheduled_time;
            int     _model_type;
        }; // struct Event

    public:
        static void allocate();
        static void release();
        static EventScheduler* getSingleton();
        
        void setModel(int model_type, Model* model);
        void setSimClock(double* sim_clock);

        double getSimClock(){ return (*_sim_clock); }

        void advanceSimClockByInterval(double time_interval);
        void advanceSimClockToAbsTime(double abs_time);

        void enqueueEvent(double schduled_time, int model_type);

        void startScheduler();

        void finished(){ _finished = true; }

    protected:
        EventScheduler();
        ~EventScheduler();

        Event dequeueEvent();

    private:
        static EventScheduler* _event_scheduler_singleton; 
        
        bool _finished;

        std::vector< Model* > _model;
        double* _sim_clock;

        class SortEvent
        {
        public:
            bool operator()(const Event& event_lhs, const Event& event_rhs)
            {
                if (event_rhs._scheduled_time == event_lhs._scheduled_time)
                    return (event_rhs._model_type < event_lhs._model_type);
                else
                    return (event_rhs._scheduled_time < event_lhs._scheduled_time); 
            }
        }; // class SortEvent
    
        std::priority_queue<Event, std::vector<Event>, SortEvent> _event_queue;

    }; // class EventScheduler

} // namespace Thermal

#endif // __THERMAL_EVENT_SCHEDULER_H__



