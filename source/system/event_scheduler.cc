

#include "source/system/event_scheduler.h" 
#include "source/models/model.h"
#include "libutil/Log.h"

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
    { 
        assert(_event_scheduler_singleton);
        return _event_scheduler_singleton;
    }

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

    void EventScheduler::advanceSimClockByInterval(double time_interval)
    { (*_sim_clock) += time_interval; }

    void EventScheduler::advanceSimClockToAbsTime(double abs_time)
    { (*_sim_clock) = abs_time; }

    void EventScheduler::startScheduler()
    {
        // Set time to zero
        assert(_sim_clock!=NULL);
        advanceSimClockToAbsTime(0);

        // Start up all models to prepare for running
        for (int i=0; i<NUM_MODEL_TYPES; ++i)
        {   
            assert(_model[i]!=NULL);
            _model[i]->startup();

            LibUtil::Log::printLine("Time: " + getSimClock + " - Startup Model[" + i + "]");
        }

        // Execute events in the event queue until
        // the end of simulation
        // i.e. scheduler is finished
        EventScheduler::Event next_event;
        while (!_finished)
        {
            next_event = dequeueEvent();

            advanceSimClockToAbsTime(next_event._scheduled_time);
            _model[next_event._model_type]->execute(next_event._scheduled_time);

            LibUtil::Log::printLine("Time: " + getSimClock + " - Execute Model[" + i + "]");
        }
    }

    EventScheduler::EventScheduler()
        : _finished     (false)
        , _sim_clock    (NULL)
    {   
        _model.resize(NUM_MODEL_TYPES);
        for(int i=0; i<NUM_MODEL_TYPES; ++i)
            _model[i] = NULL;

        assert(_event_queue.empty());
    }

    EventScheduler::~EventScheduler()
    {   
        // although _model here holds the pointer to Model instances,
        // there's no need to free the memory because the Simulator
        // itself holds the model instances.
        _model.clear();
    }



} // namespace Thermal

