
#include <stddef.h>
#include <cassert>
#include <math.h>

#include "source/models/thermal_model/package.h"
#include "source/models/thermal_model/thermal_parameters.h"
#include "source/models/thermal_model/thermal_constants.h"

namespace Thermal
{

    Package::Package()
        : _thermal_params   (NULL)
        , _n_fin            (0)
        , _sur_area_fin     (0)
        , _n_pin            (0)
        , _sur_area_pin     (0)
        , _reynolds         (0)
        , _nusselt          (0)
        , _h_coeff          (0)
        , _air_v            (0)
        , _r_th             (0)
    {}

    Package::~Package()
    {}

    void Package::setThermalParameters(ThermalParameters* thermal_params)
    { _thermal_params = thermal_params; }


    // calculate forced air flow and package thermal parameters	
    // references:															
    // 1-1  lateral flow, fin-channel heat sinks:
    //      -laminar flow:      P. terrtstra et al. "Analytical Forced Convection Modeling of Plate-Fin Heat Sinks,"
    //                          IEEE SEMI-THERM, pp 34-41, 1999
    //      -turbulent flow:    Y. A. Cengel. "Heat and Mass Transfer: A Practical Approach", Mcgraw-Hill Inc. New York 2007 
    // 1-2  lateral flow, pin-fin heat sinks:
    //      R. Ribando et al. "Estimating the Convection Coefficient for Flow Through Banks of Fins on a Heat Sink,"
    //      U.Va. MAE314 Course Notes, Spring 2007
    // 2-1  impinging flow for both fin-channel and pin-fin heat sinks:
    //      H. A. El-Sheikh et al. "Heat Transfer from Pin-Fin Heat Sinks under Multiple Impinging Jets," 
    //      IEEE Trans. on Adv. Packaging, 23(1):113-120, Feb. 2000
    // 3-1  fan model:
    //      F. P. Bleier. "Fan Handbook: Selection, Application and Design," McGraw-Hill Inc. New York, 1998
    // 3-2  motor model of the fan:
    //      Y. Zhang et al. "SODA: Sensitivity-Based Optimization of Disk Architecture," IEEE/ACM DAC, pp. 865-870, 2007
    void Package::calculateRForcedConvection()
    {
        assert(_thermal_params);

        // local variables
        double dh, a_fan, dr, r_approx, vol_v, rey_star;
        double m, eta, f, c1, c2, a_hs;
        double t1, t2;

        double  s_sink          = _thermal_params->s_sink;
        double  k_sink          = _thermal_params->k_sink;
        
        int     flow_type       = _thermal_params->flow_type;
        int     sink_type       = _thermal_params->sink_type;
        double  fin_height      = _thermal_params->fin_height;
        double  fin_width       = _thermal_params->fin_width;
        double  channel_width   = _thermal_params->channel_width;
        double  pin_height      = _thermal_params->pin_height;
        double  pin_diam        = _thermal_params->pin_diam;
        double  pin_dist        = _thermal_params->pin_dist;
        double  fan_radius      = _thermal_params->fan_radius;
        double  motor_radius    = _thermal_params->motor_radius;
        double  rpm             = _thermal_params->rpm;
        
        double  temp_val        = (s_sink-pin_diam)/(pin_diam+pin_dist);
        
        _n_fin          = ceil((s_sink-fin_width)/(fin_width+channel_width)-0.5);
        _sur_area_fin   = s_sink*(s_sink+2.0*_n_fin*fin_height);
        _n_pin          = ceil(temp_val*temp_val-0.5);
        _sur_area_pin   = s_sink*s_sink+PI*pin_diam*pin_height*_n_pin;

        // calculate volumetric air speed out of the fan 
        dr              = sqrt(fan_radius * fan_radius - motor_radius * motor_radius);
        // approximated average fan radius
        r_approx        = motor_radius + dr;
        // total fan blade area
        a_fan           = PI * dr * dr;
        
        // refer to DAC'07 SODA paper by Zhang, Gurumurthi and Stan 
        // crudely approximating an IC fan motor with a hard drive spindle motor model
        // the principle is that the dragging momentum on the blades from the air 
        // equals to the torque of the motor at steady state
        // so, torque=b*(omega^alpha)=drag_force*radius (1)
        // where b=0.5*pi*air_density*C_d*(radius^beta)
        // drag coeff C_d=drag_force/(0.5*air_density*air_velocity^2*total_blade_area)
        // manipulate both side of (1), derive volmetric velocity from the fan as... 
        vol_v           = a_fan * sqrt(0.25 * PI * pow(r_approx,FAN_BETA-1)
                          * pow(rpm * RPM_TO_RAD,FAN_ALPHA) / a_fan);
    	
        // calculate the actual air velocity through heatsink: vol_velocity/area_duct_sink
        if (flow_type==0) // lateral airflow
        {
            // hydraulic diameter
            dh = 2.0*channel_width*s_sink/(channel_width+s_sink);
            if (sink_type==0)   // fin-channel sink
                _air_v = vol_v / ((_n_fin-1)*channel_width*fin_height);
            else                // pin-fin sink
                _air_v = vol_v / ((sqrt(_n_pin)-1)*pin_dist*pin_height);
        }
        else // impinging flow
        {
            // equivalent air nozzle diameter
            dh = 2.0 * s_sink / sqrt(PI);
            _air_v = vol_v / (s_sink*s_sink-_n_pin*PI*(pin_diam*0.5)*(pin_diam*0.5));
        }
    	
        // Reynolds number
        _reynolds = AIR_DSTY * _air_v * dh / AIR_DYNVISC;
    	
        // calculate nusselt number, heat transfer coeff
        // and equivalent overall lumped convection resistance 
        if (flow_type==0) // lateral airflow
        {
            if (sink_type==0) // fin-channel sink
            {
                if (_reynolds <= REY_THRESHOLD) // laminar flow
                {
                    rey_star = AIR_DSTY*_air_v*channel_width*channel_width/(AIR_DYNVISC*s_sink);
                    t1 = pow(rey_star*PRANTDL_NUM*0.5,-3);
                    t2 = pow(0.664*sqrt(rey_star)*pow(PRANTDL_NUM,0.33)*sqrt(1+3.65/sqrt(rey_star)),-3);
                    _nusselt = pow(t1+t2,-0.33);
                    _h_coeff = _nusselt*AIR_COND/channel_width;
                }
                else // turbulent flow
                {
                    f = 1.0/pow((0.79*log(_reynolds)-1.64),2.0);
                    _nusselt = 0.125*f*_reynolds*pow(PRANTDL_NUM,0.33);
                    _h_coeff = _nusselt*AIR_COND/dh;
                }
                
                // fin parameter
                m = sqrt(2*_h_coeff/(k_sink*fin_width));
                // fin efficiency
                eta = tanh(m*s_sink)/(m*s_sink);
                _r_th = 1.0/(_h_coeff*(channel_width+2*eta*s_sink)*_n_fin*s_sink);

            }
            else // pin-fin sink
            {
                c1 = 0.6;
                c2 = 0.8+0.8333*sqrt(sqrt(_n_pin)-1)/pow((1+0.03*pow((sqrt(_n_pin)-1),2.0)),0.25);
                _nusselt = 0.4+0.9*(1.25*c2*c1*sqrt(_reynolds)+0.001*_reynolds)*pow(PRANTDL_NUM,0.33);
                _h_coeff = _nusselt*AIR_COND/pin_diam;
                _r_th = 1.0/(_h_coeff*_sur_area_pin);
            }
        }
        else // impinging flow
        {
            if (sink_type==0) // fin-channel sink
                a_hs = _sur_area_fin;
            else // pin-fin sink
                a_hs = _sur_area_pin;
            dh = 2.0 * s_sink / sqrt(PI);
            _nusselt = 1.92*pow(_reynolds,0.716)*pow(PRANTDL_NUM,0.4)*pow(a_hs/(s_sink*s_sink),-0.698);
            _h_coeff = _nusselt*AIR_COND/dh;
            _r_th = 1.0/(_h_coeff*a_hs);
        }
    } // calculateRForcedConvection
    


    // calculate convection parameters for natural convection.
    // reference: Y. A. Cengel. "Heat and Mass Transfer: A Practical Approach", Mcgraw-Hill Inc. New York 2007 
    void Package::calculateRNaturalConvection(double sink_temp) 
    {
        assert(_thermal_params);

        // local variables
        double rayleigh;
        double w;
        double sur_area;
        double r_th_rad;
        
        double  s_sink          = _thermal_params->s_sink;
        double  ambient         = _thermal_params->ambient;
        
        int     sink_type       = _thermal_params->sink_type;
        double  fin_height      = _thermal_params->fin_height;
        double  fin_width       = _thermal_params->fin_width;
        double  channel_width   = _thermal_params->channel_width;
        double  pin_height      = _thermal_params->pin_height;
        double  pin_diam        = _thermal_params->pin_diam;
        double  pin_dist        = _thermal_params->pin_dist;
        
        double  temp_val        = (s_sink-pin_diam)/(pin_diam+pin_dist);
        
        _n_fin          = ceil((s_sink-fin_width)/(fin_width+channel_width)-0.5);
        _sur_area_fin   = s_sink*(s_sink+2.0*_n_fin*fin_height);
        _n_pin          = ceil(temp_val*temp_val-0.5);
        _sur_area_pin   = s_sink*s_sink+PI*pin_diam*pin_height*_n_pin;
        
        if (sink_type==0) // fin-channel sink
        {
            sur_area = _sur_area_fin;
            w = channel_width;
        } 
        else // pin-fin heatsink
        {
            sur_area = _sur_area_pin;
            w = pin_dist;
        }
        
        // CAUTION: equations are derived for fin-channel heatsink, not validated for pin-fin heatsink
        rayleigh = GRAVITY*VOL_EXP_COEFF*(sink_temp-ambient)*pow(w,3.0)*PRANTDL_NUM/(AIR_KINVISC*AIR_KINVISC);
        _nusselt = pow((576/((rayleigh*w/s_sink)*(rayleigh*w/s_sink))+2.873/sqrt(rayleigh*w/s_sink)),-0.5);
        _h_coeff = _nusselt*AIR_COND/w;
        _r_th = 1.0/(_h_coeff*sur_area);
        
        // thermal radiation
        r_th_rad = (sink_temp-ambient)/(EMISSIVITY*STEFAN*(pow(sink_temp,4.0)-pow(ambient,4.0))*sur_area);

        // overall thermal resistance = natural convection in parallel with thermal radiation
        _r_th = _r_th*r_th_rad/(_r_th+r_th_rad);
        
    } // calculateRNaturalConvection

    void Package::updateRConvection(double sink_temp)
    {
        assert(_thermal_params);

        bool natural_convec = _thermal_params->natural_convec;
    
        if(natural_convec)
            calculateRNaturalConvection(sink_temp);
        else
            calculateRForcedConvection();

        // update r_convec
        _thermal_params->r_convec = _r_th;

    } // updateRConvection
    

} // namespace Thermal

