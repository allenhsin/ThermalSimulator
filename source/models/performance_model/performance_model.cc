
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
        , _performance_config       (NULL)
        , _ready_to_execute         (false)
    {}

    PerformanceModel::~PerformanceModel()
    { 
        if(_performance_config)
            delete _performance_config;
    }
    
    void PerformanceModel::startup()
    {
        LibUtil::Log::printLine("Startup Performance Model");

    // Configure Performance Model --------------------------------------------
        // get performance cfg file name
        std::string performance_config_file =   Simulator::getSingleton()->getConfig()
                                                ->getString("models/performance_model/performance_config_file");

        // parse performance cfg file into config class
        Misc::setConfig(performance_config_file, _performance_config, 0, NULL);
        assert(_performance_config);

        // pass physical config by pointer
    // ------------------------------------------------------------------------

        _last_execute_time = 0;
        _ready_to_execute = true;
    }

    void PerformanceModel::execute(double scheduled_time)
    {
        assert(_ready_to_execute);
        assert(_performance_config);
        
        LibUtil::Log::printLine("Execute Performance Model");





        _last_execute_time = scheduled_time;
    }


} // namespace Thermal

