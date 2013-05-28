
/** Test verilog file */

module cell_chip();

    wire laser_out;
    wire waveguide_0_out;
    wire modulator_out;
    wire waveguide_1_out;
    wire receiver_out;
    wire waveguide_2_out;

    wire driver_out;
    wire heater_out;

    // Optical path elements
    LaserSourceOffChip
        laser_source        (   .out        (laser_out));
    
    LossyOpticalNet
        waveguide_0         (   .in         (laser_out), 
                                .out        (waveguide_0_out));
                            
    ResonantRingDepletionModulator  
        modulator           (   .in         (waveguide_0_out), 
                                .thru       (modulator_out),
                                .mod_driver (driver_out),
                                .heater     (heater_out));
        
    LossyOpticalNet
        waveguide_1         (   .in         (modulator_out), 
                                .out        (waveguide_1_out));
                            
    ResonantRing
        receiver_ring       (   .in         (waveguide_1_out),
                                .thru       (receiver_out));                            
                            
    LossyOpticalNet
        waveguide_2         (   .in         (receiver_out),
                                .out        (waveguide_2_out));

    // Electrical path elements
    ModulatorDriver
        modulator_driver    (   .out        (driver_out));
        
    ThermalTuner
        thermal_tuner       (   .heater     (heater_out));


endmodule