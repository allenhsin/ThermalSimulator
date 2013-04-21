
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <string>

#include "source/models/performance_model/performance_model.h"
#include "source/models/model_type.h"
#include "source/system/simulator.h"
#include "source/system/event_scheduler.h"
#include "source/misc/misc.h"
#include "source/data/data.h"
#include "libutil/LibUtil.h"

using std::string;

namespace Thermal
{
    PerformanceModel::PerformanceModel()
        : Model()
        , _link_activity_trace_manager  ( new LinkActivityTraceManager() )
    {}

    PerformanceModel::~PerformanceModel()
    { 
        delete _link_activity_trace_manager;
    }
    
    void PerformanceModel::startup()
    {
        LibUtil::Log::printLine( "Startup " + getModelName() );

    // Configure Performance Model --------------------------------------------
        // get performance cfg file name
        std::string performance_config_file =   Simulator::getSingleton()->getConfig()
                                                ->getString("models/performance_model/performance_config_file");

        // parse performance cfg file into config class
        Misc::setConfig(performance_config_file, _config, 0, NULL);
        assert(_config);

        // pass physical config by pointer
        _link_activity_trace_manager->setPerformanceConfig(_config);
    // ------------------------------------------------------------------------

    // Link activity trace manager --------------------------------------------
        if(_config->getBool("latrace_manager/enable"))
            _link_activity_trace_manager->startup();
    // ------------------------------------------------------------------------

        _last_execute_time = 0;
        _ready_to_execute = true;
    }

    void PerformanceModel::execute(Time scheduled_time)
    {
        assert(_ready_to_execute);
        assert(_config);
        
        LibUtil::Log::printLine( "Execute " + getModelName() );

    // Link activity trace manager --------------------------------------------
        if(_config->getBool("latrace_manager/enable"))
            _link_activity_trace_manager->execute(scheduled_time);
    // ------------------------------------------------------------------------

        _last_execute_time = scheduled_time;
    }

} // namespace Thermal

