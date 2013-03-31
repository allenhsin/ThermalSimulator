
#ifndef __THERMAL_PHYSICAL_MODEL_H__
#define __THERMAL_PHYSICAL_MODEL_H__

#include <vector>
#include <stdio.h>

#include "source/models/model.h"
#include "source/data/data.h"
#include "source/system/event_scheduler.h"
#include "source/models/physical_model/power_trace_mode.h"
#include "config.hpp"

namespace Thermal
{

    class PhysicalModel : public Model
    {
    public:
        PhysicalModel();
        ~PhysicalModel();

        void startup();
        void execute(double scheduled_time);

        config::Config* getPhysicalConfig(){ return _physical_config; }

    protected:

    private:
        config::Config*             _physical_config;
        PowerTraceMode*             _power_trace_mode;

        bool                        _ready_to_execute;

    }; // class PhysicalModel

} // namespace Thermal

#endif // __THERMAL_PHYSICAL_MODEL_H__

