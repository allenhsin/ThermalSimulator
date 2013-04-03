
#include <stddef.h>
#include <cassert>

#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_manager.h"
#include "libutil/LibUtil.h"

using std::vector;

namespace Thermal
{
    DeviceManager::DeviceManager()
        : _physical_config          (NULL)
        , _device_floorplan_map     ( new DeviceFloorplanMap() )
    {
        _device_instances.clear();
        _device_root_instances.clear();
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

    void DeviceManager::startup()
    {
        assert(_physical_config);

    // Load Files -------------------------------------------------------------

        // load floorplan map file
        _device_floorplan_map->loadFloorplanMap( getPhysicalConfig()->getString("env_setup/flpmap_file") );

        // load device netlist
        //FIXME: instantiate device manually now just for test
        _device_instances.push_back( DeviceModel::createDevice( RESONANT_RING_MODULATOR, _physical_config, _device_floorplan_map) );
        _device_instances[0]->printDefinition();

    // ------------------------------------------------------------------------

    // Build the Device Traverse Sequence (Breath-First) ----------------------

    // ------------------------------------------------------------------------
    }


} // namespace Thermal

