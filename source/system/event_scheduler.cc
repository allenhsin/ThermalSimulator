

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

    void EventScheduler::advaceSimClock(double time_interval)
    { (*_sim_clock) += time_interval; }

    EventScheduler::EventScheduler()
    {   
        _model.resize(NUM_MODEL_TYPES);
        for(int i=0; i<NUM_MODEL_TYPES; ++i)
            _model[i] = NULL;
    }

    EventScheduler::~EventScheduler()
    {   
        // although _model here holds the pointer of Model instances,
        // there's no need to free the memory because the Simulator
        // itself holds the model instances.
        _model.clear();
    }



} // namespace Thermal

