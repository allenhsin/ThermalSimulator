
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <string>
#include <math.h>

#include "source/models/performance_model/link_activity_trace_manager.h"
#include "source/models/performance_model/performance_constants.h"
#include "source/models/performance_model/bit_sequence.h"
#include "source/models/performance_model/random_bit_sequence.h"
#include "source/system/event_scheduler.h"
#include "source/models/model_type.h"
#include "source/misc/misc.h"
#include "source/data/data.h"
#include "libutil/LibUtil.h"

using std::string;

namespace Thermal
{
    LinkActivityTraceManager::LinkActivityTraceManager()
        : _performance_config           (NULL)
        , _latrace_file                 (NULL)
        , _n_bit_sequence_drivers       (0)
        , _latrace_sampling_interval    (0)
        , _current_latrace_line_number  (0)
        , _ready_to_execute             (false)
        , _latrace_file_read_over       (false)
    {
        _bit_sequence_driver_names.clear();
        _bit_sequence_driver_names_set.clear();
        _bit_sequence_drver_activity_factor.clear();
        _bit_sequence_drver_bit_ratio.clear();
    }

    LinkActivityTraceManager::~LinkActivityTraceManager()
    {}

    void LinkActivityTraceManager::loadBitSequenceDriverNamesFromLatrace()
    {
        char    line[LINE_SIZE]; 
        char    temp[LINE_SIZE]; 
        char*   src;
        char    name[STR_SIZE];
        int     i;
        
        assert(_bit_sequence_driver_names.size()==0);
        assert(_bit_sequence_driver_names_set.size()==0);
        assert(_bit_sequence_drver_activity_factor.size()==0);
        assert(_bit_sequence_drver_bit_ratio.size()==0);

        _latrace_file = fopen(getPerformanceConfig()->getString("latrace_manager/latrace_file").c_str(), "r");
        if (!_latrace_file) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open latrace file.\n");

        do 
        {
            // read the entire line
            fgets(line, LINE_SIZE, _latrace_file);
            if (feof(_latrace_file))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: No bit sequence driver names in link activity trace file.\n");
            strcpy(temp, line);
            src = strtok(temp, " \r\t\n");
        // skip empty lines
        } while (!src || src[0]=='#');
    
        // if the latrace name line is too long
        if(line[strlen(line)-1] != '\n')
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: latrace bit sequence driver name line too long.\n");
    
        // chop the names from the line read
        for(i=0,src=line; *src; i++) 
        {
            if(!sscanf(src, "%s", name))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid bit sequence driver names format in latrace file.\n");
            src += strlen(name);

            if(_bit_sequence_driver_names_set.count( (string) name ))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Duplicated bit sequence driver names in latrace file.\n");
            
            _bit_sequence_driver_names.push_back( (string) name );
            _bit_sequence_driver_names_set.insert( (string) name );
            while (isspace((int)*src))
                src++;
        }
       
        assert(_bit_sequence_driver_names.size()== (unsigned int) i);
        _bit_sequence_drver_activity_factor.resize(i);
        _bit_sequence_drver_bit_ratio.resize(i);
        _n_bit_sequence_drivers = i;
    } // loadBitSequenceDriverNamesFromLatrace

    bool LinkActivityTraceManager::loadBitSequenceDriverActivityFromLatrace()
    {
        char    line[LINE_SIZE];
        char    temp[LINE_SIZE];
        char*   src;
        int     i;
        
        if (_latrace_file_read_over)
            return false;

        if (!_latrace_file) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open latrace file.\n");
    
        // skip empty lines
        do 
        {
            // read the entire line
            fgets(line, LINE_SIZE, _latrace_file);
            if (feof(_latrace_file))
            {
                fclose(_latrace_file);
                _latrace_file_read_over = true;
                return false;
            }
            strcpy(temp, line);
            src = strtok(temp, " \r\t\n");
        } while (!src || src[0]=='#');
    
        // new line not read yet
        if(line[strlen(line)-1] != '\n')
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: latrace bit sequence driver name line too long.\n");
    
        // chop the activity values from the line read
        for(i=0,src=line; *src && i < _n_bit_sequence_drivers; i++) {
            if(!sscanf(src, "%s", temp) || !sscanf(src, "%lf-%lf", &_bit_sequence_drver_activity_factor[i], &_bit_sequence_drver_bit_ratio[i]))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid activity format in latrace file.\n");
            src += strlen(temp);
            while (isspace((int)*src))
                src++;
        }
        if( (i != _n_bit_sequence_drivers) || *src )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Number of units exceeds limit.\n");
    
        return true;
    }

    void LinkActivityTraceManager::setBitSequenceDriverNamesInBitSequenceData()
    {
        for(int i=0; i<_n_bit_sequence_drivers; ++i)
            Data::getSingleton()->addBitSequenceData( _bit_sequence_driver_names[i], new RandomBitSequence() );
    }

    void LinkActivityTraceManager::startup()
    {
        assert(_performance_config);

        LibUtil::Log::printLine("Startup Link Activity Trace Manager");

    // set latrace constants --------------------------------------------------
        _latrace_sampling_interval = getPerformanceConfig()->getFloat("latrace_manager/sampling_intvl");
        _current_latrace_line_number = 0;
    // ------------------------------------------------------------------------

    // Read bit sequence driver names from latrace ----------------------------
        // read latrace file
        loadBitSequenceDriverNamesFromLatrace();
        // initialize data structure
        setBitSequenceDriverNamesInBitSequenceData();
    // ------------------------------------------------------------------------

    // Schedule the first physical model execution event ----------------------
        EventScheduler::getSingleton()->enqueueEvent(0, PERFORMANCE_MODEL);
    // ------------------------------------------------------------------------

        _ready_to_execute = true;
    }

    void LinkActivityTraceManager::execute(Time scheduled_time)
    {
        assert(_ready_to_execute);
        assert(_performance_config);
        
        // only execute latrace when the schduled execution time matches the latrace sampling time
        _current_latrace_line_number++;
        if( !Misc::eqTime(scheduled_time, ((_current_latrace_line_number-1) * _latrace_sampling_interval) ) )
        {
            _current_latrace_line_number--;
            LibUtil::Log::printLine("Link Activity Trace Manager not Executed");
            return;
        }

        LibUtil::Log::printLine("Execute Link Activity Trace Manager");
        
    // Read single line of power trace ----------------------------------------
        bool valid_line = false;
        valid_line = loadBitSequenceDriverActivityFromLatrace();
    // ------------------------------------------------------------------------

    // Update bit sequence data structure -------------------------------------
        // only update when there's a new valid line in the latrace file
        // otherwise it will set activity to zero
        if(valid_line)
        {
            for(int i=0; i<_n_bit_sequence_drivers; ++i)
            {
                Data::getSingleton()->getBitSequenceData(_bit_sequence_driver_names[i])->setActivityFactor(_bit_sequence_drver_activity_factor[i]);
                Data::getSingleton()->getBitSequenceData(_bit_sequence_driver_names[i])->setBitRatio(_bit_sequence_drver_bit_ratio[i]);
            }

            // schedule the next event
            EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + _latrace_sampling_interval), PERFORMANCE_MODEL);
        }
        else
            // schedule the next event
            EventScheduler::getSingleton()->finish();
    // ------------------------------------------------------------------------
        
    }


} // namespace Thermal

