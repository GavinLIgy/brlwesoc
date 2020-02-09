/*
	Based on the work of PicoSoc, modified and designed by Gavin
	
	Define:
		RNGMode: mode selection.
			RNG Mode 0:  LFSR pseudo RNG
			RNG Mode 1:  TRNG_CrypTech
			RNG Mode 2:  TRNG_infnoise
			
	Parameters:
		NUM_BITS: Width of output (in bits) , ranging from 4 to 32[default] at Mode = 0.
*/

`define RNGMode0
//`define RNGMode1
//`define RNGMode2

module simplerng #(parameter NUM_BITS = 32)
(
	input 			clk,
	input 			resetn,
	input 			enable,

	input         		dat_we,//we = 1; processor write 
	input         		dat_re,//re = 1; processor read
	input  [NUM_BITS-1:0] 	dat_di,
	output [NUM_BITS-1:0] 	dat_do,
	output 			dat_wait       
);

`ifdef RNGMode0
  //Mode = 0:  LFSR pseudo RNG

	wire wir_dat_wait;
	
	assign dat_wait = wir_dat_wait | ~(resetn & enable);//wait = 1, cannot read now

	lfsr #(.NUM_BITS(NUM_BITS)) dut(	
		.i_Clk(		clk		),
		.i_Enable(	enable		),
		.i_Seed_DV( !(resetn)|dat_we	),
		.i_Seed_Data(	dat_di		),
		.o_LFSR_Data(	dat_do		),
		.o_LFSR_Done(	wir_dat_wait	));

`else
  `ifdef RNGMode1
      //Mode = 1:  TRNG_CrypTech
      //To be completed
      `error "RNG mode 1 is not available yet!"
  `else
    `ifdef RNGMode2
        //Mode = 2:  TRNG_infnoise
	//To be completed
        `error "RNG mode 2 is not available yet!" 
    `else
      `error "RNG mode must be chosen!"
    `endif
  `endif
`endif

endmodule
