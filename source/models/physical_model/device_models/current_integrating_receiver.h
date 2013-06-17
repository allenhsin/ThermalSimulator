
#ifndef __THERMAL_CURRENT_INTEGRATING_RECEIVER_H__
#define __THERMAL_CURRENT_INTEGRATING_RECEIVER_H__

#include "source/models/physical_model/device_models/device_model.h"
#include "source/models/physical_model/device_floorplan_map.h"
#include "source/misc/common_types.h"

namespace Thermal
{   
    enum SenseAmpState
    {
        STATE_INTEGRATION,
        STATE_EVALUATION,
        STATE_EVALUATED,
        NUM_STATES
    };

    class CurrentIntegratingReceiver : public DeviceModel
    {
    public:
        CurrentIntegratingReceiver(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file);
        CurrentIntegratingReceiver( const CurrentIntegratingReceiver& cloned_device);
        ~CurrentIntegratingReceiver();

        // initialize device
        virtual void initializeDevice();

        // update device properties
        virtual void updateDeviceProperties(Time time_elapsed_since_last_update);

        virtual void printDefinition(FILE* device_list_file);

        virtual void initializeMonitoring();
        virtual void printMonitoredResult();

        virtual CurrentIntegratingReceiver* clone() const;

    protected:
        // check the validity of device parameters
        virtual void deviceParameterCheck();

    private:
        unsigned int            _number_wavelength;
        
        double                  _pd_responsivity;

        double                  _sa_total_min_input_cap;
        int                     _sa_number_cap_deck_steps;
        int                     _sa_preset_threshold_code;
        double                  _sa_input_cap_deck_step_size;

        Time                    _sa_bit_period;
        Time                    _sa_sense_period;
        double                  _sa_regen_start_delta_input_voltage;
        double                  _sa_proportionality_const;
        double                  _sa_regen_time_const;
        double                  _sa_required_output;
        double                  _sa_total_common_mode_current;
        
        double                  _sa_circuit_current_noise_sigma;
        double                  _sa_offset_mismatch_volt;
        double                  _sa_supply_noise_random_sigma;
        double                  _sa_supply_noise_determ;
        double                  _sa_extra_noise_margin;
        
        double                  _sa_current_pd_branch_input_cap;
        double                  _sa_current_ref_branch_input_cap;
        int                     _sa_current_threshold_code;

        Time                    _sa_current_bit_elapsed_time;
        SenseAmpState           _sa_state;
        Time                    _sa_current_bit_last_integration_time;
        Time                    _sa_current_bit_eval_start_time;
        bool                    _sa_current_bit;
        double                  _sa_current_bit_regen_start_diff_output_volt;
        double                  _sa_current_bit_pd_accumulated_input_charge;
    };

} // namespace Thermal

#endif // __THERMAL_CURRENT_INTEGRATING_RECEIVER_H__

