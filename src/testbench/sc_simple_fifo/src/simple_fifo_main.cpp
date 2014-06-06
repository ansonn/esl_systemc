/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include "sc_simple_fifo.h"


SC_MODULE(producer)
{
	public:
		sc_port<write_if>	out;

		SC_HAS_PROCESS(producer);
		producer(sc_module_name name) : sc_module(name)
		{
			SC_THREAD(worker);
		}

		void worker(void)
		{
			const char *str = "ansonn.wang@spreadtrum.com,ansonn.wang@foxmail.com/n";
			const char *p	= str;
			int total		= 100000;

			while (1)
			{
				int i = 1 + int(19.0 * rand()/RAND_MAX);
				while (--i >= 0)
				{
					out->write(*p++);
					if (!*p)
						p = str;
					--total;
				}

				if (total <= 0)
					break;

				wait(1000, SC_NS);
			}
		}
};

class consumer : public sc_module
{
	public:
		sc_port<read_if> in;

		SC_HAS_PROCESS(consumer);
		consumer(sc_module_name name) : sc_module(name)
		{
			SC_THREAD(worker);
		}

		void worker()
		{
			char c;

			while (1) 
			{
				in->read(c);
				wait(100, SC_NS);
			}
		}
};


class top : public sc_module
{
	public:
		fifo fifo_inst;
		producer prod_inst;
		consumer cons_inst;

		top(sc_module_name name, int size) : sc_module(name), fifo_inst("Fifo1", size)
										   					, prod_inst("Producer1"), cons_inst("Consumer1")
		{
			prod_inst.out(fifo_inst);
			cons_inst.in(fifo_inst);
		}
};

int sc_main (int argc , char *argv[])
{
	int size = 10;

	if (argc > 1)
		size = atoi(argv[1]);

	if (size < 1)
		size = 1;

	if (size > 100000)
		size = 100000;

	//sc_set_time_resolution(10, SC_PS);
	cout << sc_get_time_resolution() << endl;
	
	top top1("Top1", size);
	sc_start();
	return 0;
}






