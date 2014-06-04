/*-----------------------------------------------------------------------------
/	dut		: use sc_rvd classes
/
/
/
/----------------------------------------------------------------------------*/
#include "sc_dut.h"

void DUT::work_thread(void)
{
	sc_uint<8>	data[10];

	m_input.reset();
	m_output.reset();

	wait();
	while(1)
	{
		for (int outer_i = 0; outer_i < 10; outer_i++)
		{
			for (int inner_i = 0; inner_i < outer_i; inner_i++)
			{
				data[inner_i] = m_input.read();
				std::cout << "		" << std::setw(3) << data[inner_i]
					 	  << "		" << sc_time_stamp()	<< endl;
			}
			for (int inner_i = 0; inner_i < outer_i; inner_++)
			{
				m_output = data[inner_i];
			}
		}
	}
}



