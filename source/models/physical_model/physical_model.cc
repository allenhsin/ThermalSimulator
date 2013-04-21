
#include <cassert>
#include <stddef.h>
#include <string>

#include "source/models/physical_model/physical_model.h"
#include "source/system/simulator.h"
#include "source/misc/misc.h"
#include "source/data/data.h"
#include "libutil/LibUtil.h"

using std::map;
using std::string;

namespace Thermal
{
    PhysicalModel::PhysicalModel()
        : Model()
    { _floorplan_unit_power.clear(); }

    PhysicalModel::~PhysicalModel()
    {}

    void PhysicalModel::loadConfig()
    {
        // get physical cfg file name
        std::string physical_config_file =  Simulator::getSingleton()->getConfig()
                                            ->getString("models/physical_model/physical_config_file");

        // parse physical cfg file into config class
        Misc::setConfig(physical_config_file, _config, 0, NULL);
        assert(_config);
    }
    
    void PhysicalModel::startup()
    {
        LibUtil::Log::printLine( "Startup " + getModelName() );
        
        loadConfig();

        // startup manager and initialize floorplan units
        // controlled by this manager
        bool manager_enabled = startupManager();
        if(!manager_enabled)
            return;
        
        // put floorplan unit names into energy data
        addEnergyData();

        _ready_to_execute = true;
        _last_execute_time = 0;
    }

    void PhysicalModel::execute(Time scheduled_time)
    {
        assert(_config);

        if(!_ready_to_execute)
            return;
        
        if( (scheduled_time==_last_execute_time) && (_last_execute_time!=0) )
            return;

        LibUtil::Log::printLine( "Execute " + getModelName() );
    
        // update the accumulated energy with the power values
        // since the last update
        setEnergyData( (scheduled_time - _last_execute_time) );
        
        // then execute the physical model manager to update
        // power to its current values
        executeManager(scheduled_time);

        _last_execute_time = scheduled_time;
    } // execute
    
    void PhysicalModel::addEnergyData()
    {
        for( map<string, double>::iterator it = _floorplan_unit_power.begin(); it != _floorplan_unit_power.end(); ++it )
            Data::getSingleton()->addEnergyData(it->first, 0);
    }

    void PhysicalModel::setEnergyData(Time time_elapsed_since_last_update)
    {
        for( map<string, double>::iterator it = _floorplan_unit_power.begin(); it != _floorplan_unit_power.end(); ++it )
            Data::getSingleton()->setEnergyData( it->first, ((time_elapsed_since_last_update * it->second) + Data::getSingleton()->getEnergyData(it->first)) );
    }

    bool PhysicalModel::hasFloorplanUnit(std::string floorplan_unit_name)
    { return _floorplan_unit_power.count(floorplan_unit_name); }

    void PhysicalModel::addFloorplanUnit(std::string floorplan_unit_name)
    {   
        if(hasFloorplanUnit(floorplan_unit_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Floorplan unit \"" + floorplan_unit_name + "\" already exists.\n");
            
        _floorplan_unit_power[floorplan_unit_name] = 0;
    }

    void PhysicalModel::setFloorplanUnitPower(std::string floorplan_unit_name, double floorplan_unit_power)
    {
        if(!hasFloorplanUnit(floorplan_unit_name))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Floorplan unit \"" + floorplan_unit_name + "\" does not exist.\n");
            
        _floorplan_unit_power[floorplan_unit_name] = floorplan_unit_power;
    }

} // namespace Thermal

