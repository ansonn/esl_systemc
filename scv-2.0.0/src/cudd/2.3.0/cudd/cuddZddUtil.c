/**CFile***********************************************************************

  FileName    [cuddZddUtil.c]

  PackageName [cudd]

  Synopsis    [Utility functions for ZDDs.]

  Description [External procedures included in this module:
		    <ul>
		    <li> Cudd_zddPrintMinterm()
		    <li> Cudd_zddPrintCover()
		    <li> Cudd_zddPrintDebug()
		    <li> Cudd_zddDumpDot()
		    </ul>
	       Internal procedures included in this module:
		    <ul>
		    <li> cuddZddP()
		    </ul>
	       Static procedures included in this module:
		    <ul>
		    <li> zp2()
		    <li> zdd_print_minterm_aux()
		    </ul>
	      ]

  SeeAlso     []

  Author      [Hyong-Kyoon Shin, In-Ho Moon]

  Copyright [ This file was created at the University of Colorado at
  Boulder.  The University of Colorado at Boulder makes no warranty
  about the suitability of this software for any purpose.  It is
  presented on an AS IS basis.]

******************************************************************************/

#include    "util.h"
#include    "st.h"
#include    "cuddInt.h"

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
static char rcsid[] DD_UNUSED = "$Id: cuddZddUtil.c,v 1.1 2002/09/17 21:58:50 dlm Exp $";
#endif

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int zp2 ARGS((DdManager *zdd, DdNode *f, st_table *t));
static void zdd_print_minterm_aux ARGS((DdManager *zdd, DdNode *node, int level, int *list));
static void zddPrintCoverAux ARGS((DdManager *zdd, DdNode *node, int level, int *list));

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Prints a disjoint sum of product form for a ZDD.]

  Description [Prints a disjoint sum of product form for a ZDD. Returns 1
  if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_zddPrintDebug Cudd_zddPrintCover]

******************************************************************************/
int
Cudd_zddPrintMinterm(
  DdManager * zdd,
  DdNode * node)
{
    int		i, size;
    int		*list;

    size = (int)zdd->sizeZ;
    list = ALLOC(int, size);
    if (list == NULL) {
	zdd->errorCode = CUDD_MEMORY_OUT;
	return(0);
    }
    for (i = 0; i < size; i++) list[i] = 3; /* bogus value should disappear */
    zdd_print_minterm_aux(zdd, node, 0, list);
    FREE(list);
    return(1);

} /* end of Cudd_zddPrintMinterm */


/**Function********************************************************************

  Synopsis    [Prints a sum of products from a ZDD representing a cover.]

  Description [Prints a sum of products from a ZDD representing a cover.
  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_zddPrintMinterm]

******************************************************************************/
int
Cudd_zddPrintCover(
  DdManager * zdd,
  DdNode * node)
{
    int		i, size;
    int		*list;

    size = (int)zdd->sizeZ;
    if (size % 2 != 0) return(0); /* number of variables should be even */
    list = ALLOC(int, size);
    if (list == NULL) {
	zdd->errorCode = CUDD_MEMORY_OUT;
	return(0);
    }
    for (i = 0; i < size; i++) list[i] = 3; /* bogus value should disappear */
    zddPrintCoverAux(zdd, node, 0, list);
    FREE(list);
    return(1);

} /* end of Cudd_zddPrintCover */


/**Function********************************************************************

  Synopsis [Prints to the standard output a ZDD and its statistics.]

  Description [Prints to the standard output a DD and its statistics.
  The statistics include the number of nodes and the number of minterms.
  (The number of minterms is also the number of combinations in the set.)
  The statistics are printed if pr &gt; 0.  Specifically:
  <ul>
  <li> pr = 0 : prints nothing
  <li> pr = 1 : prints counts of nodes and minterms
  <li> pr = 2 : prints counts + disjoint sum of products
  <li> pr = 3 : prints counts + list of nodes
  <li> pr &gt; 3 : prints counts + disjoint sum of products + list of nodes
  </ul>
  Returns 1 if successful; 0 otherwise.
  ]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
int
Cudd_zddPrintDebug(
  DdManager * zdd,
  DdNode * f,
  int  n,
  int  pr)
{
    DdNode	*empty = DD_ZERO(zdd);
    int		nodes;
    double	minterms;
    int		retval = 1;

    if (f == empty && pr > 0) {
	(void) fprintf(zdd->out,": is the empty ZDD\n");
	(void) fflush(zdd->out);
	return(1);
    }

    if (pr > 0) {
	nodes = Cudd_zddDagSize(f);
	if (nodes == CUDD_OUT_OF_MEM) retval = 0;
	minterms = Cudd_zddCountMinterm(zdd, f, n);
	if (minterms == (double)CUDD_OUT_OF_MEM) retval = 0;
	(void) fprintf(zdd->out,": %d nodes %g minterms\n",
		       nodes, minterms);
	if (pr > 2)
	    if (!cuddZddP(zdd, f)) retval = 0;
	if (pr == 2 || pr > 3) {
	    if (!Cudd_zddPrintMinterm(zdd, f)) retval = 0;
	    (void) fprintf(zdd->out,"\n");
	}
	(void) fflush(zdd->out);
    }
    return(retval);

} /* end of Cudd_zddPrintDebug */


/**Function********************************************************************

  Synopsis    [Writes a dot file representing the argument ZDDs.]

  Description [Writes a file representing the argument ZDDs in a format
  suitable for the graph drawing program dot.
  It returns 1 in case of success; 0 otherwise (e.g., out-of-memory,
  file system full).
  Cudd_zddDumpDot does not close the file: This is the caller
  responsibility. Cudd_zddDumpDot uses a minimal unique subset of the
  hexadecimal address of a node as name for it.
  If the argument inames is non-null, it is assumed to hold the pointers
  to the names of the inputs. Similarly for onames.
  Cudd_zddDumpDot uses the following convention to draw arcs:
    <ul>
    <li> solid line: THEN arcs;
    <li> dashed line: ELSE arcs.
    </ul>
  The dot options are chosen so that the drawing fits on a letter-size
  sheet.
  ]

  SideEffects [None]

  SeeAlso     [Cudd_DumpDot Cudd_zddPrintDebug]

******************************************************************************/
int
Cudd_zddDumpDot(
  DdManager * dd /* manager */,
  int  n /* number of output nodes to be dumped */,
  DdNode ** f /* array of output nodes to be dumped */,
  char ** inames /* array of input names (or NULL) */,
  char ** onames /* array of output names (or NULL) */,
  FILE * fp /* pointer to the dump file */)
{
    DdNode	*support = NULL;
    DdNode	*scan;
    int		*sorted = NULL;
    int		nvars = dd->sizeZ;
    st_table	*visited = NULL;
    st_generator *gen;
    int		retval;
    int		i, j;
    int		slots;
    DdNodePtr	*nodelist;
    long	refAddr, diff, mask;

    /* Build a bit array with the support of f. */
    sorted = ALLOC(int,nvars);
    if (sorted == NULL) {
	dd->errorCode = CUDD_MEMORY_OUT;
	goto failure;
    }
    for (i = 0; i < nvars; i++) sorted[i] = 0;

    /* Take the union of the supports of each output function. */
    for (i = 0; i < n; i++) {
	support = Cudd_Support(dd,f[i]);
	if (support == NULL) goto failure;
	cuddRef(support);
	scan = support;
	while (!cuddIsConstant(scan)) {
	    sorted[scan->index] = 1;
	    scan = cuddT(scan);
	}
	Cudd_RecursiveDeref(dd,support);
    }
    support = NULL; /* so that we do not try to free it in case of failure */

    /* Initialize symbol table for visited nodes. */
    visited = st_init_table(st_ptrcmp, st_ptrhash);
    if (visited == NULL) goto failure;

    /* Collect all the nodes of this DD in the symbol table. */
    for (i = 0; i < n; i++) {
	retval = cuddCollectNodes(f[i],visited);
	if (retval == 0) goto failure;
    }

    /* Find how many most significant hex digits are identical
    ** in the addresses of all the nodes. Build a mask based
    ** on this knowledge, so that digits that carry no information
    ** will not be printed. This is done in two steps.
    **  1. We scan the symbol table to find the bits that differ
    **     in at least 2 addresses.
    **  2. We choose one of the possible masks. There are 8 possible
    **     masks for 32-bit integer, and 16 possible masks for 64-bit
    **     integers.
    */

    /* Find the bits that are different. */
    refAddr = (long) f[0];
    diff = 0;
    gen = st_init_gen(visited);
    while (st_gen(gen, (char **) &scan, NULL)) {
	diff |= refAddr ^ (long) scan;
    }
    st_free_gen(gen);

    /* Choose the mask. */
    for (i = 0; (unsigned) i < 8 * sizeof(long); i += 4) {
	mask = (1 << i) - 1;
	if (diff <= mask) break;
    }

    /* Write the header and the global attributes. */
    retval = fprintf(fp,"digraph \"ZDD\" {\n");
    if (retval == EOF) return(0);
    retval = fprintf(fp,
	"size = \"7.5,10\"\ncenter = true;\nedge [dir = none];\n");
    if (retval == EOF) return(0);

    /* Write the input name subgraph by scanning the support array. */
    retval = fprintf(fp,"{ node [shape = plaintext];\n");
    if (retval == EOF) goto failure;
    retval = fprintf(fp,"  edge [style = invis];\n");
    if (retval == EOF) goto failure;
    /* We use a name ("CONST NODES") with an embedded blank, because
    ** it is unlikely to appear as an input name.
    */
    retval = fprintf(fp,"  \"CONST NODES\" [style = invis];\n");
    if (retval == EOF) goto failure;
    for (i = 0; i < nvars; i++) {
        if (sorted[dd->invpermZ[i]]) {
	    if (inames == NULL) {
		retval = fprintf(fp,"\" %d \" -> ", dd->invpermZ[i]);
	    } else {
		retval = fprintf(fp,"\" %s \" -> ", inames[dd->invpermZ[i]]);
	    }
            if (retval == EOF) goto failure;
        }
    }
    retval = fprintf(fp,"\"CONST NODES\"; \n}\n");
    if (retval == EOF) goto failure;

    /* Write the output node subgraph. */
    retval = fprintf(fp,"{ rank = same; node [shape = box]; edge [style = invis];\n");
    if (retval == EOF) goto failure;
    for (i = 0; i < n; i++) {
	if (onames == NULL) {
	    retval = fprintf(fp,"\"F%d\"", i);
	} else {
	    retval = fprintf(fp,"\"  %s  \"", onames[i]);
	}
	if (retval == EOF) goto failure;
	if (i == n - 1) {
	    retval = fprintf(fp,"; }\n");
	} else {
	    retval = fprintf(fp," -> ");
	}
	if (retval == EOF) goto failure;
    }

    /* Write rank info: All nodes with the same index have the same rank. */
    for (i = 0; i < nvars; i++) {
        if (sorted[dd->invpermZ[i]]) {
	    retval = fprintf(fp,"{ rank = same; ");
	    if (retval == EOF) goto failure;
	    if (inames == NULL) {
		retval = fprintf(fp,"\" %d \";\n", dd->invpermZ[i]);
	    } else {
		retval = fprintf(fp,"\" %s \";\n", inames[dd->invpermZ[i]]);
	    }
            if (retval == EOF) goto failure;
	    nodelist = dd->subtableZ[i].nodelist;
	    slots = dd->subtableZ[i].slots;
	    for (j = 0; j < slots; j++) {
		scan = nodelist[j];
		while (scan != NULL) {
		    if (st_is_member(visited,(char *) scan)) {
			retval = fprintf(fp,"\"%lx\";\n", (mask & (long) scan) / sizeof(DdNode));
			if (retval == EOF) goto failure;
		    }
		    scan = scan->next;
		}
	    }
	    retval = fprintf(fp,"}\n");
	    if (retval == EOF) goto failure;
	}
    }

    /* All constants have the same rank. */
    retval = fprintf(fp,
	"{ rank = same; \"CONST NODES\";\n{ node [shape = box]; ");
    if (retval == EOF) goto failure;
    nodelist = dd->constants.nodelist;
    slots = dd->constants.slots;
    for (j = 0; j < slots; j++) {
	scan = nodelist[j];
	while (scan != NULL) {
	    if (st_is_member(visited,(char *) scan)) {
		retval = fprintf(fp,"\"%lx\";\n", (mask & (long) scan) / sizeof(DdNode));
		if (retval == EOF) goto failure;
	    }
	    scan = scan->next;
	}
    }
    retval = fprintf(fp,"}\n}\n");
    if (retval == EOF) goto failure;

    /* Write edge info. */
    /* Edges from the output nodes. */
    for (i = 0; i < n; i++) {
	if (onames == NULL) {
	    retval = fprintf(fp,"\"F%d\"", i);
	} else {
	    retval = fprintf(fp,"\"  %s  \"", onames[i]);
	}
	if (retval == EOF) goto failure;
	retval = fprintf(fp," -> \"%lx\" [style = solid];\n",
			 (mask & (long) f[i]) / sizeof(DdNode));
	if (retval == EOF) goto failure;
    }

    /* Edges from internal nodes. */
    for (i = 0; i < nvars; i++) {
        if (sorted[dd->invpermZ[i]]) {
	    nodelist = dd->subtableZ[i].nodelist;
	    slots = dd->subtableZ[i].slots;
	    for (j = 0; j < slots; j++) {
		scan = nodelist[j];
		while (scan != NULL) {
		    if (st_is_member(visited,(char *) scan)) {
			retval = fprintf(fp,
			    "\"%lx\" -> \"%lx\";\n",
			    (mask & (long) scan) / sizeof(DdNode),
			    (mask & (long) cuddT(scan)) / sizeof(DdNode));
			if (retval == EOF) goto failure;
			retval = fprintf(fp,
					 "\"%lx\" -> \"%lx\" [style = dashed];\n",
					 (mask & (long) scan) / sizeof(DdNode),
					 (mask & (long) cuddE(scan)) / sizeof(DdNode));
			if (retval == EOF) goto failure;
		    }
		    scan = scan->next;
		}
	    }
	}
    }

    /* Write constant labels. */
    nodelist = dd->constants.nodelist;
    slots = dd->constants.slots;
    for (j = 0; j < slots; j++) {
	scan = nodelist[j];
	while (scan != NULL) {
	    if (st_is_member(visited,(char *) scan)) {
		retval = fprintf(fp,"\"%lx\" [label = \"%g\"];\n",
		    (mask & (long) scan) / sizeof(DdNode), cuddV(scan));
		if (retval == EOF) goto failure;
	    }
	    scan = scan->next;
	}
    }

    /* Write trailer and return. */
    retval = fprintf(fp,"}\n");
    if (retval == EOF) goto failure;

    st_free_table(visited);
    FREE(sorted);
    return(1);

failure:
    if (sorted != NULL) FREE(sorted);
    if (support != NULL) Cudd_RecursiveDeref(dd,support);
    if (visited != NULL) st_free_table(visited);
    return(0);

} /* end of Cudd_zddDumpBlif */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis [Prints a ZDD to the standard output. One line per node is
  printed.]

  Description [Prints a ZDD to the standard output. One line per node is 
  printed. Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_zddPrintDebug]

******************************************************************************/
int
cuddZddP(
  DdManager * zdd,
  DdNode * f)
{
    int retval;
    st_table *table = st_init_table(st_ptrcmp, st_ptrhash);

    if (table == NULL) return(0);

    retval = zp2(zdd, f, table);
    st_free_table(table);
    (void) fputc('\n', zdd->out);
    return(retval);

} /* end of cuddZddP */


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis [Performs the recursive step of cuddZddP.]

  Description [Performs the recursive step of cuddZddP. Returns 1 in
  case of success; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static int
zp2(
  DdManager * zdd,
  DdNode * f,
  st_table * t)
{
    DdNode	*n;
    int		T, E;
    DdNode	*base = DD_ONE(zdd);
    
    if (f == NULL)
	return(0);

    if (Cudd_IsConstant(f)) {
        (void)fprintf(zdd->out, "ID = %d\n", (f == base));
	return(1);
    }
    if (st_is_member(t, (char *)f) == 1)
	return(1);

    if (st_insert(t, (char *) f, NULL) == ST_OUT_OF_MEM)
	return(0);

#if SIZEOF_VOID_P == 8
    (void) fprintf(zdd->out, "ID = 0x%lx\tindex = %d\tr = %d\t",
	(unsigned long)f / (unsigned long) sizeof(DdNode), f->index, f->ref);
#else
    (void) fprintf(zdd->out, "ID = 0x%x\tindex = %d\tr = %d\t",
	(unsigned)f / (unsigned) sizeof(DdNode), f->index, f->ref);
#endif

    n = cuddT(f);
    if (Cudd_IsConstant(n)) {
        (void) fprintf(zdd->out, "T = %d\t\t", (n == base));
	T = 1;
    } else {
#if SIZEOF_VOID_P == 8
        (void) fprintf(zdd->out, "T = 0x%lx\t", (unsigned long) n /
		       (unsigned long) sizeof(DdNode));
#else
        (void) fprintf(zdd->out, "T = 0x%x\t", (unsigned) n / (unsigned) sizeof(DdNode));
#endif
	T = 0;
    }

    n = cuddE(f);
    if (Cudd_IsConstant(n)) {
        (void) fprintf(zdd->out, "E = %d\n", (n == base));
	E = 1;
    } else {
#if SIZEOF_VOID_P == 8
        (void) fprintf(zdd->out, "E = 0x%lx\n", (unsigned long) n /
		      (unsigned long) sizeof(DdNode));
#else
        (void) fprintf(zdd->out, "E = 0x%x\n", (unsigned) n / (unsigned) sizeof(DdNode));
#endif
	E = 0;
    }

    if (E == 0)
	if (zp2(zdd, cuddE(f), t) == 0) return(0);
    if (T == 0)
	if (zp2(zdd, cuddT(f), t) == 0) return(0);
    return(1);

} /* end of zp2 */


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_zddPrintMinterm.]

  Description []

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static void
zdd_print_minterm_aux(
  DdManager * zdd /* manager */,
  DdNode * node /* current node */,
  int  level /* depth in the recursion */,
  int * list /* current recursion path */)
{
    DdNode	*Nv, *Nnv;
    int		i, v;
    DdNode	*base = DD_ONE(zdd);

    if (Cudd_IsConstant(node)) {
	if (node == base) {
	    /* Check for missing variable. */
	    if (level != zdd->sizeZ) {
		list[zdd->invpermZ[level]] = 0;
		zdd_print_minterm_aux(zdd, node, level + 1, list);
		return;
	    }
	    /* Terminal case: Print one cube based on the current recursion
	    ** path.
	    */
	    for (i = 0; i < zdd->sizeZ; i++) {
		v = list[i];
		if (v == 0)
		    (void) fprintf(zdd->out,"0");
		else if (v == 1)
		    (void) fprintf(zdd->out,"1");
		else if (v == 3)
		    (void) fprintf(zdd->out,"@");	/* should never happen */
		else
		    (void) fprintf(zdd->out,"-");
	    }
	    (void) fprintf(zdd->out," 1\n");
	}
    } else {
	/* Check for missing variable. */
	if (level != cuddIZ(zdd,node->index)) {
	    list[zdd->invpermZ[level]] = 0;
	    zdd_print_minterm_aux(zdd, node, level + 1, list);
	    return;
	}

	Nnv = cuddE(node);
	Nv = cuddT(node);
	if (Nv == Nnv) {
	    list[node->index] = 2;
	    zdd_print_minterm_aux(zdd, Nnv, level + 1, list);
	    return;
	}

	list[node->index] = 1;
	zdd_print_minterm_aux(zdd, Nv, level + 1, list);
	list[node->index] = 0;
	zdd_print_minterm_aux(zdd, Nnv, level + 1, list);
    }
    return;

} /* end of zdd_print_minterm_aux */


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_zddPrintCover.]

  Description []

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static void
zddPrintCoverAux(
  DdManager * zdd /* manager */,
  DdNode * node /* current node */,
  int  level /* depth in the recursion */,
  int * list /* current recursion path */)
{
    DdNode	*Nv, *Nnv;
    int		i, v;
    DdNode	*base = DD_ONE(zdd);

    if (Cudd_IsConstant(node)) {
	if (node == base) {
	    /* Check for missing variable. */
	    if (level != zdd->sizeZ) {
		list[zdd->invpermZ[level]] = 0;
		zddPrintCoverAux(zdd, node, level + 1, list);
		return;
	    }
	    /* Terminal case: Print one cube based on the current recursion
	    ** path.
	    */
	    for (i = 0; i < zdd->sizeZ; i += 2) {
		v = list[i] * 4 + list[i+1];
		if (v == 0)
		    (void) fprintf(zdd->out,"-");
		else if (v == 4)
		    (void) fprintf(zdd->out,"1");
		else if (v == 1)
		    (void) fprintf(zdd->out,"0");
		else
		    (void) fprintf(zdd->out,"@"); /* should never happen */
	    }
	    (void) fprintf(zdd->out," 1\n");
	}
    } else {
	/* Check for missing variable. */
	if (level != cuddIZ(zdd,node->index)) {
	    list[zdd->invpermZ[level]] = 0;
	    zddPrintCoverAux(zdd, node, level + 1, list);
	    return;
	}

	Nnv = cuddE(node);
	Nv = cuddT(node);
	if (Nv == Nnv) {
	    list[node->index] = 2;
	    zddPrintCoverAux(zdd, Nnv, level + 1, list);
	    return;
	}

	list[node->index] = 1;
	zddPrintCoverAux(zdd, Nv, level + 1, list);
	list[node->index] = 0;
	zddPrintCoverAux(zdd, Nnv, level + 1, list);
    }
    return;

} /* end of zddPrintCoverAux */
