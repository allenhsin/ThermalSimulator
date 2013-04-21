
#include <cassert>
#include <stdio.h>

#include "source/models/physical_model/device_models/laser_source_off_chip.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    LaserSourceOffChip::LaserSourceOffChip(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel( LASER_SOURCE_OFF_CHIP, device_floorplan_map, device_definition_file)
    {}

    LaserSourceOffChip::~LaserSourceOffChip()
    {}
    
    void LaserSourceOffChip::deviceParameterCheck()
    {   
        int number_wavelength = (int) getParameter("number_wavelength");
        double wavelength_begin = getParameter("wavelength_begin");
        double wavelength_end = getParameter("wavelength_end");

        if( number_wavelength < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Number of wavelengths cannot be negative.\n");
        
        if( wavelength_begin < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wavelength cannot be negative.\n");
        
        if( wavelength_end < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wavelength cannot be negative.\n");
        
        if(number_wavelength == 1)
        {
            if(wavelength_begin != wavelength_end)
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wrong wavelength range.\n");
        }
        else
        {
            if( wavelength_begin <= wavelength_end )
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wrong wavelength range.\n");
        }
    }

    void LaserSourceOffChip::initializeDevice()
    {
        // sanity check
        deviceParameterCheck();

        // there's no need to update energy consumption 
        // number cuz this is a off-chip laser
        // if this device is in the floorplan it's an error
        if(isMappedOnFloorplan())
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Off-chip laser source cannot be on the floorplan.\n");

        assert(getPort("out")->getPortPropertySize("wavelength")==0);
        assert(getPort("out")->getPortPropertySize("power")==0);

        double power_in_dbm = getParameter("laser_output_power_per_wavelength");
        double power_in_watt = pow( 10.00, ((power_in_dbm)/10 - 3) );

        unsigned int number_wavelength = (unsigned int) getParameter("number_wavelength");
        double wavelength_begin = getParameter("wavelength_begin");
        double wavelength_end = getParameter("wavelength_end");
        
        getPortForModification("out")->setPortPropertySize("wavelength", number_wavelength);
        getPortForModification("out")->setPortPropertySize("power", number_wavelength);
    
        if(number_wavelength==1)
        {
            getPortForModification("out")->setPortPropertyValueByIndex("wavelength", 0, wavelength_begin);
            getPortForModification("out")->setPortPropertyValueByIndex("power", 0, power_in_watt);
        }
        else if(number_wavelength>1)
        {
            double wavelength_interval = (wavelength_end - wavelength_begin)/((double)(number_wavelength-1));
            for(unsigned int i=0; i<number_wavelength; ++i)
            {
                getPortForModification("out")->setPortPropertyValueByIndex("wavelength", i, (wavelength_begin + (i * wavelength_interval)) );
                getPortForModification("out")->setPortPropertyValueByIndex("power", i, power_in_watt);
            }
        }

    } // initializeDevice

    // The output power won't change
    void LaserSourceOffChip::updateDeviceProperties(Time time_elapsed_since_last_update)
    {
        // just to suppress the unused warning
        (void)time_elapsed_since_last_update;
    }
    
    void LaserSourceOffChip::printDefinition(FILE* device_list_file)
    {
        DeviceModel::printDefinition(device_list_file);

        fprintf(device_list_file, "    [port]\n");
        fprintf(device_list_file, "        Name: out, Type: %s, Property: power(%d), wavlength(%d)\n", 
                ((int)getPort("out")->getPortType()==0)?"INPUT":"OUTPUT", getPort("out")->getPortPropertySize("power"), getPort("out")->getPortPropertySize("wavelength"));
        fprintf(device_list_file, "\n");

    }

} // namespace Thermal

