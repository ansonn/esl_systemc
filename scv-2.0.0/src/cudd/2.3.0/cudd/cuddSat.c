/**CFile***********************************************************************

  FileName    [cuddSat.c]

  PackageName [cudd]

  Synopsis    [Functions for the solution of satisfiability related
  problems.]

  Description [External procedures included in this file:
		<ul>
		<li> Cudd_Eval()
		<li> Cudd_ShortestPath()
		<li> Cudd_LargestCube()
		<li> Cudd_ShortestLength()
		<li> Cudd_Decreasing()
		<li> Cudd_Increasing()
		<li> Cudd_EquivDC()
		<li> Cudd_EqualSupNorm()
		</ul>
	Static procedures included in this module:
		<ul>
		<li> free_path_pair()
		<li> get_shortest()
		<li> get_path()
		<li> get_largest()
		<li> get_cube()
		</ul>]

  Author      [Seh-Woong Jeong, Fabio Somenzi]

  Copyright   [This file was created at the University of Colorado at
  Boulder.  The University of Colorado at Boulder makes no warranty
  about the suitability of this software for any purpose.  It is
  presented on an AS IS basis.]

******************************************************************************/

#include "util.h"
#include "st.h"
#include "cuddInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define	DD_BIGGY	1000000

/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef struct cuddPathPair {
    int	pos;
    int	neg;
} cuddPathPair;

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

#ifndef lint
static char rcsid[] DD_UNUSED = "$Id: cuddSat.c,v 1.1 2002/09/17 21:58:49 dlm Exp $";
#endif

static	DdNode	*one, *zero;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

#define WEIGHT(weight, col)	((weight) == NULL ? 1 : weight[col])

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static enum st_retval free_path_pair ARGS((char *key, char *value, char *arg));
static cuddPathPair get_shortest ARGS((DdNode *root, int *cost, int *support, st_table *visited));
static DdNode * get_path ARGS((DdManager *manager, st_table *visited, DdNode *f, int *weight, int cost));
static cuddPathPair get_largest ARGS((DdNode *root, st_table *visited));
static DdNode * get_cube ARGS((DdManager *manager, st_table *visited, DdNode *f, int cost));

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Returns the value of a DD for a given variable assignment.]

  Description [Finds the value of a DD for a given variable
  assignment. The variable assignment is passed in an array of int's,
  that should specify a zero or a one for each variable in the support
  of the function. Returns a pointer to a constant node. No new nodes
  are produced.]

  SideEffects [None]

  SeeAlso     [Cudd_bddLeq Cudd_addEvalConst]

******************************************************************************/
DdNode *
Cudd_Eval(
  DdManager * dd,
  DdNode * f,
  int * inputs)
{
    int comple;
    DdNode *ptr;

    comple = Cudd_IsComplement(f);
    ptr = Cudd_Regular(f);

    while (!cuddIsConstant(ptr)) {
	if (inputs[ptr->index] == 1) {
	    ptr = cuddT(ptr);
	} else {
	    comple ^= Cudd_IsComplement(cuddE(ptr));
	    ptr = Cudd_Regular(cuddE(ptr));
	}
    }
    return(Cudd_NotCond(ptr,comple));

} /* end of Cudd_Eval */


/**Function********************************************************************

  Synopsis    [Finds a shortest path in a DD.]

  Description [Finds a shortest path in a DD. f is the DD we want to
  get the shortest path for; weight\[i\] is the weight of the THEN arc
  coming from the node whose index is i. If weight is NULL, then unit
  weights are assumed for all THEN arcs. All ELSE arcs have 0 weight.
  If non-NULL, both weight and support should point to arrays with at
  least as many entries as there are variables in the manager.
  Returns the shortest path as the BDD of a cube.]

  SideEffects [support contains on return the true support of f.
  If support is NULL on entry, then Cudd_ShortestPath does not compute
  the true support info. length contains the length of the path.]

  SeeAlso     [Cudd_ShortestLength Cudd_LargestCube]

******************************************************************************/
DdNode *
Cudd_ShortestPath(
  DdManager * manager,
  DdNode * f,
  int * weight,
  int * support,
  int * length)
{
    register 	DdNode	*F;
    st_table	*visited;
    DdNode	*sol;
    cuddPathPair *rootPair;
    int		complement, cost;
    int		i;

    one = DD_ONE(manager);
    zero = DD_ZERO(manager);

    /* Initialize support. */
    if (support) {
	for (i = 0; i < manager->size; i++) {
	    support[i] = 0;
	}
    }

    if (f == Cudd_Not(one) || f == zero) {
	*length = DD_BIGGY;
	return(Cudd_Not(one));
    }
    /* From this point on, a path exists. */

    /* Initialize visited table. */
    visited = st_init_table(st_ptrcmp, st_ptrhash);

    /* Now get the length of the shortest path(s) from f to 1. */
    (void) get_shortest(f, weight, support, visited);

    complement = Cudd_IsComplement(f);

    F = Cudd_Regular(f);

    st_lookup(visited, (char *)F, (char **)&rootPair);
    
    if (complement) {
	cost = rootPair->neg;
    } else {
	cost = rootPair->pos;
    }

    /* Recover an actual shortest path. */
    do {
	manager->reordered = 0;
	sol = get_path(manager,visited,f,weight,cost);
    } while (manager->reordered == 1);

    st_foreach(visited, free_path_pair, NULL);
    st_free_table(visited);

    *length = cost;
    return(sol);

} /* end of Cudd_ShortestPath */


/**Function********************************************************************

  Synopsis    [Finds a largest cube in a DD.]

  Description [Finds a largest cube in a DD. f is the DD we want to
  get the largest cube for. The problem is translated into the one of
  finding a shortest path in f, when both THEN and ELSE arcs are assumed to
  have unit length. This yields a largest cube in the disjoint cover
  corresponding to the DD. Therefore, it is not necessarily the largest
  implicant of f.  Returns the largest cube as a BDD.]

  SideEffects [The number of literals of the cube is returned in length.]

  SeeAlso     [Cudd_ShortestPath]

******************************************************************************/
DdNode *
Cudd_LargestCube(
  DdManager * manager,
  DdNode * f,
  int * length)
{
    register 	DdNode	*F;
    st_table	*visited;
    DdNode	*sol;
    cuddPathPair *rootPair;
    int		complement, cost;

    one = DD_ONE(manager);
    zero = DD_ZERO(manager);

    if (f == Cudd_Not(one) || f == zero) {
	*length = DD_BIGGY;
	return(Cudd_Not(one));
    }
    /* From this point on, a path exists. */

    /* Initialize visited table. */
    visited = st_init_table(st_ptrcmp, st_ptrhash);

    /* Now get the length of the shortest path(s) from f to 1. */
    (void) get_largest(f, visited);

    complement = Cudd_IsComplement(f);

    F = Cudd_Regular(f);

    st_lookup(visited, (char *)F, (char **)&rootPair);
    
    if (complement) {
	cost = rootPair->neg;
    } else {
	cost = rootPair->pos;
    }

    /* Recover an actual shortest path. */
    do {
	manager->reordered = 0;
	sol = get_cube(manager,visited,f,cost);
    } while (manager->reordered == 1);

    st_foreach(visited, free_path_pair, NULL);
    st_free_table(visited);

    *length = cost;
    return(sol);

} /* end of Cudd_ShortestPath */


/**Function********************************************************************

  Synopsis    [Find the length of the shortest path(s) in a DD.]

  Description [Find the length of the shortest path(s) in a DD. f is
  the DD we want to get the shortest path for; weight\[i\] is the
  weight of the THEN edge coming from the node whose index is i. All
  ELSE edges have 0 weight. Returns the length of the shortest
  path(s) if successful; CUDD_OUT_OF_MEM otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_ShortestPath]

******************************************************************************/
int
Cudd_ShortestLength(
  DdManager * manager,
  DdNode * f,
  int * weight)
{
    register 	DdNode	*F;
    st_table	*visited;
    cuddPathPair *my_pair;
    int		complement, cost;

    one = DD_ONE(manager);
    zero = DD_ZERO(manager);

    if (f == Cudd_Not(one) || f == zero) {
	return(DD_BIGGY);
    }

    /* From this point on, a path exists. */
    /* Initialize visited table and support. */
    visited = st_init_table(st_ptrcmp, st_ptrhash);

    /* Now get the length of the shortest path(s) from f to 1. */
    (void) get_shortest(f, weight, NULL, visited);

    complement = Cudd_IsComplement(f);

    F = Cudd_Regular(f);

    st_lookup(visited, (char *)F, (char **)&my_pair);
    
    if (complement) {
	cost = my_pair->neg;
    } else {
	cost = my_pair->pos;
    }

    st_foreach(visited, free_path_pair, NULL);
    st_free_table(visited);

    return(cost);

} /* end of Cudd_ShortestLength */


/**Function********************************************************************

  Synopsis    [Determines whether a BDD is negative unate in a
  variable.]

  Description [Determines whether the function represented by BDD f is
  negative unate (monotonic decreasing) in variable i. Returns the
  constant one is f is unate and the (logical) constant zero if it is not.
  This function does not generate any new nodes.]

  SideEffects [None]

  SeeAlso     [Cudd_Increasing]

******************************************************************************/
DdNode *
Cudd_Decreasing(
  DdManager * dd,
  DdNode * f,
  int  i)
{
    unsigned int topf, level;
    DdNode *F, *fv, *fvn, *res;
    DdNode *(*cacheOp)(DdManager *, DdNode *, DdNode *);

    statLine(dd);
#ifdef DD_DEBUG
    assert(0 <= i && i < dd->size);
#endif

    F = Cudd_Regular(f);
    topf = cuddI(dd,F->index);

    /* Check terminal case. If topf > i, f does not depend on var.
    ** Therefore, f is unate in i.
    */
    level = (unsigned) dd->perm[i];
    if (topf > level) {
	return(DD_ONE(dd));
    }

    /* From now on, f is not constant. */

    /* Check cache. */
    cacheOp = (DdNode *(*)(DdManager *, DdNode *, DdNode *)) Cudd_Decreasing;
    res = cuddCacheLookup2(dd,cacheOp,f,dd->vars[i]);
    if (res != NULL) {
	return(res);
    }

    /* Compute cofactors. */
    fv = cuddT(F); fvn = cuddE(F);
    if (F != f) {
	fv = Cudd_Not(fv);
	fvn = Cudd_Not(fvn);
    }

    if (topf == (unsigned) level) {
	/* Special case: if fv is regular, fv(1,...,1) = 1;
	** If in addition fvn is complemented, fvn(1,...,1) = 0.
	** But then f(1,1,...,1) > f(0,1,...,1). Hence f is not
	** monotonic decreasing in i.
	*/
	if (!Cudd_IsComplement(fv) && Cudd_IsComplement(fvn)) {
	    return(Cudd_Not(DD_ONE(dd)));
	}
	res = Cudd_bddLeq(dd,fv,fvn) ? DD_ONE(dd) : Cudd_Not(DD_ONE(dd));
    } else {
	res = Cudd_Decreasing(dd,fv,i);
	if (res == DD_ONE(dd)) {
	    res = Cudd_Decreasing(dd,fvn,i);
	}
    }

    cuddCacheInsert2(dd,cacheOp,f,dd->vars[i],res);
    return(res);

} /* end of Cudd_Decreasing */


/**Function********************************************************************

  Synopsis    [Determines whether a BDD is positive unate in a
  variable.]

  Description [Determines whether the function represented by BDD f is
  positive unate (monotonic decreasing) in variable i. It is based on
  Cudd_Decreasing and the fact that f is monotonic increasing in i if
  and only if its complement is monotonic decreasing in i.]

  SideEffects [None]

  SeeAlso     [Cudd_Decreasing]

******************************************************************************/
DdNode *
Cudd_Increasing(
  DdManager * dd,
  DdNode * f,
  int  i)
{
    return(Cudd_Decreasing(dd,Cudd_Not(f),i));

} /* end of Cudd_Increasing */


/**Function********************************************************************

  Synopsis    [Tells whether F and G are identical wherever D is 0.]

  Description [Tells whether F and G are identical wherever D is 0.
  F and G are either two ADDs or two BDDs. D is either a 0-1 ADD or a
  BDD. The function returns 1 if F and G are equivalent, and 0
  otherwise. No new nodes are created.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
int
Cudd_EquivDC(
  DdManager * dd,
  DdNode * F,
  DdNode * G,
  DdNode * D)
{
    DdNode *tmp;
    DdNode *One;
    DdNode *Gr;
    DdNode *Dr;
    DdNode *Fv, *Fvn, *Gv, *Gvn, *Dv, *Dvn;
    int res;
    unsigned int flevel,glevel,dlevel,top;

    One = DD_ONE(dd);

    statLine(dd);
    /* Check terminal cases. */
    if (D == One) return(1);
    if (D == DD_ZERO(dd) || D == Cudd_Not(One)) return(F == G);
    if (F == G) return(1);
    if (F == Cudd_Not(G)) return(0);

    /* From now on, D is non-constant. */

    /* Normalize call to increase cache efficiency. */
    if (F > G) {
	tmp = F;
	F = G;
	G = tmp;
    }
    if (Cudd_IsComplement(F)) {
	F = Cudd_Not(F);
	G = Cudd_Not(G);
    }

    /* From now on, F is regular. */

    /* Check cache. */
    tmp = cuddCacheLookup(dd,DD_EQUIV_DC_TAG,F,G,D);
    if (tmp != NULL) return(tmp == One);

    /* Find splitting variable. */
    flevel = cuddI(dd,F->index);
    Gr = Cudd_Regular(G);
    glevel = cuddI(dd,Gr->index);
    top = ddMin(flevel,glevel);
    Dr = Cudd_Regular(D);
    dlevel = dd->perm[Dr->index];
    top = ddMin(top,dlevel);

    /* Compute cofactors. */
    if (top == flevel) {
	Fv = cuddT(F);
	Fvn = cuddE(F);
    } else {
	Fv = Fvn = F;
    }
    if (top == glevel) {
	Gv = cuddT(Gr);
	Gvn = cuddE(Gr);
	if (G != Gr) {
	    Gv = Cudd_Not(Gv);
	    Gvn = Cudd_Not(Gvn);
	}
    } else {
	Gv = Gvn = G;
    }
    if (top == dlevel) {
	Dv = cuddT(Dr);
	Dvn = cuddE(Dr);
	if (D != Dr) {
	    Dv = Cudd_Not(Dv);
	    Dvn = Cudd_Not(Dvn);
	}
    } else {
	Dv = Dvn = D;
    }

    /* Solve recursively. */
    res = Cudd_EquivDC(dd,Fv,Gv,Dv);
    if (res != 0) {
	res = Cudd_EquivDC(dd,Fvn,Gvn,Dvn);
    }
    cuddCacheInsert(dd,DD_EQUIV_DC_TAG,F,G,D,(res) ? One : Cudd_Not(one));

    return(res);

} /* end of Cudd_EquivDC */


/**Function********************************************************************

  Synopsis    [Compares two ADDs for equality within tolerance.]

  Description [Compares two ADDs for equality within tolerance. Two
  ADDs are reported to be equal if the maximum difference between them
  (the sup norm of their difference) is less than or equal to the
  tolerance parameter. Returns 1 if the two ADDs are equal (within
  tolerance); 0 otherwise. If parameter <code>pr</code> is positive
  the first failure is reported to the standard output.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
int
Cudd_EqualSupNorm(
  DdManager * dd /* manager */,
  DdNode * f /* first ADD */,
  DdNode * g /* second ADD */,
  CUDD_VALUE_TYPE  tolerance /* maximum allowed difference */,
  int  pr /* verbosity level */)
{
    DdNode *fv, *fvn, *gv, *gvn, *r;
    unsigned int topf, topg;

    statLine(dd);
    /* Check terminal cases. */
    if (f == g) return(1);
    if (Cudd_IsConstant(f) && Cudd_IsConstant(g)) {
	if (ddEqualVal(cuddV(f),cuddV(g),tolerance)) {
	    return(1);
	} else {
	    if (pr>0) {
		(void) fprintf(dd->out,"Offending nodes:\n");
#if SIZEOF_VOID_P == 8
		(void) fprintf(dd->out,
			       "f: address = %lx\t value = %40.30f\n",
			       (unsigned long) f, cuddV(f));
		(void) fprintf(dd->out,
			       "g: address = %lx\t value = %40.30f\n",
			       (unsigned long) g, cuddV(g));
#else
		(void) fprintf(dd->out,
			       "f: address = %x\t value = %40.30f\n",
			       (unsigned) f, cuddV(f));
		(void) fprintf(dd->out,
			       "g: address = %x\t value = %40.30f\n",
			       (unsigned) g, cuddV(g));
#endif
	    }
	    return(0);
	}
    }

    /* We only insert the result in the cache if the comparison is
    ** successful. Therefore, if we hit we return 1. */
    r = cuddCacheLookup2(dd,(DdNode * (*)(DdManager *, DdNode *, DdNode *))Cudd_EqualSupNorm,f,g);
    if (r != NULL) {
	return(1);
    }

    /* Compute the cofactors and solve the recursive subproblems. */
    topf = cuddI(dd,f->index);
    topg = cuddI(dd,g->index);

    if (topf <= topg) {fv = cuddT(f); fvn = cuddE(f);} else {fv = fvn = f;}
    if (topg <= topf) {gv = cuddT(g); gvn = cuddE(g);} else {gv = gvn = g;}

    if (!Cudd_EqualSupNorm(dd,fv,gv,tolerance,pr)) return(0);
    if (!Cudd_EqualSupNorm(dd,fvn,gvn,tolerance,pr)) return(0);

    cuddCacheInsert2(dd,(DdNode * (*)(DdManager *, DdNode *, DdNode *))Cudd_EqualSupNorm,f,g,DD_ONE(dd));

    return(1);

} /* end of Cudd_EqualSupNorm */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Frees the entries of the visited symbol table.]

  Description [Frees the entries of the visited symbol table. Returns
  ST_CONTINUE.]

  SideEffects [None]

******************************************************************************/
static enum st_retval
free_path_pair(
  char * key,
  char * value,
  char * arg)
{
    cuddPathPair *pair;

    pair = (cuddPathPair *) value;
	FREE(pair);
    return(ST_CONTINUE);

} /* end of free_path_pair */


/**Function********************************************************************

  Synopsis    [Finds the length of the shortest path(s) in a DD.]

  Description [Finds the length of the shortest path(s) in a DD.
  Uses a local symbol table to store the lengths for each
  node. Only the lengths for the regular nodes are entered in the table,
  because those for the complement nodes are simply obtained by swapping
  the two lenghts.
  Returns a pair of lengths: the length of the shortest path to 1;
  and the length of the shortest path to 0. This is done so as to take
  complement arcs into account.]

  SideEffects [Accumulates the support of the DD in support.]

  SeeAlso     []

******************************************************************************/
static cuddPathPair
get_shortest(
  DdNode * root,
  int * cost,
  int * support,
  st_table * visited)
{
    cuddPathPair *my_pair, res_pair, pair_T, pair_E;
    DdNode	*my_root, *T, *E;
    int		weight;

    my_root = Cudd_Regular(root);

    if (st_lookup(visited, (char *)my_root, (char **)&my_pair)) {
	if (Cudd_IsComplement(root)) {
	    res_pair.pos = my_pair->neg;
	    res_pair.neg = my_pair->pos;
	} else {
	    res_pair.pos = my_pair->pos;
	    res_pair.neg = my_pair->neg;
	}
	return(res_pair);
    }

    /* In the case of a BDD the following test is equivalent to
    ** testing whether the BDD is the constant 1. This formulation,
    ** however, works for ADDs as well, by assuming the usual
    ** dichotomy of 0 and != 0.
    */
    if (cuddIsConstant(my_root)) {
	if (my_root != zero) {
	    res_pair.pos = 0;
	    res_pair.neg = DD_BIGGY;
	} else {
	    res_pair.pos = DD_BIGGY;
	    res_pair.neg = 0;
	}
    } else {
	T = cuddT(my_root);
	E = cuddE(my_root);

	pair_T = get_shortest(T, cost, support, visited);
	pair_E = get_shortest(E, cost, support, visited);
	weight = WEIGHT(cost, my_root->index);
	res_pair.pos = ddMin(pair_T.pos+weight, pair_E.pos);
	res_pair.neg = ddMin(pair_T.neg+weight, pair_E.neg);

	/* Update support. */
	if (support != NULL) {
	    support[my_root->index] = 1;
	}
    }

    my_pair = ALLOC(cuddPathPair, 1);
    if (my_pair == NULL) {
	return(res_pair);
    }
    my_pair->pos = res_pair.pos;
    my_pair->neg = res_pair.neg;

    st_insert(visited, (char *)my_root, (char *)my_pair);
    if (Cudd_IsComplement(root)) {
	res_pair.pos = my_pair->neg;
	res_pair.neg = my_pair->pos;
    } else {
	res_pair.pos = my_pair->pos;
	res_pair.neg = my_pair->neg;
    }
    return(res_pair);

} /* end of get_shortest */


/**Function********************************************************************

  Synopsis    [Build a BDD for a shortest path of f.]

  Description [Build a BDD for a shortest path of f.
  Given the minimum length from the root, and the minimum
  lengths for each node (in visited), apply triangulation at each node.
  Of the two children of each node on a shortest path, at least one is
  on a shortest path. In case of ties the procedure chooses the THEN
  children.
  Returns a pointer to the cube BDD representing the path if
  successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static DdNode *
get_path(
  DdManager * manager,
  st_table * visited,
  DdNode * f,
  int * weight,
  int  cost)
{
    DdNode	*sol, *tmp;
    DdNode	*my_dd, *T, *E;
    cuddPathPair *T_pair, *E_pair;
    int		Tcost, Ecost;
    int		complement;

    my_dd = Cudd_Regular(f);
    complement = Cudd_IsComplement(f);

    sol = one;
    cuddRef(sol);

    while (!cuddIsConstant(my_dd)) {
	Tcost = cost - WEIGHT(weight, my_dd->index);
	Ecost = cost;

	T = cuddT(my_dd);
	E = cuddE(my_dd);

	if (complement) {T = Cudd_Not(T); E = Cudd_Not(E);}

	st_lookup(visited, (char *)Cudd_Regular(T), (char **)&T_pair);
	if ((Cudd_IsComplement(T) && T_pair->neg == Tcost) ||
	(!Cudd_IsComplement(T) && T_pair->pos == Tcost)) {
	    tmp = cuddBddAndRecur(manager,manager->vars[my_dd->index],sol);
	    if (tmp == NULL) {
		Cudd_RecursiveDeref(manager,sol);
		return(NULL);
	    }
	    cuddRef(tmp);
	    Cudd_RecursiveDeref(manager,sol);
	    sol = tmp;

	    complement =  Cudd_IsComplement(T);
	    my_dd = Cudd_Regular(T);
	    cost = Tcost;
	    continue;
	}
	st_lookup(visited, (char *)Cudd_Regular(E), (char **)&E_pair);
	if ((Cudd_IsComplement(E) && E_pair->neg == Ecost) ||
	(!Cudd_IsComplement(E) && E_pair->pos == Ecost)) {
	    tmp = cuddBddAndRecur(manager,Cudd_Not(manager->vars[my_dd->index]),sol);
	    if (tmp == NULL) {
		Cudd_RecursiveDeref(manager,sol);
		return(NULL);
	    }
	    cuddRef(tmp);
	    Cudd_RecursiveDeref(manager,sol);
	    sol = tmp;
	    complement = Cudd_IsComplement(E);
	    my_dd = Cudd_Regular(E);
	    cost = Ecost;
	    continue;
	}
	(void) fprintf(manager->err,"We shouldn't be here!!\n");
	manager->errorCode = CUDD_INTERNAL_ERROR;
	return(NULL);
    }

    cuddDeref(sol);
    return(sol);

} /* end of get_path */


/**Function********************************************************************

  Synopsis    [Finds the size of the largest cube(s) in a DD.]

  Description [Finds the size of the largest cube(s) in a DD.
  This problem is translated into finding the shortest paths from a node
  when both THEN and ELSE arcs have unit lengths.
  Uses a local symbol table to store the lengths for each
  node. Only the lengths for the regular nodes are entered in the table,
  because those for the complement nodes are simply obtained by swapping
  the two lenghts.
  Returns a pair of lengths: the length of the shortest path to 1;
  and the length of the shortest path to 0. This is done so as to take
  complement arcs into account.]

  SideEffects [none]

  SeeAlso     []

******************************************************************************/
static cuddPathPair
get_largest(
  DdNode * root,
  st_table * visited)
{
    cuddPathPair *my_pair, res_pair, pair_T, pair_E;
    DdNode	*my_root, *T, *E;

    my_root = Cudd_Regular(root);

    if (st_lookup(visited, (char *)my_root, (char **)&my_pair)) {
	if (Cudd_IsComplement(root)) {
	    res_pair.pos = my_pair->neg;
	    res_pair.neg = my_pair->pos;
	} else {
	    res_pair.pos = my_pair->pos;
	    res_pair.neg = my_pair->neg;
	}
	return(res_pair);
    }

    /* In the case of a BDD the following test is equivalent to
    ** testing whether the BDD is the constant 1. This formulation,
    ** however, works for ADDs as well, by assuming the usual
    ** dichotomy of 0 and != 0.
    */
    if (cuddIsConstant(my_root)) {
	if (my_root != zero) {
	    res_pair.pos = 0;
	    res_pair.neg = DD_BIGGY;
	} else {
	    res_pair.pos = DD_BIGGY;
	    res_pair.neg = 0;
	}
    } else {
	T = cuddT(my_root);
	E = cuddE(my_root);

	pair_T = get_largest(T, visited);
	pair_E = get_largest(E, visited);
	res_pair.pos = ddMin(pair_T.pos, pair_E.pos) + 1;
	res_pair.neg = ddMin(pair_T.neg, pair_E.neg) + 1;
    }

    my_pair = ALLOC(cuddPathPair, 1);
    if (my_pair == NULL) {
	return(res_pair);
    }
    my_pair->pos = res_pair.pos;
    my_pair->neg = res_pair.neg;

    st_insert(visited, (char *)my_root, (char *)my_pair);
    if (Cudd_IsComplement(root)) {
	res_pair.pos = my_pair->neg;
	res_pair.neg = my_pair->pos;
    } else {
	res_pair.pos = my_pair->pos;
	res_pair.neg = my_pair->neg;
    }
    return(res_pair);

} /* end of get_largest */


/**Function********************************************************************

  Synopsis    [Build a BDD for a largest cube of f.]

  Description [Build a BDD for a largest cube of f.
  Given the minimum length from the root, and the minimum
  lengths for each node (in visited), apply triangulation at each node.
  Of the two children of each node on a shortest path, at least one is
  on a shortest path. In case of ties the procedure chooses the THEN
  children.
  Returns a pointer to the cube BDD representing the path if
  successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static DdNode *
get_cube(
  DdManager * manager,
  st_table * visited,
  DdNode * f,
  int  cost)
{
    DdNode	*sol, *tmp;
    DdNode	*my_dd, *T, *E;
    cuddPathPair *T_pair, *E_pair;
    int		Tcost, Ecost;
    int		complement;

    my_dd = Cudd_Regular(f);
    complement = Cudd_IsComplement(f);

    sol = one;
    cuddRef(sol);

    while (!cuddIsConstant(my_dd)) {
	Tcost = cost - 1;
	Ecost = cost - 1;

	T = cuddT(my_dd);
	E = cuddE(my_dd);

	if (complement) {T = Cudd_Not(T); E = Cudd_Not(E);}

	st_lookup(visited, (char *)Cudd_Regular(T), (char **)&T_pair);
	if ((Cudd_IsComplement(T) && T_pair->neg == Tcost) ||
	(!Cudd_IsComplement(T) && T_pair->pos == Tcost)) {
	    tmp = cuddBddAndRecur(manager,manager->vars[my_dd->index],sol);
	    if (tmp == NULL) {
		Cudd_RecursiveDeref(manager,sol);
		return(NULL);
	    }
	    cuddRef(tmp);
	    Cudd_RecursiveDeref(manager,sol);
	    sol = tmp;

	    complement =  Cudd_IsComplement(T);
	    my_dd = Cudd_Regular(T);
	    cost = Tcost;
	    continue;
	}
	st_lookup(visited, (char *)Cudd_Regular(E), (char **)&E_pair);
	if ((Cudd_IsComplement(E) && E_pair->neg == Ecost) ||
	(!Cudd_IsComplement(E) && E_pair->pos == Ecost)) {
	    tmp = cuddBddAndRecur(manager,Cudd_Not(manager->vars[my_dd->index]),sol);
	    if (tmp == NULL) {
		Cudd_RecursiveDeref(manager,sol);
		return(NULL);
	    }
	    cuddRef(tmp);
	    Cudd_RecursiveDeref(manager,sol);
	    sol = tmp;
	    complement = Cudd_IsComplement(E);
	    my_dd = Cudd_Regular(E);
	    cost = Ecost;
	    continue;
	}
	(void) fprintf(manager->err,"We shouldn't be here!\n");
	manager->errorCode = CUDD_INTERNAL_ERROR;
	return(NULL);
    }

    cuddDeref(sol);
    return(sol);

} /* end of get_cube */

