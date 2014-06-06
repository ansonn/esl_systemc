/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#ifndef _SC_SIMPLE_FIFO_H
#define _SC_SIMPLE_FIFO_H

#include <systemc.h>

/*-----------------------------------------------------------------------------
//	write interface
*/
class write_if : virtual public sc_interface
{
	public:
		virtual void write(char)	= 0;
		virtual void reset()		= 0;
};

/*-----------------------------------------------------------------------------
//	read interface
*/
class read_if : virtual public sc_interface
{
	public:
		virtual void read(char &)	= 0;
		virtual int nr_available()	= 0;
};

/*-----------------------------------------------------------------------------
//	fifo
*/
class fifo : public sc_channel, public write_if, public read_if
{
	public:
		fifo(sc_module_name name, int size_) : sc_channel(name), size(size_)
		{
			data		= new char[size];
			nr_elm		= 0;
			first		= 0;
			nr_read		= 0;
			max_used	= 0;
			average		= 0;
			last_time	= SC_ZERO_TIME;
		}
		~fifo()
		{
			delete [] data;
			
			cout << endl << "Fifo size\t\t\t\t:" 						<< size << endl
						 << "Average fifo fill depth\t\t\t:" 			<< double(average)/nr_read << endl
						 << "Maximum fifo fill depth\t\t\t:" 			<< max_used << endl
						 << "Average transfer time per character\t:" 	<< last_time/nr_read << endl
						 << "Total character transferred\t\t:" 			<< nr_read << endl 
						 << "Total time\t\t\t\t:" 						<< last_time << endl;
		}

		void write(char c)
		{
			if (nr_elm == size)
				wait(r_event);

			data[(first + nr_elm)%size] = c;
			++nr_elm;
			w_event.notify();
		}

		void read(char &c)
		{
			last_time = sc_time_stamp();
			
			if (nr_elm == 0)
				wait(w_event);

			compute_stats();

			c = data[first];
			--nr_elm;
			first = (first + 1)%size;
			r_event.notify();
		}

		void reset()
		{
			nr_elm	= 0;
			first	= 0;
		}

		int nr_available()
		{
			return nr_elm;
		}
		
	private:
		void compute_stats()
		{
			average += nr_elm;
			if (nr_elm > max_used)
				max_used = nr_elm;
			++nr_read;
		}

	private:
		char 		*data;
		int 		nr_elm;
		int 		first;
		int 		size;
		int			nr_read;
		int			max_used;
		int			average;
		sc_event	w_event;
		sc_event	r_event;
		sc_time		last_time;
};





#endif

