
#ifndef __THERMAL_TEMPERATURE_H__
#define __THERMAL_TEMPERATURE_H__

#include <vector>

#include "source/models/thermal_model/floorplan.h"
#include "source/models/thermal_model/rc_model.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class Temperature
    {
    public:
        Temperature();
        ~Temperature();

        void setFloorplanHolder(const FloorplanHolder* floorplan_holder)
        { _floorplan_holder = floorplan_holder; }
        void setRCModelHolder(const RCModelHolder* rc_model_holder)
        { _rc_model_holder = rc_model_holder; }
        void setSamplingInterval(double sampling_intvl)
        { _sampling_intvl = sampling_intvl; }
        
        void initialize(double initial_temperature, double ambient_temperature);
        void updateTransientTemperature(Time time_elapsed_since_last_update);

    protected:
        void updateTemperatureData();
        void computeTransientTemperatureFromPower();
        void setInternalPower(double ambient_temp);

    private:
        const FloorplanHolder*  _floorplan_holder;
        const RCModelHolder*    _rc_model_holder;

        double                  _sampling_intvl;
        
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
