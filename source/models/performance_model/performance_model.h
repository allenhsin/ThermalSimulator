
#ifndef __THERMAL_PERFORMANCE_MODEL_H__
#define __THERMAL_PERFORMANCE_MODEL_H__

#include <stdio.h>
#include <string>

#include "source/models/model.h"
#include "source/misc/common_types.h"
#include "source/data/data.h"
#include "source/system/event_scheduler.h"

namespace Thermal
{

    class PerformanceModel : public Model
    {
    public:
        virtual ~PerformanceModel();

        void startup();
        void execute(Time scheduled_time);

        virtual bool startupManager() = 0;
        virtual void executeManager(Time scheduled_time) = 0;
        virtual std::string getModelName() { return "Performance Model"; }

    protected:
        PerformanceModel();

        void loadConfig();

    private:

    }; // class PerformanceModel

} // namespace Thermal

#endif // __THERMAL_PERFORMANCE_MODEL_H__

