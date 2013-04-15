
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
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Optical net loss cannot be negative.\n");

        if( getParameter("length") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Optical net Length cannot be negative.\n");

        if( getParameter("loss_per_meter") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Optical net loss cannot be negative.\n");

    }

    void LossyOpticalNet::initializeDevice()
    {
        deviceParameterCheck();

        assert(getPort("out")->getPortPropertySize("wavelength")==0);
        assert(getPort("out")->getPortPropertySize("power")==0);
        
        // initialize the number of wavelengths
        _number_wavelength = getPort("in")->getPortPropertySize("wavelength");
        getPortForModification("out")->setPortPropertySize("wavelength", _number_wavelength);
        getPortForModification("out")->setPortPropertySize("power", _number_wavelength);
        
        // set the output wavelengths
        for(unsigned int i=0; i<_number_wavelength; ++i)
            getPortForModification("out")->setPortPropertyValueByIndex("wavelength", i, getPort("in")->getPortPropertyValueByIndex("wavelength", i));

        // compute the power ratio between input and out (the same for all wavelengths)
        double loss_in_db = getParameter("loss");
        if (loss_in_db==0)
            loss_in_db = getParameter("length") * getParameter("loss_per_meter");

        _power_ratio = pow( 10.00, (-loss_in_db/10) );


    }

    void LossyOpticalNet::updateDeviceProperties(double time_elapsed_since_last_update)
    {
        // calculate and update output power for each wavelength
        double in_power;
        double out_power;
        double accumulated_power = 0;
        for(unsigned int i=0; i<_number_wavelength; ++i)
        {
            in_power         = getPort("in")->getPortPropertyValueByIndex("power", i);
            out_power        = in_power * _power_ratio;
            getPortForModification("out")->setPortPropertyValueByIndex("power", i, out_power);

            accumulated_power += out_power - in_power;
        }

        // update energy consumption in data structure if the device is in the floorplan
        if(isMappedInFloorplan())
        {
            double previous_accumulated_energy = Data::getSingleton()->getData(ACCUMULATED_ENERGY_DATA, _floorplan_unit_name);
            Data::getSingleton()->setData(  ACCUMULATED_ENERGY_DATA, _floorplan_unit_name, 
                                            (previous_accumulated_energy+(time_elapsed_since_last_update * accumulated_power))
                                         );
        }
    }
} // namespace Thermal

