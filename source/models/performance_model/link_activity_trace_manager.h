
#ifndef __THERMAL_LINK_ACTIVITY_TRACE_MANAGER_H__
#define __THERMAL_LINK_ACTIVITY_TRACE_MANAGER_H__

#include <stdio.h>
#include <string>
#include <vector>
#include <set>

#include "source/models/performance_model/performance_model.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class LinkActivityTraceManager : public PerformanceModel
    {
    public:
        LinkActivityTraceManager();
        ~LinkActivityTraceManager();

        bool startupManager();
        void executeManager(Time scheduled_time);
        std::string getModelName() { return "Link Activity Trace Manager"; }

    protected:
        void setBitSequenceDriverNamesInBitSequenceData();

        void loadBitSequenceDriverNamesFromLatrace();
        bool loadBitSequenceDriverActivityFromLatrace();

    private:
        FILE*                       _latrace_file;
        bool                        _latrace_file_read_over;
        Time                        _latrace_sampling_interval;
        int                         _current_latrace_line_number;

        std::vector<std::string>    _bit_sequence_driver_names;
        std::set<std::string>       _bit_sequence_driver_names_set;
        std::vector<double>         _bit_sequence_drver_activity_factor;
        std::vector<double>         _bit_sequence_drver_bit_ratio;


    }; // class LinkActivityTraceManager

} // namespace Thermal

#endif // __THERMAL_LINK_ACTIVITY_TRACE_MANAGER_H__

