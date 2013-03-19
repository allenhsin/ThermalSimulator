
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>

#include "source/data/data.h"
#include "source/models/thermal_model/temperature.h"
#include "source/models/thermal_model/thermal_constants.h"
#include "source/models/thermal_model/thermal_util.h"
#include "source/models/thermal_model/floorplan.h"
#include "libutil/Log.h"
#include "libutil/String.h"

using std::vector;
using std::map;
using std::string;

namespace Thermal
{
    Temperature::Temperature()
        : _thermal_params       (NULL)
        , _floorplan_holder     (NULL)
        , _rc_model_holder      (NULL)
    {
        _temperature.clear();
    }


    Temperature::~Temperature()
    {}

    void Temperature::setThermalParameters(ThermalParameters* thermal_params)
    { _thermal_params = thermal_params; }

    void Temperature::setFloorplanHolder(FloorplanHolder* floorplan_holder)
    { _floorplan_holder = floorplan_holder; }

    void Temperature::setRCModelHolder(RCModelHolder* rc_model_holder)
    { _rc_model_holder = rc_model_holder; }

    void Temperature::updateDataTemperature()
    {
        map<string, double>& data_temperature = Data::getSingleton()->getTemperature();

        assert(data_temperature.size() == (unsigned int) _floorplan_holder->_n_units);
        
        // put temperature back to the data structure
        // (only the silicon layer units)
        for(int i=0; i<_floorplan_holder->_n_units; ++i)
            data_temperature[ (string) _floorplan_holder->_flp_units[i]._name ] = _temperature[i];
        // check that it doesn't add any new block into the data structure
        assert(data_temperature.size() == (unsigned int) _floorplan_holder->_n_units);
    }

    void Temperature::readInitTemperatureFromFile()
    {
        int    i;
        int    layer;
        int    idx;
        double val;
        char*  ptr; 
        char   str1[LINE_SIZE], str2[LINE_SIZE];
        char   name[STR_SIZE],  format[STR_SIZE];
        FILE*  fp;
    
        fp = fopen (_thermal_params->init_temp_file.c_str(), "r");
    
        if (!fp) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open init temp file.\n");
    
        // temperatures of the different layers
        for (layer=0; layer < NL; layer++) 
        {
            switch(layer)
            {
            case 0:
                strcpy(format,"%s%lf");
                break;
            case IFACE:
                strcpy(format,"iface_%s%lf");
                break;
            case HSP:
                strcpy(format,"hsp_%s%lf");
                break;
            case HSINK:
                strcpy(format,"hsink_%s%lf");
                break;
            default:
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: unknown layer type.\n");
                break;
            }

            for (i=0; i < _floorplan_holder->_n_units; i++) 
            {
                fgets(str1, LINE_SIZE, fp);
                if (feof(fp))
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: no enough lines in init temp file.\n");
                strcpy(str2, str1);
    
                // ignore comments and empty lines
                ptr = strtok(str1, " \r\t\n");
                if (!ptr || ptr[0] == '#') 
                {
                    i--;
                    continue;
                }
    
                if (sscanf(str2, format, name, &val) != 2)
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: invalid init temp file format.\n");
                idx = Floorplan::getUnitIndexFromName(_floorplan_holder, name);
                _temperature[idx + layer*_floorplan_holder->_n_units] = val;
            }
        }
    
        // internal node temperatures
        for (i=0; i < EXTRA; i++) 
        {
            fgets(str1, LINE_SIZE, fp);
            if (feof(fp))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: no enough lines in init temp file.\n");
            strcpy(str2, str1);
            // ignore comments and empty lines
            ptr = strtok(str1, " \r\t\n");
            if (!ptr || ptr[0] == '#') 
            {
                i--;
                continue;
            }
            if (sscanf(str2, "%s%lf", name, &val) != 2)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: invalid init temp file format.\n");
            sprintf(str1, "inode_%d", i);
            if (strcmp(str1, name))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: invalid init temp file format.\n");
            _temperature[i+NL*_floorplan_holder->_n_units] = val;  
        }
    
        fgets(str1, LINE_SIZE, fp);
        if (!feof(fp))
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: too many lines in init temp file.\n");
                
        fclose(fp); 
    } // readInitTemperatureFromFile
    
    double Temperature::getAvgSinkTemperature()
    {
        assert(_thermal_params);
        assert(_floorplan_holder);
        assert(_rc_model_holder);
        assert(_temperature.size() == (unsigned int) _rc_model_holder->n_nodes);

        int i;
        double sum          = 0.0;
        double width        = _floorplan_holder->_total_width;
        double height       = _floorplan_holder->_total_height;
        double spr_size     = _thermal_params->s_spreader * _thermal_params->s_spreader;
        double sink_size    = _thermal_params->s_sink * _thermal_params->s_sink;
        
        /* heatsink temperatures    */
        for (i=0; i < _floorplan_holder->_n_units; i++)
            if (_temperature[HSINK * _floorplan_holder->_n_units + i] < 0)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: negative temperature.\n");
            else  /* area-weighted average */
                sum += _temperature[HSINK * _floorplan_holder->_n_units + i] * 
                       (_floorplan_holder->_flp_units[i]._width * _floorplan_holder->_flp_units[i]._height);
                
        for(i=SINK_C_W; i <= SINK_C_E; i++)
            if (_temperature[i + NL * _floorplan_holder->_n_units] < 0)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: negative temperature.\n");
            else
                sum += _temperature[i + NL * _floorplan_holder->_n_units] * 0.25 * 
                       (_thermal_params->s_spreader + height) * (_thermal_params->s_spreader - width);
    
        for(i=SINK_C_N; i <= SINK_C_S; i++)
            if (_temperature[i + NL * _floorplan_holder->_n_units] < 0)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: negative temperature.\n");
            else
                sum += _temperature[i + NL * _floorplan_holder->_n_units] * 0.25 * 
                      (_thermal_params->s_spreader + width) * (_thermal_params->s_spreader - height);
    
        for(i=SINK_W; i <= SINK_S; i++)
            if (_temperature[i + NL * _floorplan_holder->_n_units] < 0)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: negative temperature.\n");
            else
                sum += _temperature[i + NL * _floorplan_holder->_n_units] * 0.25 * 
                       (sink_size - spr_size);
        
        return (sum / sink_size);
    } // getAvgSinkTemperature

    // setting package nodes' power numbers
    void Temperature::setInternalPower()
    {
        int i;

        for(i=0; i<_floorplan_holder->_n_units; ++i)
        {
            _power[IFACE * _floorplan_holder->_n_units + i] = 0;
            _power[HSP   * _floorplan_holder->_n_units + i] = 0;
        }

        for(i=0; i < _floorplan_holder->_n_units + EXTRA; i++)
            _power[HSINK  * _floorplan_holder->_n_units + i] = _thermal_params->ambient * _rc_model_holder->g_amb[i];
    }
    
    void Temperature::computeTransientTemperatureFromPower()
    {
        // shortcuts
        int n                                       = _rc_model_holder->n_nodes;
        vector<double>& time_steps                  = _rc_model_holder->time_steps;
        vector< vector< vector<double> > >& lu_step = _rc_model_holder->lu_step;
        vector< vector<double> >& geq_step          = _rc_model_holder->geq_step;
        vector< vector<int> >& p_step               = _rc_model_holder->p_step;
        double time_elapsed                         = _thermal_params->sampling_intvl;
        
        double  time, residue_time;
        vector<double> power_new (n, 0);
        int     time_step_index;
        int     i = 0;
    
        if (!_rc_model_holder->r_ready || !_rc_model_holder->c_ready)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: RC model not ready.\n");
    
        // set power numbers for the virtual nodes
        setInternalPower();
    
        // keep advancing the time until it reaches time_elapsed.
        // always choose the next appropriate step size that is 
        // not greater than the residue time toward time_elapsed.
        time = 0.0;
        time_step_index = -1;
        while (time < time_elapsed)
        {
            residue_time = time_elapsed - time;
            for (i=0; i<N_TIME_STEPS; ++i)
            {
                if (time_steps[i] <= residue_time)
                {
                    time_step_index = i;
                    break;
                }
            }
            assert(time_step_index != -1);
            
            for (i=0; i<n; ++i)
                power_new[i] = _power[i] + (geq_step[time_step_index][i] * _temperature[i]);
    
            ThermalUtil::luSolve(lu_step[time_step_index], n, p_step[time_step_index], power_new, _temperature);
            
            time += time_steps[time_step_index];
        }
    } // computeTransientTemperatureFromPower
    
    void Temperature::updateTransientTemperature()
    {
        assert(_thermal_params);
        assert(_floorplan_holder);
        assert(_rc_model_holder);
        assert(_temperature.size() == (unsigned int) _rc_model_holder->n_nodes);

        map<string, double>& data_power = Data::getSingleton()->getPower();
        assert(data_power.size() == (unsigned int) _floorplan_holder->_n_units);
        
        // put main power data information into local _power vector
        for(map<string, double>::iterator it = data_power.begin(); it != data_power.end(); ++it)
            _power[ Floorplan::getUnitIndexFromName(_floorplan_holder, it->first.c_str()) ] = it->second;
        
        // compute temp from power
        computeTransientTemperatureFromPower();

        // write temperature back to the main data structure
        updateDataTemperature();

    } // updateTransientTemperature

    void Temperature::setInitialTemperature()
    {
        assert(_thermal_params);
        assert(_floorplan_holder);
        assert(_rc_model_holder);
        assert(_temperature.size() == 0);
        assert(_power.size() == 0);
        
        // initialize local temperature structure
        _temperature.resize(_rc_model_holder->n_nodes);
        _power.resize(_rc_model_holder->n_nodes);
        
        if(!_thermal_params->init_file_used)
        {   
            // _temperature.size() = n_nodes = _n_units*layers + EXTRA
            for(int i=0; i<(int) _temperature.size(); ++i)
                _temperature[i] = _thermal_params->init_temp;
        }
        else // read initial temperature values from file
            readInitTemperatureFromFile();
        
        // write temperature back to the main data structure
        updateDataTemperature();

    } // setInitialTemperature

}

