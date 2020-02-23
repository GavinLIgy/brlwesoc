/*
 *  PicoSoC - A simple example SoC using PicoRV32
 *
 *  Copyright (C) 2017  Clifford Wolf <clifford@clifford.at>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

module hx8kdemo (
	input clk,

	output ser_tx,
	input ser_rx,

	output [7:0] leds,

	output flash_csb,
	output flash_clk,
	inout  flash_io0,
	inout  flash_io1,
	inout  flash_io2,
	inout  flash_io3,

	output debug_ser_tx,
	output debug_ser_rx,

	output debug_flash_csb,
	output debug_flash_clk,
	output debug_flash_io0,
	output debug_flash_io1,
	output debug_flash_io2,
	output debug_flash_io3
);
	reg [5:0] reset_cnt = 0;
	wire resetn = &reset_cnt;

	always @(posedge clk) begin
		reset_cnt <= reset_cnt + !resetn;
	end

	wire flash_io0_oe, flash_io0_do, flash_io0_di;
	wire flash_io1_oe, flash_io1_do, flash_io1_di;
	wire flash_io2_oe, flash_io2_do, flash_io2_di;
	wire flash_io3_oe, flash_io3_do, flash_io3_di;

	SB_IO #(
		.PIN_TYPE(6'b 1010_01),
		.PULLUP(1'b 0)
	) flash_io_buf [3:0] (
		.PACKAGE_PIN({flash_io3, flash_io2, flash_io1, flash_io0}),
		.OUTPUT_ENABLE({flash_io3_oe, flash_io2_oe, flash_io1_oe, flash_io0_oe}),
		.D_OUT_0({flash_io3_do, flash_io2_do, flash_io1_do, flash_io0_do}),
		.D_IN_0({flash_io3_di, flash_io2_di, flash_io1_di, flash_io0_di})
	);

	wire        iomem_valid;
	reg         iomem_ready;
	wire [3:0]  iomem_wstrb;
	wire [31:0] iomem_addr;
	wire [31:0] iomem_wdata;
	reg  [31:0] iomem_rdata;

	reg [31:0] gpio;
	assign leds = gpio;
	
	//Module SimpleRNG interface
	
	wire        simplerng_enable;
	wire        simplerng_dat_we;
	wire 	    simplerng_dat_re;
	wire [31:0] simplerng_dat_di;
	wire [31:0] simplerng_dat_do;
	wire        simplerng_dat_wait;
	
	wire        simplerng_dat_sel;//data selector

	assign simplerng_enable = 1'b1;//RNG keep enable; No pause-restart feature;

	assign simplerng_dat_sel = iomem_valid && (iomem_addr == 32'h 0300_1000);

	assign simplerng_dat_we = simplerng_dat_sel ? (|iomem_wstrb) : 1'b 0;	//(sel && wstrb[0]) = 1 ==> we = 1; processor write
	assign simplerng_dat_re = simplerng_dat_sel && (~|iomem_wstrb) ;		//(sel && wstrb = 4'b 0000) = 1 ==> re = 1; processor read
	
	assign simplerng_dat_di[ 7: 0] = iomem_wstrb[0] ? iomem_wdata[ 7: 0] : 8'b 0;
	assign simplerng_dat_di[15: 8] = iomem_wstrb[1] ? iomem_wdata[15: 8] : 8'b 0;
	assign simplerng_dat_di[23:16] = iomem_wstrb[2] ? iomem_wdata[23:16] : 8'b 0;
	assign simplerng_dat_di[31:24] = iomem_wstrb[3] ? iomem_wdata[31:24] : 8'b 0;
	
	//User RAM Memory interface
	
	reg ram_ready;
	wire [31:0] ram_rdata;
	
	/*
	wire        user_ram_we;
	wire 	    user_ram_re;
	wire [31:0] user_ram_di;
	wire [31:0] user_ram_do;
	wire        user_ram_wait;
	
	wire        user_ram_dat_sel;//data selector

	assign user_ram_dat_sel = iomem_valid && (iomem_addr >= 32'h 0300_2000);

	assign user_ram_we = user_ram_dat_sel ? (|iomem_wstrb) : 1'b 0;	//(sel && wstrb[0]) = 1 ==> we = 1; processor write
	assign user_ram_re = user_ram_dat_sel && (~|iomem_wstrb) ;		//(sel && wstrb = 4'b 0000) = 1 ==> re = 1; processor read
	
	assign user_ram_di[ 7: 0] = iomem_wstrb[0] ? iomem_wdata[ 7: 0] : 8'b 0;
	assign user_ram_di[15: 8] = iomem_wstrb[1] ? iomem_wdata[15: 8] : 8'b 0;
	assign user_ram_di[23:16] = iomem_wstrb[2] ? iomem_wdata[23:16] : 8'b 0;
	assign user_ram_di[31:24] = iomem_wstrb[3] ? iomem_wdata[31:24] : 8'b 0;*/
	
	always @(posedge clk) begin	
		if (!resetn) begin
			gpio <= 0;
			iomem_rdata <= 0;
			iomem_ready <= 0;
		end else begin
			iomem_ready <= 0;
			if (iomem_valid && !iomem_ready) begin
				if (iomem_addr == 32'h 0300_0000) begin
					iomem_ready <= 1;
					iomem_rdata <= gpio;
					if (iomem_wstrb[0]) gpio[ 7: 0] <= iomem_wdata[ 7: 0];
					if (iomem_wstrb[1]) gpio[15: 8] <= iomem_wdata[15: 8];
					if (iomem_wstrb[2]) gpio[23:16] <= iomem_wdata[23:16];
					if (iomem_wstrb[3]) gpio[31:24] <= iomem_wdata[31:24];
				end
				else if (iomem_addr == 32'h 0300_1000) begin
					iomem_ready <= 1;
					iomem_rdata <= (simplerng_dat_wait | ~(simplerng_dat_re) ) ? 32'hffff_ffff : simplerng_dat_do; //wait = 1, cannot read now
				end
				else if (iomem_addr >= 32'h 0300_2000)begin
					/*iomem_ready <= 1;
					iomem_rdata <= user_ram_re ? user_ram_do : 32'h1311_2077; //wait = 1, cannot read now*/
					iomem_ready <= ram_ready;
					iomem_rdata <= ram_ready ? ram_rdata : 32'h1311_2077;
				end
			end
		end
	end

	picosoc soc (
		.clk          (clk         ),
		.resetn       (resetn      ),

		.ser_tx       (ser_tx      ),
		.ser_rx       (ser_rx      ),

		.flash_csb    (flash_csb   ),
		.flash_clk    (flash_clk   ),

		.flash_io0_oe (flash_io0_oe),
		.flash_io1_oe (flash_io1_oe),
		.flash_io2_oe (flash_io2_oe),
		.flash_io3_oe (flash_io3_oe),

		.flash_io0_do (flash_io0_do),
		.flash_io1_do (flash_io1_do),
		.flash_io2_do (flash_io2_do),
		.flash_io3_do (flash_io3_do),

		.flash_io0_di (flash_io0_di),
		.flash_io1_di (flash_io1_di),
		.flash_io2_di (flash_io2_di),
		.flash_io3_di (flash_io3_di),

		.irq_5        (1'b0        ),
		.irq_6        (1'b0        ),
		.irq_7        (1'b0        ),

		.iomem_valid  (iomem_valid ),//output
		.iomem_ready  (iomem_ready ),//input
		.iomem_wstrb  (iomem_wstrb ),//output
		.iomem_addr   (iomem_addr  ),//output
		.iomem_wdata  (iomem_wdata ),//output
		.iomem_rdata  (iomem_rdata )//input
	);
 
	simplerng #(.NUM_BITS(32)) dut(	
		.clk(		clk	),
		.resetn(	resetn	),

		.enable(	simplerng_enable	),
		.dat_we(	simplerng_dat_we	),
		.dat_re(	simplerng_dat_re	),
		.dat_di(	simplerng_dat_di	),
		.dat_do(	simplerng_dat_do	),
		.dat_wait(	simplerng_dat_wait	));	
	/*
	user_ram #(.ADDR_BIT(8)) m_ram(
		.clk_i(		clk					),
		.rst_i(		resetn				),
        .wr_en_i(	user_ram_we 		),
		.rd_en_i(	user_ram_re			),
		.addr_i(	iomem_addr[7:0]		),
		.di_i(		user_ram_di 		),
		.do_o(		user_ram_do			));
	*/
	picosoc_mem #(
		.WORDS(256)
	) user_memory (
		.clk(clk),
		.wen((iomem_valid && !iomem_ready && (iomem_addr - 32'h 0300_2000) < 4*256) ? mem_wstrb : 4'b0),
		.addr(iomem_addr[23:2]),
		.wdata(iomem_wdata),
		.rdata(ram_rdata)
	);

	assign debug_ser_tx = ser_tx;
	assign debug_ser_rx = ser_rx;

	assign debug_flash_csb = flash_csb;
	assign debug_flash_clk = flash_clk;
	assign debug_flash_io0 = flash_io0_di;
	assign debug_flash_io1 = flash_io1_di;
	assign debug_flash_io2 = flash_io2_di;
	assign debug_flash_io3 = flash_io3_di;
endmodule
