
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <string>

#include "source/models/physical_model/physical_model.h"
#include "source/models/physical_model/physical_constants.h"
#include "source/models/model_type.h"
#include "source/system/simulator.h"
#include "source/system/event_scheduler.h"
#include "source/misc/misc.h"
#include "source/data/data.h"
#include "libutil/LibUtil.h"

using std::vector;
using std::string;

namespace Thermal
{
    PhysicalModel::PhysicalModel()
        : Model()
        , _physical_config          (NULL)
        , _power_trace_mode         (new PowerTraceMode())
        , _device_manager           (new DeviceManager())
        , _ready_to_execute         (false)
    {}

    PhysicalModel::~PhysicalModel()
    {
        delete _power_trace_mode;
        delete _device_manager;
        
        if(_physical_config)
            delete _physical_config;
    }
    
    void PhysicalModel::startup()
    {
        LibUtil::Log::printLine("Startup Physical Model");

    // Configure Physical Model -----------------------------------------------
        // get physical cfg file name
        std::string physical_config_file =  Simulator::getSingleton()->getConfig()
                                            ->getString("models/physical_model/physical_config_file");

        // parse physical cfg file into config class
        Misc::setConfig(physical_config_file, _physical_config, 0, NULL);
        assert(_physical_config);

        // pass physical config by pointer
        _power_trace_mode->setPhysicalConfig(_physical_config);
        _device_manager->setPhysicalConfig(_physical_config);
    // ------------------------------------------------------------------------

    // Startup Power Trace Mode -----------------------------------------------
        if(getPhysicalConfig()->getBool("ptrace_mode/enable"))
            _power_trace_mode->startup();
    // ------------------------------------------------------------------------

    // Startup Device Manager -------------------------------------------------
        if(getPhysicalConfig()->getBool("device_manager/enable"))
            _device_manager->startup();
    // ------------------------------------------------------------------------

        _last_execute_time = 0;
        _ready_to_execute = true;
    } // startup
    
    void PhysicalModel::execute(double scheduled_time)
    {
        assert(_ready_to_execute);
        assert(_physical_config);

        LibUtil::Log::printLine("Execute Physical Model");

    // Execute Power Trace Mode -----------------------------------------------
        if(getPhysicalConfig()->getBool("ptrace_mode/enable"))
            _power_trace_mode->execute(scheduled_time);
    // ------------------------------------------------------------------------
        
    // Execute Device Manager -------------------------------------------------
        if(getPhysicalConfig()->getBool("device_manager/enable"))
            _device_manager->execute(scheduled_time);
    // ------------------------------------------------------------------------

        _last_execute_time = scheduled_time;
    } // execute

} // namespace Thermal

