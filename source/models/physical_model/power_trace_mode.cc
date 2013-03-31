
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <string>
#include <map>

#include "source/models/physical_model/power_trace_mode.h"
#include "source/models/physical_model/physical_constants.h"
#include "source/system/event_scheduler.h"
#include "source/models/model_type.h"
#include "source/misc/misc.h"
#include "source/data/data.h"
#include "libutil/LibUtil.h"

using std::map;
using std::string;

namespace Thermal
{
    PowerTraceMode::PowerTraceMode()
        : _physical_config          (NULL)
        , _ptrace_file              (NULL)
        , _n_ptrace_flp_units       (0)
    {
        _ptrace_flp_units_names.clear();
        _ptrace_flp_units_power.clear();
    }

    PowerTraceMode::~PowerTraceMode()
    {}

    void PowerTraceMode::readFloorplanUnitNamesFromPtrace()
    {
        char    line[LINE_SIZE]; 
        char    temp[LINE_SIZE]; 
        char*   src;
        char    name[STR_SIZE];
        int     i;
        
        assert(_ptrace_flp_units_names.size()==0);
        assert(_ptrace_flp_units_power.size()==0);

        _ptrace_file = fopen(getPhysicalConfig()->getString("env_setup/ptrace_file").c_str(), "r");
        if (!_ptrace_file) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open ptrace file.\n");

        do 
        {
            // read the entire line
            fgets(line, LINE_SIZE, _ptrace_file);
            if (feof(_ptrace_file))
                LibUtil::Log::printFatalLine(std::cerr, "ERROR: no flp names in power trace file");
            strcpy(temp, line);
            src = strtok(temp, " \r\t\n");
        // skip empty lines
        } while (!src);
    
        // if the ptrace name line is too long
        if(line[strlen(line)-1] != '\n')
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: ptrace flp name line too long");
    
        // chop the names from the line read
        for(i=0,src=line; *src; i++) 
        {
            if(!sscanf(src, "%s", name))
                LibUtil::Log::printFatalLine(std::cerr, "ERROR: invalid flp names format in ptrace file");
            src += strlen(name);
            _ptrace_flp_units_names.push_back( (string) name );
            while (isspace((int)*src))
                src++;
        }
       
        assert(_ptrace_flp_units_names.size()== (unsigned int) i);
        _ptrace_flp_units_power.resize(i);
        _n_ptrace_flp_units = i;
    } // readFloorplanUnitNamesFromPtrace

    bool PowerTraceMode::readFloorplanUnitPowerFromPtrace()
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
        for(i=0,src=line; *src && i < _n_ptrace_flp_units; i++) {
            if(!sscanf(src, "%s", temp) || !sscanf(src, "%lf", &_ptrace_flp_units_power[i]))
                LibUtil::Log::printFatalLine(std::cerr, "ERROR: invalid flp names format in ptrace file");
            src += strlen(temp);
            while (isspace((int)*src))
                src++;
        }
        if( (i != _n_ptrace_flp_units) || *src )
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: number of units exceeds limit");
    
        return true;

    } // readFloorplanUnitPowerFromPtrace

    void PowerTraceMode::setFloorplanUnitNamesInPowerData()
    {
        assert(_n_ptrace_flp_units != 0);
        
        map<string, double>& power = Data::getSingleton()->getPower();
        assert(power.size() == 0);

        for (int i=0; i<(int)_n_ptrace_flp_units; ++i)
            power[ _ptrace_flp_units_names[i] ] = 0;
    }
    
    void PowerTraceMode::startup()
    {
        assert(_physical_config);

        LibUtil::Log::printLine("Entering Power Trace Mode\n");

    // Read floorplan unit names from power trace name ------------------------
        // read ptrace file
        readFloorplanUnitNamesFromPtrace();
        // set power name keys
        setFloorplanUnitNamesInPowerData();
    // ------------------------------------------------------------------------
    }

    void PowerTraceMode::execute(double scheduled_time)
    {
        assert(_physical_config);

        LibUtil::Log::printLine("Entering Power Trace Mode\n");
        
        assert(_n_ptrace_flp_units!=0);
        map<string, double>& power = Data::getSingleton()->getPower();
        assert(power.size() == (unsigned int) _n_ptrace_flp_units);
        
    // Read single line of power trace ----------------------------------------
        bool valid_line = false;
        valid_line = readFloorplanUnitPowerFromPtrace();
    // ------------------------------------------------------------------------

    // update main power data structure and schedule next event ---------------
        if(valid_line)
        {
            for(int i=0; i<_n_ptrace_flp_units; ++i)
                power[ _ptrace_flp_units_names[i] ] = _ptrace_flp_units_power[i];
            assert(power.size() == (unsigned int) _n_ptrace_flp_units);

            EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + getPhysicalConfig()->getFloat("env_setup/sampling_intvl")), PHYSICAL_MODEL);
        }
        else
            EventScheduler::getSingleton()->finish();
    // ------------------------------------------------------------------------
    }

} // namespace Thermal

