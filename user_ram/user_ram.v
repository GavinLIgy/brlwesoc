module user_ram#(
	parameter ADDR_BIT = 8
) (
    input                   clk_i,
    input                   rst_i,
    input                   wr_en_i,
    input                   rd_en_i,
    input [ADDR_BIT-1:0]    addr_i,
    input [31:0]            di_i,
	output [31:0]           do_o
);

    reg [31:0]       bram[2^(ADDR_BIT) - 1:0];    
    integer          i;   
    reg [31:0]       data;

    always @(posedge clk_i)
    begin
       if (!rst_i)   
         begin
			data <= 32'b0;
			for(i = 0 ; i <= (2^(ADDR_BIT) - 1); i = i+1) //reset by word
			bram[i] <= 32'b0;
         end
       else if (wr_en_i) begin
            bram[addr_i] <= di_i;
       end
       else if (rd_en_i) begin
            data <= bram[addr_i];
			@(posedge clk_i);
			@(posedge clk_i);
			@(posedge clk_i);
       end
       else begin
			data <= 32'h16110400;      //读写均无效时，为1
       end
    end

    assign do_o = data;
endmodule
