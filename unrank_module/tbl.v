`timescale 1ns / 1ps

// Binomial coefficients table (generic)

module tbl
  #(parameter NUM_WIDTH=10,   // Width of num
    parameter ROWS_NUM=13,    // Number of rows
    parameter VALUE_WIDTH=10,  // Width of the output value
    parameter COL_WIDTH=4,
    parameter ROW_WIDTH=4)
  (input wire [NUM_WIDTH-1:0] num,	       // Number to compare
   input wire [COL_WIDTH-1:0] col,  // Column to search
   output wire [ROW_WIDTH-1:0] row,  // Row of last cell which <= num
   output wire [VALUE_WIDTH-1:0] val);       // Value of this cell

wire [ROW_WIDTH-1:0] rows [0:ROWS_NUM-1];
wire [VALUE_WIDTH-1:0] values [0:ROWS_NUM-1];

genvar i;
generate
for(i=0; i<ROWS_NUM; i=i+1) begin:tcoliarr
  tcol #(
    .NUM_WIDTH(NUM_WIDTH),
	.COL_INDEX(i),
	.ROWS_NUM(ROWS_NUM),
	.VALUE_WIDTH(VALUE_WIDTH), 
	.ROW_WIDTH(ROW_WIDTH))
  tcoli(
    .num(num),
	.row(rows[i]),
	.val(values[i]));
end
endgenerate

assign row = rows[col];
assign val = values[col];

endmodule

