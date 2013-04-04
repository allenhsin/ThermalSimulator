
#ifndef __THERMAL_RESONANT_RING_MODULATOR_H__
#define __THERMAL_RESONANT_RING_MODULATOR_H__

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"

namespace Thermal
{
    class ResonantRingModulator : public DeviceModel
    {
    public:
        ResonantRingModulator(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        ~ResonantRingModulator();

        // update device properties
        virtual void updateDeviceProperties();

    protected:

    private:

    }; // class ResonantRingModulator

} // namespace Thermal

#endif // __THERMAL_RESONANT_RING_MODULATOR_H__

