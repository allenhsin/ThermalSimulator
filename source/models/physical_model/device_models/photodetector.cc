
#include <cassert>
#include <stdio.h>

#include "source/models/physical_model/device_models/photodetector.h"
#include "libutil/LibUtil.h"

using std::map;
using std::string;
using std::cerr;

namespace Thermal
{
    Photodetector::Photodetector(DeviceFloorplanMap* device_floorplan_map, string device_definition_file)
        : DeviceModel(device_floorplan_map, device_definition_file)
        , _responsivity         (0)
        , _optical_power_ratio  (0)
        , _number_wavelength    (0)
    {}

    Photodetector::Photodetector( const Photodetector& cloned_device)
        : DeviceModel           (cloned_device)
        , _responsivity         (cloned_device._responsivity)
        , _optical_power_ratio  (cloned_device._optical_power_ratio)
        , _number_wavelength    (cloned_device._number_wavelength)
    {}

    Photodetector::~Photodetector()
    {}
    
    Photodetector* Photodetector::clone() const
    { return new Photodetector(*this); }

    void Photodetector::deviceParameterCheck()
    {   
        if( getParameter("responsivity") < 0 )
            LibUtil::Log::printFatalLine(cerr, "\nERROR: Photodetector responsivity cannot be negative.\n");

        if( getParameter("optical_power_loss") < 0 )
            LibUtil::Log::printFatalLine(cerr, "\nERROR: Photodetector optical power loss cannot be negative.\n");
    }

    void Photodetector::initializeDevice()
    {
        deviceParameterCheck();

        // load the parameters
        _responsivity = getParameter("responsivity");
        _optical_power_ratio = pow( 10.00, (-getParameter("optical_power_loss")/10) );
        _number_wavelength = getPort("optical_in")->getPortPropertySize("wavelength");

        // set output
        getPortForModification("electrical_out")->setPortPropertySize("current", 1);
        getPortForModification("electrical_out")->setPortPropertyValueByIndex("current", 0, 0);

    } // initializeDevice

    void Photodetector::updateDeviceProperties(Time time_elapsed_since_last_update)
    {
        (void)time_elapsed_since_last_update;

        // calculate and update output current and device power
        double total_optical_in_power = 0;
        double total_electrical_out_current = 0;
        double total_dissipated_power = 0;
        for(unsigned int i=0; i<_number_wavelength; ++i)
            total_optical_in_power += getPort("optical_in")->getPortPropertyValueByIndex("power", i);

        total_electrical_out_current = total_optical_in_power * _responsivity;
        getPortForModification("electrical_out")->setPortPropertyValueByIndex("current", 0, total_electrical_out_current);

        total_dissipated_power = total_optical_in_power * (1.00 - _optical_power_ratio);
        assert(total_dissipated_power>=0);
        _device_power = total_dissipated_power;
    }

    void Photodetector::initializeMonitoring()
    {
        unsigned int number_wavelength = 0;
        unsigned int i = 0;

        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if( it->first=="optical_in" )
            {
                number_wavelength = getPort(it->first)->getPortPropertySize("wavelength");
            
                for(i=0; i<number_wavelength; ++i)
                    fprintf(it->second, "%.12f ", getPort(it->first)->getPortPropertyValueByIndex("wavelength", i) );
                fprintf(it->second, "\n\n\n");
            }
        }
    }

    void Photodetector::printMonitoredResult()
    {
        unsigned int number_wavelength = 0;
        unsigned int i = 0;

        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if ( it->first == "electrical_out" )
                fprintf(it->second, "%.9f\n", getPort("electrical_out")->getPortPropertyValueByIndex("current", 0) );
            else // other optical ports
            {
                number_wavelength = getPort(it->first)->getPortPropertySize("power");
                
                for(i=0; i<number_wavelength; ++i)
                    fprintf(it->second, "%.9f ", getPort(it->first)->getPortPropertyValueByIndex("power", i) );
                fprintf(it->second, "\n");
            }
        }
    }

    void Photodetector::printDefinition(FILE* device_list_file)
    {
        DeviceModel::printDefinition(device_list_file);

        fprintf(device_list_file, "    [port]\n");
        fprintf(device_list_file, "        Name: optical_in, Type: %s, Property: power(%d), wavlength(%d)\n", 
                ((int)getPort("optical_in")->getPortType()==0)?"INPUT":"OUTPUT", getPort("optical_in")->getPortPropertySize("power"), getPort("optical_in")->getPortPropertySize("wavelength"));
        fprintf(device_list_file, "        Name: electrical_out, Type: %s, Property: current(%d)\n", 
                ((int)getPort("electrical_out")->getPortType()==0)?"INPUT":"OUTPUT", getPort("electrical_out")->getPortPropertySize("current"));
        fprintf(device_list_file, "\n");

    }
}

