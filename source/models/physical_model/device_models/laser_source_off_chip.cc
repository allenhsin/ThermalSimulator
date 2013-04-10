
#include <cassert>
#include <stdio.h>

#include "source/models/physical_model/device_models/laser_source_off_chip.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    LaserSourceOffChip::LaserSourceOffChip(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel( LASER_SOURCE_OFF_CHIP, device_floorplan_map, device_definition_file)
        , _wavelength(0)
    {}

    LaserSourceOffChip::~LaserSourceOffChip()
    {}

    void LaserSourceOffChip::initializeDevice()
    {
        double power_in_dbm = getParameter("laser_output_power");
        double power_in_watt = pow( 10.00, ((power_in_dbm)/10 - 3) );

        getPortForModification("out")->setPortProperty("power", power_in_watt);

        // there's no need to update energy consumption 
        // number cuz this is a off-chip laser
        // if this device is in the floorplan it's an error
        if(isMappedInFloorplan())
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Off-chip laser source cannot be on the floorplan.\n");
    }

    // The output power won't change
    void LaserSourceOffChip::updateDeviceProperties()
    {
        getPortForModification("out")->setPortProperty("wavelength", _wavelength);
    }
    

}

