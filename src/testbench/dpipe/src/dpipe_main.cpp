/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include "systemc.h"


/*-----------------------------------------------------------------------------
//	delay pipeline
*/
template<typename T, int N> class dpipe : public sc_module
{
	public:
		SC_CTOR(dpipe) : in(m_pipe[0]), out(m_pipe[N - 1])
		{
			SC_METHOD(rachet);
			sensitive << m_clk.pos();
		}

		void rachet(void)
		{
			for (int i = N - 1; i > 0; i--)
			{
				m_pipe[i].write(m_pipe[i - 1].read());
			}
			
			cout << "**" << endl << "------->rachet" << endl;
		}

	public:
		sc_in_clk		m_clk;
		sc_signal<T>	&in;
		sc_signal<T>	&out;
		sc_signal<T>	m_pipe[N];

};

/*-----------------------------------------------------------------------------
//	testbench
*/
template<class T> SC_MODULE(reader)
{
	public:
		SC_CTOR(reader)
		{
			SC_METHOD(extract);
			sensitive << m_clk.pos();

			// don't schedule the spawned process for an initial execution, 
			// by default it is scheduled for an initial execution
			dont_initialize();
		}

		void extract(void)
		{
			cout << sc_time_stamp() << ":" << m_in.read() << endl;
		}

	public:	
		sc_in_clk	m_clk;
		sc_in<T>	m_in;
		
};

template<class T> SC_MODULE(writer)
{
	SC_CTOR(writer)
	{
		SC_METHOD(insert);
		sensitive << m_clk.pos();
		m_counter	= 0;
	}

	void insert(void)
	{
		m_counter++;
		m_out.write(m_counter);
		cout << "------->insert: " << m_counter << endl;
	}

	sc_in_clk		m_clk;
	T				m_counter;
	sc_out<T>		m_out;
};


int sc_main (int argc , char *argv[])
{
	sc_clock					clock("clk", 1, SC_NS);
	dpipe<sc_biguint<121>, 4>	delay("pipe");
	reader<sc_biguint<121> >	r("reader");
	writer<sc_biguint<121> >	w("writer");	
	sc_trace_file 				*tf;

	delay.m_clk(clock);

	r.m_clk(clock);
	r.m_in(delay.out);

	w.m_clk(clock);
	w.m_out(delay.in);

	
	tf = sc_create_vcd_trace_file("../results/wave");	
	sc_trace(tf, clock, "clk");
	sc_trace(tf, delay.m_pipe[0], "pipe0");
	sc_trace(tf, delay.m_pipe[1], "pipe1");
	sc_trace(tf, delay.m_pipe[2], "pipe2");
	sc_trace(tf, delay.m_pipe[3], "pipe3");


	cout << "default time resolution : " << sc_get_time_resolution() << endl;

	sc_start(10, SC_NS);
	
	return 0;
}






