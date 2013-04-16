
#ifndef __THERMAL_PERFORMANCE_MODEL_H__
#define __THERMAL_PERFORMANCE_MODEL_H__

#include <stdio.h>

#include "source/models/model.h"
#include "source/data/data.h"
#include "source/system/event_scheduler.h"
#include "config.hpp"

namespace Thermal
{

    class PerformanceModel : public Model
    {
    public:
        PerformanceModel();
        ~PerformanceModel();

        void startup();
        void execute(double scheduled_time);

    protected:
        config::Config* getPerformanceConfig(){ return _performance_config; }

    private:
        config::Config*             _performance_config;

        bool                        _ready_to_execute;

    }; // class PerformanceModel

} // namespace Thermal

#endif // __THERMAL_PERFORMANCE_MODEL_H__

