
#ifndef __THERMAL_DEVICE_MONITOR_H__
#define __THERMAL_DEVICE_MONITOR_H__

#include <string>
#include <vector>

#include "source/models/physical_model/device_models/device_model.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class DeviceMonitor
    {
    public:
        DeviceMonitor();
        ~DeviceMonitor();

        void startup(std::string device_monitor_list_file, std::vector<DeviceModel*>& device_references, std::string output_dir);
        void execute(Time scheduled_time);

    protected:
        void loadMonitoredDevices(std::string device_monitor_list_file, std::vector<DeviceModel*>& device_references, std::string output_dir);

    private:
        std::vector<DeviceModel*>   _monitored_devices;

        std::vector<Time>           _monitoring_time_point;
        std::vector<Time>           _monitoring_time_duration;

        unsigned int                _current_monitoring_step;
        double                      _current_monitoring_time_start;
        double                      _current_monitoring_time_end;
        bool                        _is_monitoring;

    }; // class DeviceMonitor

} // namespace Thermal

#endif // __THERMAL_DEVICE_MONITOR_H__
