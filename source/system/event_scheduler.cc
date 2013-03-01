

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

    EventScheduler::EventScheduler()
    {}

    EventScheduler::~EventScheduler()
    {}



} // namespace Thermal

