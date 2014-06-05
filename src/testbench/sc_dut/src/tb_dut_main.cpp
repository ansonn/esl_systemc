/*-----------------------------------------------------------------------------
/
/
/
/----------------------------------------------------------------------------*/

#include "tb_dut.h"

int sc_main(int, char *argv[])
{
	sc_clock			clock("clock");
	DUT					dut("dut");
	sc_rvd<sc_uint<8> >	dut_to_tb;
	sc_rvd<sc_uint<8> >	tb_to_dut;
	sc_signal<bool>		reset;
	TB_DUT				tb("tb");
	sc_trace_file 		*tf;

	dut.m_clk(clock);
	dut.m_reset(reset);
	dut.m_input(tb_to_dut);
	dut.m_output(dut_to_tb);

	tb.m_clk(clock);
	tb.m_reset(reset);
	tb.m_from_dut(dut_to_tb);
	tb.m_to_dut(tb_to_dut);


	tf = sc_create_vcd_trace_file("../results/wave");
	sc_trace(tf, clock, "clk");
	sc_trace(tf, dut_to_tb.m_data, "data_d");
	sc_trace(tf, dut_to_tb.m_ready, "ready_d");
	sc_trace(tf, dut_to_tb.m_valid, "valid_d");
	sc_trace(tf, tb_to_dut.m_data, "data_t");
	sc_trace(tf, tb_to_dut.m_ready, "ready_t");
	sc_trace(tf, tb_to_dut.m_valid, "valid_t");
	
	std::cout << "producer dut consumer" << endl;
	reset = false;
	sc_start(1, SC_NS);
	reset = true;
	sc_start();
	std::cout << "program completed" << endl;

	return 0;
}






