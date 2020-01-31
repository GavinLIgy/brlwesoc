`timescale 1ns / 1ps

`define N_BITS 32

module tb_simplerng;

	reg clk;
	reg resetn;
	reg enable;

	reg dat_we;
	reg dat_re;
	reg [`N_BITS-1:0] dat_di;

	wire [`N_BITS-1:0] dat_do;
	wire dat_wait;

	wire [`N_BITS-1:0] wir_dat_do;
	
	simplerng #(.NUM_BITS(`N_BITS)) dut(	
		.clk(		clk		),
		.resetn(	resetn	),
		.enable(	enable	),
		.dat_we(	dat_we	),
		.dat_re(	dat_re	),
		.dat_di(	dat_di	),
		.dat_do(	dat_do	),
		.dat_wait(	dat_wait	));
	
	always #10 clk = ~clk;

	assign wir_dat_do = (dat_wait | ~(dat_we) ) ? `N_BITS'hffff_ffff : dat_do ;
	
	initial begin
		$dumpfile("testbench_simplerng.vcd");
		$dumpvars(0, tb_simplerng);
		$dumpflush;

		clk = 0;
		resetn = 1;
		enable = 0;
		dat_we = 0;
		dat_re = 0;
		dat_di = 0;
		
		//quiet
		#15;
		
		//start the RNG
		enable = 1;

		//working
		#700
		
		//read the number
		@(posedge clk);
		dat_we=1;
		@(posedge clk);
		dat_we=0;
		
		//working
		#500		
		
		//pause the RNG(the output would keep)
		enable = 0;
		
		//pausing
		#50
		
		//restart the RNG
		enable = 1;
		
		//working
		#200

		//reset the RNG(start from the beginning of the sequence)
		resetn = 0;
		@(posedge clk);
		#5;
		resetn = 1;
		
		//runing
		#50

		$finish;
	end
	
endmodule
