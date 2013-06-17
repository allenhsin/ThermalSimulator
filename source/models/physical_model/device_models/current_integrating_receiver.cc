
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "source/models/physical_model/device_models/current_integrating_receiver.h"
#include "libutil/LibUtil.h"
#include "source/misc/misc.h"

namespace Thermal
{
    CurrentIntegratingReceiver::CurrentIntegratingReceiver(DeviceFloorplanMap* device_floorplan_map, std::string device_definition_file)
        : DeviceModel(device_floorplan_map, device_definition_file)
        ,  _number_wavelength (0)
        ,  _pd_responsivity (0)
        ,  _sa_total_min_input_cap (0)
        ,  _sa_number_cap_deck_steps (0)
        ,  _sa_preset_threshold_code (0)
        ,  _sa_input_cap_deck_step_size (0)
        ,  _sa_bit_period (0)
        ,  _sa_sense_period (0)
        ,  _sa_regen_start_delta_input_voltage (0)
        ,  _sa_proportionality_const (0)
        ,  _sa_regen_time_const (0)
        ,  _sa_required_output (0)
        ,  _sa_total_common_mode_current (0)
        ,  _sa_circuit_current_noise_sigma (0)
        ,  _sa_offset_mismatch_volt (0)
        ,  _sa_supply_noise_random_sigma (0)
        ,  _sa_supply_noise_determ (0)
        ,  _sa_extra_noise_margin (0)
        ,  _sa_current_pd_branch_input_cap (0)
        ,  _sa_current_ref_branch_input_cap (0)
        ,  _sa_current_threshold_code (0)
        ,  _sa_current_bit_elapsed_time (0)
        ,  _sa_state (STATE_INTEGRATION)
        ,  _sa_current_bit_last_integration_time (0)
        ,  _sa_current_bit_eval_start_time (0)
        ,  _sa_current_bit (0)
        ,  _sa_current_bit_regen_start_diff_output_volt (0)
        ,  _sa_current_bit_pd_accumulated_input_charge (0)
    {}
    
    CurrentIntegratingReceiver::CurrentIntegratingReceiver( const CurrentIntegratingReceiver& cloned_device)
        : DeviceModel           (cloned_device)
        ,  _number_wavelength                   (cloned_device._number_wavelength)
        ,  _pd_responsivity                     (cloned_device._pd_responsivity)
        ,  _sa_total_min_input_cap              (cloned_device._sa_total_min_input_cap)
        ,  _sa_number_cap_deck_steps            (cloned_device._sa_number_cap_deck_steps)
        ,  _sa_preset_threshold_code            (cloned_device._sa_preset_threshold_code)
        ,  _sa_input_cap_deck_step_size         (cloned_device._sa_input_cap_deck_step_size)
        ,  _sa_bit_period                       (cloned_device._sa_bit_period)
        ,  _sa_sense_period                     (cloned_device._sa_sense_period)
        ,  _sa_regen_start_delta_input_voltage  (cloned_device._sa_regen_start_delta_input_voltage)
        ,  _sa_proportionality_const            (cloned_device._sa_proportionality_const)
        ,  _sa_regen_time_const                 (cloned_device._sa_regen_time_const)
        ,  _sa_required_output                  (cloned_device._sa_required_output)
        ,  _sa_total_common_mode_current        (cloned_device._sa_total_common_mode_current)
        ,  _sa_circuit_current_noise_sigma      (cloned_device._sa_circuit_current_noise_sigma)
        ,  _sa_offset_mismatch_volt             (cloned_device._sa_offset_mismatch_volt)
        ,  _sa_supply_noise_random_sigma        (cloned_device._sa_supply_noise_random_sigma)
        ,  _sa_supply_noise_determ              (cloned_device._sa_supply_noise_determ)
        ,  _sa_extra_noise_margin               (cloned_device._sa_extra_noise_margin)
        ,  _sa_current_pd_branch_input_cap      (cloned_device._sa_current_pd_branch_input_cap)
        ,  _sa_current_ref_branch_input_cap     (cloned_device._sa_current_ref_branch_input_cap)
        ,  _sa_current_threshold_code           (cloned_device._sa_current_threshold_code)
        ,  _sa_current_bit_elapsed_time         (cloned_device._sa_current_bit_elapsed_time)
        ,  _sa_state                            (cloned_device._sa_state)
        ,  _sa_current_bit_last_integration_time(cloned_device._sa_current_bit_last_integration_time)
        ,  _sa_current_bit_eval_start_time      (cloned_device._sa_current_bit_eval_start_time)
        ,  _sa_current_bit                      (cloned_device._sa_current_bit)
        ,  _sa_current_bit_regen_start_diff_output_volt (cloned_device._sa_current_bit_regen_start_diff_output_volt)
        ,  _sa_current_bit_pd_accumulated_input_charge  (cloned_device._sa_current_bit_pd_accumulated_input_charge)
    {}

    CurrentIntegratingReceiver::~CurrentIntegratingReceiver()
    {}
    
    CurrentIntegratingReceiver* CurrentIntegratingReceiver::clone() const
    { return new CurrentIntegratingReceiver(*this); }

    void CurrentIntegratingReceiver::deviceParameterCheck()
    {   
        if( getParameter("photodiode_responsivity") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: photodiode responsivity cannot be negative.\n");

        if( getParameter("photodiode_dark_current") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: photodiode dark current cannot be negative.\n");

        if( getParameter("photodiode_cap") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: photodiode capacitance cannot be negative.\n");

        if( getParameter("sense_amp_vdd") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier Vdd cannot be negative.\n");

        if( getParameter("sense_amp_input_parasitic_cap") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier input parasitic capacitance cannot be negative.\n");

        if( getParameter("sense_amp_sampling_period") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier sampling period cannot be negative.\n");

        if( getParameter("sense_amp_duty_cycle") < 0 || getParameter("sense_amp_duty_cycle") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier duty cycle cannot be negative or greater than one.\n");

        if( getParameter("sense_amp_input_regeneration_ratio") < 0 || getParameter("sense_amp_input_regeneration_ratio") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier input regeneration voltage ratio cannot be negative or greater than one.\n");

        if( getParameter("sense_amp_footer_transistor_current") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier footer transistor common-mode current cannot be negative.\n");

        if( getParameter("sense_amp_proportionality_const") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier proportionality constant cannot be negative.\n");

        if( getParameter("sense_amp_regeneration_time_const") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier regeneration time constant cannot be negative.\n");

        if( getParameter("sense_amp_required_output_level") < 0 || getParameter("sense_amp_required_output_level") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier required output level cannot be negative or greater than one.\n");

        if( getParameter("sense_amp_circuit_current_noise_sigma") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier circuit noise std dev cannot be negative.\n");

        if( getParameter("sense_amp_offset_sigma_ratio") < 0 || getParameter("sense_amp_offset_sigma_ratio") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier offset std dev ratio cannot be negative or greater than one.\n");

        if( getParameter("sense_amp_offset_compensation_bits") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier offset compensation bits cannot be negative.\n");
        
        if( getParameter("sense_amp_preset_threshold_code") < -( (int)pow(2.00, getParameter("sense_amp_offset_compensation_bits")) - 1 ) ||
            getParameter("sense_amp_preset_threshold_code") >  ( (int)pow(2.00, getParameter("sense_amp_offset_compensation_bits")) - 1 ) )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier preset threshold code out of range.\n");

        if( getParameter("sense_amp_comp_cap_deck_total_size") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier compensation capacitor deck total size cannot be negative.\n");

        if( getParameter("sense_amp_cmrr") < 0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier CMRR cannot be negative.\n");

        if( getParameter("sense_amp_sup_noise_random_sigma_ratio") < 0 || getParameter("sense_amp_sup_noise_random_sigma_ratio") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier random supply noise std dev ratio cannot be negative or greater than one.\n");

        if( getParameter("sense_amp_sup_noise_determ_sigma_ratio") < 0 || getParameter("sense_amp_sup_noise_determ_sigma_ratio") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier deterministic supply noise std dev ratio cannot be negative or greater than one.\n");

        if( getParameter("sense_amp_noise_margin_ratio") < 0 || getParameter("sense_amp_noise_margin_ratio") > 1 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: sense amplifier extra noise margin ratio cannot be negative or greater than one.\n");
    }
    
    void CurrentIntegratingReceiver::initializeDevice()
    {
        deviceParameterCheck();

        // load the parameters
        _number_wavelength = getPort("optical_in")->getPortPropertySize("wavelength");

        _pd_responsivity = getParameter("photodiode_responsivity");
        
        _sa_total_min_input_cap =   getParameter("photodiode_cap") + 
                                    getParameter("sense_amp_input_parasitic_cap") + 
                                    getParameter("sense_amp_comp_cap_deck_total_size")/2 ;
        _sa_number_cap_deck_steps = ( (int)pow(2.00, getParameter("sense_amp_offset_compensation_bits")) );
        _sa_input_cap_deck_step_size =  getParameter("sense_amp_comp_cap_deck_total_size") / 
                                        (2*_sa_number_cap_deck_steps);
        
        _sa_bit_period = getParameter("sense_amp_sampling_period");
        _sa_sense_period = getParameter("sense_amp_duty_cycle")*_sa_bit_period;
        _sa_regen_start_delta_input_voltage = getParameter("sense_amp_vdd")*( 1 - getParameter("sense_amp_input_regeneration_ratio") );
        _sa_proportionality_const = getParameter("sense_amp_proportionality_const");
        _sa_regen_time_const = getParameter("sense_amp_regeneration_time_const");
        _sa_required_output = getParameter("sense_amp_required_output_level")*getParameter("sense_amp_vdd");
        _sa_total_common_mode_current = getParameter("sense_amp_footer_transistor_current") + getParameter("photodiode_dark_current");

        _sa_circuit_current_noise_sigma = getParameter("sense_amp_circuit_current_noise_sigma");
        _sa_offset_mismatch_volt = Misc::genGaussianRandomNumber(0, getParameter("sense_amp_offset_sigma_ratio")*getParameter("sense_amp_vdd"))/(double)_sa_number_cap_deck_steps;
        _sa_supply_noise_random_sigma = getParameter("sense_amp_sup_noise_random_sigma_ratio")*getParameter("sense_amp_vdd");
        _sa_supply_noise_determ = Misc::genGaussianRandomNumber(0, getParameter("sense_amp_sup_noise_determ_sigma_ratio")*getParameter("sense_amp_vdd"));
        _sa_extra_noise_margin = getParameter("sense_amp_noise_margin_ratio")*getParameter("sense_amp_vdd");

        // initializations
        // update input cap size according to preset threshold code settings
        _sa_preset_threshold_code = (int) getParameter("sense_amp_preset_threshold_code");
        _sa_current_pd_branch_input_cap                 =   (_sa_preset_threshold_code>0) ? 
                                                            (_sa_total_min_input_cap+(_sa_preset_threshold_code*_sa_input_cap_deck_step_size)) : 
                                                            _sa_total_min_input_cap ;
        _sa_current_ref_branch_input_cap                =   (_sa_preset_threshold_code<0) ? 
                                                            (_sa_total_min_input_cap+(abs(_sa_preset_threshold_code)*_sa_input_cap_deck_step_size)) : 
                                                            _sa_total_min_input_cap ;
        _sa_current_threshold_code                      = _sa_preset_threshold_code;

        _sa_current_bit_elapsed_time                    = 0;
        _sa_state                                       = STATE_INTEGRATION;
        _sa_current_bit_last_integration_time           = 0;
        _sa_current_bit_eval_start_time                 = 0;
        _sa_current_bit                                 = false;
        _sa_current_bit_regen_start_diff_output_volt    = 0;
        _sa_current_bit_pd_accumulated_input_charge     = 0;

        // set output
        getPortForModification("out")->setPortPropertySize("bit", 1);
        getPortForModification("out")->setPortPropertyValueByIndex("bit", 0, (_sa_current_bit? 1:0) );

    } // initializeDevice

    void CurrentIntegratingReceiver::updateDeviceProperties(Time time_elapsed_since_last_update)
    {
        unsigned int i = 0;

        // update current bit elapsed time ----------------------------------------------------------------------------
        _sa_current_bit_elapsed_time += time_elapsed_since_last_update;

        // if reaching another bit period, reset the current bit settings
        if(_sa_current_bit_elapsed_time >= _sa_bit_period)
        {
            _sa_state                                   = STATE_INTEGRATION;
            _sa_current_bit_last_integration_time       = 0;
            _sa_current_bit_eval_start_time             = 0;
            _sa_current_bit_regen_start_diff_output_volt= 0;
            _sa_current_bit_pd_accumulated_input_charge = 0;
            _sa_current_bit_elapsed_time               -= _sa_bit_period;
        }

        // if not within the sensing period, do nothing
        if(_sa_current_bit_elapsed_time >= _sa_sense_period)
            return;
        // ------------------------------------------------------------------------------------------------------------
        
        // update input cap size according to threshold code settings -------------------------------------------------
        int threshold_code = _sa_preset_threshold_code;
        if(getPort("sense_amp_threshold")->getPortPropertySize("code")>0)
            threshold_code = (int) getPort("sense_amp_threshold")->getPortPropertyValueByIndex("code", 0);
        if (_sa_current_threshold_code != threshold_code)
        {
            if      (threshold_code >= _sa_number_cap_deck_steps)
                threshold_code = (_sa_number_cap_deck_steps-1);
            else if (threshold_code <= -_sa_number_cap_deck_steps)
                threshold_code = -(_sa_number_cap_deck_steps-1);

            _sa_current_pd_branch_input_cap             =   (threshold_code>0) ? 
                                                            (_sa_total_min_input_cap+(threshold_code*_sa_input_cap_deck_step_size)) : 
                                                            _sa_total_min_input_cap ;
            _sa_current_ref_branch_input_cap            =   (threshold_code<0) ? 
                                                            (_sa_total_min_input_cap+(abs(threshold_code)*_sa_input_cap_deck_step_size)) : 
                                                            _sa_total_min_input_cap ;
            _sa_current_threshold_code = threshold_code;
        }
        // ------------------------------------------------------------------------------------------------------------

        // sense amp sensing operation --------------------------------------------------------------------------------
        double eval_elapsed_time = 0;
        double regenerated_output_volt = 0;
        double total_optical_in_power = 0;
        double pd_current = 0;
        double avg_pd_current = 0;
        Time t_pd_integration = 0;
        Time t_ref_integration = 0;
        switch (_sa_state)
        {
        case STATE_INTEGRATION: 
            // calculate instant pd current
            total_optical_in_power = 0;
            for(i=0; i<_number_wavelength; ++i)
                total_optical_in_power += getPort("optical_in")->getPortPropertyValueByIndex("power", i);
            pd_current = total_optical_in_power * _pd_responsivity;
            
            // calculate average pd current
            _sa_current_bit_pd_accumulated_input_charge += pd_current*(_sa_current_bit_elapsed_time - _sa_current_bit_last_integration_time);
            avg_pd_current = _sa_current_bit_pd_accumulated_input_charge/_sa_current_bit_elapsed_time;
            
            // calculate corresponding integrating duration
            t_pd_integration   =   _sa_current_pd_branch_input_cap * _sa_regen_start_delta_input_voltage / 
                                   (_sa_total_common_mode_current + avg_pd_current);
            t_ref_integration  =   _sa_current_ref_branch_input_cap* _sa_regen_start_delta_input_voltage / 
                                   _sa_total_common_mode_current;

            // regeneration started
            if ( (t_pd_integration <= _sa_current_bit_elapsed_time) || (t_ref_integration <= _sa_current_bit_elapsed_time) )
            {   
                // check which branch wins the starting point of the integration
                _sa_current_bit_eval_start_time = (t_pd_integration<t_ref_integration)? t_pd_integration : t_ref_integration;

                // the starting voltage of regeneration at the output node
                _sa_current_bit_regen_start_diff_output_volt =  _sa_proportionality_const*
                                                                (   
                                                                    ((_sa_total_common_mode_current+avg_pd_current)*_sa_current_bit_eval_start_time/_sa_current_pd_branch_input_cap) -
                                                                    (_sa_total_common_mode_current*_sa_current_bit_eval_start_time/_sa_current_ref_branch_input_cap)
                                                                );
                // time elapsed in the evaluation phase
                eval_elapsed_time = _sa_current_bit_elapsed_time - _sa_current_bit_eval_start_time;
                // the regenerated output voltage
                regenerated_output_volt = _sa_current_bit_regen_start_diff_output_volt*exp(eval_elapsed_time/_sa_regen_time_const);

                // check if the output voltage reached the required output
                if (fabs(regenerated_output_volt) >= _sa_required_output)
                {
                    _sa_state = STATE_EVALUATED;
                    _sa_current_bit = (regenerated_output_volt>0)?true:false;
                    
                    getPortForModification("out")->setPortPropertyValueByIndex("bit", 0, (_sa_current_bit? 1:0) );
                }
                else
                    _sa_state = STATE_EVALUATION;

            }
            // else: stay at integration state
            
            _sa_current_bit_last_integration_time = _sa_current_bit_elapsed_time;
            break;
        case STATE_EVALUATION:
            // time elapsed in the evaluation phase
            eval_elapsed_time = _sa_current_bit_elapsed_time - _sa_current_bit_eval_start_time;
            // the regenerated output voltage
            regenerated_output_volt = _sa_current_bit_regen_start_diff_output_volt*exp(eval_elapsed_time/_sa_regen_time_const);
            
            // check if the output voltage reached the required output
            if (fabs(regenerated_output_volt) >= _sa_required_output)
            {
                _sa_state = STATE_EVALUATED;
                _sa_current_bit = (regenerated_output_volt>0)?true:false;

                getPortForModification("out")->setPortPropertyValueByIndex("bit", 0, (_sa_current_bit? 1:0) );
            }
            else
                _sa_state = STATE_EVALUATION;

            break;
        default:
            break;
        }
        // ------------------------------------------------------------------------------------------------------------

    }
    
    void CurrentIntegratingReceiver::initializeMonitoring()
    {
        unsigned int number_wavelength = 0;
        unsigned int i = 0;

        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if( it->first=="optical_in" )
            {
                number_wavelength = getPort(it->first)->getPortPropertySize("wavelength");
            
                for(i=0; i<number_wavelength; ++i)
                    fprintf(it->second, "%.12f ", getPort(it->first)->getPortPropertyValueByIndex("wavelength", i) );
                fprintf(it->second, "\n\n\n");
            }
        }
    }
    
    void CurrentIntegratingReceiver::printMonitoredResult()
    {
        unsigned int number_wavelength = 0;
        unsigned int i = 0;

        for(map<string, FILE*>::iterator it = _monitored_device_ports.begin(); it != _monitored_device_ports.end(); ++it)
        {
            if ( it->first == "sense_amp_threshold" )
                fprintf(it->second, "%.9f\n", getPort("sense_amp_threshold")->getPortPropertyValueByIndex("code", 0) );
            if ( it->first == "out" )
                fprintf(it->second, "%.9f\n", getPort("out")->getPortPropertyValueByIndex("bit", 0) );
            else // other optical ports
            {
                number_wavelength = getPort(it->first)->getPortPropertySize("power");
                
                for(i=0; i<number_wavelength; ++i)
                    fprintf(it->second, "%.9f ", getPort(it->first)->getPortPropertyValueByIndex("power", i) );
                fprintf(it->second, "\n");
            }
        }
    }
    
    void CurrentIntegratingReceiver::printDefinition(FILE* device_list_file)
    {
        DeviceModel::printDefinition(device_list_file);

        fprintf(device_list_file, "    [port]\n");
        fprintf(device_list_file, "        Name: optical_in, Type: %s, Property: power(%d), wavlength(%d)\n", 
                ((int)getPort("optical_in")->getPortType()==0)?"INPUT":"OUTPUT", getPort("optical_in")->getPortPropertySize("power"), getPort("optical_in")->getPortPropertySize("wavelength"));
        fprintf(device_list_file, "        Name: sense_amp_threshold, Type: %s, Property: code(%d)\n", 
                ((int)getPort("sense_amp_threshold")->getPortType()==0)?"INPUT":"OUTPUT", getPort("sense_amp_threshold")->getPortPropertySize("code"));
        fprintf(device_list_file, "        Name: out, Type: %s, Property: bit(%d)\n", 
                ((int)getPort("out")->getPortType()==0)?"INPUT":"OUTPUT", getPort("out")->getPortPropertySize("bit"));
        fprintf(device_list_file, "\n");
    }
}

