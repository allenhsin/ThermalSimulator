
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
    parameter CadenceSucks = 2223;
    input wire [0] A;
    wire [0:0] n1;    
    output wire [87:0] Y;

    wire [1:0]      n2;

endmodule

/*
 * Multi-line comments test 1
 */

/*
 * Multi-line comments
 test 2 */ module INV_X2M_A12TR (A, Y);
    input A;
    output Y; 

    wire n1, n2, n3, n4, n5;
    
endmodule

module AND_X1M_A12TR (A, B, Y);

    parameter CadenceSucks = 32'hFFF_F_FFFF;
    
    input A, B;
    output Y;

    INV_X1M_A12TR   #   (   .param1(2), 
                            .param2(3))
                blah0   (   .A(A[5]), 
                            .Y(B[6]));
                            
    INV_X2M_A12TR   #   (   .param1(2), 
                            .param2(3)) 
                blah1   (   A, B) 
                blah2   (   Y, B[12:3]);
    
endmodule
