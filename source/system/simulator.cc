
#include "source/system/simulator.h" 
#include "source/misc/misc.h"
#include "libutil/Log.h"

#include <stddef.h>
#include <cassert>
#include <stdio.h>

namespace Thermal
{
    Simulator* Simulator::_simulator_singleton = NULL;

    Simulator::Simulator()
        : _sim_clock        (0)
        , _config           (NULL)
        , _data             (NULL)
        , _event_scheduler  (NULL)
        , _config           (NULL)
    {}

    Simulator::~Simulator()
    {}

    void Simulator::allocate()
    {
        assert(_simulator_singleton==NULL);
        _simulator_singleton = new Simulator();
        assert(_simulator_singleton);
    }

    void Simulator::release()
    {
        assert(_simulator_singleton);
        delete _simulator_singleton;
        _simulator_singleton = NULL;
    }
    
    Simulator* Simulator::getSingleton()
    {
        assert(_simulator_singleton);
        return _simulator_singleton; 
    }

    //FIXME: this giant piece of code will be cleaned up later XD
    void Simulator::run(int argc_, char** argv_)
    {

    // Configure Simulator ----------------------------------------------------
        
        // default config file if not overridden by command line arguments
        std::string config_file = "./configs/thermal_sim.cfg";
        
        // set config
        Misc::setConfig(config_file, _config, argc_, argv_);
        assert(_config);
    // ------------------------------------------------------------------------


    // Create Log System ------------------------------------------------------
        LibUtil::Log::allocate( getConfig()->getString("log/log_file"),
                                getConfig()->getBool("log/log_enabled") );
    // ------------------------------------------------------------------------


    // Allocate and Link System Modules ---------------------------------------
       
        // Event Scheduler
        EventScheduler::allocate();
        _event_scheduler = EventScheduler::getSingleton();

        // Data Structure
        Data::allocate();
        _data = Data::getSingleton();

        // Models
        _model.resize(NUM_MODEL_TYPES);
        for (int i=0; i<NUM_MODEL_TYPES; ++i)
        {
            _model[i] = Model::createModel(i, _event_scheduler, _data);
            _event_scheduler->setModel(i, _model[i]);
        }

        _event_scheduler->setSimClock(&_sim_clock);
    // ------------------------------------------------------------------------


    // Start Scheduler --------------------------------------------------------

    // ------------------------------------------------------------------------


    // Finalize ---------------------------------------------------------------

        // release models
        for (int i=0; i<NUM_MODEL_TYPES; ++i)
        {
            delete _model[i];
            _model[i] = NULL;
        }

        // release data structure
        Data::release();
        _data = NULL;

        // release even scheduler
        EventScheduler::release();
        _event_scheduler = NULL;

        // release log system
        LibUtil::Log::release();

        // release config
        delete _config;
        _config = NULL;

    // ------------------------------------------------------------------------

    }
    

} // namespace Thermal
