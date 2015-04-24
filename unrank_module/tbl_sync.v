`timescale 1ns / 1ps

module tbl_sync (
       input wire clk,
       input wire [NUM_WIDTH-1:0] num,          // Number to compare
       input wire [COL_WIDTH-1:0] col,          // Column to search 
       output reg [ROW_WIDTH-1:0] row,          // Row of last cell which <= num 
       output reg [VALUE_WIDTH-1:0] val);       // Value of this cell

parameter NUM_WIDTH=10;   // Width of num
parameter ROWS_NUM=13;    // Number of rows
parameter VALUE_WIDTH=10; // Width of the output value
parameter COL_WIDTH=4;
parameter ROW_WIDTH=4;

wire [ROW_WIDTH-1:0] row_res;
wire [VALUE_WIDTH-1:0] val_res;

// dirty hack for Vivado (+1)
tbl #(
    .NUM_WIDTH(NUM_WIDTH),
	.ROWS_NUM(ROWS_NUM),
	.VALUE_WIDTH(VALUE_WIDTH), 
	.COL_WIDTH(COL_WIDTH),
	.ROW_WIDTH(ROW_WIDTH))
  tbli(
    .num(num),
    .col(col),
	.row(row_res),
	.val(val_res));

always @ (posedge clk)
begin
    row <= row_res;
    val <= val_res;
end
 
endmodule
