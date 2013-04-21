
#ifndef __THERMAL_DEVICE_FLOORPLAN_MAP_H__
#define __THERMAL_DEVICE_FLOORPLAN_MAP_H__

#include <map>
#include <string>

#define NO_MAPPED_NAME "NO MAPPED NAME"
#define NO_INSTANCE_NAME "NO INSTANCE NAME"

namespace Thermal
{
    class DeviceFloorplanMap
    {
    public:
        DeviceFloorplanMap();
        ~DeviceFloorplanMap();
        
        void loadFloorplanMap(std::string floorplan_map_file);
        std::string getFloorplanUnitNameFromInstanceName(std::string instance_name) const;

        bool isFloorplanMapLoaded() const { return _floorplan_map_loaded; }

    protected:


    private:
        bool _floorplan_map_loaded;

        // instance_name -> floorplan_unit_name
        std::map<std::string, std::string> _floorplan_map;

    }; // class DeviceFloorplanMap

} // namespace Thermal

#endif // __THERMAL_DEVICE_FLOORPLAN_MAP_H__
