
#ifndef __THERMAL_THERMAL_MODEL_H__
#define __THERMAL_THERMAL_MODEL_H__

#include "source/models/model.h"
#include "source/models/thermal_model/package.h"
#include "source/models/thermal_model/floorplan.h"
#include "config.hpp"

namespace Thermal
{
    class ThermalModel : public Model
    {
    public:
        ThermalModel();
        ~ThermalModel();
        
        void startup();
        void execute(double scheduled_time);

        config::Config* getThermalConfig(){ return _thermal_config; }

    protected:
        void checkVadilityOfThermalParameters();

    private:
        config::Config*     _thermal_config;
        
        // model packaing (heat sink and fan) parameters
        Package*            _package;
        // construct floorplan data structure
        Floorplan*          _floorplan;
        // construct RC model data structure
        RCModel*            _rc_model;

        bool                _ready_to_execute;
        bool                _parameter_ready;

    }; // class ThermalModel

} // namespace Thermal

#endif // __THERMAL_THERMAL_MODEL_H__

