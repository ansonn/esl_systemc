/**CFile***********************************************************************

  FileName    [cuddMatMult.c]

  PackageName [cudd]

  Synopsis    [Matrix multiplication functions.]

  Description [External procedures included in this module:
		<ul>
		<li> Cudd_addMatrixMultiply()
		<li> Cudd_addTimesPlus()
		<li> Cudd_addTriangle()
		</ul>
	Static procedures included in this module:
		<ul>
		<li> addMMRecur()
		<li> addTriangleRecur()
		</ul>]

  Author      [Fabio Somenzi]

  Copyright   [This file was created at the University of Colorado at
  Boulder.  The University of Colorado at Boulder makes no warranty
  about the suitability of this software for any purpose.  It is
  presented on an AS IS basis.]

******************************************************************************/

#include "util.h"
#include "cuddInt.h"


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
static char rcsid[] DD_UNUSED = "$Id: cuddMatMult.c,v 1.1 2002/09/17 21:58:48 dlm Exp $";
#endif

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static DdNode * addMMRecur ARGS((DdManager *dd, DdNode *A, DdNode *B, int topP, int *vars));
static DdNode * addTriangleRecur ARGS((DdManager *dd, DdNode *f, DdNode *g, int *vars));

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis [Calculates the product of two matrices represented as
  ADDs.]

  Description [Calculates the product of two matrices, A and B,
  represented as ADDs. This procedure implements the quasiring multiplication
  algorithm.  A is assumed to depend on varibles x (rows) and z
  (columns).  B is assumed to depend on varibles z (rows) and y
  (columns).  The product of A and B then depends on x (rows) and y
  (columns).  Only the z variables have to be explicitly identified;
  they are the "summation" variables.  Returns a pointer to the
  result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_addTimesPlus Cudd_addTriangle Cudd_bddAndAbstract]

******************************************************************************/
DdNode *
Cudd_addMatrixMultiply(
  DdManager * dd,
  DdNode * A,
  DdNode * B,
  DdNode ** z,
  int  nz)
{
    int i, nvars, *vars;
    DdNode *res; 

    /* Array vars says what variables are "summation" variables. */
    nvars = dd->size;
    vars = ALLOC(int,nvars);
    if (vars == NULL) {
	dd->errorCode = CUDD_MEMORY_OUT;
	return(NULL);
    }
    for (i = 0; i < nvars; i++) {
        vars[i] = 0;
    }
    for (i = 0; i < nz; i++) {
        vars[z[i]->index] = 1;
    }

    do {
	dd->reordered = 0;
	res = addMMRecur(dd,A,B,-1,vars);
    } while (dd->reordered == 1);
    FREE(vars);
    return(res);

} /* end of Cudd_addMatrixMultiply */


/**Function********************************************************************

  Synopsis    [Calculates the product of two matrices represented as
  ADDs.]

  Description [Calculates the product of two matrices, A and B,
  represented as ADDs, using the CMU matrix by matrix multiplication
  procedure by Clarke et al..  Matrix A has x's as row variables and z's
  as column variables, while matrix B has z's as row variables and y's
  as column variables. Returns the pointer to the result if successful;
  NULL otherwise. The resulting matrix has x's as row variables and y's
  as column variables.]

  SideEffects [None]

  SeeAlso     [Cudd_addMatrixMultiply]

******************************************************************************/
DdNode *
Cudd_addTimesPlus(
  DdManager * dd,
  DdNode * A,
  DdNode * B,
  DdNode ** z,
  int  nz)
{
    DdNode *w, *cube, *tmp, *res; 
    int i;
    tmp = Cudd_addApply(dd,Cudd_addTimes,A,B);
    if (tmp == NULL) return(NULL);
    Cudd_Ref(tmp);
    Cudd_Ref(cube = DD_ONE(dd));
    for (i = nz-1; i >= 0; i--) {
	 w = Cudd_addIte(dd,z[i],cube,DD_ZERO(dd));
	 if (w == NULL) {
	    Cudd_RecursiveDeref(dd,tmp);
	    return(NULL);
	 }
	 Cudd_Ref(w);
	 Cudd_RecursiveDeref(dd,cube);
	 cube = w;
    }
    res = Cudd_addExistAbstract(dd,tmp,cube);
    if (res == NULL) {
	Cudd_RecursiveDeref(dd,tmp);
	Cudd_RecursiveDeref(dd,cube);
	return(NULL);
    }
    Cudd_Ref(res);
    Cudd_RecursiveDeref(dd,cube);
    Cudd_RecursiveDeref(dd,tmp);
    Cudd_Deref(res);
    return(res);

} /* end of Cudd_addTimesPlus */


/**Function********************************************************************

  Synopsis    [Performs the triangulation step for the shortest path
  computation.]

  Description [Implements the semiring multiplication algorithm used in
  the triangulation step for the shortest path computation.  f
  is assumed to depend on varibles x (rows) and z (columns).  g is
  assumed to depend on varibles z (rows) and y (columns).  The product
  of f and g then depends on x (rows) and y (columns).  Only the z
  variables have to be explicitly identified; they are the
  "abstraction" variables.  Returns a pointer to the result if
  successful; NULL otherwise. ]

  SideEffects [None]

  SeeAlso     [Cudd_addMatrixMultiply Cudd_bddAndAbstract]

******************************************************************************/
DdNode *
Cudd_addTriangle(
  DdManager * dd,
  DdNode * f,
  DdNode * g,
  DdNode ** z,
  int  nz)
{
    int    i, nvars, *vars;
    DdNode *res;

    nvars = dd->size;
    vars = ALLOC(int, nvars);
    if (vars == NULL) {
	dd->errorCode = CUDD_MEMORY_OUT;
	return(NULL);
    }
    for (i = 0; i < nvars; i++) vars[i] = -1;
    for (i = 0; i < nz; i++) vars[z[i]->index] = i;

    do {
	dd->reordered = 0;
	res = addTriangleRecur(dd, f, g, vars);
    } while (dd->reordered == 1);
    FREE(vars);
    return(res);

} /* end of Cudd_addTriangle */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addMatrixMultiply.]

  Description [Performs the recursive step of Cudd_addMatrixMultiply.
  Returns a pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

******************************************************************************/
static DdNode *
addMMRecur(
  DdManager * dd,
  DdNode * A,
  DdNode * B,
  int  topP,
  int * vars)
{
    DdNode *zero,
           *At,		/* positive cofactor of first operand */
	   *Ae,		/* negative cofactor of first operand */
	   *Bt,		/* positive cofactor of second operand */
	   *Be,		/* negative cofactor of second operand */
	   *t,		/* positive cofactor of result */
	   *e,		/* negative cofactor of result */
	   *scaled,	/* scaled result */
	   *add_scale,	/* ADD representing the scaling factor */
	   *res;
    int	i;		/* loop index */
    int scale;		/* scaling factor */
    int index;		/* index of the top variable */
    CUDD_VALUE_TYPE value;
    unsigned int topA, topB, topV;
    DdNode *(*cacheOp)(DdManager *, DdNode *, DdNode *);

    statLine(dd);
    zero = DD_ZERO(dd);

    if (A == zero || B == zero) {
        return(zero);
    }

    if (cuddIsConstant(A) && cuddIsConstant(B)) {
	/* Compute the scaling factor. It is 2^k, where k is the
	** number of summation variables below the current variable.
	** Indeed, these constants represent blocks of 2^k identical
	** constant values in both A and B.
	*/
	scale = 1;
	for (i = 0; i < dd->size; i++) {
	    if (vars[i]) {
		if (dd->perm[i] > topP) {
		    scale <<= 1;
		}
	    }
	}
	value = cuddV(A) * cuddV(B) * (CUDD_VALUE_TYPE) scale;
	res = cuddUniqueConst(dd, value);
	return(res);
    }

    /* Standardize to increase cache efficiency. Clearly, A*B != B*A
    ** in matrix multiplication. However, which matrix is which is
    ** determined by the variables appearing in the ADDs and not by
    ** which one is paased as first argument.
    */
    if (A > B) {
	DdNode *tmp = A;
	A = B;
	B = tmp;
    }

    topA = cuddI(dd,A->index); topB = cuddI(dd,B->index);
    topV = ddMin(topA,topB);

    cacheOp = (DdNode *(*)(DdManager *, DdNode *, DdNode *)) addMMRecur;
    res = cuddCacheLookup2(dd,cacheOp,A,B);
    if (res != NULL) {
	/* If the result is 0, there is no need to normalize.
	** Otherwise we count the number of z variables between
	** the current depth and the top of the ADDs. These are
	** the missing variables that determine the size of the
	** constant blocks.
	*/
	if (res == zero) return(res);
	scale = 1;
	for (i = 0; i < dd->size; i++) {
	    if (vars[i]) {
		if (dd->perm[i] > topP && (unsigned) dd->perm[i] < topV) {
		    scale <<= 1;
		}
	    }
	}
	if (scale > 1) {
	    cuddRef(res);
	    add_scale = cuddUniqueConst(dd,(CUDD_VALUE_TYPE)scale);
	    if (add_scale == NULL) {
		Cudd_RecursiveDeref(dd, res);
		return(NULL);
	    }
	    cuddRef(add_scale);
	    scaled = cuddAddApplyRecur(dd,Cudd_addTimes,res,add_scale);
	    if (scaled == NULL) {
		Cudd_RecursiveDeref(dd, add_scale);
		Cudd_RecursiveDeref(dd, res);
		return(NULL);
	    }
	    cuddRef(scaled);
	    Cudd_RecursiveDeref(dd, add_scale);
	    Cudd_RecursiveDeref(dd, res);
	    res = scaled;
	    cuddDeref(res);
	}
        return(res);
    }

    /* compute the cofactors */
    if (topV == topA) {
	At = cuddT(A);
	Ae = cuddE(A);
    } else {
	At = Ae = A;
    }
    if (topV == topB) {
	Bt = cuddT(B);
	Be = cuddE(B);
    } else {
	Bt = Be = B;
    }

    t = addMMRecur(dd, At, Bt, (int)topV, vars);
    if (t == NULL) return(NULL);
    cuddRef(t);
    e = addMMRecur(dd, Ae, Be, (int)topV, vars);
    if (e == NULL) {
	Cudd_RecursiveDeref(dd, t);
	return(NULL);
    }
    cuddRef(e);

    index = dd->invperm[topV];
    if (vars[index] == 0) {
	/* We have split on either the rows of A or the columns
	** of B. We just need to connect the two subresults,
	** which correspond to two submatrices of the result.
	*/
	res = (t == e) ? t : cuddUniqueInter(dd,index,t,e);
	if (res == NULL) {
	    Cudd_RecursiveDeref(dd, t);
	    Cudd_RecursiveDeref(dd, e);
	    return(NULL);
	}
	cuddRef(res);
	cuddDeref(t);
	cuddDeref(e);
    } else {
	/* we have simultaneously split on the columns of A and
	** the rows of B. The two subresults must be added.
	*/
	res = cuddAddApplyRecur(dd,Cudd_addPlus,t,e);
	if (res == NULL) {
	    Cudd_RecursiveDeref(dd, t);
	    Cudd_RecursiveDeref(dd, e);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(dd, t);
	Cudd_RecursiveDeref(dd, e);
    }

    cuddCacheInsert2(dd,cacheOp,A,B,res);

    /* We have computed (and stored in the computed table) a minimal
    ** result; that is, a result that assumes no summation variables
    ** between the current depth of the recursion and its top
    ** variable. We now take into account the z variables by properly
    ** scaling the result.
    */
    if (res != zero) {
	scale = 1;
	for (i = 0; i < dd->size; i++) {
	    if (vars[i]) {
		if (dd->perm[i] > topP && (unsigned) dd->perm[i] < topV) {
		    scale <<= 1;
		}
	    }
	}
	if (scale > 1) {
	    add_scale = cuddUniqueConst(dd,(CUDD_VALUE_TYPE)scale);
	    if (add_scale == NULL) {
		Cudd_RecursiveDeref(dd, res);
		return(NULL);
	    }
	    cuddRef(add_scale);
	    scaled = cuddAddApplyRecur(dd,Cudd_addTimes,res,add_scale);
	    if (scaled == NULL) {
		Cudd_RecursiveDeref(dd, res);
		Cudd_RecursiveDeref(dd, add_scale);
		return(NULL);
	    }
	    cuddRef(scaled);
	    Cudd_RecursiveDeref(dd, add_scale);
	    Cudd_RecursiveDeref(dd, res);
	    res = scaled;
	}
    }
    cuddDeref(res);
    return(res);

} /* end of addMMRecur */


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addTriangle.]

  Description [Performs the recursive step of Cudd_addTriangle. Returns
  a pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

******************************************************************************/
static DdNode *
addTriangleRecur(
  DdManager * dd,
  DdNode * f,
  DdNode * g,
  int * vars)
{
    DdNode *fv, *fvn, *gv, *gvn, *t, *e, *res;
    CUDD_VALUE_TYPE value;
    int top, topf, topg, index;
    DdNode *(*cacheOp)(DdManager *, DdNode *, DdNode *);

    statLine(dd);
    if (f == DD_PLUS_INFINITY(dd) || g == DD_PLUS_INFINITY(dd)) {
	return(DD_PLUS_INFINITY(dd));
    }

    if (cuddIsConstant(f) && cuddIsConstant(g)) {
	value = cuddV(f) + cuddV(g);
	res = cuddUniqueConst(dd, value);
	return(res);
    }
    if (f < g) {
	DdNode *tmp = f;
	f = g;
	g = tmp;
    }
    cacheOp = (DdNode *(*)(DdManager *, DdNode *, DdNode *)) addTriangleRecur;
    res = cuddCacheLookup2(dd,cacheOp,f,g);
    if (res != NULL) {
	return(res);
    }

    topf = cuddI(dd,f->index); topg = cuddI(dd,g->index);
    top = ddMin(topf,topg);

    if (top == topf) {fv = cuddT(f); fvn = cuddE(f);} else {fv = fvn = f;}
    if (top == topg) {gv = cuddT(g); gvn = cuddE(g);} else {gv = gvn = g;}

    t = addTriangleRecur(dd, fv, gv, vars);
    if (t == NULL) return(NULL);
    cuddRef(t);
    e = addTriangleRecur(dd, fvn, gvn, vars);
    if (e == NULL) {
	Cudd_RecursiveDeref(dd, t);
	return(NULL);
    }
    cuddRef(e);

    index = dd->invperm[top];
    if (vars[index] < 0) {
	res = (t == e) ? t : cuddUniqueInter(dd,index,t,e);
	if (res == NULL) {
	    Cudd_RecursiveDeref(dd, t);
	    Cudd_RecursiveDeref(dd, e);
	    return(NULL);
	}
	cuddDeref(t);
	cuddDeref(e);
    } else {
	res = cuddAddApplyRecur(dd,Cudd_addMinimum,t,e);
	if (res == NULL) {
	    Cudd_RecursiveDeref(dd, t);
	    Cudd_RecursiveDeref(dd, e);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(dd, t);
	Cudd_RecursiveDeref(dd, e);
	cuddDeref(res);
    }

    cuddCacheInsert2(dd, cacheOp, f, g, res);

    return(res);

} /* end of addTriangleRecur */

