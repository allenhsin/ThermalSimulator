
#ifndef __THERMAL_THERMAL_TUNER_H__
#define __THERMAL_THERMAL_TUNER_H__

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class ThermalTuner : public DeviceModel
    {
    public:
        ThermalTuner(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        ThermalTuner( const ThermalTuner& cloned_device);
        ~ThermalTuner();

        // initialize device
        virtual void initializeDevice();

        // update device properties
        virtual void updateDeviceProperties(Time time_elapsed_since_last_update);

        virtual void printDefinition(FILE* device_list_file);

        virtual void initializeMonitoring();
        virtual void printMonitoredResult();
        
        virtual ThermalTuner* clone() const;

    protected:
        // check the validity of device parameters
        virtual void deviceParameterCheck();

    private:
        double  _heater_power;
        Time    _clock_period;
        Time    _current_clock_period_elapsed_time;
        int     _pdm_threshold;
        int     _pdm_input;

        int     _pdm_error;
        bool    _pdm_output;

    };

} // namespace Thermal

#endif // __THERMAL_THERMAL_TUNER_H__

