
#include "source/models/model.h"
#include "source/models/thermal_model/thermal_model.h"
#include "source/models/physical_model/physical_model.h"
#include "libutil/LibUtil.h"

#include <stddef.h>
#include <stdio.h>

using LibUtil::String;

namespace Thermal
{
    Model::Model()
        : _last_execute_time    (0)
    {}

    Model::~Model()
    {}

    Model*
    Model::createModel(int model_type)
    {
        switch (model_type)
        {
        //case PERFORMANCE_MODEL:
        //    return NULL;

        case PHYSICAL_MODEL:
            return new PhysicalModel();

        case THERMAL_MODEL:
            return new ThermalModel();

        //case EVALUATION_MODEL:
        //    return NULL;

        default:
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized Model Type: " + (String) model_type);
            return NULL;
        }
    }

} // namespace Thermal
