
/*
 * This file defines the list of primitive devices that the tool
 * supports. Note that these are all blackbox modules and that each
 * module should only contain the port list, declaration of input/output
 * ports, and the settable parameters. The simulator will detect when these
 * are instantiated and will replace them with the primitive C++ instance.
 * Default parameters listed here do not matter, as these parameter
 * declarations are simply placeholders used for checks during verilog
 * elaboration.
 */

// Laser source
module LaserSourceOffChip(out);    
    output wire out;    
    parameter laser_output_per_wavelength = "-10.00";
    parameter number_wavelength = "1";
    parameter wavelength_begin = "1263.76e-9";
    parameter wavelength_end = "1263.76e-9";    
    parameter efficiency = "0.25";
    parameter laser_diode_loss = "1.00";
endmodule
 
// Lossy optical net
module LossyOpticalNet(in, out);
    input wire in;
    output wire out;        
    parameter loss = "0.0001";
    parameter length = "100e-6";
    parameter loss_per_meter = "370";    
endmodule

// Resonant ring
module ResonantRing(in, thru, add, drop, heater);
    input wire in, add, heater;
    output wire thru, drop;    
    parameter t1                        = "0.995";
    parameter t2                        = "0.995";
    parameter waveguide_loss            = "1380";
    parameter ring_radius               = "4.53e-6";
    parameter ring_width                = "1.2e-6";
    parameter ring_height               = "80e-9";
    parameter confinement_factor        = "0.47";    
endmodule

// Resonant ring depletion modulator
module ResonantRingDepletionModulator(in, thru, add, drop, mod_driver, heater);
    input wire in, add, mod_driver, heater;
    output wire thru, drop;
    parameter t1                        = "0.995";
    parameter t2                        = "0.995";
    parameter waveguide_loss            = "1380";
    parameter ring_radius               = "7.00e-6";
    parameter ring_width                = "1.2e-6";
    parameter ring_height               = "80e-9";
    parameter confinement_factor        = "0.47";
    parameter Na                        = "5e23";
    parameter Nd                        = "5e23";
    parameter number_junctions          = "84";
    parameter fcpd_n_fe                 = "6.0e-28";
    parameter fcpd_n_fh                 = "5.6e-24";
endmodule

// Modulator driver
module ModulatorDriver(ref, out);
    input wire ref;
    output wire out;
    parameter bit_period                = "1e-9";
    parameter bit_zero_voltage          = "0.6";
    parameter bit_one_voltage           = "-4.0";
    parameter bit_one_transition_time   = "200e-12";
    parameter bit_zero_transition_time  = "200e-12";
endmodule

// Thermal tuner
module ThermalTuner(heater);
    output wire heater;
    parameter heater_power              = "3e-3";
    parameter bit_width                 = "8";
    parameter clock_period              = "500e-6";
    parameter pdm_pattern               = "255";
    parameter heater_init               = "1";
endmodule


