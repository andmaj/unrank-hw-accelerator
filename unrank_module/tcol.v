`timescale 1ns / 1ps

// Binomial coefficients - Column (generic)
// Finds the highest value in the column which <= num

module tcol
  #(parameter NUM_WIDTH=10,   // Width of num
    parameter COL_INDEX=1,   // Which column is it?
    parameter ROWS_NUM=13,    // Number of rows in the column
    parameter VALUE_WIDTH=10, // Width of the output value
    parameter ROW_WIDTH=4)
  (input wire  [NUM_WIDTH-1:0] num,         // Number to compare
   output wire [ROW_WIDTH-1:0] row,  // Row of last cell which <= num	
   output wire [VALUE_WIDTH-1:0] val);      // Value of this cell

wire [ROWS_NUM:0] bitrun;	     // like [1] 1 1 1 0 0 0 0 
wire [ROWS_NUM-1:0] bitrunxor;   // like 0 0 0 0 0 1 0 0 0
wire [VALUE_WIDTH-1:0] values [0:ROWS_NUM-1];

assign bitrun[ROWS_NUM] = 1;

// Calculates factorial
function integer fact; 
  input [31:0] operand; 
  integer i; 
  integer tmp;
  begin
    tmp = operand;
    if(tmp==0 || tmp==1)
      fact = 1;
    else
      begin
        i = 1;
        fact = 1;
        while(tmp>0)
          begin
            fact = fact * i;
            tmp = tmp - 1;
            i = i + 1;
          end
      end
  end
endfunction

// Calculates a binomial coefficient
function integer binom;
  input [31:0] n;
  input [31:0] k;
  begin
    if(k>n)
      binom = 0;
    else
      binom = fact(n)/(fact(k)*fact(n-k));
  end
endfunction

function integer bits2num;
  input [ROWS_NUM-1:0] operand;
  integer i;
  integer n;
  begin
    n = 0;
	for(i=ROWS_NUM-1; i>=0; i=i-1)
	  begin
	    if(operand[i]==1)
	      n = n + i;
	  end
	bits2num = n;
  end  
endfunction

genvar i;
generate
for(i=0; i<ROWS_NUM; i=i+1) begin:tcelliarr
  tcell #(
    .NUM_WIDTH(NUM_WIDTH),
    .CELL_WIDTH(VALUE_WIDTH),
    .CELL_VALUE(binom(i, COL_INDEX))) 
  tcelli(
    .num(num),
	.gt(bitrun[i]),
	.val(values[i])
  );
end
endgenerate

genvar a;
for (a = ROWS_NUM-1; a >= 0; a = a - 1)
begin:valami
  assign bitrunxor[a] = bitrun[a] ^ bitrun[a+1];
end

assign row = bits2num(bitrunxor);
assign val = values[bits2num(bitrunxor)];

endmodule
