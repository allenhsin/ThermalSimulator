
#ifndef __THERMAL_OPTICAL_DATA_GENERATOR_H__
#define __THERMAL_OPTICAL_DATA_GENERATOR_H__

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class OpticalDataGenerator : public DeviceModel
    {
    public:
        OpticalDataGenerator(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        OpticalDataGenerator( const OpticalDataGenerator& cloned_device);
        ~OpticalDataGenerator();

        // initialize device
        virtual void initializeDevice();

        // update device properties
        virtual void updateDeviceProperties(Time time_elapsed_since_last_update);

        virtual void printDefinition(FILE* device_list_file);

        virtual void initializeMonitoring();
        virtual void printMonitoredResult();

        virtual OpticalDataGenerator* clone() const;

    protected:
        // check the validity of device parameters
        virtual void deviceParameterCheck();

    private:
        Time    _bit_period;
        
        double  _bit_zero_optical_power_in_watt;
        double  _bit_one_optical_power_in_watt;
        double  _bit_one_time_const;
        double  _bit_zero_time_const;
        
        // true: 1, false: 0
        bool    _current_bit;
        
        double  _current_out_optical_power;
        double  _current_delta_optical_power;
        double  _current_target_optical_power;
        double  _current_time_const;
        Time    _current_bit_elapsed_time;

    };

} // namespace Thermal

#endif // __THERMAL_OPTICAL_DATA_GENERATOR_H__

