
#include "source/models/model.h"
#include "libutil/Log.h"

#include <stddef.h>
#include <stdio.h>

namespace Thermal
{
    Model::Model(EventScheduler* event_scheduler, Data* data)
        : _event_scheduler  (event_scheduler)
        , _data             (data)
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
        case PERFORMANCE_MODEL:
            return NULL;

        case PHYSICAL_MODEL:
            return NULL;

        case THERMAL_MODEL:
            return NULL;

        case EVALUATION_MODEL:
            return NULL;

        default:
            LibUtil::Log::printLine(stderr, "Unrecognized Model Type: " + model_type);
            abort();
            return NULL;
        }
    }

    void Model::setEventScheduler(EventScheduler* event_scheduler)
    { _event_scheduler = event_scheduler; }

    void Model::setData(Data* data)
    { _data = data; }



} // namespace Thermal
