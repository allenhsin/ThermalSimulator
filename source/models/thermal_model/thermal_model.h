
#ifndef __THERMAL_THERMAL_MODEL_H__
#define __THERMAL_THERMAL_MODEL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "source/models/model.h"
#include "source/misc/common_types.h"
#include "source/data/data.h"
#include "source/system/event_scheduler.h"
#include "source/models/thermal_model/package.h"
#include "source/models/thermal_model/floorplan.h"
#include "source/models/thermal_model/temperature.h"
#include "source/models/thermal_model/rc_model.h"
#include "config.hpp"

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
        config::Config* getThermalConfig(){ return _thermal_config; }
        void checkVadilityOfThermalParameters();

    private:
        config::Config*     _thermal_config;

        // model packaing (heat sink and fan) parameters
        Package*            _package;
        // construct floorplan data structure
        Floorplan*          _floorplan;
        // construct RC model data structure
        RCModel*            _rc_model;
        // temperature handler class
        Temperature*        _temperature;

        bool                _ready_to_execute;
        bool                _parameter_ready;
        FILE*               _ttrace_file;

    }; // class ThermalModel

} // namespace Thermal

#endif // __THERMAL_THERMAL_MODEL_H__

