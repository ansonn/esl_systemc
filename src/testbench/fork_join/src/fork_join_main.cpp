/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include "systemc.h"


/*-----------------------------------------------------------------------------
*/
int test_function(double d)
{
	cout << endl << "Test_function sees " << d << endl;
	return int(d);
}

/*-----------------------------------------------------------------------------
*/
void void_function(double d)
{
	cout << endl << "void_function sees " << d << endl;
}

/*-----------------------------------------------------------------------------
*/	
int ref_function(const double &d)
{
	cout << endl << "ref_function sees " << d << endl;
	return int(d);
}
	
/*-----------------------------------------------------------------------------
*/
class top : public sc_module
{
	public:
		SC_HAS_PROCESS(top);
		top(sc_module_name name) : sc_module(name) 
		{
			SC_THREAD(worker);
		}

		void worker()
		{
			int r;
			sc_event e1, e2, e3, e4;

			cout << endl;
			e1.notify(100, SC_NS);

			// Spawn several threads that co-operatively execute in round robin order
			// SC_FORK ... SC_JOIN struct only wait until all spawned processes have returned.
			SC_FORK
			sc_spawn(&r, sc_bind(&top::round_robin, this, "1", sc_ref(e1), sc_ref(e2), 3), "1") ,
			sc_spawn(&r, sc_bind(&top::round_robin, this, "2", sc_ref(e2), sc_ref(e3), 3), "2") ,
			sc_spawn(&r, sc_bind(&top::round_robin, this, "3", sc_ref(e3), sc_ref(e4), 3), "3") ,
			sc_spawn(&r, sc_bind(&top::round_robin, this, "4", sc_ref(e4), sc_ref(e1), 3), "4") ,
			SC_JOIN

			cout << "returned int is " << r << endl;
			cout << endl << endl;

			// Test that threads in thread pool are successfully reused ...
			for (int i = 0 ; i < 10; i++)
			{
				sc_spawn(&r, sc_bind(&top::wait_and_end, this, i));
			}


			wait(20, SC_NS);
			// Show how to use sc_spawn_options 
			sc_spawn_options o;
			o.set_stack_size(0);


			// Demo of a function rather than method call, & use return value ...
			wait(sc_spawn(&r, sc_bind(&test_function, 3.14159)).terminated_event());
			cout << "returned int is " << r << endl;

			sc_process_handle handle1 = sc_spawn(sc_bind(&void_function, 1.2345));
			wait(handle1.terminated_event());

			double d = 9.8765;
			wait(sc_spawn(&r, sc_bind(&ref_function, sc_cref(d))).terminated_event());

			cout << "returned int is " << r << endl;
			cout << endl << "Done." << endl;
		}


	public:
	
		int round_robin(const char *str, sc_event &receive, sc_event &send, int cnt)
		{
			while (--cnt >= 0)
			{
			  wait(receive);
			  cout << "Round robin thread " << str << " at time " << sc_time_stamp() << endl;
			  wait(10, SC_NS);
			  send.notify();
			}

			return 0;
		}

		int wait_and_end(int i)
		{
			wait(i + 1, SC_NS);
			cout << "Thread " << i << " ending." << " at time " << sc_time_stamp() << endl;
			return 0;
		}
};


/*-----------------------------------------------------------------------------
*/
int sc_main (int, char*[]) 
{
	top top1("Top1");
	sc_start();

	return 0;
}





