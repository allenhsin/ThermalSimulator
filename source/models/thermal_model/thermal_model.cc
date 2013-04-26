
#include <cassert>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <map>

#include "source/system/event_scheduler.h"
#include "source/models/model_type.h"
#include "source/models/thermal_model/thermal_model.h"
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
        , _floorplan            (new Floorplan())
        , _rc_model             (new RCModel())
        , _temperature          (new Temperature())
        , _ttrace_file          (NULL)
        , _sampling_intvl       (0)
    {}

    ThermalModel::~ThermalModel()
    {
        if(_ttrace_file)
            fclose(_ttrace_file);

        delete _floorplan;
        delete _rc_model;
        delete _temperature;
    }

    void ThermalModel::loadConfig()
    {
        // get thermal cfg file name
        std::string thermal_config_file = Simulator::getSingleton()->getConfig()
                                          ->getString("models/thermal_model/thermal_config_file");

        // parse thermal cfg file into config class
        Misc::setConfig(thermal_config_file, _config, 0, NULL);
        assert(_config);

        // setup local config variables
        _sampling_intvl = _config->getFloat("env_setup/sampling_intvl");
    }

    void ThermalModel::startup()
    {
        LibUtil::Log::printLine( "Startup " + getModelName() );

    // Configure Thermal Model ------------------------------------------------
        loadConfig();
    // ------------------------------------------------------------------------

    // Load floorplan ---------------------------------------------------------
        // read in floorplan from flp file
        _floorplan->loadFloorplan( _config->getString("floorplan/floorplan_file"), _config->getString("floorplan/top_flp_object") );
        assert(_floorplan->getFloorplanHolder());

        // print parsed floorplan to file for debug if specified
        if( _config->getBool("debug/debug_print_enable") )
            _floorplan->printParsedFloorplan( _config->getString("debug/debug_flp_file") );

        // setup floorplan unit names in temperature data
        _floorplan->setFloorplanUnitNamesInTemperatureData();
        // setup filler floorplan unit names in energy data
        _floorplan->setFillerFloorplanUnitNamesInEnergyData();
    // ------------------------------------------------------------------------

    // Construct RC Model -----------------------------------------------------
        // allocate the RC model
        _rc_model->setFloorplanHolder(_floorplan->getFloorplanHolder());
        _rc_model->allocateRCModelHolder( _config->getString("chip/process_file"), _sampling_intvl );
        assert(_rc_model->getRCModelHolder());
        // populate RC equivalent circuit
        _rc_model->populateR( _config->getFloat("chip/r_convec") );
        _rc_model->populateC( _config->getFloat("chip/c_convec") );
        // precompute LUP decomposition for different time step sizes
        _rc_model->precomputeStepLupDcmp();

        // print rc model to file for debug if specified
        if( _config->getBool("debug/debug_print_enable") )
            _rc_model->printRCModelToFile( _config->getString("debug/debug_model_file") );
    // ------------------------------------------------------------------------

    // Setup initial temperature ----------------------------------------------
        // set related data holders and variables
        _temperature->setFloorplanHolder(_floorplan->getFloorplanHolder());
        _temperature->setRCModelHolder(_rc_model->getRCModelHolder());
        _temperature->setSamplingInterval( _sampling_intvl );
        _temperature->setTtraceFileName( _config->getString("env_setup/ttrace_file") );

        // set initial temperature and initialize temperature model
        _temperature->initialize( _config->getFloat("chip/init_temp"), _config->getFloat("chip/ambient_temp") );
    // ------------------------------------------------------------------------
        
    // Schedule the first thermal model execution event -----------------------
        // first update any accumulated energy if necessary
        EventScheduler::getSingleton()->enqueueEvent(_sampling_intvl, DEVICE_MANAGER);
        EventScheduler::getSingleton()->enqueueEvent(_sampling_intvl, POWER_TRACE_MANAGER);
        // then execute thermal model
        EventScheduler::getSingleton()->enqueueEvent(_sampling_intvl, THERMAL_MODEL);
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

        assert(_config);
        LibUtil::Log::printLine( "Execute " + getModelName() );
        
    // Compute Transient Temperature ------------------------------------------
        Time time_since_last_update = scheduled_time - _last_execute_time;
        _temperature->updateTransientTemperature(time_since_last_update);
    // ------------------------------------------------------------------------

    // Clear the accumulated energy in the data structure ---------------------
        assert(Data::getSingleton()->getEnergyDataSize() == (unsigned int) _floorplan->getFloorplanHolder()->_n_units);
        for(int i =0; i<_floorplan->getFloorplanHolder()->_n_units; ++i)
            Data::getSingleton()->setEnergyData(_floorplan->getFloorplanHolder()->_flp_units[i]._name, 0);
    // ------------------------------------------------------------------------

    // Schedule next thermal model execution event ----------------------------
        // first update any accumulated energy if necessary
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + _sampling_intvl), DEVICE_MANAGER );
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + _sampling_intvl), POWER_TRACE_MANAGER );
        // then execute thermal model
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + _sampling_intvl), THERMAL_MODEL );
    // ------------------------------------------------------------------------

        _last_execute_time = scheduled_time;
    } // execute

} // namespace Thermal

