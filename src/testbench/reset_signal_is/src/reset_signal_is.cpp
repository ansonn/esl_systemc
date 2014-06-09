/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include "systemc.h"


/*-----------------------------------------------------------------------------
//	module consumer
*/
SC_MODULE(consumer)
{
	SC_CTOR(consumer)
	{
		SC_CTHREAD(worker, m_clk.pos());
		reset_signal_is(m_reset, false);
	}
	
	void worker()
	{
		cout << sc_time_stamp() << "\t: reset" << endl;
		
		while (1)
		{
			cout << "+++" << sc_time_stamp() << "\t c_value: " << m_value.read() << endl;
			
			m_ready = true;
			do { 
				wait(); 
			} while ( !m_valid);

			
		}
	}
	sc_in_clk		m_clk;
	sc_inout<bool>	m_ready;
	sc_in<bool> 	m_reset;
	sc_in<bool> 	m_valid;
	sc_in<int>		m_value;
};

/*-----------------------------------------------------------------------------
//	module producer
*/
SC_MODULE(producer)
{
	SC_CTOR(producer)
	{
		SC_CTHREAD(worker, m_clk.pos());
		reset_signal_is(m_reset, false);
	}
	
	void worker()
	{
		m_value = 0;
		cout << "producer reset" << endl;

		//systemc signal have delta delay!
		
		while (1)
		{
			tmp = m_value.read();
			cout << "---#" << sc_time_stamp() << "\t p_value: " << m_value   << " " << tmp << endl;
			m_valid = true;
			do { 
				wait(); 
			} while (!m_ready);

			m_value = m_value + 1;
			tmp = m_value.read() + 1;
			cout << "---*" << sc_time_stamp() << "\t p_value: " << m_value  << " " << tmp << endl;
		}
	}
	
	sc_in_clk		m_clk;
	sc_in<bool> 	m_ready;
	sc_in<bool> 	m_reset;
	sc_inout<bool>	m_valid;
	sc_inout<int>	m_value;
	int 			tmp;
};



/*-----------------------------------------------------------------------------
//	testbench
*/
SC_MODULE(testbench)
{
	SC_CTOR(testbench) : m_consumer("consumer"), m_producer("producer")
	{
		SC_CTHREAD(worker, m_clk.pos());
		m_consumer.m_clk(m_clk);
		m_consumer.m_ready(m_ready);
		m_consumer.m_reset(m_reset);
		m_consumer.m_valid(m_valid);
		m_consumer.m_value(m_value);
		m_producer.m_clk(m_clk);
		m_producer.m_ready(m_ready);
		m_producer.m_reset(m_reset);
		m_producer.m_valid(m_valid);
		m_producer.m_value(m_value);
	}
	void worker()
	{
		for (int state = 0; state < 100; state++)
		{
			m_reset = ((state + 1)%10 == 0) ? false : true;
			wait();
		}
		sc_stop();
	}
	
	sc_in_clk			m_clk;
	consumer			m_consumer;
	producer			m_producer;
	sc_signal<bool> 	m_ready;
	sc_signal<bool> 	m_reset;
	sc_signal<bool> 	m_valid;
	sc_signal<int>		m_value;
};



int sc_main (int argc , char *argv[])
{
	sc_clock  clock;
	testbench testbench("testbench");

	testbench.m_clk(clock);
	sc_start(5000, SC_NS);
	cout << "Done" << endl;

	
	return 0;
}






