`timescale 1ns / 100ps

`define N_BITS 4
module tb_lfsr;

	reg i_Clk;
	reg i_Enable;

	// Optional Seed Value
	reg i_Seed_DV;
	reg [`N_BITS-1:0] i_Seed_Data;

	wire [`N_BITS-1:0] o_LFSR_Data;
	wire o_LFSR_Done;

	lfsr #(.NUM_BITS(`N_BITS)) dut(	
		.i_Clk(		i_Clk		),
		.i_Enable(	i_Enable	),
		.i_Seed_DV(	i_Seed_DV	),
		.i_Seed_Data(	i_Seed_Data	),
		.o_LFSR_Data(	o_LFSR_Data	),
		.o_LFSR_Done(	o_LFSR_Done	));
	
	always #10 i_Clk = ~i_Clk;
	
	initial begin
		$dumpfile("testbench_LFSR.vcd");
		$dumpvars(0, tb_lfsr);
		$dumpflush;

		i_Clk = 0;
		i_Enable = 0;
		i_Seed_DV = 0;
		i_Seed_Data = 0;
		
		//start the RNG
		#15;
		i_Enable = 1;
		i_Seed_DV = 1;
		@(posedge i_Clk);
		#5;
		i_Seed_DV = 0;

		#500
		
		//stop the RNG(the output would keep)
		//i_Enable = 0;
		@(posedge i_Clk);
		@(posedge i_Clk);
		//#5

		//reset the RNG(start from the beginning of the sequence)
		i_Enable = 1;
		i_Seed_DV = 1;
		@(posedge i_Clk);
		@(posedge i_Clk);
		//#5;
		i_Seed_DV = 0;
		@(posedge i_Clk);
		@(posedge i_Clk);
		#50

		$finish;
	end
	
endmodule
