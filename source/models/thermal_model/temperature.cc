
#include <cassert>

#include "source/data/data.h"
#include "source/models/thermal_model/temperature.h"
#include "source/models/thermal_model/thermal_constants.h"
#include "source/models/thermal_model/thermal_util.h"
#include "libutil/LibUtil.h"

using std::vector;
using std::string;
using LibUtil::String;

namespace Thermal
{
    Temperature::Temperature()
        : _floorplan_holder         (NULL)
        , _rc_model_holder          (NULL)
        , _sampling_intvl           (0)
    {
        _temperature.clear();
        _power.clear();
    }

    Temperature::~Temperature()
    {
        if(_ttrace_file)
            fclose(_ttrace_file);
    }

    void Temperature::updateTemperatureData()
    {
        if(Data::getSingleton()->getTemperatureDataSize() != (unsigned int) _floorplan_holder->_n_units)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Mismatch between size of temperature data and number of floorplan blocks.\n");
        
        // put temperature back to the data structure
        // (only the silicon layer units)
        for(int i=0; i<_floorplan_holder->_n_units; ++i)
            Data::getSingleton()->setTemperatureData(_floorplan_holder->_flp_units[i]._name, _temperature[i]);
    }

    void Temperature::setInternalPower( double ambient_temp )
    {
        int i = 0, j = 0;
        int n_units = _floorplan_holder->_n_units;
        int n_layers = _rc_model_holder->layer.n_layers;

        for(i=0; i<n_units; ++i)
        {
            for(j=1; j<(n_layers-1);  ++j)
                _power[ j * n_units + i] = 0;
            
            _power[ (n_layers-1) * n_units + i ] = ambient_temp * _rc_model_holder->g_amb[i];
        }
    }

    void Temperature::printTtraceFiles(bool initialization)
    {
        assert(_rc_model_holder);
        assert(_floorplan_holder);
        int i = 0, j = 0;

        if(initialization)
        {   
            _ttrace_file = fopen(_ttrace_file_name.c_str(), "w");
            if(!_ttrace_file)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open ttrace file for output.\n");
            
            // not including the air layer
            for(i = 0; i < _rc_model_holder->layer.n_layers; ++i)
            {
                // print flp unit names
                for(j = 0; j < _floorplan_holder->_n_units; ++j)
                    fprintf(_ttrace_file, "layer%d.%s ", i, (_floorplan_holder->_flp_units[j]._name).c_str() );
            }
            fprintf(_ttrace_file, "\n");
        }

        for(i = 0; i < _rc_model_holder->layer.n_layers; ++i)
        {
            for(j = 0; j < _floorplan_holder->_n_units; ++j)
                fprintf(_ttrace_file, "%.5f ", _temperature[ (i*_floorplan_holder->_n_units) + j]);
        }
        fprintf(_ttrace_file, "\n");
    }
    
    void Temperature::computeTransientTemperatureFromPower()
    {
        // shortcuts
        int n                                               = _rc_model_holder->n_nodes;
        const vector<Time>& time_steps                      = _rc_model_holder->time_steps;
        const vector< vector< vector<double> > >& lu_step   = _rc_model_holder->lu_step;
        const vector< vector<double> >& geq_step            = _rc_model_holder->geq_step;
        const vector< vector<int> >& p_step                 = _rc_model_holder->p_step;
        Time  sampling_intvl                                = _sampling_intvl;
        
        Time time, residue_time;
        vector<double> power_new (n, 0);
        int     time_step_index;
        int     i = 0;
    
        if (!_rc_model_holder->r_ready || !_rc_model_holder->c_ready)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: RC model not ready.\n");
    
    
        // keep advancing the time until it reaches sampling_intvl.
        // always choose the next appropriate step size that is 
        // not greater than the residue time toward sampling_intvl.
        time = 0.0;
        time_step_index = -1;
        while (time < sampling_intvl)
        {
            residue_time = sampling_intvl - time;
            for (i=0; i<N_TIME_STEPS; ++i)
            {
                if (time_steps[i] < residue_time)
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
    
    void Temperature::updateTransientTemperature(Time time_elapsed_since_last_update)
    {
        assert(_floorplan_holder);
        assert(_rc_model_holder);
        assert(_temperature.size() == (unsigned int) _rc_model_holder->n_nodes);
        assert(_power.size() == (unsigned int) _rc_model_holder->n_nodes);

        if(Data::getSingleton()->getEnergyDataSize() != (unsigned int) _floorplan_holder->_n_units)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Mismatch between number of blocks in the floorplan and accumulated energy data.\n");
        
        // put main power data information into local _power vector
        // the main power data only contains the power for silicon layer blocks
        // so in "computeTransientTemperatureFromPower" function, it will set
        // the power for nodes in others layers
        for(int i=0; i<_floorplan_holder->_n_units; ++i)
            _power[i] = Data::getSingleton()->getEnergyData(_floorplan_holder->_flp_units[i]._name)/time_elapsed_since_last_update;
        
        // compute temp from power
        computeTransientTemperatureFromPower();

        // print ttrace files
        printTtraceFiles(false);

        // write temperature back to the main data structure
        updateTemperatureData();

    } // updateTransientTemperature

    void Temperature::initialize( double initial_temperature, double ambient_temperature )
    {
        assert(_floorplan_holder);
        assert(_rc_model_holder);
        assert(_temperature.size() == 0);
        assert(_power.size() == 0);
        
        // initialize local temperature structure
        _temperature.resize(_rc_model_holder->n_nodes);
        _power.resize(_rc_model_holder->n_nodes);
        
        // set power for internal nodes
        setInternalPower(ambient_temperature);

        // _temperature.size() = n_nodes = _n_units * layers
        for(int i=0; i<(int) _temperature.size(); ++i)
            _temperature[i] = initial_temperature;
        
        // print ttrace files
        printTtraceFiles(true);

        // write temperature back to the main data structure
        updateTemperatureData();

    } // setInitialTemperature

} // namespace Thermal

