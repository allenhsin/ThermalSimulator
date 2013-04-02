
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
    }

    DeviceManager::~DeviceManager()
    {   
        delete _device_floorplan_map;
        _device_floorplan_map = NULL;
        
        _physical_config = NULL;

        for (vector<DeviceModel*>::iterator it = _device_instances.begin() ; it != _device_instances.end(); ++it)
        {
            if(*it)
            {
                delete (*it);
                *it = NULL;
            }
        }
    }

    void DeviceManager::startup()
    {
        assert(_physical_config);
        
        // load the floorplan map file
        _device_floorplan_map->loadFloorplanMap( getPhysicalConfig()->getString("env_setup/flpmap_file") );

        // load the link netlist
        //FIXME: instantiate each device just for test now
        _device_instances.push_back( DeviceModel::createDevice( (int) RESONANT_RING_MODULATOR, _physical_config, _device_floorplan_map) );

    }


} // namespace Thermal

