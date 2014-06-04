/*-----------------------------------------------------------------------------
/		implementing a channel, in port, and out port may be used to transfer 
/	data via a "ready-valid" protocol.
/
-----------------------------------------------------------------------------*/
#ifndef _SC_RVD_H
#define _SC_RVD_H

/*-----------------------------------------------------------------------------
//  sc_rvd_in<D>	: port to read value from an sc_rvd channel
*/
template<typename D>
class sc_rvd_in
{
	public:
		sc_rvd_in(const char *name = sc_gen_unique_name("sc_rvd_in"))
								: m_data((std::string(name) + "_data").c_str())
								, m_ready((std::string(name) + "_ready").c_str())
								, m_valid((std::string(name) + "_valid").c_str())
		{	
			return;
		}
	
	public:
		template<typename CHANNEL>
		inline void bind(CHANNEL &channel)
		{
			m_data(channel.m_data);
			m_ready(channel.m_ready);
			m_valid(channel.m_valid);
		}
		
		template<typename CHANNEL>
		inline void operator() (CHANNEL &channel)
		{
			bind(channel);
		}

	public:
		inline bool nb_can_read(void)
		{
			return m_valid.read();
		}

		inline bool nb_read(D &data)
		{
			data = m_data;
			if (m_valid.read() == true)
			{
				m_ready	= true;
				return true;
			}
			else
				return false;
		}

		inline void reset(void)
		{
			m_ready	= false;
		}
		
		inline D read()
		{
			m_ready = true;
			do {
				::wait();
			} while (m_valid.read() == false);

			m_ready = false;
			return m_data.read();
		}
	
		inline operator D ()
		{
			return read();
		}
	
	protected:
		sc_in<D>		m_data;
		sc_out<bool>	m_ready;
		sc_in<bool>		m_valid;
};


/*-----------------------------------------------------------------------------
//  sc_rvd_out<D>	: port to write values to an sc_rvd channel
*/
template<typename D>
class sc_rvd_out
{
	public:
		sc_rvd_out(const char *name = sc_gen_unique_name("sc_rvd_out"))
								: m_data((std::string(name) + "_data").c_str())
								, m_ready((std::string(name) + "_ready").c_str())
								, m_valid((std::string(name) + "_valid").c_str())
		{
			return;
		}

	public:
		// channel binding method and operator
		template<typename CHANNEL>
		inline void bind(CHANNEL &channel)
		{
			m_data(channel.m_data);
			m_ready(channel.m_ready);
			m_valid(channel.m_valid);
		}

		template<typename CHANNEL>
		inline void operator() (CHANNEL &channel)
		{
			bind(channel);
		}
	public:
		inline bool nb_can_write(void)
		{
			return m_valid.read() == false || m_ready.read() == true;
		}

		inline bool nb_write(const D &data)
		{
			if (m_ready.read() == true)
			{
				m_data 	= data;
				m_valid	= true;
				return true;
			}
			else
				return false;
		}
		
		inline void reset(void)
		{
			m_valid = false;
		}
	
		inline void write(const D &data)
		{
			m_data 	= data;
			m_valid	= true;
			do {
				::wait();
			} while (m_ready.read() == false);
	
			m_valid	= false;
		}
	
		inline const D &operator= (const D &data)
		{
			write(data);
			return data;
		}

	protected:
		sc_out<D> 		m_data;
		sc_in<bool>		m_ready;
		sc_out<bool>	m_valid;

};

/*-----------------------------------------------------------------------------
//  sc_rvd<D>	: communication channel with a ready-valid handshake
*/
template<typename D>
class sc_rvd
{
	public:
		// channel
		typedef sc_rvd<D>		channel;
		// input port
		typedef sc_rvd_in<D>	in;
		// output port
		typedef sc_rvd_out<D>	out;

	public:
		// data to be transferred
		sc_signal<D>			m_data;
		// true if the channel reader if ready for data
		sc_signal<D>			m_ready;
		// true if the channel writer has provided data
		sc_signal<D>			m_valid;
};

#endif

