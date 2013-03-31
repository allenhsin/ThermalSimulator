
#include <stddef.h>
#include <stdio.h>
#include <cassert>

#include "source/models/physical_model/device_floorplan_map.h"
#include "source/models/physical_model/physical_constants.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

using std::string;
using LibUtil::String;

namespace Thermal
{
    DeviceFloorplanMap::DeviceFloorplanMap()
    {
        _floorplan_map.clear();
    }
    
    DeviceFloorplanMap::~DeviceFloorplanMap()
    {}

    void DeviceFloorplanMap::loadFloorplanMap(std::string floorplan_map_file)
    {
        char    line[LINE_SIZE];
        string  line_copy;
        char*   line_token;
        char    instance_name[STR_SIZE];
        char    floorplan_unit_name[STR_SIZE];

        assert(_floorplan_map.size()==0);

        FILE* fp = fopen (floorplan_map_file.c_str(), "r");
        if(!fp)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open floorplan map file: " + floorplan_map_file + "\n");

        fseek(fp, 0, SEEK_SET);
        while(!feof(fp))
        {
            // read a line from the file
            fgets(line, LINE_SIZE, fp);
            if (feof(fp))
                break;

            line_copy = (string) line;
            line_token = strtok(line, " \r\t\n");
            
            // ignore comments and empty lines
            if      (!line_token || line_token[0] == '#')
                continue;
            else
            {
                if (sscanf(line_copy.c_str(), "%s%s", instance_name, floorplan_unit_name) == 2) 
                    _floorplan_map[(string) instance_name] = (string) floorplan_unit_name;
                else
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: wrong floorplan map file format.\n");
                
                Misc::isEndOfLine(1);
            }
        }

        fclose(fp);
    }
    
    string DeviceFloorplanMap::getFloorplanUnitNameFromInstanceName(std::string instance_name)
    {
        if(!_floorplan_map.count(instance_name))
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Unrecognized device instance name: " + (String) instance_name);

        return _floorplan_map[instance_name];
    }
}

