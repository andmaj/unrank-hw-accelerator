`timescale 1ns / 1ps

module unrank32_test (
);

reg clk;
reg [31:0] num;
reg [5:0] col;
wire [5:0] row;
     
unrank32 testi (
    .clk(clk),
    .num(num),
    .col(col),
    .row(row));
    
initial begin
    clk = 0;
    num = 0;
    col = 3;
    #100
    #20
    num = 1;
    #20
    num = 2;
    #20
    num = 3;
    #20
    num = 4;
    #20
    num = 5;
    #20
    num = 6;
    #20
    num = 7;
    #20
    num = 8;
    #20
    num = 9;
    #20
    num = 10;
    #20
    num = 11;
    #20
    num = 12;
    #20
    num = 13;
    #20
    num = 14;
    #20
    num = 15;
    #20
    num = 16;
    #20
    num = 17;
    #20
    num = 18;
    #20
    num = 19;
    #20
    num = 20;
    #20
    $finish;
end

always #5 clk <= ~clk;

endmodule