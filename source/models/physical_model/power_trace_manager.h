
#ifndef __THERMAL_POWER_TRACE_MANAGER_H__
#define __THERMAL_POWER_TRACE_MANAGER_H__

#include <stdio.h>

#include "config.hpp"

namespace Thermal
{
    class PowerTraceManager
    {
    public:
        PowerTraceManager();
        ~PowerTraceManager();

        void startup();
        void execute(double scheduled_time);

        void setPhysicalConfig(config::Config* physical_config) 
        { _physical_config = physical_config; }

    protected:
        config::Config* getPhysicalConfig(){ return _physical_config; }

        void setFloorplanUnitNamesInPowerData();

        void loadFloorplanUnitNamesFromPtrace();
        bool loadFloorplanUnitPowerFromPtrace();

    private:
        config::Config*             _physical_config;

        FILE*                       _ptrace_file;
        int                         _n_ptrace_flp_units;
        std::vector<std::string>    _ptrace_flp_units_names;
        std::vector<double>         _ptrace_flp_units_power;

        double                      _ptrace_sampling_interval;
        int                         _current_ptrace_line_number;

        bool                        _ready_to_execute;
        bool                        _ptrace_file_read_over;
    }; // class PowerTraceManager

} // namespace Thermal

#endif // __THERMAL_POWER_TRACE_MANAGER_H__
