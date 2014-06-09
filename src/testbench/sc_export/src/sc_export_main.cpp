/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/


/*
In this example, module D contains a channel of type C which
implements an interface C_if. D makes the interface C visible to the
outside by an interface-port named "IFP". Module E contains an
instance of D and also contains another instance of C. E exports both
interfaces as interface-ports IFP1 and IFP2. Both IFP1 and IFP2 are
bound to ports P1 and P2 of module X.

              +-------------+             +-------------------+
              |     x       |             |     e    +----+   |
              |             |p_1     ifp_1|          | c  |   |
              |            [ ]------------O----------@    |   |
              |             |             |          |    |   |
              |             |             |          +----+   |
              |             |             |                   |
              |             |             |      +----------+ |
              |             |             |      | d        | |
              |             |             |      |   +----+ | |
              |             |p_2     ifp_2| ipf_0|   | c  | | |
              |            [ ]------------O------O---@    | | |
              |             |             |      |   |    | | |
              |             |             |      |   +----+ | |
              |             |             |      |          | |
              |             |             |      +----------+ |
              +-------------+             +-------------------+

 Legend:
   [ ]   port
    O    interface-port
    @    interface

*/






#include "systemc.h"


/*-----------------------------------------------------------------------------
/  interface */
class c_interface : virtual public sc_interface
{
	public:
		virtual void run(void)		= 0;
};

/* channel */
class c : public c_interface, public sc_channel
{
	public:
		SC_CTOR(c)
		{
		}

		virtual void run(void)
		{
			cout << sc_time_stamp() << "  in channel run()" << endl;
		}
};

/*-----------------------------------------------------------------------------
/  module D;  export channel c throught ifp */
SC_MODULE(d)
{
	sc_export<c_interface>	ifp_0;

	SC_CTOR(d) : ifp_0("ifp_0"), m_c("moduled")
	{
		/* bind sc_export->interface by name */
		ifp_0(m_c);
	}

	private:
		/* channel */
		c	m_c;

};

/*-----------------------------------------------------------------------------
/  module e; export two interface port */
SC_MODULE(e)
{
	private:
		c	m_c;
		d	m_d;
		

	public:
		sc_export<c_interface>	ifp_1;
		sc_export<c_interface>	ifp_2;

		SC_CTOR(e) : m_c("e_chn"), m_d("e_moduled"), ifp_1("ifp_1"), ifp_2("ifp_2")
		{
			ifp_1(m_c);
			ifp_2(m_d.ifp_0);

			ifp_1.get_interface();
			ifp_2.get_interface();
		}


};

/*-----------------------------------------------------------------------------
/  module x; connected to the channels through e */
SC_MODULE(x)
{
	sc_port<c_interface>	p_1;
	sc_port<c_interface> 	p_2;

	SC_CTOR(x)
	{
		SC_THREAD(run);
	}

	void run(void)
	{
		cout << "*" << endl;
		
		wait(10, SC_NS);
		p_1->run();

		wait(10, SC_NS);
		p_2->run();
	}
};



/*-----------------------------------------------------------------------------
/
*/
int sc_main (int argc , char *argv[])
{
	e 	the_e("e");
	x	the_x("x");

	the_x.p_1(the_e.ifp_1);
	the_x.p_2(the_e.ifp_2);


	cout << "default time resolution : " << sc_get_time_resolution() << endl;



	sc_start(17, SC_NS);

	cout << "----" << endl;
	the_e.ifp_1->run();
	cout << "----" << endl;

	sc_start(50, SC_NS);
	
	return 0;
}






