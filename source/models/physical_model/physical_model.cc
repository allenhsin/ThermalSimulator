
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <ctype.h>
#include <string>

#include "source/models/physical_model/physical_model.h"
#include "source/models/physical_model/physical_constants.h"
#include "source/models/model_type.h"
#include "source/system/simulator.h"
#include "source/system/event_scheduler.h"
#include "source/misc/misc.h"
#include "source/data/data.h"
#include "libutil/Log.h"
#include "libutil/String.h"

using std::vector;
using std::string;

namespace Thermal
{
    PhysicalModel::PhysicalModel(EventScheduler* event_scheduler, Data* data)
        : Model(event_scheduler, data)
        , _physical_config          (NULL)
        , _ptrace_file              (NULL)
        , _n_flp_units              (0)
        , _ready_to_execute         (false)
    {
        _ptrace_flp_units_names.clear();
        _flp_units_power.clear();
    }

    PhysicalModel::~PhysicalModel()
    {

        if(_physical_config)
            delete _physical_config;
    }
    
    void PhysicalModel::readFloorplanUnitNamesFromPtrace()
    {
        char    line[LINE_SIZE]; 
        char    temp[LINE_SIZE]; 
        char*   src;
        char    name[STR_SIZE];
        int     i;
        
        assert(_ptrace_flp_units_names.size()==0);
        assert(_flp_units_power.size()==0);
        _ptrace_flp_units_names.resize(MAX_FLP_UNITS);

        _ptrace_file = fopen(getPhysicalConfig()->getString("env_setup/ptrace_file").c_str(), "r");
        if (!_ptrace_file) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open ptrace file.\n");

        // skip empty lines
        do 
        {
            // read the entire line
            fgets(line, LINE_SIZE, _ptrace_file);
            if (feof(_ptrace_file))
                LibUtil::Log::printFatalLine(std::cerr, "ERROR: no flp names in power trace file");
            strcpy(temp, line);
            src = strtok(temp, " \r\t\n");
        } while (!src);
    
        // new line not read yet
        if(line[strlen(line)-1] != '\n')
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: ptrace flp name line too long");
    
        // chop the names from the line read
        for(i=0,src=line; *src && i < MAX_FLP_UNITS; i++) 
        {
            if(!sscanf(src, "%s", name))
                LibUtil::Log::printFatalLine(std::cerr, "ERROR: invalid flp names format in ptrace file");
            src += strlen(name);
            _ptrace_flp_units_names[i] = (string) name;
            while (isspace((int)*src))
                src++;
        }

        if(*src && i == MAX_FLP_UNITS)
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: number of units exceeds limit");
        
        _ptrace_flp_units_names.resize(i);
        _flp_units_power.resize(i);
        _n_flp_units = i;
    } // readFloorplanUnitNamesFromPtrace

    bool PhysicalModel::readFloorplanUnitPowerFromPtrace()
    {
        char    line[LINE_SIZE];
        char    temp[LINE_SIZE];
        char*   src;
        int     i;


        if (!_ptrace_file) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open ptrace file.\n");
    
        // skip empty lines
        do 
        {
            // read the entire line
            fgets(line, LINE_SIZE, _ptrace_file);
            if (feof(_ptrace_file))
            {
                fclose(_ptrace_file);
                return false;
            }
            strcpy(temp, line);
            src = strtok(temp, " \r\t\n");
        } while (!src);
    
        // new line not read yet
        if(line[strlen(line)-1] != '\n')
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: ptrace flp name line too long");
    
        // chop the power values from the line read
        for(i=0,src=line; *src && i < _n_flp_units; i++) {
            if(!sscanf(src, "%s", temp) || !sscanf(src, "%lf", &_flp_units_power[i]))
                LibUtil::Log::printFatalLine(std::cerr, "ERROR: invalid flp names format in ptrace file");
            src += strlen(temp);
            while (isspace((int)*src))
                src++;
        }
        if( (i != _n_flp_units) || *src )
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: number of units exceeds limit");
    
        return true;

    } // readFloorplanUnitPowerFromPtrace

    void PhysicalModel::setFloorplanUnitNamesInPowerData()
    {
        assert(_n_flp_units != 0);
        
        map<string, double>& power = Data::getSingleton()->getPower();
        assert(power.size() == 0);

        for (int i=0; i<(int)_n_flp_units; ++i)
            power[ _ptrace_flp_units_names[i] ] = 0;
    }
    
    void PhysicalModel::startupNormalMode()
    {
        LibUtil::Log::printLine("Entering Normal Mode\n");
    }

    void PhysicalModel::startupPtraceMode()
    {
        LibUtil::Log::printLine("Entering Power Trace Mode\n");

    // Read floorplan unit names from power trace name ------------------------
        // read ptrace file
        readFloorplanUnitNamesFromPtrace();
        // set power name keys
        setFloorplanUnitNamesInPowerData();
    // ------------------------------------------------------------------------
    }

    void PhysicalModel::executeNormalMode(double scheduled_time)
    {
        LibUtil::Log::printLine("Entering Normal Mode\n");

    // Schedule next physical model calculation event -------------------------
    // ------------------------------------------------------------------------
    }

    void PhysicalModel::executePtraceMode(double scheduled_time)
    {
        LibUtil::Log::printLine("Entering Power Trace Mode\n");
        
        assert(_n_flp_units!=0);
        map<string, double>& power = Data::getSingleton()->getPower();
        assert(power.size() == (unsigned int) _n_flp_units);
        
    // Read single line of power trace ----------------------------------------
        bool valid_line = false;
        valid_line = readFloorplanUnitPowerFromPtrace();
    // ------------------------------------------------------------------------

    // update main power data structure and schedule next event ---------------
        if(valid_line)
        {
            for(int i=0; i<_n_flp_units; ++i)
                power[ _ptrace_flp_units_names[i] ] = _flp_units_power[i];
            assert(power.size() == (unsigned int) _n_flp_units);

            EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + getPhysicalConfig()->getFloat("env_setup/sampling_intvl")), PHYSICAL_MODEL);
        }
        else
            EventScheduler::getSingleton()->finish();
    // ------------------------------------------------------------------------
    }


    void PhysicalModel::startup()
    {
        LibUtil::Log::printLine("Startup Physical Model");

    // Configure Physical Model -----------------------------------------------
        // get physical cfg file name
        std::string physical_config_file =  Simulator::getSingleton()->getConfig()
                                            ->getString("models/physical_model/physical_config_file");

        // parse thermal cfg file into config class
        Misc::setConfig(physical_config_file, _physical_config, 0, NULL);
        assert(_physical_config);
    // ------------------------------------------------------------------------

    // Mode Selection ---------------------------------------------------------
        if(!getPhysicalConfig()->getBool("env_setup/ptrace_mode_enable"))
            startupNormalMode();
        else
            startupPtraceMode();
    // ------------------------------------------------------------------------

    // Schedule the first physical model calculation event --------------------
        EventScheduler::getSingleton()->enqueueEvent(getPhysicalConfig()->getFloat("env_setup/sampling_intvl"), PHYSICAL_MODEL);
    // ------------------------------------------------------------------------

        _ready_to_execute = true;
    } // startup
    
    void PhysicalModel::execute(double scheduled_time)
    {
        assert(_ready_to_execute);
        assert(_physical_config);

        LibUtil::Log::printLine("Execute Physical Model");

    // Mode Selection ---------------------------------------------------------
        if(!getPhysicalConfig()->getBool("env_setup/ptrace_mode_enable"))
            executeNormalMode(scheduled_time);
        else
            executePtraceMode(scheduled_time);
    // ------------------------------------------------------------------------

    } // execute

} // namespace Thermal

