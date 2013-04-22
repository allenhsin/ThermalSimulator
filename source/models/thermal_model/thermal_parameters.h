
#ifndef __THERMAL_THERMAL_PARAMETERS_H__
#define __THERMAL_THERMAL_PARAMETERS_H__

#include <string>

#include "config.hpp"

namespace Thermal
{
    class ThermalParameters
    {
    public:
        // create singleton
        static void allocate(config::Config* thermal_config);
        // destroy singleton
        static void release();
        // get singleton pointer
        static ThermalParameters* getSingleton();
        

    protected:
        ThermalParameters(config::Config* thermal_config);
        ~ThermalParameters();

        config::Config* getThermalConfig(){ return _thermal_config; }

    private:
        static ThermalParameters* _thermal_parameters_singleton;
        
        config::Config* _thermal_config;

    public:
        // list of parameters in the thermal config file
        // [env_setup]
        std::string     floorplan_file;
        std::string     top_flp_object;
        double          ambient;
        bool            init_file_used;
        std::string     init_temp_file;
        double          init_temp;
        double          sampling_intvl;
        std::string     model_type;
        std::string     ttrace_file;
        bool            debug_print_enable;
        std::string     debug_model_file;
        std::string     debug_flp_file;
        // [chip]
        double          t_chip;
        double          k_chip;
        double          p_chip;
        // [heat_sink]
        double          c_convec;
        double          r_convec;
        double          s_sink;
        double          t_sink;
        double          k_sink;
        double          p_sink;
        // [heat_spreader]
        double          s_spreader;
        double          t_spreader;
        double          k_spreader;
        double          p_spreader;
        // [interface_material]
        double          t_interface;
        double          k_interface;
        double          p_interface;
        // [secondary_path]
        bool            model_secondary;
        double          r_convec_sec;
        double          c_convec_sec;
        int             n_metal;
        double          t_metal;
        double          t_c4;
        double          s_c4;
        int             n_c4;
        double          s_sub;
        double          t_sub;
        double          s_solder;
        double          t_solder;
        double          s_pcb;
        double          t_pcb;
        // [leakage_model]
        bool            leakage_used;
        int             leakage_mode;
        // [block_model]
        bool            block_omit_lateral;
        // [grid_model]
        int             grid_rows;
        int             grid_cols;
        std::string     grid_layer_file;
        std::string     grid_map_mode;
        // [package_model]
        bool            package_model_used;
        // [package_model/heat_sink]
        bool            natural_convec;
        int             flow_type;
        int             sink_type;
        // [package_model/heat_sink/fin_channel]
        double          fin_height;
        double          fin_width;
        double          channel_width;
        // [package_model/heat_sink/pin_fin]
        double          pin_height;
        double          pin_diam;
        double          pin_dist;
        // [package_model/heat_sink/fan]
        double          fan_radius;
        double          motor_radius;
        double          rpm;

    }; // class ThermalParameters

} // namespace Thermal

#endif // __THERMAL_THERMAL_PARAMETERS_H__

