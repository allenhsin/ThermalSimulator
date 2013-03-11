

#include "source/system/event_scheduler.h" 

#include <cassert>
#include <stddef.h>

namespace Thermal
{
    EventScheduler* EventScheduler::_event_scheduler_singleton = NULL;

    void EventScheduler::allocate()
    {
        assert(_event_scheduler_singleton==NULL);
        _event_scheduler_singleton = new EventScheduler();
        assert(_event_scheduler_singleton);
    }

    void EventScheduler::release()
    {
        assert(_event_scheduler_singleton);
        delete _event_scheduler_singleton;
        _event_scheduler_singleton = NULL;
    }
    
    EventScheduler* EventScheduler::getSingleton()
    { return _event_scheduler_singleton; }


    void EventScheduler::setModel(int model_type, Model* model)
    { _model[model_type] = model; }

    void EventScheduler::setSimClock(double* sim_clock)
    { _sim_clock = sim_clock; }

    void EventScheduler::enqueueEvent(double scheduled_time, int model_type)
    {
        Event new_event;
        new_event._model_type = model_type;
        new_event._scheduled_time = scheduled_time;

        _event_queue.push(new_event);
    }

    EventScheduler::Event EventScheduler::dequeueEvent()
    {
        Event next_event;
        next_event = _event_queue.top();
        _event_queue.pop();

        return next_event;
    }

    void EventScheduler::advaceSimClockByInterval(double time_interval)
    { (*_sim_clock) += time_interval; }

    void EventScheduler::advaceSimClockToAbsTime(double abs_time)
    { (*_sim_clock) = abs_time; }

    void startScheduler()
    {
        // Start up all models to prepare for running
        for (int i=0; i<NUM_MODEL_TYPES; ++i)
        {   
            // TODO: assert(_model[i]!=NULL);
            // TODO: _model[i]->startup();
        }

        // Execute events in the event queue until
        // the end of simulation
        // i.e. scheduler is finished

    }

    EventScheduler::EventScheduler()
    {   
        _finished = false;

        _model.resize(NUM_MODEL_TYPES);
        for(int i=0; i<NUM_MODEL_TYPES; ++i)
            _model[i] = NULL;
    }

    EventScheduler::~EventScheduler()
    {   
        // although _model here holds the pointer to Model instances,
        // there's no need to free the memory because the Simulator
        // itself holds the model instances.
        _model.clear();
    }



} // namespace Thermal

