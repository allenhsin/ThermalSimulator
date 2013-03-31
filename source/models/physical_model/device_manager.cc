
#include "source/models/physical_model/device_manager.cc"

using std::vector;

namespace Thermal
{
    DeviceManager::DeviceManager()
        : _device_floorplan_map (new DeviceFloorplanMap())
    {
        _device_instances.clear();
    }

    DeviceManager::~DeviceManager()
    {   
        delete _device_floorplan_map;
        _device_floorplan_map = NULL;

        for (vector<DeviceModel*>::iterator it = _device_instances.begin() ; it != _device_instance.end(); ++it)
        {
            if(*it)
            {
                delete (*it);
                *it = NULL;
            }
        }
    }

} // namespace Thermal

