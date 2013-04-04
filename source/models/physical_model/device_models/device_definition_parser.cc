
#include <stdio.h>
#include <cassert>
#include <stddef.h>
#include <stdlib.h>

#include "source/misc/misc.h"
#include "source/models/physical_model/device_models/device_definition_parser.h"
#include "source/models/physical_model/physical_constants.h"
#include "libutil/LibUtil.h"

using std::string;
using std::map;
using LibUtil::String;

namespace Thermal
{
    DeviceDefinitionParser::DeviceDefinitionParser(DeviceModel* device_model, port_name_map& device_ports, param_name_map& device_parameters)
        : _device_model     (device_model)
        , _device_ports     (device_ports)
        , _device_parameters (device_parameters)
    {}
    
    void DeviceDefinitionParser::loadDeviceDefinitionFile(string device_definition_file)
    {
        assert(_device_ports.size()==0);
        assert(_device_parameters.size()==0);

        char    line[LINE_SIZE];
        char    line_copy[LINE_SIZE];
        char*   line_token;
        int     line_number = 0;

        char        name[STR_SIZE];
        PortType    port_type = NULL_PORT;
        double      value;

        enum DefType
        {
            TYPE_PORT,
            TYPE_PARAMETER,
            TYPE_NULL
        };
        
        DefType def_type = TYPE_NULL;


        FILE* def_file = fopen(device_definition_file.c_str(), "r");
        if (!def_file)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open def file.\n");

        while(!feof(def_file))
        {
            // read a line from the file
            fgets(line, LINE_SIZE, def_file);
            line_number++;
            if (feof(def_file))
                break;

            strcpy(line_copy, line);
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
            else
            {
                switch(def_type)
                {
                case TYPE_PORT:
                    // check if the beginning is '<'
                    if(line_token[0]!='<')
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Not start with '<' in file: " + device_definition_file + " at line " + (String) line_number + ".\n");
                    
                    // check if the end is '>'
                    do
                    {
                        strcpy(name, line_token);
                        line_token = strtok(NULL, " \r\t\n");
                    }
                    while(line_token);
                    if(name[strlen(name)-1]!='>')
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Not end with '>' in file: " + device_definition_file + " at line " + (String) line_number + ".\n");

                    // get port name
                    line_token = strtok(line_copy, "<>, \r\t\n");
                    if(line_token)
                        strcpy(name, line_token);
                    else
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot read port name in file: " + device_definition_file + " at line " + (String) line_number + ".\n");

                    // get port type
                    line_token = strtok(NULL, "<>, \r\t\n");
                    if(line_token)
                    {
                        if      (!strcmp(line_token, "INPUT"))
                            port_type = INPUT_PORT;
                        else if (!strcmp(line_token, "OUTPUT"))
                            port_type = OUTPUT_PORT;
                        else if (!strcmp(line_token, "INOUT"))
                            port_type = INOUT_PORT;
                        else
                            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid port type in file: " + device_definition_file + " at line " + (String) line_number + ".\n");
                    }
                    else
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot read port type in file: " + device_definition_file + " at line " + (String) line_number + ".\n");

                    _device_ports[(string) name] = new Port(_device_model, port_type);

                    // add port properties
                    line_token = strtok(NULL, "<>, \r\t\n");
                    while(line_token)
                    {   
                        _device_ports[(string) name]->addPortProperty((string) line_token);
                        line_token = strtok(NULL, "<>, \r\t\n");
                    }

                    break;
                        
                case TYPE_PARAMETER:
                    if( sscanf(line_copy, " %[^= ] =%lf", name, &value) == 2)
                        _device_parameters[(string) name] = value;
                    else
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid def file parameter syntax in file: " + device_definition_file + " at line " + (String) line_number + ".\n");
                    Misc::isEndOfLineWithEqual(1);
                    break;
                
                default: // TYPE_NULL
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid def file syntax in file: " + device_definition_file + " at line " + (String) line_number + ".\n");
                    return;
                }
            }
        } // while

        fclose(def_file);
    }
} // namespace Thermal

