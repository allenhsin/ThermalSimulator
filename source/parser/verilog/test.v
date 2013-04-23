
module INV_X1M_A12TR(A, Y);
    input wire [299:0] A;
    output wire [87:0] Y;

    wire [31:0]     n1;    
    wire [1:0]      n2;

endmodule

module INV_X2M_A12TR (A, Y);
    input A;
    output Y; 

    wire n1, n2, n3, n4, n5;
    
endmodule

module AND_X1M_A12TR (A, B, Y);

    parameter CadenceSucks = 2;
    
    input A, B;
    output Y;

    INV_X1M_A12TR   #   (   .param1(2), 
                            .param2(3))
                blah0   (   .A(10), 
                            .Y(15));
                            
    INV_X2M_A12TR   #   (   .param1(2), 
                            .param2(3)) 
                blah1   (   1, 22) 
                blah2   (   2, 5);
    
endmodule
