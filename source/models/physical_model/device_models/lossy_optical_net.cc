
#include <cassert>
#include <stdio.h>

#include "source/data/data.h"
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

    void LossyOpticalNet::initializeDevice()
    {
        double loss_in_db = getParameter("loss");

        if      (loss_in_db < 0)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Optical net loss cannot be smaller than zero.\n");
        else if (loss_in_db == 0)
            loss_in_db = getParameter("length") * getParameter("loss_per_meter");

        _power_ratio = pow( 10.00, (-loss_in_db/10) );

        // initialize device energy consumption to zero if in floorplan
        if(isMappedInFloorplan())
        {
            map<string, double>& data_energy = Data::getSingleton()->getAccumulatedEnergyConsumption(); 
            data_energy[_floorplan_unit_name] = 0;
        }
    }

    void LossyOpticalNet::updateDeviceProperties()
    {
        // update the input ports' device properties from its connected devices
        getPortForModification("in")->updatePortPropertyFromConnectedPort("power");
        getPortForModification("in")->updatePortPropertyFromConnectedPort("wavelength");

        // calculate and update output power
        double output_power = getPort("in")->getPortProperty("power") * _power_ratio;
        double output_wavelength = getPort("in")->getPortProperty("wavelength");
        getPortForModification("out")->setPortProperty("power", output_power);
        getPortForModification("out")->setPortProperty("wavelength", output_wavelength);
    }
}

