# ---- iCE40 HX8K Breakout Board ----

rngsim: simplerng_tb.vvp
	vvp -N $^
	
rngsynsim: simplerng_syn_tb.vvp
	vvp -N $^
	
simplerng.blif: simplerng.v
	yosys -ql simplerng.log -p 'synth_ice40 -top simplerng -blif simplerng.blif' $^
	
simplerng_tb.vvp: simplerng_tb.v simplerng.v ../LFSR/lfsr.v
	iverilog -s tb_simplerng -o $@ $^ `yosys-config --datdir/ice40/cells_sim.v`
	
simplerng_syn_tb.vvp: simplerng_tb.v simplerng_syn.v ../LFSR/lfsr.v
	iverilog -s tb_simplerng -o $@ $^ `yosys-config --datdir/ice40/cells_sim.v`
	
simplerng_syn.v: simplerng.blif
	yosys -p 'read_blif -wideports simplerng.blif; write_verilog simplerng_syn.v'
	
clean:
	rm -f simplerng_syn.v simplerng_syn_tb.vvp simplerng_tb.vvp

.PHONY: rngsim rngsynsim
