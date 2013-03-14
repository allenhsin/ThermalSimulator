
#include <cassert>
#include <stddef.h>
#include <stdio.h>
#include <string>

#include "source/models/thermal_model/thermal_model.h"
#include "source/models/thermal_model/thermal_parameters.h"
#include "source/models/thermal_model/thermal_constants.h"
#include "source/misc/misc.h"
#include "source/system/simulator.h"
#include "libutil/Log.h"

namespace Thermal
{
    ThermalModel::ThermalModel(EventScheduler* event_scheduler, Data* data)
        : Model(event_scheduler, data)
        , _thermal_config       (NULL)
        , _package              (new Package())
        , _floorplan            (new Floorplan())
        , _rc_model             (new RCModel())
        , _ready_to_execute     (false)
        , _parameter_ready      (false)
    {}

    ThermalModel::~ThermalModel()
    {
        if(_package)
            delete _package;

        if(_floorplan)
            // this will also delete _floorplan_holder
            delete _floorplan;

        if(_parameter_ready)
            ThermalParameters::release();

        if(_thermal_config)
            delete _thermal_config;
    }

    void ThermalModel::checkVadilityOfThermalParameters()
    {
        ThermalParameters* thermal_params = ThermalParameters::getSingleton;
        assert(thermal_params);

        // check if block model is chosen
        if(thermal_params->model_type.compare(BLOCK)!=0)
        {
            LibUtil::Log::printLine(stderr, "\nThermal model type not supported\n
                                               (Only \"block\" is supported)\n");
            abort();
        }
        // check if secondary path is false
        if(thermal_params->model_secondary)
        {
            LibUtil::Log::printLine(stderr, "\nSecondary path not supported\n
                                               (set model_secondary to false)\n");
            abort();
        }
        // check if leakage model is false
        if(thermal_params->leakage_used)
        {
            LibUtil::Log::printLine(stderr, "\nLeakage model not supported\n
                                               (set leakage_used to false)\n");
            abort();
        }
    }

    void ThermalModel::startup()
    {

    // Configure Thermal Model ------------------------------------------------
        ThermalParameters* thermal_params;
        
        // get thermal cfg file name
        std::string thermal_config_file =   Simulator::getSingleton->getConfig()->
                                            getString("models/thermal_model/thermal_config_file");

        // parse thermal cfg file into config class
        Misc::setConfig(thermal_config_file, _thermal_config, 0, NULL);
        assert(_thermal_config);
        
        // read in all config parameters
        ThermalParameters::allocate(_thermal_config);
        assert(ThermalParameters::getSingleton);
        _parameter_ready = true;
        
        // check vadility of thermal parameters
        checkVadilityOfThermalParameters();
        
        // pass thermal parameter by pointer
        thermal_params = ThermalParameters::getSingleton;
        _package->setThermalParameters(thermal_params);
        _rc_model->setThermalParameters(thermal_params);
    // ------------------------------------------------------------------------

    // Run Package Model if used ----------------------------------------------
        if( thermal_params->package_model_used )
        {   
            // update r_convec
            _package->updateRConvection( thermal_params->ambient + SMALL_FOR_CONVEC );
            // check r_convec value validity
            if (thermal_params->r_convec<R_CONVEC_LOW || thermal_params->r_convec>R_CONVEC_HIGH)
                LibUtil::Log::printLine(stderr, "\nWARNING: Heatsink convection resistance is not realistic, 
                                                   double-check your package settings...\n");
        }
    // ------------------------------------------------------------------------

    // Construct floorplan and RC models --------------------------------------
        
        // read in floorplan from flp file
        _floorplan->readFloorplan(thermal_params->floorplan_file);
        assert(_floorplan->getFloorplanHolder());

        // allocate the RC model
        _rc_model->setFloorplanHolder(_floorplan->getFloorplanHolder());
        _rc_model->allocateRCModelHolder();
        assert(getRCModelHolder());
        // populate RC equivalent circuit
        _rc_model->populateR();
        _rc_model->populateC();
        // precompute LUP decomposition for different time step sizes
        _rc_model->precomputeStepLupDcmp();


    // ------------------------------------------------------------------------
    
    
    
    

        _ready_to_execute = true;
    }

    void ThermalModel::execute(double scheduled_time)
    {
        assert(_ready_to_execute);
    }






} // namespace Thermal
