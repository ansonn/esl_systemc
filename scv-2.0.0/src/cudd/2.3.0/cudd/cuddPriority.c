/**CFile***********************************************************************

  FileName    [cuddPriority.c]

  PackageName [cudd]

  Synopsis    [Priority functions.]

  Description [External procedures included in this file:
	    <ul>
	    <li> Cudd_PrioritySelect()
	    <li> Cudd_Xgty()
	    <li> Cudd_Xeqy()
	    <li> Cudd_addXeqy()
	    <li> Cudd_Dxygtdxz()
	    <li> Cudd_Dxygtdyz()
	    <li> Cudd_CProjection()
	    <li> Cudd_addHamming()
	    <li> Cudd_MinHammingDist()
	    </ul>
	Internal procedures included in this module:
	    <ul>
	    <li> cuddCProjectionRecur()
	    <li> cuddMinHammingDistRecur()
	    </ul>
	    ]

  SeeAlso     []

  Author      [Fabio Somenzi]

  Copyright [ This file was created at the University of Colorado at
  Boulder.  The University of Colorado at Boulder makes no warranty
  about the suitability of this software for any purpose.  It is
  presented on an AS IS basis.]

******************************************************************************/

#include "util.h"
#include "cuddInt.h"
#include <assert.h>


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

#ifndef lint
static char rcsid[] DD_UNUSED = "$Id: cuddPriority.c,v 1.1 2002/09/17 21:58:48 dlm Exp $";
#endif

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static int cuddMinHammingDistRecur ARGS((DdNode * f, int *minterm, DdHashTable * table, int upperBound));

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Selects pairs from R using a priority function.]

  Description [Selects pairs from a relation R(x,y) (given as a BDD)
  in such a way that a given x appears in one pair only. Uses a
  priority function to determine which y should be paired to a given x.
  Cudd_PrioritySelect returns a pointer to
  the selected function if successful; NULL otherwise.
  Three of the arguments--x, y, and z--are vectors of BDD variables.
  The first two are the variables on which R depends. The third vectore
  is a vector of auxiliary variables, used during the computation. This
  vector is optional. If a NULL value is passed instead,
  Cudd_PrioritySelect will create the working variables on the fly.
  The sizes of x and y (and z if it is not NULL) should equal n.
  The priority function Pi can be passed as a BDD, or can be built by
  Cudd_PrioritySelect. If NULL is passed instead of a DdNode *,
  parameter Pifunc is used by Cudd_PrioritySelect to build a BDD for the
  priority function. (Pifunc is a pointer to a C function.) If Pi is not
  NULL, then Pifunc is ignored. Pifunc should have the same interface as
  the standard priority functions (e.g., Cudd_Dxygtdxz).
  Cudd_PrioritySelect and Cudd_CProjection can sometimes be used
  interchangeably. Specifically, calling Cudd_PrioritySelect with
  Cudd_Xgty as Pifunc produces the same result as calling
  Cudd_CProjection with the all-zero minterm as reference minterm.
  However, depending on the application, one or the other may be
  preferable:
  <ul>
  <li> When extracting representatives from an equivalence relation,
  Cudd_CProjection has the advantage of nor requiring the auxiliary
  variables.
  <li> When computing matchings in general bipartite graphs,
  Cudd_PrioritySelect normally obtains better results because it can use
  more powerful matching schemes (e.g., Cudd_Dxygtdxz).
  </ul>
  ]

  SideEffects [If called with z == NULL, will create new variables in
  the manager.]

  SeeAlso     [Cudd_Dxygtdxz Cudd_Dxygtdyz Cudd_Xgty
  Cudd_bddAdjPermuteX Cudd_CProjection]

******************************************************************************/
DdNode *
Cudd_PrioritySelect(
  DdManager * dd /* manager */,
  DdNode * R /* BDD of the relation */,
  DdNode ** x /* array of x variables */,
  DdNode ** y /* array of y variables */,
  DdNode ** z /* array of z variables (optional: may be NULL) */,
  DdNode * Pi /* BDD of the priority function (optional: may be NULL) */,
  int  n /* size of x, y, and z */,
  DdNode * (*Pifunc)(DdManager *, int, DdNode **, DdNode **, DdNode **) /* function used to build Pi if it is NULL */)
{
    DdNode *res = NULL;
    DdNode *zcube = NULL;
    DdNode *Rxz, *Q;
    int createdZ = 0;
    int createdPi = 0;
    int i;

    /* Create z variables if needed. */
    if (z == NULL) {
	if (Pi != NULL) return(NULL);
	z = ALLOC(DdNode *,n);
	if (z == NULL) {
	    dd->errorCode = CUDD_MEMORY_OUT;
	    return(NULL);
	}
	createdZ = 1;
	for (i = 0; i < n; i++) {
	    if (dd->size >= (int) CUDD_MAXINDEX - 1) goto endgame;
	    z[i] = cuddUniqueInter(dd,dd->size,dd->one,Cudd_Not(dd->one));
	    if (z[i] == NULL) goto endgame;
	}
    }

    /* Create priority function BDD if needed. */
    if (Pi == NULL) {
	Pi = Pifunc(dd,n,x,y,z);
	if (Pi == NULL) goto endgame;
	createdPi = 1;
	cuddRef(Pi);
    }

    /* Initialize abstraction cube. */
    zcube = DD_ONE(dd);
    cuddRef(zcube);
    for (i = n - 1; i >= 0; i--) {
	DdNode *tmpp;
	tmpp = Cudd_bddAnd(dd,z[i],zcube);
	if (tmpp == NULL) goto endgame;
	cuddRef(tmpp);
	Cudd_RecursiveDeref(dd,zcube);
	zcube = tmpp;
    }

    /* Compute subset of (x,y) pairs. */
    Rxz = Cudd_bddSwapVariables(dd,R,y,z,n);
    if (Rxz == NULL) goto endgame;
    cuddRef(Rxz);
    Q = Cudd_bddAndAbstract(dd,Rxz,Pi,zcube);
    if (Q == NULL) {
	Cudd_RecursiveDeref(dd,Rxz);
	goto endgame;
    }
    cuddRef(Q);
    Cudd_RecursiveDeref(dd,Rxz);
    res = Cudd_bddAnd(dd,R,Cudd_Not(Q));
    if (res == NULL) {
	Cudd_RecursiveDeref(dd,Q);
	goto endgame;
    }
    cuddRef(res);
    Cudd_RecursiveDeref(dd,Q);

endgame:
    if (zcube != NULL) Cudd_RecursiveDeref(dd,zcube);
    if (createdZ) {
	FREE(z);
    }
    if (createdPi) {
	Cudd_RecursiveDeref(dd,Pi);
    }
    if (res != NULL) cuddDeref(res);
    return(res);

} /* Cudd_PrioritySelect */


/**Function********************************************************************

  Synopsis    [Generates a BDD for the function x &gt; y.]

  Description [This function generates a BDD for the function x &gt; y.
  Both x and y are N-bit numbers, x\[0\] x\[1\] ... x\[N-1\] and
  y\[0\] y\[1\] ...  y\[N-1\], with 0 the most significant bit.
  The BDD is built bottom-up.
  It has 3*N-1 internal nodes, if the variables are ordered as follows: 
  x\[0\] y\[0\] x\[1\] y\[1\] ... x\[N-1\] y\[N-1\].
  Argument z is not used by Cudd_Xgty: it is included to make it
  call-compatible to Cudd_Dxygtdxz and Cudd_Dxygtdyz.]

  SideEffects [None]

  SeeAlso     [Cudd_PrioritySelect Cudd_Dxygtdxz Cudd_Dxygtdyz]

******************************************************************************/
DdNode *
Cudd_Xgty(
  DdManager * dd /* DD manager */,
  int  N /* number of x and y variables */,
  DdNode ** z /* array of z variables: unused */,
  DdNode ** x /* array of x variables */,
  DdNode ** y /* array of y variables */)
{
    DdNode *u, *v, *w;
    int     i;

    /* Build bottom part of BDD outside loop. */
    u = Cudd_bddAnd(dd, x[N-1], Cudd_Not(y[N-1]));
    if (u == NULL) return(NULL);
    cuddRef(u);

    /* Loop to build the rest of the BDD. */
    for (i = N-2; i >= 0; i--) {
	v = Cudd_bddAnd(dd, y[i], Cudd_Not(u));
	if (v == NULL) {
	    Cudd_RecursiveDeref(dd, u);
	    return(NULL);
	}
	cuddRef(v);
	w = Cudd_bddAnd(dd, Cudd_Not(y[i]), u);
	if (w == NULL) {
	    Cudd_RecursiveDeref(dd, u);
	    Cudd_RecursiveDeref(dd, v);
	    return(NULL);
	}
	cuddRef(w);
	Cudd_RecursiveDeref(dd, u);
	u = Cudd_bddIte(dd, x[i], Cudd_Not(v), w);
	if (u == NULL) {
	    Cudd_RecursiveDeref(dd, v);
	    Cudd_RecursiveDeref(dd, w);
	    return(NULL);
	}
	cuddRef(u);
	Cudd_RecursiveDeref(dd, v);
	Cudd_RecursiveDeref(dd, w);

    }
    cuddDeref(u);
    return(u);

} /* end of Cudd_Xgty */


/**Function********************************************************************

  Synopsis    [Generates a BDD for the function x==y.]

  Description [This function generates a BDD for the function x==y.
  Both x and y are N-bit numbers, x\[0\] x\[1\] ... x\[N-1\] and
  y\[0\] y\[1\] ...  y\[N-1\], with 0 the most significant bit.
  The BDD is built bottom-up.
  It has 3*N-1 internal nodes, if the variables are ordered as follows: 
  x\[0\] y\[0\] x\[1\] y\[1\] ... x\[N-1\] y\[N-1\]. ]

  SideEffects [None]

  SeeAlso     [Cudd_addXeqy]

******************************************************************************/
DdNode *
Cudd_Xeqy(
  DdManager * dd /* DD manager */,
  int  N /* number of x and y variables */,
  DdNode ** x /* array of x variables */,
  DdNode ** y /* array of y variables */)
{
    DdNode *u, *v, *w;
    int     i;

    /* Build bottom part of BDD outside loop. */
    u = Cudd_bddIte(dd, x[N-1], y[N-1], Cudd_Not(y[N-1]));
    if (u == NULL) return(NULL);
    cuddRef(u);

    /* Loop to build the rest of the BDD. */
    for (i = N-2; i >= 0; i--) {
	v = Cudd_bddAnd(dd, y[i], u);
	if (v == NULL) {
	    Cudd_RecursiveDeref(dd, u);
	    return(NULL);
	}
	cuddRef(v);
	w = Cudd_bddAnd(dd, Cudd_Not(y[i]), u);
	if (w == NULL) {
	    Cudd_RecursiveDeref(dd, u);
	    Cudd_RecursiveDeref(dd, v);
	    return(NULL);
	}
	cuddRef(w);
	Cudd_RecursiveDeref(dd, u);
	u = Cudd_bddIte(dd, x[i], v, w);
	if (u == NULL) {
	    Cudd_RecursiveDeref(dd, v);
	    Cudd_RecursiveDeref(dd, w);
	    return(NULL);
	}
	cuddRef(u);
	Cudd_RecursiveDeref(dd, v);
	Cudd_RecursiveDeref(dd, w);
    }
    cuddDeref(u);
    return(u);

} /* end of Cudd_Xeqy */


/**Function********************************************************************

  Synopsis    [Generates an ADD for the function x==y.]

  Description [This function generates an ADD for the function x==y.
  Both x and y are N-bit numbers, x\[0\] x\[1\] ... x\[N-1\] and
  y\[0\] y\[1\] ...  y\[N-1\], with 0 the most significant bit.
  The ADD is built bottom-up.
  It has 3*N-1 internal nodes, if the variables are ordered as follows: 
  x\[0\] y\[0\] x\[1\] y\[1\] ... x\[N-1\] y\[N-1\]. ]

  SideEffects [None]

  SeeAlso     [Cudd_Xeqy]

******************************************************************************/
DdNode *
Cudd_addXeqy(
  DdManager * dd /* DD manager */,
  int  N /* number of x and y variables */,
  DdNode ** x /* array of x variables */,
  DdNode ** y /* array of y variables */)
{
    DdNode *one, *zero;
    DdNode *u, *v, *w;
    int     i;

    one = DD_ONE(dd);
    zero = DD_ZERO(dd);

    /* Build bottom part of ADD outside loop. */
    v = Cudd_addIte(dd, y[N-1], one, zero);
    if (v == NULL) return(NULL);
    cuddRef(v);
    w = Cudd_addIte(dd, y[N-1], zero, one);
    if (w == NULL) {
	Cudd_RecursiveDeref(dd, v);
	return(NULL);
    }
    cuddRef(w);
    u = Cudd_addIte(dd, x[N-1], v, w);
    if (w == NULL) {
	Cudd_RecursiveDeref(dd, v);
	Cudd_RecursiveDeref(dd, w);
	return(NULL);
    }
    cuddRef(u);
    Cudd_RecursiveDeref(dd, v);
    Cudd_RecursiveDeref(dd, w);

    /* Loop to build the rest of the ADD. */
    for (i = N-2; i >= 0; i--) {
	v = Cudd_addIte(dd, y[i], u, zero);
	if (v == NULL) {
	    Cudd_RecursiveDeref(dd, u);
	    return(NULL);
	}
	cuddRef(v);
	w = Cudd_addIte(dd, y[i], zero, u);
	if (w == NULL) {
	    Cudd_RecursiveDeref(dd, u);
	    Cudd_RecursiveDeref(dd, v);
	    return(NULL);
	}
	cuddRef(w);
	Cudd_RecursiveDeref(dd, u);
	u = Cudd_addIte(dd, x[i], v, w);
	if (w == NULL) {
	    Cudd_RecursiveDeref(dd, v);
	    Cudd_RecursiveDeref(dd, w);
	    return(NULL);
	}
	cuddRef(u);
	Cudd_RecursiveDeref(dd, v);
	Cudd_RecursiveDeref(dd, w);
    }
    cuddDeref(u);
    return(u);

} /* end of Cudd_addXeqy */


/**Function********************************************************************

  Synopsis    [Generates a BDD for the function d(x,y) &gt; d(x,z).]

  Description [This function generates a BDD for the function d(x,y)
  &gt; d(x,z);
  x, y, and z are N-bit numbers, x\[0\] x\[1\] ... x\[N-1\],
  y\[0\] y\[1\] ...  y\[N-1\], and z\[0\] z\[1\] ...  z\[N-1\],
  with 0 the most significant bit.
  The distance d(x,y) is defined as:
	\sum_{i=0}^{N-1}(|x_i - y_i| \cdot 2^{N-i-1}).
  The BDD is built bottom-up.
  It has 7*N-3 internal nodes, if the variables are ordered as follows: 
  x\[0\] y\[0\] z\[0\] x\[1\] y\[1\] z\[1\] ... x\[N-1\] y\[N-1\] z\[N-1\]. ]

  SideEffects [None]

  SeeAlso     [Cudd_PrioritySelect Cudd_Dxygtdyz Cudd_Xgty Cudd_bddAdjPermuteX]

******************************************************************************/
DdNode *
Cudd_Dxygtdxz(
  DdManager * dd /* DD manager */,
  int  N /* number of x, y, and z variables */,
  DdNode ** x /* array of x variables */,
  DdNode ** y /* array of y variables */,
  DdNode ** z /* array of z variables */)
{
    DdNode *one, *zero;
    DdNode *z1, *z2, *z3, *z4, *y1_, *y2, *x1;
    int     i;

    one = DD_ONE(dd);
    zero = Cudd_Not(one);

    /* Build bottom part of BDD outside loop. */
    y1_ = Cudd_bddIte(dd, y[N-1], one, Cudd_Not(z[N-1]));
    if (y1_ == NULL) return(NULL);
    cuddRef(y1_);
    y2 = Cudd_bddIte(dd, y[N-1], z[N-1], one);
    if (y2 == NULL) {
	Cudd_RecursiveDeref(dd, y1_);
	return(NULL);
    }
    cuddRef(y2);
    x1 = Cudd_bddIte(dd, x[N-1], y1_, y2);
    if (x1 == NULL) {
	Cudd_RecursiveDeref(dd, y1_);
	Cudd_RecursiveDeref(dd, y2);
	return(NULL);
    }
    cuddRef(x1);
    Cudd_RecursiveDeref(dd, y1_);
    Cudd_RecursiveDeref(dd, y2);

    /* Loop to build the rest of the BDD. */
    for (i = N-2; i >= 0; i--) {
	z1 = Cudd_bddIte(dd, z[i], one, Cudd_Not(x1));
	if (z1 == NULL) {
	    Cudd_RecursiveDeref(dd, x1);
	    return(NULL);
	}
	cuddRef(z1);
	z2 = Cudd_bddIte(dd, z[i], x1, one);
	if (z2 == NULL) {
	    Cudd_RecursiveDeref(dd, x1);
	    Cudd_RecursiveDeref(dd, z1);
	    return(NULL);
	}
	cuddRef(z2);
	z3 = Cudd_bddIte(dd, z[i], one, x1);
	if (z3 == NULL) {
	    Cudd_RecursiveDeref(dd, x1);
	    Cudd_RecursiveDeref(dd, z1);
	    Cudd_RecursiveDeref(dd, z2);
	    return(NULL);
	}
	cuddRef(z3);
	z4 = Cudd_bddIte(dd, z[i], x1, zero);
	if (z4 == NULL) {
	    Cudd_RecursiveDeref(dd, x1);
	    Cudd_RecursiveDeref(dd, z1);
	    Cudd_RecursiveDeref(dd, z2);
	    Cudd_RecursiveDeref(dd, z3);
	    return(NULL);
	}
	cuddRef(z4);
	Cudd_RecursiveDeref(dd, x1);
	y1_ = Cudd_bddIte(dd, y[i], z2, Cudd_Not(z1));
	if (y1_ == NULL) {
	    Cudd_RecursiveDeref(dd, z1);
	    Cudd_RecursiveDeref(dd, z2);
	    Cudd_RecursiveDeref(dd, z3);
	    Cudd_RecursiveDeref(dd, z4);
	    return(NULL);
	}
	cuddRef(y1_);
	y2 = Cudd_bddIte(dd, y[i], z4, z3);
	if (y2 == NULL) {
	    Cudd_RecursiveDeref(dd, z1);
	    Cudd_RecursiveDeref(dd, z2);
	    Cudd_RecursiveDeref(dd, z3);
	    Cudd_RecursiveDeref(dd, z4);
	    Cudd_RecursiveDeref(dd, y1_);
	    return(NULL);
	}
	cuddRef(y2);
	Cudd_RecursiveDeref(dd, z1);
	Cudd_RecursiveDeref(dd, z2);
	Cudd_RecursiveDeref(dd, z3);
	Cudd_RecursiveDeref(dd, z4);
	x1 = Cudd_bddIte(dd, x[i], y1_, y2);
	if (x1 == NULL) {
	    Cudd_RecursiveDeref(dd, y1_);
	    Cudd_RecursiveDeref(dd, y2);
	    return(NULL);
	}
	cuddRef(x1);
	Cudd_RecursiveDeref(dd, y1_);
	Cudd_RecursiveDeref(dd, y2);
    }
    cuddDeref(x1);
    return(Cudd_Not(x1));

} /* end of Cudd_Dxygtdxz */


/**Function********************************************************************

  Synopsis    [Generates a BDD for the function d(x,y) &gt; d(y,z).]

  Description [This function generates a BDD for the function d(x,y)
  &gt; d(y,z);
  x, y, and z are N-bit numbers, x\[0\] x\[1\] ... x\[N-1\],
  y\[0\] y\[1\] ...  y\[N-1\], and z\[0\] z\[1\] ...  z\[N-1\],
  with 0 the most significant bit.
  The distance d(x,y) is defined as:
	\sum_{i=0}^{N-1}(|x_i - y_i| \cdot 2^{N-i-1}).
  The BDD is built bottom-up.
  It has 7*N-3 internal nodes, if the variables are ordered as follows: 
  x\[0\] y\[0\] z\[0\] x\[1\] y\[1\] z\[1\] ... x\[N-1\] y\[N-1\] z\[N-1\]. ]

  SideEffects [None]

  SeeAlso     [Cudd_PrioritySelect Cudd_Dxygtdxz Cudd_Xgty Cudd_bddAdjPermuteX]

******************************************************************************/
DdNode *
Cudd_Dxygtdyz(
  DdManager * dd /* DD manager */,
  int  N /* number of x, y, and z variables */,
  DdNode ** x /* array of x variables */,
  DdNode ** y /* array of y variables */,
  DdNode ** z /* array of z variables */)
{
    DdNode *one, *zero;
    DdNode *z1, *z2, *z3, *z4, *y1_, *y2, *x1;
    int     i;

    one = DD_ONE(dd);
    zero = Cudd_Not(one);

    /* Build bottom part of BDD outside loop. */
    y1_ = Cudd_bddIte(dd, y[N-1], one, z[N-1]);
    if (y1_ == NULL) return(NULL);
    cuddRef(y1_);
    y2 = Cudd_bddIte(dd, y[N-1], z[N-1], zero);
    if (y2 == NULL) {
	Cudd_RecursiveDeref(dd, y1_);
	return(NULL);
    }
    cuddRef(y2);
    x1 = Cudd_bddIte(dd, x[N-1], y1_, Cudd_Not(y2));
    if (x1 == NULL) {
	Cudd_RecursiveDeref(dd, y1_);
	Cudd_RecursiveDeref(dd, y2);
	return(NULL);
    }
    cuddRef(x1);
    Cudd_RecursiveDeref(dd, y1_);
    Cudd_RecursiveDeref(dd, y2);

    /* Loop to build the rest of the BDD. */
    for (i = N-2; i >= 0; i--) {
	z1 = Cudd_bddIte(dd, z[i], x1, zero);
	if (z1 == NULL) {
	    Cudd_RecursiveDeref(dd, x1);
	    return(NULL);
	}
	cuddRef(z1);
	z2 = Cudd_bddIte(dd, z[i], x1, one);
	if (z2 == NULL) {
	    Cudd_RecursiveDeref(dd, x1);
	    Cudd_RecursiveDeref(dd, z1);
	    return(NULL);
	}
	cuddRef(z2);
	z3 = Cudd_bddIte(dd, z[i], one, x1);
	if (z3 == NULL) {
	    Cudd_RecursiveDeref(dd, x1);
	    Cudd_RecursiveDeref(dd, z1);
	    Cudd_RecursiveDeref(dd, z2);
	    return(NULL);
	}
	cuddRef(z3);
	z4 = Cudd_bddIte(dd, z[i], one, Cudd_Not(x1));
	if (z4 == NULL) {
	    Cudd_RecursiveDeref(dd, x1);
	    Cudd_RecursiveDeref(dd, z1);
	    Cudd_RecursiveDeref(dd, z2);
	    Cudd_RecursiveDeref(dd, z3);
	    return(NULL);
	}
	cuddRef(z4);
	Cudd_RecursiveDeref(dd, x1);
	y1_ = Cudd_bddIte(dd, y[i], z2, z1);
	if (y1_ == NULL) {
	    Cudd_RecursiveDeref(dd, z1);
	    Cudd_RecursiveDeref(dd, z2);
	    Cudd_RecursiveDeref(dd, z3);
	    Cudd_RecursiveDeref(dd, z4);
	    return(NULL);
	}
	cuddRef(y1_);
	y2 = Cudd_bddIte(dd, y[i], z4, Cudd_Not(z3));
	if (y2 == NULL) {
	    Cudd_RecursiveDeref(dd, z1);
	    Cudd_RecursiveDeref(dd, z2);
	    Cudd_RecursiveDeref(dd, z3);
	    Cudd_RecursiveDeref(dd, z4);
	    Cudd_RecursiveDeref(dd, y1_);
	    return(NULL);
	}
	cuddRef(y2);
	Cudd_RecursiveDeref(dd, z1);
	Cudd_RecursiveDeref(dd, z2);
	Cudd_RecursiveDeref(dd, z3);
	Cudd_RecursiveDeref(dd, z4);
	x1 = Cudd_bddIte(dd, x[i], y1_, Cudd_Not(y2));
	if (x1 == NULL) {
	    Cudd_RecursiveDeref(dd, y1_);
	    Cudd_RecursiveDeref(dd, y2);
	    return(NULL);
	}
	cuddRef(x1);
	Cudd_RecursiveDeref(dd, y1_);
	Cudd_RecursiveDeref(dd, y2);
    }
    cuddDeref(x1);
    return(Cudd_Not(x1));

} /* end of Cudd_Dxygtdyz */


/**Function********************************************************************

  Synopsis    [Computes the compatible projection of R w.r.t. cube Y.]

  Description [Computes the compatible projection of relation R with
  respect to cube Y. Returns a pointer to the c-projection if
  successful; NULL otherwise. For a comparison between Cudd_CProjection
  and Cudd_PrioritySelect, see the documentation of the latter.]

  SideEffects [None]

  SeeAlso     [Cudd_PrioritySelect]

******************************************************************************/
DdNode *
Cudd_CProjection(
  DdManager * dd,
  DdNode * R,
  DdNode * Y)
{
    DdNode *res;
    DdNode *support;

    if (cuddCheckCube(dd,Y) == 0) {
	(void) fprintf(dd->err,
	"Error: The third argument of Cudd_CProjection should be a cube\n");
	dd->errorCode = CUDD_INVALID_ARG;
	return(NULL);
    }

    /* Compute the support of Y, which is used by the abstraction step
    ** in cuddCProjectionRecur.
    */
    support = Cudd_Support(dd,Y);
    if (support == NULL) return(NULL);
    cuddRef(support);

    do {
	dd->reordered = 0;
	res = cuddCProjectionRecur(dd,R,Y,support);
    } while (dd->reordered == 1);

    if (res == NULL) {
	Cudd_RecursiveDeref(dd,support);
	return(NULL);
    }
    cuddRef(res);
    Cudd_RecursiveDeref(dd,support);
    cuddDeref(res);

    return(res);

} /* end of Cudd_CProjection */


/**Function********************************************************************

  Synopsis    [Computes the Hamming distance ADD.]

  Description [Computes the Hamming distance ADD. Returns an ADD that
  gives the Hamming distance between its two arguments if successful;
  NULL otherwise. The two vectors xVars and yVars identify the variables
  that form the two arguments.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
Cudd_addHamming(
  DdManager * dd,
  DdNode ** xVars,
  DdNode ** yVars,
  int  nVars)
{
    DdNode  *result,*tempBdd;
    DdNode  *tempAdd,*temp;
    int     i;

    result = DD_ZERO(dd);
    cuddRef(result);

    for (i = 0; i < nVars; i++) {
	tempBdd = Cudd_bddIte(dd,xVars[i],Cudd_Not(yVars[i]),yVars[i]);
	if (tempBdd == NULL) {
	    Cudd_RecursiveDeref(dd,result);
	    return(NULL);
	}
	cuddRef(tempBdd);
	tempAdd = Cudd_BddToAdd(dd,tempBdd);
	if (tempAdd == NULL) {
	    Cudd_RecursiveDeref(dd,tempBdd);
	    Cudd_RecursiveDeref(dd,result);
	    return(NULL);
	}
	cuddRef(tempAdd);
	Cudd_RecursiveDeref(dd,tempBdd);
	temp = Cudd_addApply(dd,Cudd_addPlus,tempAdd,result);
	if (temp == NULL) {
	    Cudd_RecursiveDeref(dd,tempAdd);
	    Cudd_RecursiveDeref(dd,result);
	    return(NULL);
	}
	cuddRef(temp);
	Cudd_RecursiveDeref(dd,tempAdd);
	Cudd_RecursiveDeref(dd,result);
	result = temp;
    }

    cuddDeref(result);
    return(result);

} /* end of Cudd_addHamming */


/**Function********************************************************************

  Synopsis    [Returns the minimum Hamming distance between f and minterm.]

  Description [Returns the minimum Hamming distance between the
  minterms of a function f and a reference minterm. The function is
  given as a BDD; the minterm is given as an array of integers, one
  for each variable in the manager.  Returns the minimum distance if
  it is less than the upper bound; the upper bound if the minimum
  distance is at least as large; CUDD_OUT_OF_MEM in case of failure.]

  SideEffects [None]

  SeeAlso     [Cudd_addHamming]

******************************************************************************/
int
Cudd_MinHammingDist(
  DdManager *dd /* DD manager */,
  DdNode *f /* function to examine */,
  int *minterm /* reference minterm */,
  int upperBound /* distance above which an approximate answer is OK */)
{
    DdHashTable *table;
    CUDD_VALUE_TYPE epsilon;
    int res;

    table = cuddHashTableInit(dd,1,2);
    if (table == NULL) {
	return(CUDD_OUT_OF_MEM);
    }
    epsilon = Cudd_ReadEpsilon(dd);
    Cudd_SetEpsilon(dd,(CUDD_VALUE_TYPE)0.0);
    res = cuddMinHammingDistRecur(f,minterm,table,upperBound);
    cuddHashTableQuit(table);
    Cudd_SetEpsilon(dd,epsilon);

    return(res);
    
} /* end of Cudd_MinHammingDist */

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_CProjection.]

  Description [Performs the recursive step of Cudd_CProjection. Returns
  the projection if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_CProjection]

******************************************************************************/
DdNode *
cuddCProjectionRecur(
  DdManager * dd,
  DdNode * R,
  DdNode * Y,
  DdNode * Ysupp)
{
    DdNode *res, *res1, *res2, *resA;
    DdNode *r, *y, *RT, *RE, *YT, *YE, *Yrest, *Ra, *Ran, *Gamma, *Alpha;
    unsigned int topR, topY, top;
    unsigned int index = ~0; /* SCV: enable check for legal index value (0 < index < CUDD_MAXINDEX) */
    DdNode *one = DD_ONE(dd);

    statLine(dd);
    if (Y == one) return(R);

#ifdef DD_DEBUG
    assert(!Cudd_IsConstant(Y));
#endif

    if (R == Cudd_Not(one)) return(R);

    res = cuddCacheLookup2(dd, Cudd_CProjection, R, Y);
    if (res != NULL) return(res);

    r = Cudd_Regular(R);
    topR = cuddI(dd,r->index);
    y = Cudd_Regular(Y);
    topY = cuddI(dd,y->index);

    top = ddMin(topR, topY);

    /* Compute the cofactors of R */
    if (topR == top) {
	index = r->index;
	RT = cuddT(r);
	RE = cuddE(r);
	if (r != R) {
	    RT = Cudd_Not(RT); RE = Cudd_Not(RE);
	}
    } else {
	RT = RE = R;
    }

    if (topY > top) {
	/* Y does not depend on the current top variable.
	** We just need to compute the results on the two cofactors of R
	** and make them the children of a node labeled r->index.
	*/
	res1 = cuddCProjectionRecur(dd,RT,Y,Ysupp);
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	res2 = cuddCProjectionRecur(dd,RE,Y,Ysupp);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(dd,res1);
	    return(NULL);
	}
	cuddRef(res2);
        /* SCV: assert that index got initialized before its usage. */
        assert(index <= CUDD_MAXINDEX);
	res = cuddBddIteRecur(dd, dd->vars[index], res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(dd,res1);
	    Cudd_RecursiveDeref(dd,res2);
	    return(NULL);
	}
	/* If we have reached this point, res1 and res2 are now
	** incorporated in res. cuddDeref is therefore sufficient.
	*/
	cuddDeref(res1);
	cuddDeref(res2);
    } else {
	/* Compute the cofactors of Y */
	index = y->index;
	YT = cuddT(y);
	YE = cuddE(y);
	if (y != Y) {
	    YT = Cudd_Not(YT); YE = Cudd_Not(YE);
	}
	if (YT == Cudd_Not(one)) {
	    Alpha  = Cudd_Not(dd->vars[index]);
	    Yrest = YE;
	    Ra = RE;
	    Ran = RT;
	} else {
	    Alpha = dd->vars[index];
	    Yrest = YT;
	    Ra = RT;
	    Ran = RE;
	}
	Gamma = cuddBddExistAbstractRecur(dd,Ra,cuddT(Ysupp));
	if (Gamma == NULL) return(NULL);
	if (Gamma == one) {
	    res1 = cuddCProjectionRecur(dd,Ra,Yrest,cuddT(Ysupp));
	    if (res1 == NULL) return(NULL);
	    cuddRef(res1);
	    res = cuddBddAndRecur(dd, Alpha, res1);
	    if (res == NULL) {
		Cudd_RecursiveDeref(dd,res1);
		return(NULL);
	    }
	    cuddDeref(res1);
	} else if (Gamma == Cudd_Not(one)) {
	    res1 = cuddCProjectionRecur(dd,Ran,Yrest,cuddT(Ysupp));
	    if (res1 == NULL) return(NULL);
	    cuddRef(res1);
	    res = cuddBddAndRecur(dd, Cudd_Not(Alpha), res1);
	    if (res == NULL) {
		Cudd_RecursiveDeref(dd,res1);
		return(NULL);
	    }
	    cuddDeref(res1);
	} else {
	    cuddRef(Gamma);
	    resA = cuddCProjectionRecur(dd,Ran,Yrest,cuddT(Ysupp));
	    if (resA == NULL) {
		Cudd_RecursiveDeref(dd,Gamma);
		return(NULL);
	    }
	    cuddRef(resA);
	    res2 = cuddBddAndRecur(dd, Cudd_Not(Gamma), resA);
	    if (res2 == NULL) {
		Cudd_RecursiveDeref(dd,Gamma);
		Cudd_RecursiveDeref(dd,resA);
		return(NULL);
	    }
	    cuddRef(res2);
	    Cudd_RecursiveDeref(dd,Gamma);
	    Cudd_RecursiveDeref(dd,resA);
	    res1 = cuddCProjectionRecur(dd,Ra,Yrest,cuddT(Ysupp));
	    if (res1 == NULL) {
		Cudd_RecursiveDeref(dd,res2);
		return(NULL);
	    }
	    cuddRef(res1);
	    res = cuddBddIteRecur(dd, Alpha, res1, res2);
	    if (res == NULL) {
		Cudd_RecursiveDeref(dd,res1);
		Cudd_RecursiveDeref(dd,res2);
		return(NULL);
	    }
	    cuddDeref(res1);
	    cuddDeref(res2);
	}
    }

    cuddCacheInsert2(dd,Cudd_CProjection,R,Y,res);

    return(res);

} /* end of cuddCProjectionRecur */


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_MinHammingDist.]

  Description [Performs the recursive step of Cudd_MinHammingDist.
  It is based on the following identity. Let H(f) be the
  minimum Hamming distance of the minterms of f from the reference
  minterm. Then:
  <xmp>
    H(f) = min(H(f0)+h0,H(f1)+h1)
  </xmp>
  where f0 and f1 are the two cofactors of f with respect to its top
  variable; h0 is 1 if the minterm assigns 1 to the top variable of f;
  h1 is 1 if the minterm assigns 0 to the top variable of f.
  The upper bound on the distance is used to bound the depth of the
  recursion.
  Returns the minimum distance unless it exceeds the upper bound or
  computation fails.]

  SideEffects [None]

  SeeAlso     [Cudd_MinHammingDist]

******************************************************************************/
static int
cuddMinHammingDistRecur(
  DdNode * f,
  int *minterm,
  DdHashTable * table,
  int upperBound)
{
    DdNode	*F, *Ft, *Fe;
    double	h, hT, hE;
    DdNode	*zero, *res;
    DdManager	*dd = table->manager;

    statLine(dd);
    if (upperBound == 0) return(0);

    F = Cudd_Regular(f);

    if (cuddIsConstant(F)) {
	zero = Cudd_Not(DD_ONE(dd));
	if (f == dd->background || f == zero) {
	    return(upperBound);
	} else {
	    return(0);
	}
    }
    if ((res = cuddHashTableLookup1(table,f)) != NULL) {
	h = cuddV(res);
	if (res->ref == 0) {
	    dd->dead++;
	    dd->constants.dead++;
	}
	return((int) h);
    }

    Ft = cuddT(F); Fe = cuddE(F);
    if (Cudd_IsComplement(f)) {
	Ft = Cudd_Not(Ft); Fe = Cudd_Not(Fe);
    }
    if (minterm[F->index] == 0) {
	DdNode *temp = Ft;
	Ft = Fe; Fe = temp;
    }

    hT = cuddMinHammingDistRecur(Ft,minterm,table,upperBound);
    if (hT == CUDD_OUT_OF_MEM) return(CUDD_OUT_OF_MEM);
    if (hT == 0) {
	hE = upperBound;
    } else {
	hE = cuddMinHammingDistRecur(Fe,minterm,table,upperBound - 1);
	if (hE == CUDD_OUT_OF_MEM) return(CUDD_OUT_OF_MEM);
    }
    h = ddMin(hT, hE + 1);

    if (F->ref != 1) {
	ptrint fanout = (ptrint) F->ref;
	cuddSatDec(fanout);
	res = cuddUniqueConst(dd, (CUDD_VALUE_TYPE) h);
	if (!cuddHashTableInsert1(table,f,res,fanout)) {
	    cuddRef(res); Cudd_RecursiveDeref(dd, res);
	    return(CUDD_OUT_OF_MEM);
	}
    }

    return((int) h);

} /* end of cuddMinHammingDistRecur */
