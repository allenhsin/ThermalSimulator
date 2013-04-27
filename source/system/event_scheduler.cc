
#include "source/system/event_scheduler.h" 
#include "source/models/model.h"
#include "libutil/LibUtil.h"

#include <string>
#include <cassert>
#include <stddef.h>
#include <stdio.h>

using LibUtil::String;
using std::cout;
using std::scientific;

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

    void EventScheduler::setModel(ModelType model_type, Model* model)
    { _model[static_cast<int>(model_type)] = model; }

    void EventScheduler::enqueueEvent(Time scheduled_time, ModelType model_type)
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

    void EventScheduler::advanceSimClockByInterval(Time time_interval)
    { _sim_clock += time_interval; }

    void EventScheduler::advanceSimClockToAbsTime(Time abs_time)
    { _sim_clock = abs_time; }

    void EventScheduler::startScheduler()
    {
        Time next_progress_report_time = 1e-6;
        Time progress_report_time_interval = 1e-6;
        fprintf(stdout, "\n\n");

        // Set time to zero
        advanceSimClockToAbsTime(0);

        // Start up all models to prepare for running
        for (int i=0; i<NUM_MODEL_TYPES; ++i)
        {   
            assert(_model[i]!=NULL);
            LibUtil::Log::printLine ( "\nTime: " + (String) _sim_clock );
            _model[i]->startup();
        }

        // Execute events in the event queue until
        // the end of simulation, i.e. scheduler is finished
        EventScheduler::Event next_event;
        while ( (!_finished) && (_event_queue.size()!=0) )
        {
            next_event = dequeueEvent();
            advanceSimClockToAbsTime(next_event._scheduled_time);
            
            // progress report
            if(_sim_clock >= next_progress_report_time)
            {
                fprintf(stdout, "Time Progress: %e sec\n", next_progress_report_time);
                next_progress_report_time += progress_report_time_interval;
            }

            LibUtil::Log::printLine ( "\nTime: " + (String) _sim_clock );
            _model[static_cast<int>(next_event._model_type)]->execute(next_event._scheduled_time);
        }

        if(!_finished)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Simulation is not properly terminated.\n");
    }

    EventScheduler::EventScheduler()
        : _finished     (false)
        , _sim_clock    (0)
    {   
        _model.resize(NUM_MODEL_TYPES);
        for(int i=0; i<NUM_MODEL_TYPES; ++i)
            _model[i] = NULL;
    }

    EventScheduler::~EventScheduler()
    {}

} // namespace Thermal

