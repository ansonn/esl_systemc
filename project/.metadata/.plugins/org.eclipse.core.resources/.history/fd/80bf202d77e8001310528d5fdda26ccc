/*---------------------------------------------------------------------------*/
/*
**
**
**
**---------------------------------------------------------------------------*/
#include <systemc.h>



/*---------------------------------------------------------------------------*/
// write interface
// 
class write_if : virtual public sc_interface
{
	public:
		virtual void write(char) 	= 0;
		virtual void reset() 		= 0;
};

/*---------------------------------------------------------------------------*/
// read interface
//
class read_if : virtual public sc_interface
{
	public:
		virtual void read(char &) 	= 0;
		virtual int num_available() = 0;
};


class fifo : public sc_channel, public write_if, public read_if
{
	public:
		fifo(sc_module_name name, int size_) : sc_channel(name), size(size_)
		{
			data 			= new char[size];
			num_elements 	= 0;
			first 			= 0;
			num_read 		= 0;
			max_used 		= 0;
			average 		= 0;
			last_time 		= SC_ZERO_TIME;
		}

		~fifo()
		{
			delete[] data;

			cout << endl << "Fifo size is: " << size << endl;
			cout << "Average fifo fill depth: " <<
			double(average) / num_read << endl;
			cout << "Maximum fifo fill depth: " << max_used << endl;
			cout << "Average transfer time per character: "
				 << last_time / num_read << endl;
			cout << "Total characters transferred: " << num_read << endl;
			cout << "Total time: " << last_time << endl;
		}

		void write(char c) 
		{
			if (num_elements == size)
				wait(read_event);

			data[(first + num_elements) % size] = c;
			++num_elements;
			write_event.notify();
		}

	    void read(char &c)
		{
			last_time = sc_time_stamp();
			
			if (num_elements == 0)
				wait(write_event);

			compute_stats();

			c = data[first];
			-- num_elements;
			first = (first + 1) % size;
			read_event.notify();
	    }

    	void reset() 
		{ 
			num_elements = 0;
			first 		 = 0; 
		}

    	int num_available() 
		{ 
			return num_elements;
		}

	private:
		char *data;
		int num_elements, first;
		sc_event write_event, read_event;
		int size, num_read, max_used, average;
		sc_time last_time;

		void compute_stats()
		{
			average += num_elements;

			if (num_elements > max_used)
			 max_used = num_elements;

			++num_read;
		}
};

class producer : public sc_module
{
	public:
		sc_port<write_if> out;

		SC_HAS_PROCESS(producer);
		producer(sc_module_name name) : sc_module(name)
		{
			SC_THREAD(main);
		}

		void main()
		{
			const char *str = "Visit www.systemc.org and see what SystemC can do for you today!\n";
			const char *p = str;
			int total = 100000;

			while (true)
			{
				int i = 1 + int(19.0 * rand() / RAND_MAX);  //  1 <= i <= 19

				while (--i >= 0)
				{
					out->write(*p++);
					if (!*p) p = str;
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
			SC_THREAD(main);
		}

		void main()
		{
			char c;

			while (true) 
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
	int size = 100;

	if (argc > 1)
		size = atoi(argv[1]);

	if (size < 1)
		size = 1;

	if (size > 100000)
		size = 100000;

	top top1("Top1", size);
	sc_start();
	return 0;
}
