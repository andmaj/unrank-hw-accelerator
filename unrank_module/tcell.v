`timescale 1ns / 1ps

// Table cell with greater comparator

module tcell
  #(parameter NUM_WIDTH=10,   // Width of num
    parameter CELL_WIDTH=10,  // Width of cell value
    parameter CELL_VALUE=1)  // Value of cell
  (input wire [NUM_WIDTH-1:0] num,                // Number to compare
   output wire gt,                                // Value is greater
   output reg [CELL_WIDTH-1:0] val = CELL_VALUE); // Cell value

gtm
  #(.NUM1_WIDTH(CELL_WIDTH), 
    .NUM2_WIDTH(NUM_WIDTH))
  gtmi
    (.num1(val), 
     .num2(num), 
     .gt(gt));
     
endmodule
