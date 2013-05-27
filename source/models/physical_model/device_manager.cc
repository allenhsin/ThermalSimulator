
#include <stddef.h>
#include <cassert>
#include <queue>

#include "source/models/physical_model/verilog_to_thermal.h"
#include "source/models/physical_model/device_manager.h"
#include "source/system/event_scheduler.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

using std::vector;
using std::queue;

namespace Thermal
{
    DeviceManager::DeviceManager()
        : PhysicalModel()
        , _device_floorplan_map     ( new DeviceFloorplanMap() )
        , _device_monitor           ( new DeviceMonitor() )
        , _sub_bit_sampling_intvl   (0)
    {
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

    // check if manager is enabled --------------------------------------------
        if(!_config->getBool("device_manager/enable"))
        {
            LibUtil::Log::printLine( "    Device Manager not enabled" );
            return false;
        }
    // ------------------------------------------------------------------------

    // set device manager constants -------------------------------------------
        // sampling interval
        _sub_bit_sampling_intvl = _config->getFloat("device_manager/sub_bit_sampling_intvl");
    // ------------------------------------------------------------------------

    // Load Files and Initialize Devices --------------------------------------
        // load floorplan map file
        _device_floorplan_map->loadFloorplanMap( _config->getString("device_manager/flpmap_file") );

        // load device netlist
        vector<string> files;
        files.push_back("configs/physical_model/device_primitives.v");
        files.push_back("tests/test.v");
        VerilogToThermal::dumpDevicesFromVerilog(this, _config, _device_floorplan_map, files, "TestTop");

        //FIXME: hardcode device now just for test ------------------------------------------------

        // waveguide x 3, laser, modulator, modulator driver, thermal tuner, receiver ring.
        _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        _device_instances.push_back( DeviceModel::createDevice( RESONANT_RING, _config, _device_floorplan_map) );
        _device_instances.push_back( DeviceModel::createDevice( RESONANT_RING_DEPLETION_MODULATOR, _config, _device_floorplan_map) );
        _device_instances.push_back( DeviceModel::createDevice( LASER_SOURCE_OFF_CHIP, _config, _device_floorplan_map) );
        _device_instances.push_back( DeviceModel::createDevice( MODULATOR_DRIVER, _config, _device_floorplan_map) );
        _device_instances.push_back( DeviceModel::createDevice( THERMAL_TUNER, _config, _device_floorplan_map) );
        
        _device_instances[0]->setDeviceName("waveguide_0");
        _device_instances[1]->setDeviceName("waveguide_1");
        _device_instances[2]->setDeviceName("waveguide_2");
        _device_instances[3]->setDeviceName("receiver_ring");
        _device_instances[4]->setDeviceName("modulator");
        _device_instances[5]->setDeviceName("laser_source");
        _device_instances[6]->setDeviceName("modulator_driver");
        _device_instances[7]->setDeviceName("thermal_tuner");
        
        // laser -> waveguide_0
        _device_instances[5]->setTargetPortName("out");
        _device_instances[5]->setTargetPortConnectedPort( _device_instances[0]->getPort("in") );
        _device_instances[0]->setTargetPortName("in");
        _device_instances[0]->setTargetPortConnectedPort( _device_instances[5]->getPort("out") );

        // modulator driver -> modulator
        _device_instances[6]->setTargetPortName("out");
        _device_instances[6]->setTargetPortConnectedPort( _device_instances[4]->getPort("mod_driver") );
        _device_instances[4]->setTargetPortName("mod_driver");
        _device_instances[4]->setTargetPortConnectedPort( _device_instances[6]->getPort("out") );

        // thermal_tuner -> modulator
        _device_instances[7]->setTargetPortName("heater");
        _device_instances[7]->setTargetPortConnectedPort( _device_instances[4]->getPort("heater") );
        _device_instances[4]->setTargetPortName("heater");
        _device_instances[4]->setTargetPortConnectedPort( _device_instances[7]->getPort("heater") );

        // waveguide_0 -> modulator
        _device_instances[0]->setTargetPortName("out");
        _device_instances[0]->setTargetPortConnectedPort( _device_instances[4]->getPort("in") );
        _device_instances[4]->setTargetPortName("in");
        _device_instances[4]->setTargetPortConnectedPort( _device_instances[0]->getPort("out") );
        
        // modulator -> waveguide_1
        _device_instances[4]->setTargetPortName("thru");
        _device_instances[4]->setTargetPortConnectedPort( _device_instances[1]->getPort("in") );
        _device_instances[1]->setTargetPortName("in");
        _device_instances[1]->setTargetPortConnectedPort( _device_instances[4]->getPort("thru") );
        
        // waveguide_1 -> receiver ring
        _device_instances[1]->setTargetPortName("out");
        _device_instances[1]->setTargetPortConnectedPort( _device_instances[3]->getPort("in") );
        _device_instances[3]->setTargetPortName("in");
        _device_instances[3]->setTargetPortConnectedPort( _device_instances[1]->getPort("out") );
        
        // receiver_ring -> waveguide_2
        _device_instances[3]->setTargetPortName("thru");
        _device_instances[3]->setTargetPortConnectedPort( _device_instances[2]->getPort("in") );
        _device_instances[2]->setTargetPortName("in");
        _device_instances[2]->setTargetPortConnectedPort( _device_instances[3]->getPort("thru") );

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

