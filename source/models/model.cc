
#include <stddef.h>

#include "source/models/model.h"
#include "libutil/LibUtil.h"

// models & managers
#include "source/models/thermal_model/thermal_model.h"
#include "source/models/performance_model/link_activity_trace_manager.h"
#include "source/models/performance_model/performance_model.h"
#include "source/models/physical_model/power_trace_manager.h"
#include "source/models/physical_model/device_manager.h"

using LibUtil::String;

namespace Thermal
{
    Model::Model()
        : _config               (NULL)
        , _ready_to_execute     (false)
        , _last_execute_time    (0)
    {}

    Model::~Model()
    {
        if(_config)
            delete _config;
    }

    Model*
    Model::createModel(ModelType model_type)
    {
        switch (model_type)
        {
        //case LINK_ACTIVITY_TRACE_MANAGER:
        //    return new LinkActivityTraceManager();
        case PERFORMANCE_MODEL:
            return new PerformanceModel();

        case DEVICE_MANAGER:
            return new DeviceManager();

        case POWER_TRACE_MANAGER:
            return new PowerTraceManager();

        case THERMAL_MODEL:
            return new ThermalModel();

        default:
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized Model Type: " + (String) model_type + ".\n");
            return NULL;
        }
    }

} // namespace Thermal

