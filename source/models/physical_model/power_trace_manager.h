
#ifndef __THERMAL_POWER_TRACE_MANAGER_H__
#define __THERMAL_POWER_TRACE_MANAGER_H__

#include <stdio.h>
#include <vector>

#include "source/models/physical_model/physical_model.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class PowerTraceManager : public PhysicalModel
    {
    public:
        PowerTraceManager();
        ~PowerTraceManager();

        void startupManager();
        void executeManager(Time scheduled_time);

        std::string getModelName() { return "Power Trace Manager"; }

    protected:
        void loadFloorplanUnitNamesFromPtrace();
        void loadFloorplanUnitPowerFromPtrace();

    private:
        FILE*                       _ptrace_file;
        bool                        _ptrace_file_read_over;
        Time                        _ptrace_sampling_interval;
        int                         _current_ptrace_line_number;
        
        // need to keep another vector structure since
        // the map won't keep the insertion order
        std::vector<std::string>    _ptrace_flp_units_names;


    }; // class PowerTraceManager

} // namespace Thermal

#endif // __THERMAL_POWER_TRACE_MANAGER_H__

