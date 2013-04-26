
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
    parameter Wow = HolyShit;
    
    input wire [CadenceSucks:0] A;
    wire [0:0] n1;    
    output wire [87:0] Y;

    wire [1:0]      n2;

    // parameter Wow2 = n2;


endmodule

/*
 * Multi-line comments test 1
 */

/*
 * Multi-line comments
 test 2 */ module INV_X2M_A12TR (A, Y);
 
    parameter bad_param1 = 0;
    parameter bad_param2 = 0;
 
    input A;
    output Y; 

    wire n1, n2, n3, n4, n5;
    
endmodule

module AND_X1M_A12TR (A, B, Y);

    parameter CadenceSucks = 32'hFFF_F_FFFF;
    //parameter UnsupportedConcat = { CadenceSucks, CadenceSucks };
    
    input A, B;
    output Y;

    INV_X1M_A12TR   #   (   .CadenceSucks(2), 
                            .Wow(3))
                blah0   (   .A(A[5]), 
                            .Y(B[6]));
                            
    INV_X2M_A12TR   #   (   .bad_param1(2), 
                            .bad_param2(3)) 
                blah1   (   A, B)
                blah2   (   Y, B[12:3]);
    
endmodule
