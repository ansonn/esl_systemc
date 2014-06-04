/*-----------------------------------------------------------------------------
/
/
/
/-----------------------------------------------------------------------------*/
#ifndef _SC_DUT_H
#define _SC_DUT_H

#include <iomanip>
#include <systemc.h>
#include "sc_rvd.h"


SC_MODULE(DUT)
{
	sc_in<bool>					m_clk;
	sc_rvd<sc_uint<8> >::in		m_input;
	sc_rvd<sc_uint<8> >::out	m_output;
	sc_in<bool>					m_reset;

	void work_thread(void);

	SC_CTOR(DUT)
	{
		SC_CTHREAD(work_thread, m_clk.pos());
		reset_signal_is(m_reset, false);
	}
};





#endif

