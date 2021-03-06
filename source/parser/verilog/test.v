
/*
 * Multi-line comments test 0
 */

//Single line 
//comments test
module INV_X1M_A12TR(A, Y);
    // Comment
    // More
    // Comment
    // More
    parameter CadenceSucks = 23;
    parameter HolyShit = CadenceSucks;
    parameter Wow = HolyShit-1;
    parameter Crap = "this is a string";
    parameter ConcatTest = { 1'd0, 2'd1, 3'd4 };
    
    input A;
    output Y;
    
    wire [33:0] n1;    
    wire [1:0]      n2;
    wire [CadenceSucks*(2-1-1)*5:0] n3;
    wire [CadenceSucks-1:0] n4;
    wire ["0":0] \1.5425@$%escaped_identifier3 ;

    // parameter Wow2 = n2;


endmodule

/*
 * Multi-line comments test 1
 */

/*
 * Multi-line comments
 test 2 */ module INV_X2M_A12TR (A, Y);
 
    parameter bad_param1 = 0;
    parameter bad_param2 = 32'd7734;
    parameter whoa = 1;
 
    input [bad_param2-1:0] A;
    output Y; 

    wire [bad_param2-1:0] n1, n2, n3, n4, n5;

    INV_X1M_A12TR   #   (   .Wow(3))
            terrible0   (   .A(A),
                            .Y(Y));
    
endmodule

module AND_X1M_A12TR (A, B, Y);

    parameter CadenceSucks = 32'hFFF_F_FFFF;
    parameter YesIt_Does = 4'b11010;
    parameter TestParam = YesIt_Does;
    parameter String_thing = "Awesome giant big string parameter that is very long";
    parameter AwesomeConcat = { YesIt_Does, YesIt_Does };
    
    input [6:0] A, B;
    output [AwesomeConcat-1:0] Y;

    INV_X1M_A12TR   #   (   .CadenceSucks(2), 
                            .Wow(3))
                blah0   (   A[5], 
                            B[6]);
                            

    INV_X1M_A12TR   wow   
                        (   .A(), 
                            .Y(B[6]));

    INV_X2M_A12TR   #   (   .bad_param1(CadenceSucks / (2*123)), 
                            .bad_param2(YesIt_Does * 2),
                            .whoa(String_thing)) 
                blah1   (   .A(A), .Y(B[1]))
                blah2   (   .A(Y), .Y(B[3:3]));
    
endmodule
