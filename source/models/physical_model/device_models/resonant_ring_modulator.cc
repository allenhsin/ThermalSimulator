
#include <cassert>
#include <stdio.h>

#include "source/models/physical_model/device_models/resonant_ring_modulator.h"
#include "source/models/physical_model/device_models/device_type.h"

namespace Thermal
{
    ResonantRingModulator::ResonantRingModulator(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel( RESONANT_RING_MODULATOR, device_floorplan_map, device_definition_file)
    {}

    ResonantRingModulator::~ResonantRingModulator()
    {}

    // TODO:
    void ResonantRingModulator::updateDeviceProperties()
    {}
    

}

