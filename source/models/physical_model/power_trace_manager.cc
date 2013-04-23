
#include <cassert>
#include <stddef.h>
#include <cstring>
#include <string>

#include "source/models/physical_model/power_trace_manager.h"
#include "source/models/physical_model/physical_constants.h"
#include "source/system/event_scheduler.h"
#include "source/models/model_type.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

using std::string;

namespace Thermal
{
    PowerTraceManager::PowerTraceManager()
        : PhysicalModel()
        , _ptrace_file                  (NULL)
        , _ptrace_file_read_over        (false)
        , _ptrace_sampling_interval     (0)
        , _current_ptrace_line_number   (0)
    {
        _ptrace_flp_units_names.clear();
    }

    PowerTraceManager::~PowerTraceManager()
    {}

    void PowerTraceManager::loadFloorplanUnitNamesFromPtrace()
    {
        char    line[LINE_SIZE]; 
        char    temp[LINE_SIZE]; 
        char*   src;
        char    name[STR_SIZE];
        int     i;
        
        assert(_ptrace_flp_units_names.size()==0);
        assert(getFloorplanUnitSize()==0);

        _ptrace_file = fopen(_config->getString("ptrace_manager/ptrace_file").c_str(), "r");
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

            if(hasFloorplanUnit( (string) name ))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Duplicated flp names in ptrace file.\n");
            
            _ptrace_flp_units_names.push_back( (string) name );
            addFloorplanUnit( (string) name );
            while (isspace((int)*src))
                src++;
        }
        assert(_ptrace_flp_units_names.size()== (unsigned int) i);
    } // loadFloorplanUnitNamesFromPtrace

    void PowerTraceManager::loadFloorplanUnitPowerFromPtrace()
    {
        char    line[LINE_SIZE];
        char    temp[LINE_SIZE];
        char*   src;
        double  power_value;
        unsigned int i;
        
        const unsigned int number_units = _ptrace_flp_units_names.size();
        
        if (_ptrace_file_read_over)
            return;

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
                _ptrace_file_read_over = true;
                return;
            }
            strcpy(temp, line);
            src = strtok(temp, " \r\t\n");
        } while (!src || src[0]=='#');
    
        // new line not read yet
        if(line[strlen(line)-1] != '\n')
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: ptrace flp name line too long.\n");
    
        // chop the power values from the line read
        for(i=0,src=line; *src && i < number_units; i++) {
            if(!sscanf(src, "%s", temp) || !sscanf(src, "%lf", &power_value))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid number format in ptrace file.\n");
            setFloorplanUnitPower(_ptrace_flp_units_names[i], power_value);
            src += strlen(temp);
            while (isspace((int)*src))
                src++;
        }
        if( (i != number_units) || *src )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Number of units exceeds limit.\n");
    
        return;

    } // loadFloorplanUnitPowerFromPtrace

    bool PowerTraceManager::startupManager()
    {
        assert(_config);

    // check if manager is enabled --------------------------------------------
        if(!_config->getBool("ptrace_manager/enable"))
        {
            LibUtil::Log::printLine( "    Power Trace Manager not enabled" );
            return false;
        }
    // ------------------------------------------------------------------------

    // set ptrace constants ---------------------------------------------------
        _ptrace_sampling_interval = _config->getFloat("ptrace_manager/sampling_intvl");
        _current_ptrace_line_number = 0;
    // ------------------------------------------------------------------------

    // Read floorplan unit names from power trace name ------------------------
        // read ptrace file
        loadFloorplanUnitNamesFromPtrace();
    // ------------------------------------------------------------------------

    // Schedule the first physical model execution event ----------------------
        EventScheduler::getSingleton()->enqueueEvent(0, POWER_TRACE_MANAGER);
    // ------------------------------------------------------------------------
        
        return true;
    } // startup

    bool PowerTraceManager::executeManager(Time scheduled_time)
    {
        _current_ptrace_line_number++;
        // only execute power trace when the schduled execution time matches the power trace sampling time
        if( !Misc::eqTime(scheduled_time, ( (_current_ptrace_line_number-1) * _ptrace_sampling_interval)) )
        {
            _current_ptrace_line_number--;
            LibUtil::Log::printLine("    Power Trace new line not read");
            return false;
        }
        
    // Read single line of power trace ----------------------------------------
        loadFloorplanUnitPowerFromPtrace();
    // ------------------------------------------------------------------------

    // Schedule the next ptrace event -----------------------------------------
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + _ptrace_sampling_interval), POWER_TRACE_MANAGER);
    // ------------------------------------------------------------------------
        
        return true;
    } // execute

} // namespace Thermal

