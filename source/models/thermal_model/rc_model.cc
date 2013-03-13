
#include "cassert"

#include "source/models/thermal_model/rc_model.h"

namespace Thermal
{

    RCModel::RCModel()
        : _thermal_params   (NULL)
        , _floorplan_holder (NULL)
        , _rc_model_holder  (NULL)
    {}

    RCModel::~RCModel()
    {
        if(_rc_model_holder)
            delete _rc_model_holder;
    }

    void RCModel::setThermalParameters(ThermalParameters* thermal_params)
    { _thermal_params = thermal_params; }

    void RCModel::setFloorplanHolder(FloorplanHolder* floorplan_holder)
    { _floorplan_holder = floorplan_holder; }
    
    void allocateRCModelHolder()
    {
        assert(_floorplan_holder);

        int n = _floorplan_holder->_n_units;

    }

} // namespace Thermal

