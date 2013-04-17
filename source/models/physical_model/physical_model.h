
#ifndef __THERMAL_PHYSICAL_MODEL_H__
#define __THERMAL_PHYSICAL_MODEL_H__

#include <vector>
#include <string>
#include <stdio.h>

#include "source/models/model.h"
#include "source/data/data.h"
#include "source/system/event_scheduler.h"
#include "source/models/physical_model/power_trace_manager.h"
#include "source/models/physical_model/device_manager.h"
#include "source/misc/common_types.h"
#include "config.hpp"

namespace Thermal
{

    class PhysicalModel : public Model
    {
    public:
        PhysicalModel();
        ~PhysicalModel();

        virtual void startup();
        virtual void execute(Time scheduled_time);

        virtual std::string getModelName() { return "Physical Model"; }

    protected:
        config::Config* getPhysicalConfig(){ return _physical_config; }

    private:
        config::Config*             _physical_config;

        PowerTraceManager*          _power_trace_manager;
        DeviceManager*              _device_manager;

        bool                        _ready_to_execute;

    }; // class PhysicalModel

} // namespace Thermal

#endif // __THERMAL_PHYSICAL_MODEL_H__

