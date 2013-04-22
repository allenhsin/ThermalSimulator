
#include <cassert>
#include <stddef.h>
#include <stdio.h>

#include "source/models/physical_model/device_monitor.h"
#include "source/models/physical_model/physical_constants.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

using std::string;
using std::vector;
using LibUtil::String;

namespace Thermal
{
    DeviceMonitor::DeviceMonitor()
    {
        _monitored_devices.clear();
        _monitoring_time_duration.clear();
        _monitoring_time_point.clear();
    }

    DeviceMonitor::~DeviceMonitor()
    {}

    void DeviceMonitor::loadMonitoredDevices(string device_monitor_list_file, vector<DeviceModel*>& device_references)
    {
        assert(_monitored_devices.size()==0);
        assert(_monitoring_time_point.size()==0);
        assert(_monitoring_time_duration.size()==0);

        char    line[LINE_SIZE];
        char    line_copy[LINE_SIZE];
        char*   line_token;
        int     line_number = 0;

        char            name[STR_SIZE];
        DeviceModel*    monitored_device;
        unsigned int    number_monitoring_time_point = 0;
        double          current_monitoring_time_point = 0;
        double          current_monitoring_time_duration = 0;

        enum MonitorType
        {
            TYPE_DEVICE_LIST,
            TYPE_TIME,
            TYPE_NULL
        };
        MonitorType monitor_type = TYPE_NULL;

        FILE* fp = fopen(device_monitor_list_file.c_str(), "r");
        if (!fp)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open monitor list file.\n");

        while(!feof(fp))
        {
            // read a line from the file
            fgets(line, LINE_SIZE, fp);
            line_number++;
            if (feof(fp))
                break;

            strcpy(line_copy, line);
            line_token = strtok(line, " \r\t\n");
            
            // ignore comments and empty lines
            if      (!line_token || line_token[0] == '#')
                continue;
            else if (!strcmp(line_token, "[device_list]"))
            {   
                monitor_type = TYPE_DEVICE_LIST;
                Misc::isEndOfLine(0);
            }
            else if (!strcmp(line_token, "[time]"))
            {   
                monitor_type = TYPE_TIME;
                Misc::isEndOfLine(0);
            }
            else
            {
                switch(monitor_type)
                {
                case TYPE_DEVICE_LIST:
                    // check if the beginning is '<' --------------------------
                    if(line_token[0]!='<')
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Not start with '<' in file: " + device_monitor_list_file + " at line " + (String) line_number + ".\n");
                    // --------------------------------------------------------

                    // check if the end is '>' --------------------------------
                    do
                    {
                        strcpy(name, line_token);
                        line_token = strtok(NULL, " \r\t\n");
                    }
                    while(line_token);
                    if(name[strlen(name)-1]!='>')
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Not end with '>' in file: " + device_monitor_list_file + " at line " + (String) line_number + ".\n");
                    // --------------------------------------------------------

                    // get device instance name -------------------------------
                    line_token = strtok(line_copy, "<>, \r\t\n");
                    for(vector<DeviceModel*>::iterator it = device_references.begin(); it != device_references.end(); ++it)
                    {
                        if( ((string) line_token) == (*it)->getInstanceName() )
                        {
                            monitored_device = (*it);
                            break;
                        }
                    }
                    // --------------------------------------------------------

                    // set monitored ports of this device ---------------------
                    line_token = strtok(NULL, "<>, \r\t\n");
                    while(line_token)
                    {
                        monitored_device->setMonitoredPort( (string) line_token );
                        line_token = strtok(NULL, "<>, \r\t\n");
                    }
                    // --------------------------------------------------------
                    
                    _monitored_devices.push_back(monitored_device);

                    break;

                case TYPE_TIME:
                    // set monitoring time point and duration -----------------
                    if(sscanf(line_token, "%lf-%lf", &current_monitoring_time_point, &current_monitoring_time_duration)!=2)
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wrong time format in file: " + device_monitor_list_file + " at line " + (String) line_number + ".\n");
                    
                    if( (number_monitoring_time_point > 0) && 
                        (current_monitoring_time_point <= (_monitoring_time_point[number_monitoring_time_point-1]+_monitoring_time_duration[number_monitoring_time_point-1])) 
                      )
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Monitoring time should be in ascending order in file: " + device_monitor_list_file + " at line " + (String) line_number + ".\n");
                    
                    _monitoring_time_point.push_back(current_monitoring_time_point);
                    _monitoring_time_duration.push_back(current_monitoring_time_duration);

                    number_monitoring_time_point++;
                    Misc::isEndOfLine(1);
                    // --------------------------------------------------------

                    break;

                default: // TYPE_NULL
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Invalid monitor list file syntax in file: " + device_monitor_list_file + " at line " + (String) line_number + ".\n");
                    break;
                } // switch
            }
        } // while

        fclose(fp);
    }

    void DeviceMonitor::startup(string device_monitor_list_file, vector<DeviceModel*>& device_references)
    {
        loadMonitoredDevices(device_monitor_list_file, device_references);
    }
    


} // namespace Thermal

