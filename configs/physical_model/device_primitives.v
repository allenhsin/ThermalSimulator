
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
    
    parameter t1 = "0.995";
    parameter t2 = "0.995";
    parameter waveguide_loss = "1380";
    parameter ring_radius = "4.53e-6";
    parameter ring_width = "1.2e-6";
    parameter ring_height = "80e-9";
    parameter confinement_factor = "0.47";
    
endmodule
