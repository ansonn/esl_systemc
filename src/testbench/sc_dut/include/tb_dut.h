/*-----------------------------------------------------------------------------
/
/
/
/----------------------------------------------------------------------------*/
#ifndef _TB_DUT_H
#define _TB_DUT_H

#include "sc_dut.h"


SC_MODULE(TB_DUT)
{
	sc_in<bool>					m_clk;
	sc_in<bool>					m_reset;
	sc_rvd<sc_uint<8> >::in		m_from_dut;
	sc_rvd<sc_uint<8> >::out	m_to_dut;
	
	void consumer(void);
	void producer(void);
	
	SC_CTOR(TB_DUT)
	{
		SC_CTHREAD(consumer, m_clk.pos());
		reset_signal_is(m_reset, false);
		SC_CTHREAD(producer, m_clk.pos());
		reset_signal_is(m_reset, false);
	}

};




#endif

