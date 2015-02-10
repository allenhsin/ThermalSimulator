
/** Test verilog file */

module Cell();

    wire [3:0] laser_out;
    wire waveguide_0_out;
    wire modulator_out;
    wire waveguide_1_out;
    wire rec_ring_out;
    wire rec_ring_drop;
    wire waveguide_2_out;
    wire optical_data_gen_out;
    wire threshold_code;
    wire receiver_out;

    wire driver_out;
    wire heater_out;
    
    assign laser_out[3:1] = laser_out[2:0];

    // Optical path elements
    LaserSourceOffChip
                        #   (   .wavelength_begin("1264.00e-9"),
                                .wavelength_end("1264.00e-9"),
                                .laser_output_power_per_wavelength("-10.00"))
        laser_source        (   .out        (laser_out[0]));
    
    OpticalDataGenerator
                        #   (   .wavelength("1285.50e-9"),
                                .bit_period("400e-12"),
                                .bit_zero_optical_power("-5.00"),
                                .bit_one_optical_power("-2.00"),
                                .bit_zero_transition_time("50e-12"),
                                .bit_one_transition_time("50e-12"))
        optical_data_gen    (   .out        (optical_data_gen_out));

    LossyOpticalNet
                        #   (   .loss("0.1")) // 270um
        waveguide_0         (   .in         (laser_out[3]), 
                                .out        (waveguide_0_out));
                            
    ResonantRingDepletionModulator  
                        #   (   .t1("0.954"),
                                .t2("0.971"),
                                .waveguide_loss("4000"),
                                .ring_radius("7.00e-6"),
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
                        #   (   .t1("0.954"),
                                .t2("0.971"),
                                .waveguide_loss("4000"),
                                .ring_radius("7.00e-6"),
                                .ring_width("1.8e-6"),
                                .ring_height("200e-9"))
        receiver_ring       (   .in         (waveguide_1_out),
                                .thru       (rec_ring_out),
                                .drop       (rec_ring_drop));
                            
    LossyOpticalNet
                        #   (   .loss("0.1")) // 270um
        waveguide_2         (   .in         (rec_ring_out),
                                .out        (waveguide_2_out));

    // Electrical path elements
    ModulatorDriver
                        #   (   .bit_period("400e-12"),
                                .bit_zero_voltage("0.60"),
                                .bit_one_voltage("-4.00"),
                                .bit_one_transition_time("50e-12"),
                                .bit_zero_transition_time("50e-12"))
        modulator_driver    (   .out        (driver_out));
        
    ThermalTuner
                        #   (   .heater_power("3e-3"),
                                .bit_width("8"),
                                .clock_period("250e-9"),
                                .pdm_pattern("0"),
                                .heater_init("0"))
        thermal_tuner       (   .heater     (heater_out));

    CurrentIntegratingReceiver
                        #   (   .sense_amp_sampling_period("400e-12"),
                                .sense_amp_preset_threshold_code("5"))
        curr_int_receiver   (   .optical_in(optical_data_gen_out),
                                .sense_amp_threshold(threshold_code),
                                .out(receiver_out));
    

endmodule

