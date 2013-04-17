
#ifndef __THERMAL_TEMPERATURE_H__
#define __THERMAL_TEMPERATURE_H__

#include <vector>

#include "source/models/thermal_model/rc_model.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class Temperature
    {
    public:
        Temperature();
        ~Temperature();

        void setThermalParameters(ThermalParameters* thermal_params);
        void setFloorplanHolder(const FloorplanHolder* floorplan_holder);
        void setRCModelHolder(const RCModelHolder* rc_model_holder);
        
        void setInitialTemperature();
        void updateTransientTemperature(Time time_elapsed_since_last_update);

        // calculate avg sink temp for natural convection package model
        double getAvgSinkTemperature();

    protected:
        void updateDataTemperature();
        void loadInitTemperatureFromFile();
        void computeTransientTemperatureFromPower();
        void setInternalPower();

    private:
        ThermalParameters*      _thermal_params;

        const FloorplanHolder*  _floorplan_holder;
        const RCModelHolder*    _rc_model_holder;

        bool                    _is_internal_power_set;
        
        // _temperature is in the floorplan unit order
        // for each layer, plus EXTRA nodes.
        // i.e. within each layer the order is the same
        // with _floorplan_holder->_flp_units[i]
        std::vector<double>      _temperature;
        // _power is in also in the floorplan unit order
        std::vector<double>      _power;

    }; // class Temperature

} // namespace Thermal

#endif // __THERMAL_TEMPERATURE_H__
