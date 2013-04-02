
#include <cassert>
#include <stddef.h>

#include "source/misc/misc.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_models/resonant_ring_modulator.h"
#include "source/models/physical_model/physical_constants.h"
#include "libutil/LibUtil.h"

using std::string;
using LibUtil::String;

namespace Thermal
{
    DeviceModel::DeviceModel(DeviceFloorplanMap* device_floorplan_map, string device_definition_file)
        : _instance_name            ("")
        , _floorplan_unit_name      ("")
        
        , _device_definition_file   (device_definition_file)
        , _device_floorplan_map     (device_floorplan_map)
        , _target_parameter_name    ("")
        , _target_port_name         ("")
    {
        _device_port_connections.clear();
        _device_parameters.clear();
        _device_properties.clear();
        
        // initilize the device's ports/parameters/properties
        loadDeviceDefinition();
    }

    DeviceModel::~DeviceModel()
    {
        _device_floorplan_map   = NULL;
    }

    DeviceModel*
    DeviceModel::createDevice(int device_type, config::Config* physical_config, DeviceFloorplanMap* device_floorplan_map)
    {
        DeviceModel* device_model;

        switch(device_type)
        {
        case RESONANT_RING:
            //device_model = new ResonantRing( device_floorplan_map, physical_config->getString("device/resonant_ring/def_file") );
            break;

        case RESONANT_RING_MODULATOR:
            device_model = new ResonantRingModulator( device_floorplan_map, physical_config->getString("device/resonant_ring_modulator/def_file") );
            break;

        case LOSSY_OPTICAL_NET:
            //device_model = new LossyOpticalNet( device_floorplan_map, physical_config->getString("device/lossy_optical_net/def_file") );
            break;

        case LASER_SOURCE_OFF_CHIP:
            //device_model = new LaserSourceOffChip( device_floorplan_map, physical_config->getString("device/laser_source_off-chip/def_file") );
            break;

        case LASER_SOURCE_ON_CHIP:
            //device_model = new LaserSourceOnChip( device_floorplan_map, physical_config->getString("device/laser_source_on-chip/def_file") );
            break;

        case PHOTODETECTOR:
            //device_model = new Photodetector( device_floorplan_map, physical_config->getString("device/photodetector/def_file") );
            break;

        case RECEIVER:
            //device_model = new Receiver( device_floorplan_map, physical_config->getString("device/receiver/def_file") );
            break;

        default:
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized Device Type: " + (String) device_type);
            return NULL;
        }

        return device_model;
    }

    // TODO: 
    void DeviceModel::loadDeviceDefinition()
    {
        assert(_device_port_connections.size()==0);
        assert(_device_parameters.size()==0);
        assert(_device_properties.size()==0);

        char    line[LINE_SIZE];
        string  line_copy;
        char*   line_token;
        int     line_number = 0;

        char    name[STR_SIZE];
        double  value;

        enum DefType
        {
            TYPE_PORT,
            TYPE_PARAMETER,
            TYPE_PROPERTY,
            TYPE_NULL
        };
        
        DefType def_type = TYPE_NULL;


        FILE* def_file = fopen(_device_definition_file.c_str(), "r");
        if (!def_file)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open def file.\n");

        while(!feof(def_file))
        {
            // read a line from the file
            fgets(line, LINE_SIZE, def_file);
            line_number++;
            if (feof(def_file))
                break;

            line_copy = (string) line;
            line_token = strtok(line, " \r\t\n");
            
            // ignore comments and empty lines
            if      (!line_token || line_token[0] == '#')
                continue;
            else if (!strcmp(line_token, "[port]"))
            {   
                def_type = TYPE_PORT;
                Misc::isEndOfLine(0);
            }
            else if (!strcmp(line_token, "[parameter]"))
            {
                def_type = TYPE_PARAMETER;
                Misc::isEndOfLine(0);
            }
            else if (!strcmp(line_token, "[property]"))
            {
                def_type = TYPE_PROPERTY;
                Misc::isEndOfLine(0);
            }
            else
            {
                switch(def_type)
                {
                case TYPE_PORT:
                    _device_port_connections[(string) line_token] = NULL;
                    Misc::isEndOfLine(0);
                    break;
                        
                case TYPE_PROPERTY:
                    _device_properties[(string) line_token] = 0;
                    Misc::isEndOfLine(0);
                    break;

                case TYPE_PARAMETER:
                    if( sscanf(line_copy.c_str(), " %[^= ]=%lf", name, &value) == 1)
                        _device_parameters[(string) name] = value;
                    else
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: invalid def file parameter syntax in file: " + _device_definition_file + " at line " + (String) line_number + "\n");
                    Misc::isEndOfLineWithEqual(1);
                    break;
                
                default: // TYPE_NULL
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: invalid def file syntax in file: " + _device_definition_file + " at line " + (String) line_number + " \n");
                    return;
                }
            }
        } // while

        fclose(def_file);
    }

    void DeviceModel::setDeviceInstanceAndFloorplanUnitName(string instance_name)
    {
        _instance_name          = instance_name;
        _floorplan_unit_name    = _device_floorplan_map->getFloorplanUnitNameFromInstanceName(instance_name);
    }

    bool DeviceModel::hasParameter(string parameter_name)
    { return _device_parameters.count(parameter_name); }

    void DeviceModel::setTargetParameterName(string parameter_name)
    { 
        if(!hasParameter(parameter_name))
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized parameter: " + (String) parameter_name);
        _target_parameter_name = parameter_name; 
    }

    void DeviceModel::setTargetParameterValue(double parameter_value)
    {
        assert(_target_parameter_name != "");
        _device_parameters[_target_parameter_name] = parameter_value;
    }

    bool DeviceModel::hasPort(string port_name)
    { return _device_port_connections.count(port_name); }

    void DeviceModel::setTargetPortName(string port_name)
    { 
        if(!hasParameter(port_name))
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized port: " + (String) port_name);
        _target_port_name = port_name; 
    }

    void DeviceModel::setTargetPortConnectedDevice(DeviceModel* device)
    {
        assert(_target_port_name != "");
        _device_port_connections[_target_port_name] = device;
    }


} // namespace Thermal

