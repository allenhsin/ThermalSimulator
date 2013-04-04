
#include <cassert>
#include <stdio.h>

#include "source/models/physical_model/device_models/laser_source_off_chip.h"
#include "source/models/physical_model/device_models/device_type.h"

namespace Thermal
{
    LaserSourceOffChip::LaserSourceOffChip(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel( LASER_SOURCE_OFF_CHIP, device_floorplan_map, device_definition_file)
    {}

    LaserSourceOffChip::~LaserSourceOffChip()
    {}

    // TODO:
    void LaserSourceOffChip::updateDeviceProperties()
    {}
    

}

