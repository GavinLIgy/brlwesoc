# ---- iCE40 HX8K Breakout Board ----

ramsim: user_ram_tb.vvp
	vvp -N $^
	
ramsynsim: user_ram_syn_tb.vvp
	vvp -N $^
	
user_ram.blif: user_ram.v
	yosys -ql user_ram.log -p 'synth_ice40 -top user_ram -blif user_ram.blif' $^
	
user_ram_tb.vvp: user_ram_tb.v user_ram.v
	iverilog -s tb_user_ram -o $@ $^ `yosys-config --datdir/ice40/cells_sim.v`
	
user_ram_syn_tb.vvp: user_ram_tb.v user_ram_syn.v
	iverilog -s tb_user_ram -o $@ $^ `yosys-config --datdir/ice40/cells_sim.v`
	
user_ram_syn.v: user_ram.blif
	yosys -p 'read_blif -wideports user_ram.blif; write_verilog user_ram_syn.v'
	
clean:
	rm -f user_ram_syn.v user_ram_syn_tb.vvp user_ram_tb.vvp

.PHONY: ramsim ramsynsim
