
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <string>
#include <math.h>
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
        : _physical_config              (NULL)
        , _ptrace_file                  (NULL)
        , _n_ptrace_flp_units           (0)
        , _ptrace_sampling_interval     (0)
        , _current_ptrace_line_number   (0)
    {
        _ptrace_flp_units_names.clear();
        _ptrace_flp_units_power.clear();
    }

    PowerTraceMode::~PowerTraceMode()
    {}

    void PowerTraceMode::loadFloorplanUnitNamesFromPtrace()
    {
        char    line[LINE_SIZE]; 
        char    temp[LINE_SIZE]; 
        char*   src;
        char    name[STR_SIZE];
        int     i;
        
        assert(_ptrace_flp_units_names.size()==0);
        assert(_ptrace_flp_units_power.size()==0);

        _ptrace_file = fopen(getPhysicalConfig()->getString("ptrace_mode/ptrace_file").c_str(), "r");
        if (!_ptrace_file) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open ptrace file.\n");

        do 
        {
            // read the entire line
            fgets(line, LINE_SIZE, _ptrace_file);
            if (feof(_ptrace_file))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: No flp names in power trace file.\n");
            strcpy(temp, line);
            src = strtok(temp, " \r\t\n");
        // skip empty lines
        } while (!src || src[0]=='#');
    
        // if the ptrace name line is too long
        if(line[strlen(line)-1] != '\n')
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: ptrace flp name line too long.\n");
    
        // chop the names from the line read
        for(i=0,src=line; *src; i++) 
        {
            if(!sscanf(src, "%s", name))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid flp names format in ptrace file.\n");
            src += strlen(name);
            _ptrace_flp_units_names.push_back( (string) name );
            while (isspace((int)*src))
                src++;
        }
       
        assert(_ptrace_flp_units_names.size()== (unsigned int) i);
        _ptrace_flp_units_power.resize(i);
        _n_ptrace_flp_units = i;
    } // loadFloorplanUnitNamesFromPtrace

    bool PowerTraceMode::loadFloorplanUnitPowerFromPtrace()
    {
        char    line[LINE_SIZE];
        char    temp[LINE_SIZE];
        char*   src;
        int     i;

        if (!_ptrace_file) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open ptrace file.\n");
    
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
        } while (!src || src[0]=='#');
    
        // new line not read yet
        if(line[strlen(line)-1] != '\n')
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: ptrace flp name line too long.\n");
    
        // chop the power values from the line read
        for(i=0,src=line; *src && i < _n_ptrace_flp_units; i++) {
            if(!sscanf(src, "%s", temp) || !sscanf(src, "%lf", &_ptrace_flp_units_power[i]))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid number format in ptrace file.\n");
            src += strlen(temp);
            while (isspace((int)*src))
                src++;
        }
        if( (i != _n_ptrace_flp_units) || *src )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Number of units exceeds limit.\n");
    
        return true;

    } // loadFloorplanUnitPowerFromPtrace

    void PowerTraceMode::setFloorplanUnitNamesInPowerData()
    {
        assert(_n_ptrace_flp_units != 0);

        for (int i=0; i<(int)_n_ptrace_flp_units; ++i)
            Data::getSingleton()->addData(ACCUMULATED_ENERGY_DATA, _ptrace_flp_units_names[i], 0);
    }
    
    void PowerTraceMode::startup()
    {
        assert(_physical_config);

        LibUtil::Log::printLine("Startup Power Trace Mode\n");

    // set ptrace constants ---------------------------------------------------
        _ptrace_sampling_interval = getPhysicalConfig()->getFloat("ptrace_mode/sampling_intvl");
        _current_ptrace_line_number = 0;
    // ------------------------------------------------------------------------

    // Read floorplan unit names from power trace name ------------------------
        // read ptrace file
        loadFloorplanUnitNamesFromPtrace();
        // set power name keys
        setFloorplanUnitNamesInPowerData();
    // ------------------------------------------------------------------------

    // Schedule the first physical model execution event ----------------------
        EventScheduler::getSingleton()->enqueueEvent(_ptrace_sampling_interval, PHYSICAL_MODEL);
    // ------------------------------------------------------------------------
    } // startup

    void PowerTraceMode::execute(double scheduled_time)
    {
        assert(_physical_config);
        
        _current_ptrace_line_number++;
        // only execute power trace when the schduled execution time matches the power trace sampling time
        if( !Misc::eqTime(scheduled_time, (_current_ptrace_line_number * _ptrace_sampling_interval)) )
        {
            _current_ptrace_line_number--;
            LibUtil::Log::printLine("Power Trace not Executed\n");
            return;
        }

        LibUtil::Log::printLine("Execute Power Trace\n");
        
        assert(_n_ptrace_flp_units!=0);
        
    // Read single line of power trace ----------------------------------------
        bool valid_line = false;
        valid_line = loadFloorplanUnitPowerFromPtrace();
    // ------------------------------------------------------------------------

    // update main accumulated energy data structure and schedule next event --
        if(valid_line)
        {
            for(int i=0; i<_n_ptrace_flp_units; ++i)
                Data::getSingleton()->setData(  ACCUMULATED_ENERGY_DATA, _ptrace_flp_units_names[i],
                                                (Data::getSingleton()->getData(ACCUMULATED_ENERGY_DATA, _ptrace_flp_units_names[i]) + 
                                                (_ptrace_flp_units_power[i] * _ptrace_sampling_interval))
                                             );

            EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + _ptrace_sampling_interval), PHYSICAL_MODEL);
        }
        else
            EventScheduler::getSingleton()->finish();
    // ------------------------------------------------------------------------
    } // execute

} // namespace Thermal


