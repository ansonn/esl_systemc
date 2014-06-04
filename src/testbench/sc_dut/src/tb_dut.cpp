/*-----------------------------------------------------------------------------
/
/
/
/----------------------------------------------------------------------------*/
#include "tb_dut.h"

void TB_DUT::consumer(void)
{
	sc_uint<8> data;
	
	m_from_dut.reset();
	wait();

	for (int i = 0; i < 40; i++)
	{
		data = m_from_dut.read();
		std::cout << "		" << std::setw(3) << data << "		"
				  << sc_time_stamp()	<< endl;
	}
	sc_stop();
}

void TB_DUT::producer(void)
{
	sc_uint<8> data;
	
	m_to_dut.reset();
	wait();

	for (int i = 0;; i++)
	{
		std::cout << "		" << std::setw(3) << i << "		"
				  << sc_time_stamp() << endl;
		data = i;
		m_to_dut = data;

		if (i && (i % 6 == 0))
			wait();
	}
}


