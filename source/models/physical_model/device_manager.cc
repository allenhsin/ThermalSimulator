
#include <stddef.h>
#include <cassert>
#include <queue>

#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_manager.h"
#include "libutil/LibUtil.h"

using std::vector;
using std::queue;

namespace Thermal
{
    DeviceManager::DeviceManager()
        : _physical_config          (NULL)
        , _device_floorplan_map     ( new DeviceFloorplanMap() )
        , _number_devices           (0)
    {
        _device_instances.clear();
        _device_root_instances.clear();
        _device_sequence.clear();
    }

    DeviceManager::~DeviceManager()
    {   
        delete _device_floorplan_map;
        _device_floorplan_map = NULL;
        
        _physical_config = NULL;
        
        // delete device instance (non-root)
        for (vector<DeviceModel*>::iterator it = _device_instances.begin() ; it != _device_instances.end(); ++it)
        { if(*it) { delete (*it); } }
        // delete device instance (root)
        for (vector<DeviceModel*>::iterator itr = _device_root_instances.begin() ; itr != _device_root_instances.end(); ++itr)
        { if(*itr) { delete (*itr); } }
    }

    void DeviceManager::buildDeviceSequence()
    {   
        if(_device_root_instances.size() == 0)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Links must have laser sources or drivers.\n");

        int i, j;
        bool all_parent_traversed = false;
        queue<DeviceModel*> device_search_queue;
        vector<DeviceModel*> child_device_list;
        vector<DeviceModel*> parent_device_list;

        DeviceModel* current_device = NULL;
        DeviceModel* current_child_device = NULL;
        
        // push root instances into the queue
        for(i=0; i<(int)_device_root_instances.size(); ++i)
            device_search_queue.push(_device_root_instances[i]);
        
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
                    if(!parent_device_list[j]->getTraversedInBFS())
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

    // Load Files and Initialize Devices --------------------------------------
        // load floorplan map file
        _device_floorplan_map->loadFloorplanMap( getPhysicalConfig()->getString("env_setup/flpmap_file") );

        // load device netlist
        //FIXME: hardcode device now just for test ------------------------------------------------
        _device_instances.push_back( DeviceModel::createDevice( RESONANT_RING_MODULATOR, _physical_config, _device_floorplan_map) );
        //_device_instances[0]->printDefinition();

        _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _physical_config, _device_floorplan_map) );
        //_device_instances[1]->printDefinition();

        _device_instances.push_back( DeviceModel::createDevice( RESONANT_RING_MODULATOR, _physical_config, _device_floorplan_map) );
        //_device_instances[2]->printDefinition();

        _device_root_instances.push_back( DeviceModel::createDevice( LASER_SOURCE_OFF_CHIP, _physical_config, _device_floorplan_map) );
        //_device_root_instances[0]->printDefinition();

        _device_root_instances.push_back( DeviceModel::createDevice( LASER_SOURCE_OFF_CHIP, _physical_config, _device_floorplan_map) );
        //_device_root_instances[1]->printDefinition();

        _device_instances[0]->setTargetPortName("in");
        _device_instances[0]->setTargetPortConnectedPort( _device_instances[1]->getPort("out") );

        _device_instances[1]->setTargetPortName("out");
        _device_instances[1]->setTargetPortConnectedPort( _device_instances[0]->getPort("in") );
        
        _device_instances[1]->setTargetPortName("in");
        _device_instances[1]->setTargetPortConnectedPort( _device_root_instances[0]->getPort("out") );
        
        _device_root_instances[0]->setTargetPortName("out");
        _device_root_instances[0]->setTargetPortConnectedPort( _device_instances[1]->getPort("in") );
        
        _device_instances[0]->setTargetPortName("thru");
        _device_instances[0]->setTargetPortConnectedPort( _device_instances[2]->getPort("in") );

        _device_instances[2]->setTargetPortName("in");
        _device_instances[2]->setTargetPortConnectedPort( _device_instances[0]->getPort("thru") );

        _device_instances[2]->setTargetPortName("add");
        _device_instances[2]->setTargetPortConnectedPort( _device_root_instances[1]->getPort("out") );

        _device_root_instances[1]->setTargetPortName("out");
        _device_root_instances[1]->setTargetPortConnectedPort( _device_instances[2]->getPort("add") );

        //END FIXME -------------------------------------------------------------------------------

        
        // update the total number of devices
        _number_devices = _device_instances.size() + _device_root_instances.size();
    // ------------------------------------------------------------------------

    // Build the Device Traverse Sequence (Breath-First) ----------------------
        assert(_device_sequence.size()==0);
        // build device sequence and stroe sequence in _device_sequence
        buildDeviceSequence();
        assert(_device_sequence.size()==_number_devices);
    // ------------------------------------------------------------------------
    }
    
    // TODO: 
    void DeviceManager::execute()
    {
    }

} // namespace Thermal

