
#ifndef __THERMAL_DEVICE_MANAGER_H__
#define __THERMAL_DEVICE_MANAGER_H__

#include <vector>
#include <string>
#include <map>

#include "source/models/physical_model/physical_model.h"
#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "source/models/physical_model/device_monitor.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class DeviceManager : public PhysicalModel
    {
    public:
        DeviceManager();
        ~DeviceManager();

        bool startupManager();
        bool executeManager(Time scheduled_time);
        std::string getModelName() { return "Device Manager"; }

    protected:
        // Breadth-First Search to build device sequence
        void buildDeviceSequence();

    private:
        DeviceFloorplanMap*         _device_floorplan_map;
        DeviceMonitor*              _device_monitor;

        Time                        _sub_bit_sampling_intvl;

        // set of primitive devices for device instance
        // initialization. This is to avoid to read the
        // device definition file for multiple times.
        // string: the device type name
        std::map<std::string, DeviceModel*> _primitive_devices;

        // _device_instances are data placeholders 
        // resposible for device instance construction 
        // and destruction. The creation of the device 
        // instance is when loading the netlist.
        std::vector<DeviceModel*>   _device_instances;
        
        // after loading the netlist, device manager will
        // trace down the device instances connections to
        // figure out the device sequence along the link
        // and store it in _device_sequence.
        std::vector<DeviceModel*>   _device_sequence;

    }; // calss DeviceManager

} // namespace Thermal

#endif // __THERMAL_DEVICE_MANAGER_H__

