
#include <cassert>
#include <stdio.h>

#include "source/models/physical_model/device_models/lossy_optical_net.h"
#include "source/models/physical_model/device_models/device_type.h"

namespace Thermal
{
    LossyOpticalNet::LossyOpticalNet(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel( LOSSY_OPTICAL_NET, device_floorplan_map, device_definition_file)
    {}

    LossyOpticalNet::~LossyOpticalNet()
    {}

    // TODO:
    void LossyOpticalNet::updateDeviceProperties()
    {}
    

}

