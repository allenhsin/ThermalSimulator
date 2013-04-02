
#include <cassert>
#include <stdio.h>

#include "source/models/physical_model/device_models/resonant_ring_modulator.h"

namespace Thermal
{
    ResonantRingModulator::ResonantRingModulator(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel(device_floorplan_map, device_definition_file)
    {}

    ResonantRingModulator::~ResonantRingModulator()
    {}

    // TODO:
    void ResonantRingModulator::updateDeviceProperties()
    {}
    
    // TODO:
    void ResonantRingModulator::getDevicePower()
    {}




}

