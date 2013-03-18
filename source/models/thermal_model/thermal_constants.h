
#ifndef __THERMAL_THERMAL_CONSTANTS_H__
#define __THERMAL_THERMAL_CONSTANTS_H__

// [general] --------------------------------------------------------------------------------------
#define STR_SIZE        512
#define LINE_SIZE       65536
#define MAX_ENTRIES     512

#define BLOCK           "block"
#define GRID            "grid"
// ------------------------------------------------------------------------------------------------


// [RC model] -------------------------------------------------------------------------------------
// number of extra nodes due to the model:
// 4 spreader nodes, 4 heat sink nodes under
// the spreader (center), 4 peripheral heat 
// sink nodes (north, south, east and west)
// and a separate node for the ambient 
#define EXTRA       12
// spreader nodes
#define SP_W        0
#define SP_E        1
#define SP_N        2
#define SP_S        3
// central sink nodes (directly under the spreader)
#define SINK_C_W    4
#define SINK_C_E    5
#define SINK_C_N    6
#define SINK_C_S    7
// peripheral sink nodes
#define SINK_W      8
#define SINK_E      9
#define SINK_N      10
#define SINK_S      11

// total functional block laters
#define NL          4
// silicon is always layer 0
// interface layer
#define IFACE       1
// heat spreader
#define HSP         2
// heat sink
#define HSINK       3

// typical thermal properties for secondary path layers 
#define RHO_METAL           0.0025  
#define RHO_DIELECTRIC      1.0
#define RHO_C4              0.8
#define RHO_UNDERFILL       0.03
#define RHO_SUB             0.5
#define RHO_SOLDER          0.06
#define RHO_PCB             0.333
#define K_METAL             (1.0/RHO_METAL) 
#define K_DIELECTRIC        (1.0/RHO_DIELECTRIC)
#define K_C4                (1.0/RHO_C4)
#define K_UNDERFILL         (1.0/RHO_UNDERFILL)
#define K_SUB               (1.0/RHO_SUB)   
#define K_SOLDER            (1.0/RHO_SOLDER)    
#define K_PCB               (1.0/RHO_PCB)
#define SPEC_HEAT_METAL         3.55e6  // specfic heat of silicon in J/(m^3K)
#define SPEC_HEAT_DIELECTRIC    2.2e6  
#define SPEC_HEAT_C4            1.65e6  
#define SPEC_HEAT_UNDERFILL     2.65e6  
#define SPEC_HEAT_SUB           1.6e6   // specific heat of copper in J/(m^3K
#define SPEC_HEAT_SOLDER        2.1e6       // specific heat of the interface material in J/(m^3K)
#define SPEC_HEAT_PCB           1.32e6  

// model specific constants
// changed from 1/2 to 1/3 due to the difference from traditional Elmore Delay scenario
#define C_FACTOR    0.333       // fitting factor to match floworks (due to lumping)

// number time step sizes
#define N_TIME_STEPS    1
#define TIME_STEP_0     2.5e-6
#define TIME_STEP_1     1.0e-6
#define TIME_STEP_2     250e-9
#define TIME_STEP_3     100e-9

// ------------------------------------------------------------------------------------------------


// [package] --------------------------------------------------------------------------------------
// santity check convection resistance boundary values.
// if outside this range, most likely some settings of 
// the heat sink or the fan are unreasonable.
#define R_CONVEC_HIGH       50.0    // upper-bound value for natural convection without heat sink
#define R_CONVEC_LOW        0.01    // lower-bound value for best cooling solution, e.g. liquid cooling

// fluid (air) properties
#define AIR_DSTY            1.059   // density
#define AIR_SPECHT          1007    // specific heat
#define AIR_COND            0.028   // thermal conductivity
#define AIR_DIFF            2.6e-5  // thermal diffusivity
#define AIR_DYNVISC         2.0e-5  // dynamic viscosity
#define AIR_KINVISC         1.9e-5  // kinetic viscosity

#define PRANTDL_NUM         0.73    // Prantdl Number of air
#define REY_THRESHOLD       3500    // laminar/turbulent reynolds threshold

// parameters specific to natural convection and radiation
#define VOL_EXP_COEFF       3.0e-3  // volume expansion coefficient
#define GRAVITY             9.8     // gravity acceleration rate
#define STEFAN              5.67e-8 // Stefan-Boltzmann constant
#define EMISSIVITY          0.95    // emissivity, typical value for most heatsink materials
#define SMALL_FOR_CONVEC    0.01    // initial small temperature diff b/w heatsink and ambient, to avoid sigular matrices
#define NATURAL_CONVEC_TOL  0.01    // r_convec convergence criterion for natural convection calculations
#define MAX_SINK_TEMP       1000    // max avg sink temperature during iteration for natural convection. if exceeded, report thermal runaway

// flow resistance constants
#define KC                  0.42
#define KE                  1.0
#define PI                  3.1416

// fan constants
#define FAN_ALPHA           1       // 1 for laminar flow, >=2 for turbulent flow
#define FAN_BETA            4       // 4 for laminar flow, 4.6 for turbulent flow
#define FAN_POWER_COEFF     2.8     // fan_power=b*omega^2.8
#define RPM_TO_RAD          0.105   // convert rpm to radians per sec
#define RAD_TO_RPM          9.55    // convert radians per sec to rpm
// ------------------------------------------------------------------------------------------------

#endif // __THERMAL_THERMAL_CONSTANTS_H__

