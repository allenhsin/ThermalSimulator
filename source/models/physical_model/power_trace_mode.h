
#ifndef __THERMAL_POWER_TRACE_MODE_H__
#define __THERMAL_POWER_TRACE_MODE_H__

#include <stdio.h>

#include "config.hpp"

namespace Thermal
{
    class PowerTraceMode
    {
    public:
        PowerTraceMode();
        ~PowerTraceMode();

        void startup();
        void execute(double scheduled_time);

        void setPhysicalConfig(config::Config* physical_config) 
        { _physical_config = physical_config; }

        config::Config* getPhysicalConfig(){ return _physical_config; }

    protected:
        void setFloorplanUnitNamesInPowerData();

        void readFloorplanUnitNamesFromPtrace();
        bool readFloorplanUnitPowerFromPtrace();

    private:
        config::Config*             _physical_config;

        FILE*                       _ptrace_file;
        int                         _n_ptrace_flp_units;
        std::vector<std::string>    _ptrace_flp_units_names;
        std::vector<double>         _ptrace_flp_units_power;

    }; // class PowerTraceMode

} // namespace Thermal

#endif // __THERMAL_POWER_TRACE_MODE_H__

