
#include "source/models/model.h"
#include "source/models/thermal_model/thermal_model.h"
#include "source/models/physical_model/physical_model.h"
#include "libutil/Log.h"
#include "libutil/String.h"

#include <stddef.h>
#include <stdio.h>

using LibUtil::String;

namespace Thermal
{
    Model::Model(EventScheduler* event_scheduler, Data* data)
        : _data             (data)
        , _event_scheduler  (event_scheduler)
    {}

    Model::~Model()
    {
        // there's no need to free the memory because the
        // simulator will do that in the end.
        _event_scheduler = NULL;
        _data = NULL;
    }

    Model*
    Model::createModel(int model_type, EventScheduler* event_scheduler, Data* data)
    {
        switch (model_type)
        {
        //case PERFORMANCE_MODEL:
        //    return NULL;

        case PHYSICAL_MODEL:
            return new PhysicalModel(event_scheduler, data);

        case THERMAL_MODEL:
            return new ThermalModel(event_scheduler, data);

        //case EVALUATION_MODEL:
        //    return NULL;

        default:
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized Model Type: " + (String) model_type);
            return NULL;
        }
    }

    void Model::setEventScheduler(EventScheduler* event_scheduler)
    { _event_scheduler = event_scheduler; }

    void Model::setData(Data* data)
    { _data = data; }



} // namespace Thermal
