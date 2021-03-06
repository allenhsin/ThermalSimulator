
#ifndef __THERMAL_THERMAL_MODEL_H__
#define __THERMAL_THERMAL_MODEL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "source/models/model.h"
#include "source/misc/common_types.h"
#include "source/data/data.h"
#include "source/system/event_scheduler.h"
#include "source/models/thermal_model/floorplan.h"
#include "source/models/thermal_model/temperature.h"
#include "source/models/thermal_model/rc_model.h"

namespace Thermal
{
    class ThermalModel : public Model
    {
    public:
        ThermalModel();
        ~ThermalModel();
        
        virtual void startup();
        virtual void execute(Time scheduled_time);

        virtual std::string getModelName() { return "Thermal Model"; }

    protected:
        void loadConfig();

    private:
        // construct floorplan data structure
        Floorplan*          _floorplan;
        // construct RC model data structure
        RCModel*            _rc_model;
        // temperature handler class
        Temperature*        _temperature;

        FILE*               _ttrace_file;
        double              _sampling_intvl;

    }; // class ThermalModel

} // namespace Thermal

#endif // __THERMAL_THERMAL_MODEL_H__

