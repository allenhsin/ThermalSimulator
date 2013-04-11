
#include <cassert>
#include <stdio.h>

#include "source/data/data.h"
#include "source/misc/misc.h"
#include "source/models/physical_model/device_models/lossy_optical_net.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    LossyOpticalNet::LossyOpticalNet(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel( LOSSY_OPTICAL_NET, device_floorplan_map, device_definition_file)
        , _power_ratio (0)
    {}

    LossyOpticalNet::~LossyOpticalNet()
    {}

    void LossyOpticalNet::deviceParameterCheck()
    {   
        if( getParameter("loss") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Optical net loss cannot be smaller than zero.\n");

    }

    void LossyOpticalNet::initializeDevice()
    {
        deviceParameterCheck();

        double loss_in_db = getParameter("loss");

        if (Misc::eq(loss_in_db, 0))
            loss_in_db = getParameter("length") * getParameter("loss_per_meter");

        _power_ratio = pow( 10.00, (-loss_in_db/10) );
    }

    void LossyOpticalNet::updateDeviceProperties(double time_elapsed_since_last_update)
    {
        // update the input ports' device properties from its connected devices
        getPortForModification("in")->updatePortPropertyFromConnectedPort("power");
        getPortForModification("in")->updatePortPropertyFromConnectedPort("wavelength");

        // calculate and update output power
        double in_power         = getPort("in")->getPortProperty("power");
        double out_power        = in_power * _power_ratio;
        double out_wavelength   = getPort("in")->getPortProperty("wavelength");
        getPortForModification("out")->setPortProperty("power", out_power);
        getPortForModification("out")->setPortProperty("wavelength", out_wavelength);

        // update energy consumption in data structure if the device is in the floorplan
        if(isMappedInFloorplan())
            Data::getSingleton()->setData(ACCUMULATED_ENERGY_DATA, _floorplan_unit_name, (time_elapsed_since_last_update * (in_power-out_power)));
    }
} // namespace Thermal

