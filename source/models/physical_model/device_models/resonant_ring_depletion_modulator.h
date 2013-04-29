
#ifndef __THERMAL_RESONANT_RING_DEPLETION_MODULATOR_H__
#define __THERMAL_RESONANT_RING_DEPLETION_MODULATOR_H__

#include <set>

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "source/misc/common_types.h"

namespace Thermal
{
    class ResonantRingDepletionModulator : public DeviceModel
    {
    public:
        ResonantRingDepletionModulator(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        ~ResonantRingDepletionModulator();

        // initialize device
        virtual void initializeDevice();

        // update device properties
        virtual void updateDeviceProperties(Time time_elapsed_since_last_update);

        virtual void printDefinition(FILE* device_list_file);

        virtual void initializeMonitoring();
        virtual void printMonitoredResult();

    protected:
        // check the validity of device parameters
        virtual void deviceParameterCheck();

    private:
        // the set of output wavelengths from in port and add port
        set<double> _output_wavelengths;
        // number of output wavelength
        unsigned int _number_wavelength;

        // precompute parts of p-n junction built-in voltage
        double _v_bi_partial;
        // precompute parts of p-n junction open-circuit depletion width
        double _w_dep_partial;
        // precompute parts of p-n junction open-circuit cap
        double _c_j0_partial;
        // precompute parts of change in carrier density
        double _delta_N_partial;
        // ring one-round phase shift
        double _theta_partial;
        // ring one-round power loss
        double _alpha;
        // other stuff
        double _t2_sq_alpha_sq_plus_t1_sq;
        double _t1_sq_alpha_sq_plus_t2_sq;
        double _two_t1_t2_alpha;
        double _one_plus_sq_of_t1_t2_alpha;
        double _one_t1_t2_alpha;
        
        // temperature from last update -> avoid recomputation
        double _last_temperature;
        // voltage from last update -> avoid recomputation
        double _last_voltage;
        // p-n junction built-in voltage
        double _v_bi;
        // p-n junction open-circuit cap
        double _c_j0;
        // effective refractive index
        double _n_eff;

    }; // class ResonantRingDepletionModulator

} // namespace Thermal

#endif // __THERMAL_RESONANT_RING_DEPLETION_MODULATOR_H__

