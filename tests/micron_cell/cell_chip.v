
/** Test verilog file */

module cell_chip();

    wire [3:0] laser_out;
    wire waveguide_0_out;
    wire modulator_out;
    wire waveguide_1_out;
    wire receiver_out;
    wire waveguide_2_out;

    wire driver_out;
    wire heater_out;
    
    assign laser_out[3:1] = laser_out[2:0];

    // Optical path elements
    LaserSourceOffChip
                        #   (   .wavelength_begin("1264.00e-9"),
                                .wavelength_end("1264.00e-9"),
                                .laser_output_power_per_wavelength("10.00"))
        laser_source        (   .out        (laser_out[0]));
    
    LossyOpticalNet
                        #   (   .loss("0.1")) // 270um
        waveguide_0         (   .in         (laser_out[3]), 
                                .out        (waveguide_0_out));
                            
    ResonantRingDepletionModulator  
                        #   (   .t1("0.985"),
                                .t2("0.96"),
                                .waveguide_loss("4000"),
                                .ring_radius("5.00e-6"),
                                .ring_width("1.8e-6"),
                                .ring_height("200e-9"))
        modulator           (   .in         (waveguide_0_out), 
                                .thru       (modulator_out),
                                .mod_driver (driver_out),
                                .heater     (heater_out));
        
    LossyOpticalNet
                        #   (   .loss("0.1")) // 270um
        waveguide_1         (   .in         (modulator_out), 
                                .out        (waveguide_1_out));
                            
    ResonantRing
                        #   (   .t1("0.985"),
                                .t2("0.96"),
                                .waveguide_loss("4000"),
                                .ring_radius("5.00e-6"),
                                .ring_width("1.8e-6"),
                                .ring_height("200e-9"))
        receiver_ring       (   .in         (waveguide_1_out),
                                .thru       (receiver_out));                            
                            
    LossyOpticalNet
                        #   (   .loss("0.1")) // 270um
        waveguide_2         (   .in         (receiver_out),
                                .out        (waveguide_2_out));

    // Electrical path elements
    ModulatorDriver
                        #   (   .bit_period("250e-9"),
                                .bit_zero_voltage("0.60"),
                                .bit_one_voltage("-4.00"),
                                .bit_one_transition_time("200e-12"),
                                .bit_zero_transition_time("200e-12"))
        modulator_driver    (   .out        (driver_out));
        
    ThermalTuner
                        #   (   .heater_power("3e-3"),
                                .bit_width("8"),
                                .clock_period("250e-9"),
                                .pdm_pattern("0"),
                                .heater_init("0"))
        thermal_tuner       (   .heater     (heater_out));


endmodule

