
#include <stddef.h>
#include <cassert>
#include <queue>

#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_manager.h"
#include "source/system/event_scheduler.h"
#include "source/misc/misc.h"
#include "source/data/data.h"
#include "libutil/LibUtil.h"

using std::vector;
using std::queue;

namespace Thermal
{
    DeviceManager::DeviceManager()
        : _physical_config          (NULL)
        , _device_floorplan_map     ( new DeviceFloorplanMap() )
        , _sub_bit_sampling_intvl   (0)
        , _last_execute_time        (0)
        , _ready_to_execute         (false)
    {
        _device_instances.clear();
        _device_sequence.clear();
    }

    DeviceManager::~DeviceManager()
    {   
        delete _device_floorplan_map;
        _device_floorplan_map = NULL;
        
        _physical_config = NULL;
        
        // delete device instance
        for (vector<DeviceModel*>::iterator it = _device_instances.begin() ; it != _device_instances.end(); ++it)
        { if(*it) { delete (*it); } }
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

    void DeviceManager::startup()
    {
        assert(_physical_config);

        LibUtil::Log::printLine("Startup Device Manager");

    // set device manager constants -------------------------------------------
        // sampling interval
        _sub_bit_sampling_intvl = getPhysicalConfig()->getFloat("device_manager/sub_bit_sampling_intvl");
    // ------------------------------------------------------------------------

    // Load Files and Initialize Devices --------------------------------------
        // load floorplan map file
        _device_floorplan_map->loadFloorplanMap( getPhysicalConfig()->getString("device_manager/flpmap_file") );

        // load device netlist
        //FIXME: hardcode device now just for test ------------------------------------------------
        _device_instances.push_back( DeviceModel::createDevice( LASER_SOURCE_OFF_CHIP, _physical_config, _device_floorplan_map) );
        _device_instances.push_back( DeviceModel::createDevice( LASER_SOURCE_OFF_CHIP, _physical_config, _device_floorplan_map) );

        //_device_instances[0]->setTargetPortName("in");
        //_device_instances[0]->setTargetPortConnectedPort( _device_instances[1]->getPort("out") );
        //END FIXME -------------------------------------------------------------------------------

    // ------------------------------------------------------------------------

    // Build the Device Traverse Sequence (Breath-First) ----------------------
        assert(_device_sequence.size()==0);
        // build device sequence and stroe sequence in _device_sequence
        buildDeviceSequence();
        assert(_device_sequence.size()==_device_instances.size());
    // ------------------------------------------------------------------------

    // Initialize Devices -----------------------------------------------------
        for (vector<DeviceModel*>::iterator it=_device_sequence.begin(); it!=_device_sequence.end(); ++it)
            (*it)->initializeDevice();
    // ------------------------------------------------------------------------

    // Print Device list if debug enabled -------------------------------------
        if( getPhysicalConfig()->getBool("device_manager/debug_print_enable") )
        {
            FILE* device_list_file = fopen(getPhysicalConfig()->getString("device_manager/debug_print_device_file").c_str(), "w");
            
            for (vector<DeviceModel*>::iterator it=_device_sequence.begin(); it!=_device_sequence.end(); ++it)
                (*it)->printDefinition(device_list_file);

            fclose(device_list_file);
        }
    // ------------------------------------------------------------------------

    // Schedule the first event -----------------------------------------------
        EventScheduler::getSingleton()->enqueueEvent(_sub_bit_sampling_intvl, PHYSICAL_MODEL);
    // ------------------------------------------------------------------------

        _last_execute_time = 0;
        _ready_to_execute = true;
    } // startup

    void DeviceManager::execute(double scheduled_time)
    {
        assert(_ready_to_execute);
        LibUtil::Log::printLine("Execute Device Manager");

        double time_since_last_update = scheduled_time - _last_execute_time;
        
    // Execute devices --------------------------------------------------------
        for (vector<DeviceModel*>::iterator it=_device_sequence.begin(); it!=_device_sequence.end(); ++it)
            (*it)->updateDeviceProperties(time_since_last_update);
    // ------------------------------------------------------------------------

    // Schedule the next event ------------------------------------------------
    if(scheduled_time < 100*_sub_bit_sampling_intvl )
        EventScheduler::getSingleton()->enqueueEvent( (scheduled_time + _sub_bit_sampling_intvl), PHYSICAL_MODEL);
    else
        EventScheduler::getSingleton()->finish();
    // ------------------------------------------------------------------------

        _last_execute_time = scheduled_time;
    } // execute

} // namespace Thermal

