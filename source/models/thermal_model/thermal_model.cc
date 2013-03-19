
#include <cassert>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <map>

#include "source/system/event_scheduler.h"
#include "source/models/model_type.h"
#include "source/models/thermal_model/thermal_model.h"
#include "source/models/thermal_model/thermal_parameters.h"
#include "source/models/thermal_model/thermal_constants.h"
#include "source/misc/misc.h"
#include "source/system/simulator.h"
#include "source/system/event_scheduler.h"
#include "source/data/data.h"
#include "libutil/Log.h"
#include "libutil/String.h"

using std::map;
using std::string;

namespace Thermal
{
    ThermalModel::ThermalModel(EventScheduler* event_scheduler, Data* data)
        : Model(event_scheduler, data)
        , _thermal_config       (NULL)
        , _package              (new Package())
        , _floorplan            (new Floorplan())
        , _rc_model             (new RCModel())
        , _temperature          (new Temperature())
        , _ready_to_execute     (false)
        , _parameter_ready      (false)
        , _ttrace_file          (NULL)
    {}

    ThermalModel::~ThermalModel()
    {
        if(_ttrace_file)
            fclose(_ttrace_file);

        if(_package)
            delete _package;

        if(_floorplan)
            delete _floorplan;

        if(_rc_model)
            delete _rc_model;

        if(_temperature)
            delete _temperature;

        if(_parameter_ready)
            ThermalParameters::release();

        if(_thermal_config)
            delete _thermal_config;
    }

    void ThermalModel::checkVadilityOfThermalParameters()
    {
        ThermalParameters* thermal_params = ThermalParameters::getSingleton();

        // check if block model is chosen
        if(thermal_params->model_type.compare(BLOCK)!=0)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Thermal model type not supported\n \
                                                    (Only \"block\" is supported)\n");
        // check if secondary path is false
        if(thermal_params->model_secondary)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Secondary path not supported\n \
                                                    (set model_secondary to false)\n");
        // check if leakage model is false
        if(thermal_params->leakage_used)
        {
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Leakage model not supported\n \
                                                    (set leakage_used to false)\n");
            abort();
        }
    }

    void ThermalModel::startup()
    {
        LibUtil::Log::printLine("Startup Thermal Model\n");

    // Configure Thermal Model ------------------------------------------------
        ThermalParameters* thermal_params;
        
        // get thermal cfg file name
        std::string thermal_config_file =   Simulator::getSingleton()->getConfig()
                                            ->getString("models/thermal_model/thermal_config_file");

        // parse thermal cfg file into config class
        Misc::setConfig(thermal_config_file, _thermal_config, 0, NULL);
        assert(_thermal_config);
        
        // read in all config parameters
        ThermalParameters::allocate(_thermal_config);
        thermal_params = ThermalParameters::getSingleton();
        assert(thermal_params);
        _parameter_ready = true;
        
        // check vadility of thermal parameters
        checkVadilityOfThermalParameters();
        
        // pass thermal parameter by pointer
        _package->setThermalParameters(thermal_params);
        _rc_model->setThermalParameters(thermal_params);
        _temperature->setThermalParameters(thermal_params);
    // ------------------------------------------------------------------------

    // Run Package Model if used ----------------------------------------------
        if( thermal_params->package_model_used )
        {   
            // update r_convec
            _package->updateRConvection( thermal_params->ambient + SMALL_FOR_CONVEC );
            // check r_convec value validity
            if (thermal_params->r_convec<R_CONVEC_LOW || thermal_params->r_convec>R_CONVEC_HIGH)
                LibUtil::Log::printLine(std::cerr, "\nWARNING: Heatsink convection resistance is not realistic, \
                                                   double-check your package settings...\n");
        }
    // ------------------------------------------------------------------------

    // Construct floorplan and RC models --------------------------------------
        
        // read in floorplan from flp file
        _floorplan->readFloorplan(thermal_params->floorplan_file);
        assert(_floorplan->getFloorplanHolder());

        // setup power and temperature floorplan unit names in data
        _floorplan->setFloorplanUnitNmaesInTemperatureData();

        // allocate the RC model
        _rc_model->setFloorplanHolder(_floorplan->getFloorplanHolder());
        _rc_model->allocateRCModelHolder();
        assert(_rc_model->getRCModelHolder());
        // populate RC equivalent circuit
        _rc_model->populateR();
        _rc_model->populateC();
        // precompute LUP decomposition for different time step sizes
        _rc_model->precomputeStepLupDcmp();

        // print rc model to file for debug if specified
        if(thermal_params->debug_print_enable)
            _rc_model->printRCModelToFile();
    // ------------------------------------------------------------------------

    // Setup initial temperature ----------------------------------------------
        
        // set related data holders
        _temperature->setFloorplanHolder(_floorplan->getFloorplanHolder());
        _temperature->setRCModelHolder(_rc_model->getRCModelHolder());

        // set initial temp based on init temp value or init temp file (in config)
        _temperature->setInitialTemperature();
        
        // print ttrace to file for debug if specified
        if(thermal_params->debug_print_enable)
        {
            _ttrace_file = fopen(thermal_params->debug_ttrace_file.c_str(), "w");

            if(!_ttrace_file)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open ttrace file for output\n");

            map<string, double>& data_temperature = Data::getSingleton()->getTemperature();
            assert(data_temperature.size()!=0);
            // print flp unit names
            for(map<string, double>::iterator it = data_temperature.begin(); it != data_temperature.end(); ++it)
                fprintf(_ttrace_file, "%s ", it->first.c_str());
            fprintf(_ttrace_file, "\n");
        }
    // ------------------------------------------------------------------------

    // Schedule the first temperature calculation event -----------------------
        EventScheduler::getSingleton()->enqueueEvent(thermal_params->sampling_intvl, THERMAL_MODEL);
    // ------------------------------------------------------------------------

        _ready_to_execute = true;
    }

    void ThermalModel::execute(double scheduled_time)
    {
        assert(_ready_to_execute);
        LibUtil::Log::printLine("Execute Thermal Model\n");

        ThermalParameters* thermal_params = ThermalParameters::getSingleton();
        
    // Update RC Model if using Natural Convection ----------------------------
        if( thermal_params->package_model_used && thermal_params->natural_convec )
        {   
            double avg_sink_temp;
            // calculate average sink temp
            avg_sink_temp = _temperature->getAvgSinkTemperature();
            // update r_convec based on avg sink temp
            _package->updateRConvection( avg_sink_temp );
            // update RC model
            _rc_model->populateR();
        }
    // ------------------------------------------------------------------------

    // Compute Transient Temperature ------------------------------------------
        _temperature->updateTransientTemperature();

        // print ttrace to file for debug if specified
        if(thermal_params->debug_print_enable)
        {
            if(!_ttrace_file)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open ttrace file for output\n");

            map<string, double>& data_temperature = Data::getSingleton()->getTemperature();
            assert(data_temperature.size()!=0);
            // print flp unit names
            for(map<string, double>::iterator it = data_temperature.begin(); it != data_temperature.end(); ++it)
                fprintf(_ttrace_file, "%.2f ", it->second);
            fprintf(_ttrace_file, "\n");
        }
    // ------------------------------------------------------------------------

    // Schedule next transient temperature calculation event ------------------
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + thermal_params->sampling_intvl), THERMAL_MODEL);
    // ------------------------------------------------------------------------
    }


} // namespace Thermal

