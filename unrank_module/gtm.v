`timescale 1ns / 1ps

// Greater comparator module

module gtm
  #(parameter NUM1_WIDTH=10,
    parameter NUM2_WIDTH=10)
  (input wire [NUM1_WIDTH-1:0] num1,
   input wire [NUM2_WIDTH-1:0] num2,
   output wire gt); 	              // Is num1 > num2 ?
   
assign gt = (num1>num2);

endmodule
