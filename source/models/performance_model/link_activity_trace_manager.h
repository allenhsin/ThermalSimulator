
#ifndef __THERMAL_LINK_ACTIVITY_TRACE_MANAGER_H__
#define __THERMAL_LINK_ACTIVITY_TRACE_MANAGER_H__

#include <stdio.h>
#include <vector>
#include <set>

#include "config.hpp"
#include "source/misc/common_types.h"

namespace Thermal
{
    class LinkActivityTraceManager
    {
    public:
        LinkActivityTraceManager();
        ~LinkActivityTraceManager();

        void startup();
        void execute(Time scheduled_time);

        void setPerformanceConfig(config::Config* performance_config) 
        { _performance_config = performance_config; }

    protected:
        config::Config* getPerformanceConfig(){ return _performance_config; }

        void setBitSequenceDriverNamesInBitSequenceData();

        void loadBitSequenceDriverNamesFromLatrace();
        bool loadBitSequenceDriverActivityFromLatrace();

    private:
        config::Config*             _performance_config;

        FILE*                       _latrace_file;

        int                         _n_bit_sequence_drivers;
        std::vector<std::string>    _bit_sequence_driver_names;
        std::set<std::string>       _bit_sequence_driver_names_set;
        std::vector<double>         _bit_sequence_drver_activity_factor;
        std::vector<double>         _bit_sequence_drver_bit_ratio;

        Time                        _latrace_sampling_interval;
        int                         _current_latrace_line_number;

        bool                        _ready_to_execute;
        bool                        _latrace_file_read_over;
    }; // class LinkActivityTraceManager

} // namespace Thermal

#endif // __THERMAL_LINK_ACTIVITY_TRACE_MANAGER_H__

