# ---- iCE40 HX8K Breakout Board ----

lfsrsim: lfsr_tb.vvp
	vvp -N $^
	
lfsrsynsim: lfsr_syn_tb.vvp
	vvp -N $^
	
lfsr.blif: lfsr.v
	yosys -ql lfsr.log -p 'synth_ice40 -top LFSR -blif lfsr.blif' $^
	
lfsr_tb.vvp: lfsr_tb.v lfsr.v
	iverilog -s tb_lfsr -o $@ $^ `yosys-config --datdir/ice40/cells_sim.v`
	
lfsr_syn_tb.vvp: lfsr_tb.v lfsr_syn.v
	iverilog -s tb_lfsr -o $@ $^ `yosys-config --datdir/ice40/cells_sim.v`
	
lfsr_syn.v: lfsr.blif
	yosys -p 'read_blif -wideports lfsr.blif; write_verilog lfsr_syn.v'
	
clean:
	rm -f lfsr_tb.vvp
	rm -f hx8kdemo_syn.v hx8kdemo_syn_tb.vvp hx8kdemo_tb.vvp

.PHONY: lfsrsim lfsrsynsim