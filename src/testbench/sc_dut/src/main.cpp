/*-----------------------------------------------------------------------------
/
/
/
/----------------------------------------------------------------------------*/

#include "tb_dut.h"

int sc_main(int, char *argv[])
{
	sc_clock			clock;
	DUT					dut("dut");
	sc_rvd<sc_uint<8> >	dut_to_tb;
	sc_rvd<sc_uint<8> > tb_to_dut;
	sc_signal<bool>		reset;
	TB_DUT				tb("tb");

	dut.m_clk(clock);
	dut.m_reset(reset);
	dut.m_input(tb_to_dut);
	dut.m_output(dut_to_tb);

	tb.m_clk(clock);
	tb.m_reset(reset);
	tb.m_from_dut(dut_to_tb);
	tb.m_to_dut(tb_to_dut);

	std::cout << "producer dut consumer" << endl;
	reset = false;
	sc_start(1, SC_NS);
	reset = true;
	sc_start();
	std::cout << "program completed" << endl;

	return 0;
}






