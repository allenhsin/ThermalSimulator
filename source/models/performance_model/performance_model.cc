
#include <cassert>
#include <string>

#include "source/models/performance_model/performance_model.h"
#include "source/system/simulator.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

using std::string;

namespace Thermal
{
    PerformanceModel::PerformanceModel()
        : Model()
    {}

    PerformanceModel::~PerformanceModel()
    {}
    
    void PerformanceModel::loadConfig()
    {
        // get performance cfg file name
        string performance_config_file =    Simulator::getSingleton()->getConfig()
                                            ->getString("models/performance_model/performance_config_file");

        // parse performance cfg file into config class
        Misc::setConfig(performance_config_file, _config, 0, NULL);
    }

    void PerformanceModel::startup()
    {
        LibUtil::Log::printLine( "Startup " + getModelName() );
        
        loadConfig();

        bool manager_enabled = startupManager();
        if(!manager_enabled)
            return;

        _last_execute_time = 0;
        _ready_to_execute = true;
    }

    void PerformanceModel::execute(Time scheduled_time)
    {
        if(!_ready_to_execute)
            return;
        
        if( Misc::eqTime(scheduled_time,_last_execute_time) && (_last_execute_time!=0) )
            return;

        assert(_config);
        LibUtil::Log::printLine( "Execute " + getModelName() );
        
        executeManager(scheduled_time);

        _last_execute_time = scheduled_time;
    }

} // namespace Thermal

