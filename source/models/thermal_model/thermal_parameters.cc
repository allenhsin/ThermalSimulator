
#include <stddef.h>
#include <cassert>
#include <stdio.h>

#include "source/models/thermal_model/thermal_parameters.h"

namespace Thermal
{
    ThermalParameters* ThermalParameters::_thermal_parameters_singleton = NULL;
    
    void ThermalParameters::allocate(config::Config* thermal_config)
    {
        assert(_thermal_parameters_singleton==NULL);
        _thermal_parameters_singleton = new ThermalParameters(thermal_config);
        assert(_thermal_parameters_singleton);
    }

    void ThermalParameters::release()
    {
        assert(_thermal_parameters_singleton);
        delete _thermal_parameters_singleton;
        _thermal_parameters_singleton = NULL;
    }
    
    ThermalParameters* ThermalParameters::getSingleton()
    { 
        assert(_thermal_parameters_singleton);
        return _thermal_parameters_singleton; 
    }

    ThermalParameters::ThermalParameters(config::Config* thermal_config)
    {
        _thermal_config = thermal_config;
        assert(_thermal_config);

        // [env_setup]
        floorplan_file      = getThermalConfig()->getString("env_setup/floorplan_file");
        top_flp_object      = getThermalConfig()->getString("env_setup/top_flp_object");
        ambient             = getThermalConfig()->getFloat("env_setup/ambient");
        init_file_used      = getThermalConfig()->getBool("env_setup/init_file_used");
        init_temp_file      = getThermalConfig()->getString("env_setup/init_temp_file");
        init_temp           = getThermalConfig()->getFloat("env_setup/init_temp");
        sampling_intvl      = getThermalConfig()->getFloat("env_setup/sampling_intvl");
        model_type          = getThermalConfig()->getString("env_setup/model_type");
        debug_print_enable  = getThermalConfig()->getBool("env_setup/debug_print_enable");
        debug_model_file    = getThermalConfig()->getString("env_setup/debug_model_file");
        debug_flp_file      = getThermalConfig()->getString("env_setup/debug_flp_file");
        debug_ttrace_file   = getThermalConfig()->getString("env_setup/debug_ttrace_file");
        // [chip]
        t_chip              = getThermalConfig()->getFloat("chip/t_chip");
        k_chip              = getThermalConfig()->getFloat("chip/k_chip");
        p_chip              = getThermalConfig()->getFloat("chip/p_chip");
        // [heat_sink]
        c_convec            = getThermalConfig()->getFloat("heat_sink/c_convec");
        r_convec            = getThermalConfig()->getFloat("heat_sink/r_convec");
        s_sink              = getThermalConfig()->getFloat("heat_sink/s_sink");
        t_sink              = getThermalConfig()->getFloat("heat_sink/t_sink");
        k_sink              = getThermalConfig()->getFloat("heat_sink/k_sink");
        p_sink              = getThermalConfig()->getFloat("heat_sink/p_sink");
        // [heat_spreader]
        s_spreader          = getThermalConfig()->getFloat("heat_spreader/s_spreader");
        t_spreader          = getThermalConfig()->getFloat("heat_spreader/t_spreader");
        k_spreader          = getThermalConfig()->getFloat("heat_spreader/k_spreader");
        p_spreader          = getThermalConfig()->getFloat("heat_spreader/p_spreader");
        // [interface_material]
        t_interface         = getThermalConfig()->getFloat("interface_material/t_interface");
        k_interface         = getThermalConfig()->getFloat("interface_material/k_interface");
        p_interface         = getThermalConfig()->getFloat("interface_material/p_interface");
        // [secondary_path]
        model_secondary     = getThermalConfig()->getBool("secondary_path/model_secondary");
        r_convec_sec        = getThermalConfig()->getFloat("secondary_path/r_convec_sec");
        c_convec_sec        = getThermalConfig()->getFloat("secondary_path/c_convec_sec");
        n_metal             = getThermalConfig()->getInt("secondary_path/n_metal");
        t_metal             = getThermalConfig()->getFloat("secondary_path/t_metal");
        t_c4                = getThermalConfig()->getFloat("secondary_path/t_c4");
        s_c4                = getThermalConfig()->getFloat("secondary_path/s_c4");
        n_c4                = getThermalConfig()->getInt("secondary_path/n_c4");
        s_sub               = getThermalConfig()->getFloat("secondary_path/s_sub");
        t_sub               = getThermalConfig()->getFloat("secondary_path/t_sub");
        s_solder            = getThermalConfig()->getFloat("secondary_path/s_solder");
        t_solder            = getThermalConfig()->getFloat("secondary_path/t_solder");
        s_pcb               = getThermalConfig()->getFloat("secondary_path/s_pcb");
        t_pcb               = getThermalConfig()->getFloat("secondary_path/t_pcb");
        // [leakage_model]
        leakage_used        = getThermalConfig()->getBool("leakage_model/leakage_used");
        leakage_mode        = getThermalConfig()->getInt("leakage_model/leakage_mode");
        // [block_model]
        block_omit_lateral  = getThermalConfig()->getBool("block_model/block_omit_lateral");
        // [grid_model]
        grid_rows           = getThermalConfig()->getInt("grid_model/grid_rows");
        grid_cols           = getThermalConfig()->getInt("grid_model/grid_cols");
        grid_layer_file     = getThermalConfig()->getString("grid_model/grid_layer_file");
        grid_map_mode       = getThermalConfig()->getString("grid_model/grid_map_mode");
        // [package_model]
        package_model_used  = getThermalConfig()->getBool("package_model/package_model_used");
        // [package_model/heat_sink]
        natural_convec      = getThermalConfig()->getBool("package_model/heat_sink/natural_convec");
        flow_type           = getThermalConfig()->getInt("package_model/heat_sink/flow_type");
        sink_type           = getThermalConfig()->getInt("package_model/heat_sink/sink_type");
        // [package_model/heat_sink/fin_channel]
        fin_height          = getThermalConfig()->getFloat("package_model/heat_sink/fin_channel/fin_height");
        fin_width           = getThermalConfig()->getFloat("package_model/heat_sink/fin_channel/fin_width");
        channel_width       = getThermalConfig()->getFloat("package_model/heat_sink/fin_channel/channel_width");
        // [package_model/heat_sink/pin_fin]
        pin_height          = getThermalConfig()->getFloat("package_model/heat_sink/pin_fin/pin_height");
        pin_diam            = getThermalConfig()->getFloat("package_model/heat_sink/pin_fin/pin_diam");
        pin_dist            = getThermalConfig()->getFloat("package_model/heat_sink/pin_fin/pin_dist");
        // [package_model/heat_sink/fan]
        fan_radius          = getThermalConfig()->getFloat("package_model/heat_sink/fan/fan_radius");
        motor_radius        = getThermalConfig()->getFloat("package_model/heat_sink/fan/motor_radius");
        rpm                 = getThermalConfig()->getFloat("package_model/heat_sink/fan/rpm");
    }

    ThermalParameters::~ThermalParameters()
    {
        _thermal_config = NULL;
    }

} // namespace Thermal

