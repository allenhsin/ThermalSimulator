
#ifndef __THERMAL_MODULATOR_DRIVER_H__
#define __THERMAL_MODULATOR_DRIVER_H__

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"

namespace Thermal
{
    class ModulatorDriver : public DeviceModel
    {
    public:
        ModulatorDriver(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        ~ModulatorDriver();

        // initialize device
        virtual void initializeDevice();

        // update device properties
        virtual void updateDeviceProperties(double time_elapsed_since_last_update);

    protected:
        // check the validity of device parameters
        virtual void deviceParameterCheck();

    private:
        double  _bit_one_time_const;
        double  _bit_zero_time_const;
        // true: 1, false: 0
        bool    _current_bit;
        double  _current_out_voltage;
        double  _current_delta_voltage;
        double  _current_target_voltage;
        double  _current_time_const;
        double  _transition_elapsed_time;

    };

} // namespace Thermal

#endif // __THERMAL_MODULATOR_DRIVER_H__

