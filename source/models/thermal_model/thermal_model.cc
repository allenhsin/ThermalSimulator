
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
#include "libutil/LibUtil.h"

using std::map;
using std::string;

namespace Thermal
{
    ThermalModel::ThermalModel()
        : Model()
        , _package              (new Package())
        , _floorplan            (new Floorplan())
        , _rc_model             (new RCModel())
        , _temperature          (new Temperature())
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
    }

    void ThermalModel::checkVadilityOfThermalParameters()
    {
        ThermalParameters* thermal_params = ThermalParameters::getSingleton();

        // check if block model is chosen
        if(thermal_params->model_type.compare(BLOCK)!=0)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Thermal model type not supported\n \
                                                    (Only \"block\" is supported).\n");
        // check if secondary path is false
        if(thermal_params->model_secondary)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Secondary path not supported\n \
                                                    (set model_secondary to false).\n");
        // check if leakage model is false
        if(thermal_params->leakage_used)
        {
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Leakage model not supported\n \
                                                    (set leakage_used to false).\n");
            abort();
        }
    }

    void ThermalModel::loadConfig()
    {
        // get thermal cfg file name
        std::string thermal_config_file = Simulator::getSingleton()->getConfig()
                                          ->getString("models/thermal_model/thermal_config_file");

        // parse thermal cfg file into config class
        Misc::setConfig(thermal_config_file, _config, 0, NULL);
        
        // read in all config parameters
        ThermalParameters::allocate(_config);
        ThermalParameters* thermal_params = ThermalParameters::getSingleton();
        assert(thermal_params);
        _parameter_ready = true;
        
        // check vadility of thermal parameters
        checkVadilityOfThermalParameters();
        
        // pass thermal parameter by pointer
        _package->setThermalParameters(thermal_params);
        _rc_model->setThermalParameters(thermal_params);
        _temperature->setThermalParameters(thermal_params);
    }

    void ThermalModel::startup()
    {
        LibUtil::Log::printLine( "Startup " + getModelName() );

    // Configure Thermal Model ------------------------------------------------
        loadConfig();
        ThermalParameters* thermal_params = ThermalParameters::getSingleton();
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
    
    // Load floorplan ---------------------------------------------------------
        // read in floorplan from flp file
        _floorplan->loadFloorplan(thermal_params->floorplan_file, thermal_params->top_flp_object);
        assert(_floorplan->getFloorplanHolder());

        // print parsed floorplan to file for debug if specified
        if(thermal_params->debug_print_enable)
            _floorplan->printParsedFloorplan(thermal_params->debug_flp_file);

        // setup floorplan unit names in temperature data
        _floorplan->setFloorplanUnitNamesInTemperatureData();
        // setup filler floorplan unit names in energy data
        _floorplan->setFillerFloorplanUnitNamesInEnergyData();
    // ------------------------------------------------------------------------

    // Construct RC Model -----------------------------------------------------
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
    // ------------------------------------------------------------------------
        
    // Print temperature trace file -------------------------------------------
        _ttrace_file = fopen(thermal_params->ttrace_file.c_str(), "w");

        if(!_ttrace_file)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open ttrace file for output.\n");

        assert(Data::getSingleton()->getTemperatureDataSize() == (unsigned int) _floorplan->getFloorplanHolder()->_n_units);
        // print flp unit names
        for(int i =0; i<_floorplan->getFloorplanHolder()->_n_units; ++i)
            fprintf(_ttrace_file, "%s ", (_floorplan->getFloorplanHolder()->_flp_units[i]._name).c_str() );
        fprintf(_ttrace_file, "\n");
        // print init temp values
        for(int i =0; i<_floorplan->getFloorplanHolder()->_n_units; ++i)
            fprintf(_ttrace_file, "%.2f ", Data::getSingleton()->getTemperatureData(_floorplan->getFloorplanHolder()->_flp_units[i]._name));
        fprintf(_ttrace_file, "\n");
    // ------------------------------------------------------------------------

    // Schedule the first thermal model execution event -----------------------
        // first update any accumulated energy if necessary
        EventScheduler::getSingleton()->enqueueEvent(thermal_params->sampling_intvl, DEVICE_MANAGER);
        EventScheduler::getSingleton()->enqueueEvent(thermal_params->sampling_intvl, POWER_TRACE_MANAGER);
        // then execute thermal model
        EventScheduler::getSingleton()->enqueueEvent(thermal_params->sampling_intvl, THERMAL_MODEL);
    // ------------------------------------------------------------------------

        _last_execute_time = 0;
        _ready_to_execute = true;
    } // startup

    void ThermalModel::execute(Time scheduled_time)
    {
        if(!_ready_to_execute)
            return;

        if( Misc::eqTime(scheduled_time,_last_execute_time) )
            return;

        LibUtil::Log::printLine( "Execute " + getModelName() );
        
        assert(_config);
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
            // FIXME:   the direct calling of populateR is actually unnecessary
            //          since only part of the R values need to be updated.
            //          but this is not a big problem now because we don't use
            //          natural convection often. Need to fix later though.
            _rc_model->populateR();
        }
    // ------------------------------------------------------------------------

    // Compute Transient Temperature ------------------------------------------
        _temperature->updateTransientTemperature( (scheduled_time - _last_execute_time) );
    // ------------------------------------------------------------------------

    // Print temperature trace file -------------------------------------------
        if(!_ttrace_file)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open ttrace file for output.\n");

        assert(Data::getSingleton()->getTemperatureDataSize() == (unsigned int) _floorplan->getFloorplanHolder()->_n_units);
        // print flp unit names
        for(int i =0; i<_floorplan->getFloorplanHolder()->_n_units; ++i)
            fprintf(_ttrace_file, "%.2f ", Data::getSingleton()->getTemperatureData(_floorplan->getFloorplanHolder()->_flp_units[i]._name));
        fprintf(_ttrace_file, "\n");
    // ------------------------------------------------------------------------

    // Clear the accumulated energy in the data structure ---------------------
        assert(Data::getSingleton()->getEnergyDataSize() == (unsigned int) _floorplan->getFloorplanHolder()->_n_units);
        for(int i =0; i<_floorplan->getFloorplanHolder()->_n_units; ++i)
            Data::getSingleton()->setEnergyData(_floorplan->getFloorplanHolder()->_flp_units[i]._name, 0);
    // ------------------------------------------------------------------------

    // Schedule next thermal model execution event ----------------------------
        // first update any accumulated energy if necessary
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + thermal_params->sampling_intvl), DEVICE_MANAGER );
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + thermal_params->sampling_intvl), POWER_TRACE_MANAGER );
        // then execute thermal model
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + thermal_params->sampling_intvl), THERMAL_MODEL );
    // ------------------------------------------------------------------------

        _last_execute_time = scheduled_time;
    } // execute

} // namespace Thermal

