// Test program for the C++ object-oriented encapsulation of CUDD.

#include "cuddObj.hh"

static void testBdd(Cudd& mgr, int verbosity);
static void testAdd(Cudd& mgr, int verbosity);
static void testZdd(Cudd& mgr, int verbosity);
static void testBdd2(Cudd& mgr, int verbosity);
static void testBdd3(Cudd& mgr, int verbosity);
static void testZdd2(Cudd& mgr, int verbosity);


int
main()
{
    int verbosity = 4;

    Cudd mgr(0,2);		// changed zdd vars from 2 to 0!
    mgr.makeVerbose();		// trace constructors and destructors
    testBdd(mgr,verbosity);
    testAdd(mgr,verbosity);
    testZdd(mgr,verbosity);
    testBdd2(mgr,verbosity);
    testBdd3(mgr,verbosity);
    testZdd2(mgr,verbosity);
    mgr.info();
    return 0;

} // main


// Test basic operators on BDDs. The function returns void because it
// relies on the error hadling done by the interface. The default error
// handler causes program termination.
static void
testBdd(
  Cudd& mgr,
  int verbosity)
{
    cout << "Entering testBdd\n";
    // Create two new variables in the manager. If testBdd is called before
    // any variable is created in mgr, then x gets index 0 and y gets index 1.
    BDD x = mgr.bddVar();
    BDD y = mgr.bddVar();

    BDD f = x * y;
    cout << "f"; f.print(2,verbosity);

    BDD g = y + !x;
    cout << "g"; g.print(2,verbosity);

    cout << "f and g are" << (f == !g ? "" : " not") << " complementary\n";
    cout << "f is" << (f <= g ? "" : " not") << " less than or equal to g\n";

    g = f | ~g;
    cout << "g"; g.print(2,verbosity);

    BDD h = f = y;
    cout << "h"; h.print(2,verbosity);

    cout << "x + h has " << (x+h).nodeCount() << " nodes\n";

    h += x;
    cout << "h"; h.print(2,verbosity);

} // testBdd


static void
testAdd(
  Cudd& mgr,
  int verbosity)
{
    cout << "Entering testAdd\n";
    // Create two ADD variables. If we called method addVar without an
    // argument, we would get two new indices. If testAdd is indeed called
    // after testBdd, then those indices would be 2 and 3. By specifying the
    // arguments, on the other hand, we avoid creating new unnecessary BDD
    // variables.
    ADD p = mgr.addVar(0);
    ADD q = mgr.addVar(1);

    // Test arithmetic operators.
    ADD r = p + q;
    cout << "r"; r.print(2,verbosity);

    // CUDD_VALUE_TYPE is double.
    ADD s = mgr.constant(3.0);
    s *= p * q;
    cout << "s"; s.print(2,verbosity);

    s += mgr.plusInfinity();
    cout << "s"; s.print(2,verbosity);

    // Test relational operators.
    cout << "p is" << (p <= r ? "" : " not") << " less than or equal to r\n";

    // Test logical operators.
    r = p | q;
    cout << "r"; r.print(2,verbosity);

} // testAdd


static void
testZdd(
  Cudd& mgr,
  int verbosity)
{
    cout << "Entering testZdd\n";
    ZDD v = mgr.zddVar(0);
    ZDD w = mgr.zddVar(1);

    ZDD s = v + w;
    cout << "s"; s.print(2,verbosity);

    cout << "v is" << (v < s ? "" : " not") << " less than s\n";

    s -= v;
    cout << "s"; s.print(2,verbosity);

} // testZdd


static void
testBdd2(
  Cudd& mgr,
  int verbosity)
{
    cout << "Entering testBdd2\n";
    // Loop indices are best declared in the loops themselves, but
    // some compilers won't let us do that.
    int i;
    BDDvector x(4);
    for (i = 0; i < 4; i++) {
	x[i] = mgr.bddVar(i);
    }

    // Create the BDD for the Achilles' Heel function.
    BDD p1 = x[0] * x[2];
    BDD p2 = x[1] * x[3];
    BDD f = p1 + p2;
    cout << "f"; f.print(4,verbosity);

    BDDvector vect = f.CharToVect();
    for (i = 0; i < vect.count(); i++) {
	cout << "vect[" << i << "]"; vect[i].print(4,verbosity);
    }

    char* inames[] = {"x0", "x1", "x2", "x3"};
    // v0,...,v3 suffice if testBdd2 is called before testBdd3.
    char* onames[] = {"v0", "v1", "v2", "v3", "v4", "v5"};
    vect.DumpDot(inames,onames);

} // testBdd2


static void
testBdd3(
  Cudd& mgr,
  int verbosity)
{
    cout << "Entering testBdd3\n";
    BDDvector x(6);
    for (int i = 0; i < 6; i++) {
	x[i] = mgr.bddVar(i);
    }

    BDD G = x[4] + !x[5];
    BDD H = x[4] * x[5];
    BDD E = x[3].Ite(G,!x[5]);
    BDD F = x[3] + !H;
    BDD D = x[2].Ite(F,!H);
    BDD C = x[2].Ite(E,!F);
    BDD B = x[1].Ite(C,!F);
    BDD A = x[0].Ite(B,!D);
    BDD f = !A;
    cout << "f"; f.print(6,verbosity);

    BDD f1 = f.RemapUnderApprox(6);
    cout << "f1"; f1.print(6,verbosity);
    cout << "f1 is" << (f1 <= f ? "" : " not") << " less than or equal to f\n";

    BDD g;
    BDD h;
    f.GenConjDecomp(&g,&h);
    cout << "g"; g.print(6,verbosity);
    cout << "h"; h.print(6,verbosity);
    cout << "g * h " << (g * h == f ? "==" : "!=") << " f\n";

} // testBdd3


// This function builds the BDDs for a transformed adder: one in which
// the inputs are transformations of the original inputs. It then
// creates ZDDs for the covers from the BDDs.
static void
testZdd2(
  Cudd& mgr,
  int verbosity)
{
    cout << "Entering testZdd2\n";
    int N = 3;			// number of bits
    // Loop indices are best declared in the loops themselves, but
    // some compilers won't let us do that.
    int i;
    // Create variables.
    BDDvector a(N,&mgr);
    BDDvector b(N,&mgr);
    BDDvector c(N+1,&mgr);
    for (i = 0; i < N; i++) {
	a[N-1-i] = mgr.bddVar(2*i);
	b[N-1-i] = mgr.bddVar(2*i+1);
    }
    c[0] = mgr.bddVar(2*N);
    // Build functions.
    BDDvector s(N,&mgr);
    for (i = 0; i < N; i++) {
	s[i] = a[i].Xnor(c[i]);
	c[i+1] = a[i].Ite(b[i],c[i]);
    }

    // Create array of outputs and print it.
    BDDvector p(N+1,&mgr);
    for (i = 0; i < N; i++) {
	p[i] = s[i];
    }
    p[N] = c[N];
    for (i = 0; i < p.count(); i++) {
	cout << "p[" << i << "]"; p[i].print(2*N+1,verbosity);
    }
    char* inames[] = {"a2", "b2", "a1", "b1", "a0", "b0", "c0"};
    char* onames[] = {"s0", "s1", "s2", "c3"};
    p.DumpDot(inames,onames);

    // Create ZDD variables and build ZDD covers from BDDs.
    mgr.zddVarsFromBddVars(2);
    ZDDvector z(N+1,&mgr);
    for (i = 0; i < N+1; i++) {
	ZDD temp;
	BDD dummy = p[i].zddIsop(p[i],&temp);
	z[i] = temp;
    }

    // Print out covers.
    for (i = 0; i < z.count(); i++) {
	cout << "z[" << i << "]"; z[i].print(4*N+2,verbosity);
    }
    char* znames[] = {"a2+", "a2-", "b2+", "b2-", "a1+", "a1-", "b1+",
		      "b1-", "a0+", "a0-", "b0+", "b0-", "c0+", "c0-"};
    z.DumpDot(znames,onames);

} // testZdd2
