
#ifndef __THERMAL_LOSSY_OPTICAL_NET_H__
#define __THERMAL_LOSSY_OPTICAL_NET_H__

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class LossyOpticalNet : public DeviceModel
    {
    public:
        LossyOpticalNet(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        ~LossyOpticalNet();

        // initialize device
        virtual void initializeDevice();

        // update device properties
        virtual void updateDeviceProperties(Time time_elapsed_since_last_update);

        virtual void printDefinition(FILE* device_list_file);

        virtual void initializeMonitoring();
        virtual void printMonitoredResult();

    protected:
        // check the validity of device parameters
        virtual void deviceParameterCheck();

    private:
        // the laser power ratio between net output and net input
        double _power_ratio;
        unsigned int _number_wavelength;
    };

} // namespace Thermal

#endif // __THERMAL_LOSSY_OPTICAL_NET_H__

