
#include <stddef.h>
#include <cassert>
#include <queue>

#include "source/models/physical_model/verilog_to_thermal.h"
#include "source/models/physical_model/device_manager.h"
#include "source/system/event_scheduler.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

using std::vector;
using std::map;
using std::queue;
using std::string;

namespace Thermal
{
    DeviceManager::DeviceManager()
        : PhysicalModel()
        , _device_floorplan_map     ( new DeviceFloorplanMap() )
        , _device_monitor           ( new DeviceMonitor() )
        , _sub_bit_sampling_intvl   (0)
    {
        _primitive_devices.clear();
        _device_instances.clear();
        _device_sequence.clear();
    }

    DeviceManager::~DeviceManager()
    {   
        delete _device_floorplan_map;
        delete _device_monitor;
        
        // delete device instance
        for (vector<DeviceModel*>::iterator it = _device_instances.begin() ; it != _device_instances.end(); ++it)
        { if(*it) { delete (*it); } }

        // delete primitive devices
        for (map<string, DeviceModel*>::iterator it_prim = _primitive_devices.begin(); it_prim != _primitive_devices.end(); ++it_prim)
        { if(it_prim->second) { delete (it_prim->second); } }
    }

    void DeviceManager::buildDeviceSequence()
    {   
        int i, j;
        bool all_parent_traversed = false;
        queue<DeviceModel*> device_search_queue;
        vector<DeviceModel*> child_device_list;
        vector<DeviceModel*> parent_device_list;

        DeviceModel* current_device = NULL;
        DeviceModel* current_child_device = NULL;
        
        // push root device into the queue
        for(i=0; i<(int)_device_instances.size(); ++i)
        {   
            if( _device_instances[i]->isRootDevice() )
                device_search_queue.push(_device_instances[i]);
        }
        
        // start BFS from the root instances
        while(!device_search_queue.empty())
        {
            // get the front device in the queue
            // and push back to the device sequence
            current_device = device_search_queue.front();
            current_device->setTraversedInBFS();
            _device_sequence.push_back(current_device);
            
            // get the child devices from the current device
            current_device->getChildDevices(child_device_list);
            // iterate thru the child devices
            for(i=0; i<(int)child_device_list.size(); ++i)
            {
                current_child_device = child_device_list[i];

                // get the parent devices of the child device
                current_child_device->getParentDevices(parent_device_list);
                // check if all the parent devices are traversed
                all_parent_traversed = true;
                for(j=0; j<(int)parent_device_list.size(); ++j)
                {
                    // if any of the parent device of this child
                    // device has not been traversed yet, it 
                    // cannot be put into the search sequence.
                    if(!parent_device_list[j]->isTraversedInBFS())
                    {   
                        all_parent_traversed = false;
                        break;
                    }
                }
                
                // if all the parent devices of this child device 
                // are already being traversed, then put this
                // child device into the search queue.
                if(all_parent_traversed)
                    device_search_queue.push(current_child_device);
            }

            device_search_queue.pop();
        }
    }

    bool DeviceManager::startupManager()
    {
        assert(_config);

    // Check if manager is enabled --------------------------------------------
        if(!_config->getBool("device_manager/enable"))
        {
            LibUtil::Log::printLine( "    Device Manager not enabled" );
            return false;
        }
    // ------------------------------------------------------------------------

    // Set device manager constants -------------------------------------------
        // sampling interval
        _sub_bit_sampling_intvl = _config->getFloat("device_manager/sub_bit_sampling_intvl");
    // ------------------------------------------------------------------------
    
    // Load floorplan map file ------------------------------------------------
        // load floorplan map file
        _device_floorplan_map->loadFloorplanMap( _config->getString("device_manager/flpmap_file") );
    // ------------------------------------------------------------------------

    // Load primitive devices -------------------------------------------------
        DeviceModel* temp_model = NULL;
        assert(_primitive_devices.size() == 0);
        for (int i = 0; i < NUM_DEVICE_TYPES; ++i)
        {
            temp_model = DeviceModel::createDevice((DeviceType) i, _config, _device_floorplan_map);
            _primitive_devices[temp_model->getDeviceTypeName()] = temp_model;
        }
    // ------------------------------------------------------------------------

    // Load Netlist File and Initialize Devices -------------------------------
        // Verilog files to read
        vector<string> files;
        // Verilog netlist that defines the connectivity
        files.push_back(_config->getString("device_manager/netlist_file"));
        // Dump devices from the netlist
        VerilogToThermal::dumpDevicesFromVerilog(_device_instances, _primitive_devices,
            files, _config->getString("device_manager/top_module"));
    // ------------------------------------------------------------------------

    // Build the Device Traverse Sequence (Breath-First) ----------------------
        assert(_device_sequence.size()==0);
        // build device sequence and stroe sequence in _device_sequence
        buildDeviceSequence();        
        assert(_device_sequence.size()==_device_instances.size());
    // ------------------------------------------------------------------------

    // Initialize Devices -----------------------------------------------------
        for (vector<DeviceModel*>::iterator it=_device_sequence.begin(); it!=_device_sequence.end(); ++it)
        {
            (*it)->initializeDevice();

            // add mapped devices to the floorplan unit power map
            if( (*it)->isMappedOnFloorplan() )
                addFloorplanUnit( (*it)->getFloorplanUnitName() );
        }
    // ------------------------------------------------------------------------

    // Start device monitoring ------------------------------------------------
        _device_monitor->startup( _config->getString("device_monitor/device_monitor_list"), _device_instances, _config->getString("device_monitor/results_dir") );
    // ------------------------------------------------------------------------

    // Print Device list if debug enabled -------------------------------------
        if( _config->getBool("device_manager/debug_print_enable") )
        {
            FILE* device_list_file = fopen(_config->getString("device_manager/debug_print_device_file").c_str(), "w");
            
            for (vector<DeviceModel*>::iterator it=_device_sequence.begin(); it!=_device_sequence.end(); ++it)
                (*it)->printDefinition(device_list_file);

            fclose(device_list_file);
        }
    // ------------------------------------------------------------------------

    // Schedule the first event -----------------------------------------------
        EventScheduler::getSingleton()->enqueueEvent(0, DEVICE_MANAGER);
    // ------------------------------------------------------------------------

        return true;
    } // startup

    bool DeviceManager::executeManager(Time scheduled_time)
    {
        if( (scheduled_time != 0) && !Misc::eqTime(scheduled_time,(_last_execute_time + _sub_bit_sampling_intvl)) )
        {
            LibUtil::Log::printLine("    Device properties not updated");
            return false;
        }
        Time time_since_last_update = scheduled_time - _last_execute_time;
        
    // Execute devices --------------------------------------------------------
        for (vector<DeviceModel*>::iterator it=_device_sequence.begin(); it!=_device_sequence.end(); ++it)
        {
            (*it)->updateDeviceProperties(time_since_last_update);

            // update the mapped device power numbers to the floorplan unit power map
            if( (*it)->isMappedOnFloorplan() )
                setFloorplanUnitPower( (*it)->getFloorplanUnitName(), (*it)->getDevicePower() );
        }
    // ------------------------------------------------------------------------

    // Execute device monitor -------------------------------------------------
        _device_monitor->execute(scheduled_time);
    // ------------------------------------------------------------------------

    // Schedule the next event ------------------------------------------------
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + _sub_bit_sampling_intvl), DEVICE_MANAGER);
    // ------------------------------------------------------------------------

        return true;
    } // execute

} // namespace Thermal

