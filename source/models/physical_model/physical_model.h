
#ifndef __THERMAL_PHYSICAL_MODEL_H__
#define __THERMAL_PHYSICAL_MODEL_H__

#include <vector>
#include <stdio.h>

#include "source/models/model.h"
#include "source/data/data.h"
#include "source/system/event_scheduler.h"
#include "config.hpp"

namespace Thermal
{

    class PhysicalModel : public Model
    {
    public:
        PhysicalModel(EventScheduler* event_scheduler, Data* data);
        ~PhysicalModel();

        void startup();
        void execute(double scheduled_time);

        config::Config* getPhysicalConfig(){ return _physical_config; }

    protected:
        void readFloorplanUnitNamesFromPtrace();
        bool readFloorplanUnitPowerFromPtrace();

        void startupNormalMode();
        void startupPtraceMode();

        void executeNormalMode(double scheduled_time);
        void executePtraceMode(double scheduled_time);

        void setFloorplanUnitNamesInPowerData();

    private:
        config::Config*             _physical_config;
       
        FILE*                       _ptrace_file;
        int                         _n_flp_units;
        std::vector<std::string>    _ptrace_flp_units_names;
        std::vector<double>         _flp_units_power;

        bool                        _ready_to_execute;

    }; // class PhysicalModel

} // namespace Thermal

#endif // __THERMAL_PHYSICAL_MODEL_H__

