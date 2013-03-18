
#ifndef __THERMAL_TEMPERATURE_H__
#define __THERMAL_TEMPERATURE_H__

#include <vector>

#include "source/models/thermal_model/rc_model.h"

namespace Thermal
{
    class Temperature
    {
    public:
        Temperature();
        ~Temperature();

        void setThermalParameters(ThermalParameters* thermal_params);
        void setFloorplanHolder(FloorplanHolder* floorplan_holder);
        void setRCModelHolder(RCModelHolder* rc_model_holder);
        
        void setInitialTemperature();
        void updateTransientTemperature();

        // calculate avg sink temp for natural convection package model
        double getAvgSinkTemperature();

    protected:
        void updateDataTemperature();
        void readInitTemperatureFromFile();
        void computeTransientTemperatureFromPower();
        void setInternalPower();

    private:
        ThermalParameters*  _thermal_params;

        FloorplanHolder*    _floorplan_holder;
        RCModelHolder*      _rc_model_holder;
        
        // _temperature is in the floorplan unit order
        // for each layer, plus EXTRA nodes.
        // i.e. within each layer the order is the same
        // with _floorplan_holder->_flp_units[i]
        std::vector<double>      _temperature;
        // _power is in also in the floorplan unit
        // order but only with the floorplan unit nodes
        std::vector<double>      _power;

    }; // class Temperature

} // namespace Thermal

#endif // __THERMAL_TEMPERATURE_H__
