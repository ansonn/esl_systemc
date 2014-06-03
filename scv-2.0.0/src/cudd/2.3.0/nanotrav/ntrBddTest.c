/**CFile***********************************************************************
 
   FileName    [ntrBddTest.c]

   PackageName [ntr]

   Synopsis    [BDD test functions for the nanotrav program.]

   Description []

   SeeAlso     []

   Author      [Fabio Somenzi]
		   
   Copyright [This file was created at the University of Colorado at
   Boulder.  The University of Colorado at Boulder makes no warranty
   about the suitability of this software for any purpose.  It is
   presented on an AS IS basis.]

******************************************************************************/

#include "ntr.h"
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
static char rcsid[] UTIL_UNUSED = "$Id: ntrBddTest.c,v 1.1 2002/09/17 21:59:08 dlm Exp $";
#endif

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int ntrTestMinimizationAux ARGS((DdManager *dd, BnetNetwork *net1, DdNode *f, char *name, DdNode *c, char *cname, NtrOptions *option));
static int ntrTestDensityAux ARGS((DdManager *dd, BnetNetwork *net, DdNode *f, char *name, NtrOptions *option));
static int ntrTestDecompAux ARGS((DdManager *dd, BnetNetwork *net, DdNode *f, char *name, NtrOptions *option));
static int ntrTestCofEstAux ARGS((DdManager * dd, BnetNetwork * net, DdNode * f, char * name, NtrOptions * option));
static int ntrTestClippingAux ARGS((DdManager *dd, BnetNetwork *net1, DdNode *f, char *name, DdNode *g, char *gname, NtrOptions *option));
static DdNode * ntrCompress1 ARGS((DdManager *dd, DdNode *f, int nvars, int threshold));
static DdNode * ntrCompress2 ARGS((DdManager *dd, DdNode *f, int nvars, int threshold));
static BnetNode * ntrNodeIsBuffer ARGS((BnetNode *nd, st_table *hash));

/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Tests BDD minimization functions.]

  Description [Tests BDD minimization functions, including
  leaf-identifying compaction, squeezing, and restrict. This function
  uses as constraint the first output of net2 and computes positive
  and negative cofactors of all the outputs of net1. For each
  cofactor, it checks whether compaction was safe (cofactor not larger
  than original function) and that the expansion based on each
  minimization function (used as a generalized cofactor) equals the
  original function.  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

*****************************************************************************/
int
Ntr_TestMinimization(
  DdManager * dd,
  BnetNetwork * net1,
  BnetNetwork * net2,
  NtrOptions * option)
{
    DdNode *f;
    DdNode *c = NULL;
    char *cname = NULL;
    BnetNode *node;
    int i;
    int result;
    int nsize, csize;

    if (option->second == FALSE) return(1);

    (void) printf("Testing BDD minimization algorithms\n");
    /* Use largest output of second network as constraint. */
    csize = -1;
    for (i = 0; i < net2->noutputs; i++) {
	if (!st_lookup(net2->hash,net2->outputs[i],(char **)&node)) {
	    return(0);
	}
	nsize = Cudd_DagSize(node->dd);
	if (nsize > csize) {
	    c = node->dd;
	    cname = node->name;
	    csize = nsize;
	}
    }
    if (c == NULL || cname == NULL) return(0);
    (void) printf("TEST-MINI: Constrain (%s) %d nodes\n",
		  cname, Cudd_DagSize(c));

    if (option->node == NULL) {
	for (i = 0; i < net1->noutputs; i++) {
	    if (!st_lookup(net1->hash,net1->outputs[i],(char **)&node)) {
		return(0);
	    }
	    f = node->dd;
	    if (f == NULL) return(0);
	    result = ntrTestMinimizationAux(dd,net1,f,node->name,c,cname,
					    option);
	    if (result == 0) return(0);
	}
    } else {
	if (!st_lookup(net1->hash,option->node,(char **) &node)) {
	    return(0);
	}
	f = node->dd;
	if (f == NULL) return(0);
	result = ntrTestMinimizationAux(dd,net1,f,option->node,c,cname,option);
	if (result == 0) return(0);
    }

    return(1);

} /* end of Ntr_TestMinimization */


/**Function********************************************************************

  Synopsis    [Tests BDD density-related functions.]

  Description [Tests BDD density-related functions.
  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

*****************************************************************************/
int
Ntr_TestDensity(
  DdManager * dd,
  BnetNetwork * net1,
  NtrOptions * option)
{
    DdNode *f;
    BnetNode *node;
    int i;
    int result;

    if (option->density == FALSE) return(1);

    (void) printf("Testing BDD density-related algorithms\n");
    if (option->node == NULL) {
	for (i = 0; i < net1->noutputs; i++) {
	    if (!st_lookup(net1->hash,net1->outputs[i],(char **)&node)) {
		return(0);
	    }
	    f = node->dd;
	    if (f == NULL) return(0);
	    result = ntrTestDensityAux(dd,net1,f,node->name,option);
	    if (result == 0) return(0);
	}
    } else {
	if (!st_lookup(net1->hash,option->node,(char **) &node)) {
	    return(0);
	}
	f = node->dd;
	if (f == NULL) return(0);
	result = ntrTestDensityAux(dd,net1,f,option->node,option);
	if (result == 0) return(0);
    }

    return(1);

} /* end of Ntr_TestDensity */


/**Function********************************************************************

  Synopsis    [Tests BDD decomposition functions.]

  Description [Tests BDD decomposition functions.
  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

*****************************************************************************/
int
Ntr_TestDecomp(
  DdManager * dd,
  BnetNetwork * net1,
  NtrOptions * option)
{
    DdNode *f;
    BnetNode *node;
    int i;
    int result;

    if (option->decomp == FALSE) return(1);

    (void) printf("Testing BDD decomposition algorithms\n");
    if (option->node == NULL) {
	for (i = 0; i < net1->noutputs; i++) {
	    if (!st_lookup(net1->hash,net1->outputs[i],(char **)&node)) {
		return(0);
	    }
	    f = node->dd;
	    if (f == NULL) return(0);
	    result = ntrTestDecompAux(dd,net1,f,node->name,option);
	    if (result == 0) return(0);
	}
    } else {
	if (!st_lookup(net1->hash,option->node,(char **) &node)) {
	    return(0);
	}
	f = node->dd;
	if (f == NULL) return(0);
	result = ntrTestDecompAux(dd,net1,f,option->node,option);
	if (result == 0) return(0);
    }

    return(1);

} /* end of ntrTestDecomp */


/**Function********************************************************************

  Synopsis    [Verify equivalence of combinational networks.]

  Description [Verify equivalence of combinational networks.
  Returns 1 if successful and if the networks are eqivalent; -1 if
  successful, but the networks are not equivalent; 0 otherwise.
  The two networks are supposed to have the same names for inputs and
  outputs. The only exception is that the second network may miss
  output buffers that are present in the first network. This function tries
  to match both the output and the input of the buffer.]

  SideEffects [None]

  SeeAlso     []

*****************************************************************************/
int
Ntr_VerifyEquivalence(
  DdManager * dd,
  BnetNetwork * net1,
  BnetNetwork * net2,
  NtrOptions * option)
{
    BnetNode *node;
    char *oname;
    DdNode *odd1, *odd2;
    int i;
    int pr;

    (void) printf("Testing equivalence\n");
    if (net2->noutputs != net1->noutputs) {
	(void) printf("The two networks have different number of outputs\n");
	(void) printf("  %s has %d outputs\n  %s has %d outputs\n",
		      net1->name, net1->noutputs, net2->name, net2->noutputs);
	return(-1);
    }
    if (net2->nlatches != net1->nlatches) {
	(void) printf("The two networks have different number of latches\n");
	(void) printf("  %s has %d latches\n  %s has %d latches\n",
		      net1->name, net1->nlatches, net2->name, net2->nlatches);
	return(-1);
    }

    pr = option->verb;
    for (i = 0; i < net1->noutputs; i++) {
	oname = net1->outputs[i];
	if (!st_lookup(net1->hash,oname,(char **)&node)) {
	    return(0);
	}
	odd1 = node->dd;
	(void) printf("%s", oname);
	Cudd_PrintDebug(dd, node->dd, Cudd_ReadSize(dd), pr);
	if (!st_lookup(net2->hash,oname,(char **)&node)) {
	    BnetNode *inpnd;
	    if ((inpnd = ntrNodeIsBuffer(node,net1->hash)) == NULL ||
		!st_lookup(net2->hash,inpnd->name,(char **)&node)) {
		(void) printf("Output %s missing from network %s\n",
			      oname, net2->name);
		return(-1);
	    } else {
		odd2 = inpnd->dd;
	    }
	} else {
	    odd2 = node->dd;
	}
	if (odd1 != odd2) {
	    (void) printf("Output %s is not equivalent\n", oname);
	    return(-1);
	}
    }
    return(1);
    
} /* end of Ntr_VerifyEquivalence */


/**Function********************************************************************

  Synopsis    [Tests BDD cofactor estimate functions.]

  Description [Tests BDD cofactor estimate functions.
  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

*****************************************************************************/
int
Ntr_TestCofactorEstimate(
  DdManager * dd,
  BnetNetwork * net,
  NtrOptions * option)
{
    DdNode *f;
    BnetNode *node;
    int i;
    int result;

    if (option->cofest == FALSE) return(1);

    (void) printf("Testing BDD cofactor estimation algorithms\n");
    if (option->node == NULL) {
	for (i = 0; i < net->noutputs; i++) {
	    if (!st_lookup(net->hash,net->outputs[i],(char **)&node)) {
		return(0);
	    }
	    f = node->dd;
	    if (f == NULL) return(0);
	    result = ntrTestCofEstAux(dd,net,f,node->name,option);
	    if (result == 0) return(0);
	}
    } else {
	if (!st_lookup(net->hash,option->node,(char **) &node)) {
	    return(0);
	}
	f = node->dd;
	if (f == NULL) return(0);
	result = ntrTestCofEstAux(dd,net,f,option->node,option);
	if (result == 0) return(0);
    }

    return(1);

} /* end of Ntr_TestCofactorEstimate */


/**Function********************************************************************

  Synopsis    [Tests BDD clipping functions.]

  Description [Tests BDD clipping functions.
  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

*****************************************************************************/
int
Ntr_TestClipping(
  DdManager * dd,
  BnetNetwork * net1,
  BnetNetwork * net2,
  NtrOptions * option)
{
    DdNode *f;
    DdNode *g = NULL;
    char *gname = NULL;
    BnetNode *node;
    int i;
    int result;
    int nsize, gsize;

    if (option->clip < 0.0) return(1);

    (void) printf("Testing BDD clipping algorithms\n");
    /* Use largest output of second network as second operand. */
    gsize = -1;
    for (i = 0; i < net2->noutputs; i++) {
	if (!st_lookup(net2->hash,net2->outputs[i],(char **)&node)) {
	    return(0);
	}
	nsize = Cudd_DagSize(node->dd);
	if (nsize > gsize) {
	    g = node->dd;
	    gname = node->name;
	    gsize = nsize;
	}
    }
    if (g == NULL || gname == NULL) return(0);
    (void) printf("TEST-CLIP: Second operand (%s) %d nodes\n",
		  gname, Cudd_DagSize(g));

    if (option->node == NULL) {
	for (i = 0; i < net1->noutputs; i++) {
	    if (!st_lookup(net1->hash,net1->outputs[i],(char **)&node)) {
		return(0);
	    }
	    f = node->dd;
	    if (f == NULL) return(0);
	    result = ntrTestClippingAux(dd,net1,f,node->name,g,gname,option);
	    if (result == 0) return(0);
	}
    } else {
	if (!st_lookup(net1->hash,option->node,(char **) &node)) {
	    return(0);
	}
	f = node->dd;
	if (f == NULL) return(0);
	result = ntrTestClippingAux(dd,net1,f,option->node,g,gname,option);
	if (result == 0) return(0);
    }

    return(1);

} /* end of Ntr_TestClipping */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Processes one BDD for Ntr_TestMinimization.]

  Description [Processes one BDD for Ntr_TestMinimization.
  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     [Ntr_TestMinimization]

*****************************************************************************/
static int
ntrTestMinimizationAux(
  DdManager * dd,
  BnetNetwork * net1,
  DdNode * f,
  char * name,
  DdNode * c,
  char * cname,
  NtrOptions * option)
{
    DdNode *com1, *com0, *min1, *min0, *sq1, *sq0, *rs1, *rs0, *cs1, *cs0;
    DdNode *g, *u1, *l1, *u0, *l0;
    int pr, nvars;
    int sizeF, sizeMin1, sizeMin0, sizeSq1, sizeSq0;
    int sizeCom1, sizeCom0, sizeRs1, sizeRs0, sizeCs1, sizeCs0;
    static char *onames[9];
    DdNode *outputs[9];
    DdNode *fc[2];

    pr = option->verb;
    fc[0] = f; fc[1] = c;
    nvars = Cudd_VectorSupportSize(dd,fc,2);
    if (nvars == CUDD_OUT_OF_MEM) return(0);
    (void) printf("TEST-MINI:: %s\n", name);
    (void) printf("T-M    ");
    Cudd_PrintDebug(dd, f, nvars, pr);
    sizeF = Cudd_DagSize(f);

    /* Compute positive generalized cofactor. */
    com1 = Cudd_bddLICompaction(dd, f, c);
    if (com1 == NULL) {
	(void) printf("TEST-MINI: LI-compaction failed (1).\n");
	return(0);
    }
    Cudd_Ref(com1);
    (void) printf("T-M L1 ");
    Cudd_PrintDebug(dd, com1, nvars, pr);
    sizeCom1 = Cudd_DagSize(com1);
    if (sizeCom1 > sizeF) {
	(void) printf("TEST-MINI: LI-compaction not safe (1).\n");
	return(0);
    }
    min1 = Cudd_bddMinimize(dd, f, c);
    if (min1 == NULL) {
	(void) printf("TEST-MINI: minimize failed (1).\n");
	return(0);
    }
    Cudd_Ref(min1);
    (void) printf("T-M M1 ");
    Cudd_PrintDebug(dd, min1, nvars, pr);
    sizeMin1 = Cudd_DagSize(min1);
    if (sizeMin1 > sizeF) {
	(void) printf("TEST-MINI: minimize not safe (1).\n");
	return(0);
    }
    rs1 = Cudd_bddRestrict(dd, f, c);
    if (rs1 == NULL) {
	(void) printf("TEST-MINI: restrict failed (1).\n");
	return(0);
    }
    Cudd_Ref(rs1);
    (void) printf("T-M R1 ");
    Cudd_PrintDebug(dd, rs1, nvars, pr);
    sizeRs1 = Cudd_DagSize(rs1);
    cs1 = Cudd_bddConstrain(dd, f, c);
    if (cs1 == NULL) {
	(void) printf("TEST-MINI: constrain failed (1).\n");
	return(0);
    }
    Cudd_Ref(cs1);
    (void) printf("T-M C1 ");
    Cudd_PrintDebug(dd, cs1, nvars, pr);
    sizeCs1 = Cudd_DagSize(cs1);
    l1 = Cudd_bddAnd(dd, f, c);
    if (l1 == NULL) {
	(void) printf("TEST-MINI: lower bound failed (1).\n");
	return(0);
    }
    Cudd_Ref(l1);
    u1 = Cudd_bddOr(dd, f, Cudd_Not(c));
    if (u1 == NULL) {
	(void) printf("TEST-MINI: upper bound failed (1).\n");
	return(0);
    }
    Cudd_Ref(u1);
    (void) printf("TEST-MINI: (lb,ub) : (%d,%d) nodes\n",
		  Cudd_DagSize(l1), Cudd_DagSize(u1));
    sq1 = Cudd_bddSqueeze(dd, l1, u1);
    if (sq1 == NULL) {
	(void) printf("TEST-MINI: squeezing failed (1).\n");
	return(0);
    }
    Cudd_Ref(sq1);
    Cudd_RecursiveDeref(dd,l1);
    Cudd_RecursiveDeref(dd,u1);
    sizeSq1 = Cudd_DagSize(sq1);
    if (sizeSq1 > sizeF) {
	Cudd_RecursiveDeref(dd,sq1);
	sq1 = f;
	Cudd_Ref(sq1);
	sizeSq1 = sizeF;
    }
    (void) printf("T-M S1 ");
    Cudd_PrintDebug(dd, sq1, nvars, pr);
    (void) printf("TEST-MINI: f %d comp %d mini %d rest %d cons %d sque %d\n",
		  sizeF, sizeCom1, sizeMin1, sizeRs1, sizeCs1, sizeSq1);
    if (option->bdddump) {
	onames[0] = name;	outputs[0] = f;
	onames[1] = cname;	outputs[1] = c;
	onames[2] = "cons";	outputs[2] = cs1;
	onames[3] = "rest";	outputs[3] = rs1;
	onames[4] = "comp";	outputs[4] = com1;
	onames[5] = "mini";	outputs[5] = min1;
	onames[6] = "sqee";	outputs[6] = sq1;
	onames[7] = "lb";	outputs[7] = l1;
	onames[8] = "ub";	outputs[8] = u1;
	if (!Bnet_bddArrayDump(dd, net1, option->dumpfile, outputs, onames,
			       9, option->dumpFmt))
	    return(0);
    }

    /* Compute negative generalized cofactor. */
    (void) printf("TEST-MINI:: %s\n", name);
    (void) printf("T-M    ");
    Cudd_PrintDebug(dd, f, nvars, pr);

    com0 = Cudd_bddLICompaction(dd, f, Cudd_Not(c));
    if (com0 == NULL) {
	(void) printf("TEST-MINI: LI-compaction failed (2).\n");
	return(0);
    }
    Cudd_Ref(com0);
    (void) printf("T-M L0 ");
    Cudd_PrintDebug(dd, com0, nvars, pr);
    sizeCom0 = Cudd_DagSize(com0);
    if (sizeCom0 > sizeF) {
	(void) printf("TEST-MINI: LI-compaction not safe (2).\n");
	return(0);
    }
    min0 = Cudd_bddMinimize(dd, f, Cudd_Not(c));
    if (min0 == NULL) {
	(void) printf("TEST-MINI: minimize failed (2).\n");
	return(0);
    }
    Cudd_Ref(min0);
    (void) printf("T-M M0 ");
    Cudd_PrintDebug(dd, min0, nvars, pr);
    sizeMin0 = Cudd_DagSize(min0);
    if (sizeMin0 > sizeF) {
	(void) printf("TEST-MINI: minimize not safe (2).\n");
	return(0);
    }
    rs0 = Cudd_bddRestrict(dd, f, Cudd_Not(c));
    if (rs0 == NULL) {
	(void) printf("TEST-MINI: restrict failed (2).\n");
	return(0);
    }
    Cudd_Ref(rs0);
    (void) printf("T-M R0 ");
    Cudd_PrintDebug(dd, rs0, nvars, pr);
    sizeRs0 = Cudd_DagSize(rs0);
    cs0 = Cudd_bddConstrain(dd, f, Cudd_Not(c));
    if (cs0 == NULL) {
	(void) printf("TEST-MINI: constrain failed (2).\n");
	return(0);
    }
    Cudd_Ref(cs0);
    (void) printf("T-M C0 ");
    Cudd_PrintDebug(dd, cs0, nvars, pr);
    sizeCs0 = Cudd_DagSize(cs0);

    l0 = Cudd_bddAnd(dd, f, Cudd_Not(c));
    if (l0 == NULL) {
	(void) printf("TEST-MINI: lower bound failed (2).\n");
	return(0);
    }
    Cudd_Ref(l0);
    u0 = Cudd_bddOr(dd, f, c);
    if (u0 == NULL) {
	(void) printf("TEST-MINI: upper bound failed (2).\n");
	return(0);
    }
    Cudd_Ref(u0);
    (void) printf("TEST-MINI: (lb,ub) : (%d,%d) nodes\n",
		  Cudd_DagSize(l0), Cudd_DagSize(u0));
    sq0 = Cudd_bddSqueeze(dd, l0, u0);
    if (sq0 == NULL) {
	(void) printf("TEST-MINI: squeezing failed (2).\n");
	return(0);
    }
    Cudd_Ref(sq0);
    Cudd_RecursiveDeref(dd,l0);
    Cudd_RecursiveDeref(dd,u0);
    sizeSq0 = Cudd_DagSize(sq0);
    if (sizeSq0 > sizeF) {
	Cudd_RecursiveDeref(dd,sq0);
	sq0 = f;
	Cudd_Ref(sq0);
	sizeSq0 = sizeF;
    }
    (void) printf("T-M S0 ");
    Cudd_PrintDebug(dd, sq0, nvars, pr);
    (void) printf("TEST-MINI: f %d comp %d mini %d rest %d cons %d sque %d\n",
		  sizeF, sizeCom0, sizeMin0, sizeRs0, sizeCs0, sizeSq0);    

    /* Check fundamental identity. */
    g = Cudd_bddIte(dd,c,com1,com0);
    if (g == NULL) {
	(void) printf("TEST-MINI: LI-compaction failed (3).\n");
	return(0);
    }
    Cudd_Ref(g);
    if (g != f) {
	(void) printf("TEST-MINI: LI-compaction failed (4).\n");
	return(0);
    }
    Cudd_RecursiveDeref(dd,com1);
    Cudd_RecursiveDeref(dd,com0);
    Cudd_RecursiveDeref(dd,g);
    g = Cudd_bddIte(dd,c,min1,min0);
    if (g == NULL) {
	(void) printf("TEST-MINI: minimize failed (3).\n");
	return(0);
    }
    Cudd_Ref(g);
    if (g != f) {
	(void) printf("TEST-MINI: minimize failed (4).\n");
	return(0);
    }
    Cudd_RecursiveDeref(dd,min1);
    Cudd_RecursiveDeref(dd,min0);
    Cudd_RecursiveDeref(dd,g);
    g = Cudd_bddIte(dd,c,sq1,sq0);
    if (g == NULL) {
	(void) printf("TEST-MINI: squeezing failed (3).\n");
	return(0);
    }
    Cudd_Ref(g);
    if (g != f) {
	(void) printf("TEST-MINI: squeezing failed (4).\n");
	return(0);
    }
    Cudd_RecursiveDeref(dd,sq1);
    Cudd_RecursiveDeref(dd,sq0);
    Cudd_RecursiveDeref(dd,g);
    g = Cudd_bddIte(dd,c,rs1,rs0);
    if (g == NULL) {
	(void) printf("TEST-MINI: restrict failed (3).\n");
	return(0);
    }
    Cudd_Ref(g);
    if (g != f) {
	(void) printf("TEST-MINI: restrict failed (4).\n");
	return(0);
    }
    Cudd_RecursiveDeref(dd,rs1);
    Cudd_RecursiveDeref(dd,rs0);
    Cudd_RecursiveDeref(dd,g);
    g = Cudd_bddIte(dd,c,cs1,cs0);
    if (g == NULL) {
	(void) printf("TEST-MINI: constrain failed (3).\n");
	return(0);
    }
    Cudd_Ref(g);
    if (g != f) {
	(void) printf("TEST-MINI: constrain failed (4).\n");
	return(0);
    }
    Cudd_RecursiveDeref(dd,cs1);
    Cudd_RecursiveDeref(dd,cs0);
    Cudd_RecursiveDeref(dd,g);

    return(1);

} /* end of ntrTestMinimizationAux */


/**Function********************************************************************

  Synopsis    [Processes one BDD for Ntr_TestDensity.]

  Description [Processes one BDD for Ntr_TestDensity.  Returns 1 if
  successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     [Ntr_TestDensity ntrCompress1]

*****************************************************************************/
static int
ntrTestDensityAux(
  DdManager * dd,
  BnetNetwork * net,
  DdNode * f,
  char * name,
  NtrOptions * option)
{
    DdNode *s, *hb, *sp, *ua, *c1, *c2;
    int pr;
    int result;
    int nvars;
    int size, sizeF, sizeS, sizeHB, sizeSP, sizeUA, sizeC1, sizeC2;
    double densityF, densityS, densityHB, densitySP, densityUA;
    double densityC1, densityC2;
    char *onames[7];
    DdNode *outputs[7];

    result = 1;
    pr = option->verb;
    nvars = Cudd_SupportSize(dd,f);
    if (nvars == CUDD_OUT_OF_MEM) return(0);
    sizeF = Cudd_DagSize(f);
    densityF = Cudd_Density(dd,f,nvars);
    (void) printf("TEST-DENSITY:: %s (%d variables)\n", name, nvars);
    if (pr > 0) {
	(void) printf("T-D    (%g)", densityF);
	Cudd_PrintDebug(dd, f, nvars, pr);
	(void) printf("T-D APA ");
	if (!Cudd_ApaPrintMinterm(stdout, dd, f, nvars))
	    result = 0;
    }
    /* s = Cudd_SubsetRemap(dd,f); */
    s = Cudd_RemapUnderApprox(dd,f,nvars,0,option->quality);
    if (s == NULL) {
	(void) printf("TEST-DENSITY: computation failed\n");
	return(0);
    }
    Cudd_Ref(s);
    sizeS = Cudd_DagSize(s);
    densityS = Cudd_Density(dd,s,nvars);
    if (pr > 0) {
	(void) printf("T-D ID (%g)", densityS);
	Cudd_PrintDebug(dd, s, nvars, pr);
    }
    if (!Cudd_bddLeq(dd,s,f)) {
	(void) printf("TEST-DENSITY: result not a subset\n");
	result = 0;
    }
    if (densityF > densityS) {
	(void) printf("TEST-DENSITY: result less dense\n");
	/* result = 0; */
    }
    /* size = (sizeS < sizeF/2) ? sizeS : sizeF/2; */
    size = sizeS;
    hb = Cudd_SubsetHeavyBranch(dd, f, nvars, size);
    if (hb == NULL) {
	(void) printf("TEST-DENSITY: HB computation failed\n");
	Cudd_RecursiveDeref(dd,s);
	return(0);
    }
    Cudd_Ref(hb);
    sizeHB = Cudd_DagSize(hb);
    densityHB = Cudd_Density(dd,hb,nvars);
    if (pr > 0) {
	(void) printf("T-D HB (%g)", densityHB);
	Cudd_PrintDebug(dd, hb, nvars, pr);
    }
    if (!Cudd_bddLeq(dd,hb,f)) {
	(void) printf("TEST-DENSITY: HB not a subset\n");
	result = 0;
    }
    sp = Cudd_SubsetShortPaths(dd, f, nvars, size, 0);
    if (sp == NULL) {
	(void) printf("TEST-DENSITY: SP computation failed\n");
	Cudd_RecursiveDeref(dd,s);
	Cudd_RecursiveDeref(dd,hb);
	return(0);
    }
    Cudd_Ref(sp);
    sizeSP = Cudd_DagSize(sp);
    densitySP = Cudd_Density(dd,sp,nvars);
    if (pr > 0) {
	(void) printf("T-D SP (%g)", densitySP);
	Cudd_PrintDebug(dd, sp, nvars, pr);
    }
    if (!Cudd_bddLeq(dd,sp,f)) {
	(void) printf("TEST-DENSITY: SP not a subset\n");
	result = 0;
    }
    ua = Cudd_UnderApprox(dd,f,nvars,0,FALSE,option->quality);
    if (ua == NULL) {
	(void) printf("TEST-DENSITY: computation failed\n");
	Cudd_RecursiveDeref(dd,s);
	Cudd_RecursiveDeref(dd,hb);
	Cudd_RecursiveDeref(dd,sp);
	return(0);
    }
    Cudd_Ref(ua);
    sizeUA = Cudd_DagSize(ua);
    densityUA = Cudd_Density(dd,ua,nvars);
    if (pr > 0) {
	(void) printf("T-D UA (%g)", densityUA);
	Cudd_PrintDebug(dd, ua, nvars, pr);
    }
    if (!Cudd_bddLeq(dd,ua,f)) {
	(void) printf("TEST-DENSITY: result not a subset\n");
	result = 0;
    }
    if (densityF > densityUA) {
	(void) printf("TEST-DENSITY: result less dense\n");
    }
    c1 = ntrCompress2(dd, f, nvars, size);
    if (c1 == NULL) {
	(void) printf("TEST-DENSITY: C1 computation failed\n");
	Cudd_RecursiveDeref(dd,s);
	Cudd_RecursiveDeref(dd,hb);
	Cudd_RecursiveDeref(dd,sp);
	Cudd_RecursiveDeref(dd,ua);
	return(0);
    }
    sizeC1 = Cudd_DagSize(c1);
    densityC1 = Cudd_Density(dd,c1,nvars);
    if (pr > 0) {
	(void) printf("T-D C1 (%g)", densityC1);
	Cudd_PrintDebug(dd, c1, nvars, pr);
    }
    if (!Cudd_bddLeq(dd,c1,f)) {
	(void) printf("TEST-DENSITY: C1 not a subset\n");
	result = 0;
    }
    c2 = Cudd_SubsetCompress(dd, f, nvars, size);
    if (c2 == NULL) {
	(void) printf("TEST-DENSITY: C2 computation failed\n");
	Cudd_RecursiveDeref(dd,s);
	Cudd_RecursiveDeref(dd,hb);
	Cudd_RecursiveDeref(dd,sp);
	Cudd_RecursiveDeref(dd,ua);
	Cudd_RecursiveDeref(dd,c1);
	return(0);
    }
    Cudd_Ref(c2);
    sizeC2 = Cudd_DagSize(c2);
    densityC2 = Cudd_Density(dd,c2,nvars);
    if (pr > 0) {
	(void) printf("T-D C2 (%g)", densityC2);
	Cudd_PrintDebug(dd, c2, nvars, pr);
    }
    if (!Cudd_bddLeq(dd,c2,f)) {
	(void) printf("TEST-DENSITY: C2 not a subset\n");
	result = 0;
    }
    if (option->bdddump) {
	onames[0] = name;	outputs[0] = f;
	onames[1] = "id";	outputs[1] = s;
	onames[2] = "hb";	outputs[2] = hb;
	onames[3] = "sp";	outputs[3] = sp;
	onames[4] = "ua";	outputs[4] = ua;
	onames[5] = "c1";	outputs[5] = c1;
	onames[6] = "c2";	outputs[6] = c2;
	result &= Bnet_bddArrayDump(dd, net, option->dumpfile, outputs,
				     onames, 7, option->dumpFmt);
    }

    Cudd_RecursiveDeref(dd,s);
    Cudd_RecursiveDeref(dd,hb);
    Cudd_RecursiveDeref(dd,sp);
    Cudd_RecursiveDeref(dd,ua);
    Cudd_RecursiveDeref(dd,c1);
    Cudd_RecursiveDeref(dd,c2);

    return(result);

} /* end of ntrTestDensityAux */


/**Function********************************************************************

  Synopsis    [Processes one BDD for Ntr_TestDecomp.]

  Description [Processes one BDD for Ntr_TestDecomp.  Returns 1 if
  successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     [Ntr_TestDecomp]

*****************************************************************************/
static int
ntrTestDecompAux(
  DdManager * dd,
  BnetNetwork * net,
  DdNode * f,
  char * name,
  NtrOptions * option)
{
    DdNode *one, *g, *h, *product;
    DdNode **A, **I, **G, **V;
    int pr;
    int i, result;
    int nA, nI, nG, nV;
    int nvars;
    int sizeF, sizeGa, sizeHa, sizeSa;
    int sizeGi, sizeHi, sizeSi, sizeGg, sizeHg, sizeSg, sizeGv, sizeHv, sizeSv;
    char *onames[9];
    DdNode *outputs[9];

    result = 1;
    pr = option->verb;
    nvars = Cudd_SupportSize(dd,f);
    if (nvars == CUDD_OUT_OF_MEM) return(0);
    sizeF = Cudd_DagSize(f);
    (void) printf("TEST-DECOMP:: %s (%d variables)\n", name, nvars);
    if (pr > 0) {
	(void) printf("T-d    ");
	Cudd_PrintDebug(dd, f, nvars, pr);
    }
    one = Cudd_ReadOne(dd);

    /* Test Cudd_bddApproxConjDecomp */
    nA = Cudd_bddApproxConjDecomp(dd,f,&A);
    if (nA == 0) {
	(void) printf("TEST-DECOMP: computation failed\n");
	return(0);
    }
    g = A[0];
    h = (nA == 2) ? A[1] : one;
    sizeGa = Cudd_DagSize(g);
    sizeHa = Cudd_DagSize(h);
    sizeSa = Cudd_SharingSize(A,nA);
    if (pr > 0) {
	(void) printf("T-d SS : %d nodes\n", sizeSa);
	(void) printf("T-d GS ");
	Cudd_PrintDebug(dd, g, nvars, pr);
	(void) printf("T-d HS ");
	Cudd_PrintDebug(dd, h, nvars, pr);
    }
    product = Cudd_bddAnd(dd,g,h);
    if (product == NULL) {
	(void) printf("TEST-DECOMP: computation failed\n");
	return(0);
    }
    Cudd_Ref(product);
    if (product != f) {
	(void) printf("TEST-DECOMP: result not a decomposition\n");
	result = 0;
    }
    Cudd_RecursiveDeref(dd,product);

    /* Test Cudd_bddIterConjDecomp */
    nI = Cudd_bddIterConjDecomp(dd,f,&I);
    if (nI == 0) {
	(void) printf("TEST-DECOMP: computation failed\n");
	return(0);
    }
    g = I[0];
    h = (nI == 2) ? I[1] : one;
    sizeGi = Cudd_DagSize(g);
    sizeHi = Cudd_DagSize(h);
    sizeSi = Cudd_SharingSize(I,nI);
    if (pr > 0) {
	(void) printf("T-d SI : %d nodes\n", sizeSi);
	(void) printf("T-d GI ");
	Cudd_PrintDebug(dd, g, nvars, pr);
	(void) printf("T-d HI ");
	Cudd_PrintDebug(dd, h, nvars, pr);
    }
    product = Cudd_bddAnd(dd,g,h);
    if (product == NULL) {
	(void) printf("TEST-DECOMP: computation failed\n");
	return(0);
    }
    Cudd_Ref(product);
    if (product != f) {
	(void) printf("TEST-DECOMP: result not a decomposition\n");
	result = 0;
    }
    Cudd_RecursiveDeref(dd,product);

    /* Test Cudd_bddGenConjDecomp */
    nG = Cudd_bddGenConjDecomp(dd,f,&G);
    if (nG == 0) {
	(void) printf("TEST-DECOMP: computation failed\n");
	return(0);
    }
    g = G[0];
    h = (nG == 2) ? G[1] : one;
    sizeGg = Cudd_DagSize(g);
    sizeHg = Cudd_DagSize(h);
    sizeSg = Cudd_SharingSize(G,nG);
    if (pr > 0) {
	(void) printf("T-d SD : %d nodes\n", sizeSg);
	(void) printf("T-d GD ");
	Cudd_PrintDebug(dd, g, nvars, pr);
	(void) printf("T-d HD ");
	Cudd_PrintDebug(dd, h, nvars, pr);
    }
    product = Cudd_bddAnd(dd,g,h);
    if (product == NULL) {
	(void) printf("TEST-DECOMP: computation failed\n");
	return(0);
    }
    Cudd_Ref(product);
    if (product != f) {
	(void) printf("TEST-DECOMP: result not a decomposition\n");
	result = 0;
    }
    Cudd_RecursiveDeref(dd,product);

    /* Test Cudd_bddVarConjDecomp */
    nV = Cudd_bddVarConjDecomp(dd,f,&V);
    if (nV == 0) {
	(void) printf("TEST-DECOMP: computation failed\n");
	return(0);
    }
    g = V[0];
    h = (nV == 2) ? V[1] : one;
    sizeGv = Cudd_DagSize(g);
    sizeHv = Cudd_DagSize(h);
    sizeSv = Cudd_SharingSize(V,nV);
    if (pr > 0) {
	(void) printf("T-d SQ : %d nodes\n", sizeSv);
	(void) printf("T-d GQ ");
	Cudd_PrintDebug(dd, g, nvars, pr);
	(void) printf("T-d HQ ");
	Cudd_PrintDebug(dd, h, nvars, pr);
    }
    product = Cudd_bddAnd(dd,g,h);
    if (product == NULL) {
	(void) printf("TEST-DECOMP: computation failed\n");
	return(0);
    }
    Cudd_Ref(product);
    if (product != f) {
	(void) printf("TEST-DECOMP: result not a decomposition\n");
	result = 0;
    }
    Cudd_RecursiveDeref(dd,product);

    /* Dump to file if requested. */
    if (option->bdddump) {
	onames[0] = name;	outputs[0] = f;
	onames[1] = "ga";	outputs[1] = A[0];
	onames[2] = "ha";	outputs[2] = (nA == 2) ? A[1] : one;
	onames[3] = "gi";	outputs[3] = I[0];
	onames[4] = "hi";	outputs[4] = (nI == 2) ? I[1] : one;
	onames[5] = "gg";	outputs[5] = G[0];
	onames[6] = "hg";	outputs[6] = (nG == 2) ? G[1] : one;
	onames[7] = "gv";	outputs[7] = V[0];
	onames[8] = "hv";	outputs[8] = (nV == 2) ? V[1] : one;
	result &= Bnet_bddArrayDump(dd, net, option->dumpfile, outputs,
				     onames, 9, option->dumpFmt);
    }

    /* Clean up. */
    for (i = 0; i < nA; i++) {
	Cudd_RecursiveDeref(dd,A[i]);
    }
    for (i = 0; i < nI; i++) {
	Cudd_RecursiveDeref(dd,I[i]);
    }
    for (i = 0; i < nG; i++) {
	Cudd_RecursiveDeref(dd,G[i]);
    }
    for (i = 0; i < nV; i++) {
	Cudd_RecursiveDeref(dd,V[i]);
    }
    FREE(A);
    FREE(I);
    FREE(G);
    FREE(V);

    return(result);

} /* end of ntrTestDecompAux */


/**Function********************************************************************

  Synopsis    [Processes one BDD for Ntr_TestCofactorEstimate.]

  Description [Processes one BDD for Ntr_TestCofactorEstimate.  Returns 1 if
  successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

*****************************************************************************/
static int
ntrTestCofEstAux(
  DdManager * dd,
  BnetNetwork * net,
  DdNode * f,
  char * name,
  NtrOptions * option)
{
    DdNode *support, *scan, *cof;
    int pr;
    int nvars;
    int exactSize, estimate, estimateS;
    int totalExactSize = 0;
    int totalEstimate = 0;
    int totalEstimateS = 0;
    int largestError = -1;
    int largestErrorS = -1;
    DdNode *errorVar = NULL;

    pr = option->verb;
    support = Cudd_Support(dd,f);
    if (support == NULL) return(0);
    Cudd_Ref(support);
    nvars = Cudd_DagSize(support) - 1;
    scan = support;
    while (!Cudd_IsConstant(scan)) {
	DdNode *var = Cudd_bddIthVar(dd,scan->index);
	cof = Cudd_Cofactor(dd,f,var);
	if (cof == NULL) return(0);
	Cudd_Ref(cof);
	exactSize = Cudd_DagSize(cof);
	totalExactSize += exactSize;
	estimate = Cudd_EstimateCofactor(dd,f,scan->index,1);
	totalEstimate += estimate;
	if (estimate < exactSize)
	    (void) printf("Optimistic estimate!\n");
	if (estimate - exactSize > largestError) {
	    largestError = estimate - exactSize;
	    errorVar = var;
	}
	estimateS = Cudd_EstimateCofactorSimple(f,scan->index);
	totalEstimateS += estimateS;
	if (estimateS < exactSize)
	    (void) printf("Optimistic estimateS!\n");
	if (estimateS - exactSize > largestErrorS)
	    largestErrorS = estimateS - exactSize;
	Cudd_RecursiveDeref(dd,cof);
	scan = cuddT(scan);
    }
    Cudd_RecursiveDeref(dd,support);
    (void) printf("TEST-COF:: %s (%d vars)", name, nvars);
    Cudd_PrintDebug(dd, f, nvars, pr);
    (void) printf("T-c   : %d\n", totalExactSize);
    (void) printf("T-c E : %d %d\n", totalEstimate, largestError);
    (void) printf("T-c S : %d %d\n", totalEstimateS, largestErrorS);

    /* Dump to file if requested. */
    if (option->bdddump) {
	char *onames[3];
	DdNode *outputs[3];
	int result;
	cof = Cudd_Cofactor(dd,f,errorVar);
	if (cof == NULL) return(0);
	Cudd_Ref(cof);
	onames[0] = name;	outputs[0] = f;
	onames[1] = "var";	outputs[1] = errorVar;
	onames[2] = "cof";	outputs[2] = cof;
	result = Bnet_bddArrayDump(dd, net, option->dumpfile, outputs,
				     onames, 3, option->dumpFmt);
	Cudd_RecursiveDeref(dd,cof);
	if (result == 0) return(0);
    }

    return(1);

} /* end of ntrTestCofEstAux */


/**Function********************************************************************

  Synopsis    [Processes one BDD for Ntr_TestClipping.]

  Description [Processes one BDD for Ntr_TestClipping.  It checks whether
  clipping was correct.  Returns 1 if successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     [Ntr_TestClipping]

*****************************************************************************/
static int
ntrTestClippingAux(
  DdManager * dd,
  BnetNetwork * net1,
  DdNode * f,
  char * name,
  DdNode * g,
  char * gname,
  NtrOptions * option)
{
    DdNode *prod, *sub, *sup;
    DdNode *subF, *subG, *psub;
    DdNode *supF, *supG, *psup;
    int pr, nvars, depth;
    int sizeF, sizeProd, sizeSub, sizeSup;
    static char *onames[7];
    DdNode *outputs[7];
    DdNode *operands[2];
    int retval = 1;
    int threshold = (option->threshold < 0) ? 0 : option->threshold;

    pr = option->verb;
    operands[0] = f; operands[1] = g;
    nvars = Cudd_VectorSupportSize(dd,operands,2);
    if (nvars == CUDD_OUT_OF_MEM) return(0);
    depth = (int) ((double) nvars * option->clip);
    (void) printf("TEST-CLIP:: %s depth = %d\n", name, depth);
    (void) printf("T-C    ");
    Cudd_PrintDebug(dd, f, nvars, pr);
    sizeF = Cudd_DagSize(f);

    /* Compute product. */
    prod = Cudd_bddAnd(dd, f, g);
    if (prod == NULL) {
	(void) printf("TEST-CLIP: product failed.\n");
	return(0);
    }
    Cudd_Ref(prod);
    (void) printf("T-C P= ");
    Cudd_PrintDebug(dd, prod, nvars, pr);
    sizeProd = Cudd_DagSize(prod);

    /* Compute subset of product. */
    sub = Cudd_bddClippingAnd(dd, f, g, depth, 0);
    if (sub == NULL) {
	(void) printf("TEST-CLIP: subsetting product failed.\n");
	return(0);
    }
    Cudd_Ref(sub);
    (void) printf("T-C P- ");
    Cudd_PrintDebug(dd, sub, nvars, pr);
    sizeSub = Cudd_DagSize(sub);
    if (sizeSub > sizeProd) {
	(void) printf("TEST-CLIP: subsetting product not safe.\n");
    }

    /* Compute product of subsets. */
    subF = Cudd_RemapUnderApprox(dd,f,nvars,threshold,option->quality);
    if (subF == NULL) {
	(void) printf("TEST-CLIP: subsetting of f failed.\n");
	return(0);
    }
    Cudd_Ref(subF);
    subG = Cudd_RemapUnderApprox(dd,g,nvars,threshold,option->quality);
    if (subF == NULL) {
	(void) printf("TEST-CLIP: subsetting of g failed.\n");
	return(0);
    }
    Cudd_Ref(subG);
    psub = Cudd_bddAnd(dd,subF,subG);
    if (psub == NULL) {
	(void) printf("TEST-CLIP: product of subsets failed.\n");
	return(0);
    }
    Cudd_Ref(psub);
    Cudd_RecursiveDeref(dd,subF);
    Cudd_RecursiveDeref(dd,subG);
    (void) printf("T-C P< ");
    Cudd_PrintDebug(dd, psub, nvars, pr);

    /* Compute superset of product. */
    sup = Cudd_bddClippingAnd(dd, f, g, depth, 1);
    if (sup == NULL) {
	(void) printf("TEST-CLIP: supersetting product failed.\n");
	return(0);
    }
    Cudd_Ref(sup);
    (void) printf("T-C P+ ");
    Cudd_PrintDebug(dd, sup, nvars, pr);
    sizeSup = Cudd_DagSize(sup);
    if (sizeSup > sizeProd) {
	(void) printf("TEST-CLIP: supersetting product not safe.\n");
    }

    /* Compute product of supersets. */
    supF = Cudd_RemapOverApprox(dd,f,nvars,threshold,option->quality);
    if (supF == NULL) {
	(void) printf("TEST-CLIP: supersetting of f failed.\n");
	return(0);
    }
    Cudd_Ref(supF);
    supG = Cudd_RemapOverApprox(dd,g,nvars,threshold,option->quality);
    if (supF == NULL) {
	(void) printf("TEST-CLIP: supersetting of g failed.\n");
	return(0);
    }
    Cudd_Ref(supG);
    psup = Cudd_bddAnd(dd,supF,supG);
    if (psup == NULL) {
	(void) printf("TEST-CLIP: product of supersets failed.\n");
	return(0);
    }
    Cudd_Ref(psup);
    Cudd_RecursiveDeref(dd,supF);
    Cudd_RecursiveDeref(dd,supG);
    (void) printf("T-C P> ");
    Cudd_PrintDebug(dd, psup, nvars, pr);

    if (option->bdddump) {
	onames[0] = name;	outputs[0] = f;
	onames[1] = gname;	outputs[1] = g;
	onames[2] = "prod";	outputs[2] = prod;
	onames[3] = "sub";	outputs[3] = sub;
	onames[4] = "sup";	outputs[4] = sup;
	onames[5] = "psub";	outputs[5] = psub;
	onames[6] = "psup";	outputs[6] = psup;
	retval &= Bnet_bddArrayDump(dd, net1, option->dumpfile, outputs,
				     onames, 7, option->dumpFmt);
    }

    /* Check correctness. */
    if (!Cudd_bddLeq(dd,sub,prod)) {
	(void) printf("TEST-CLIP: subsetting product not a subset.\n");
	return(0);
    }
    if (!Cudd_bddLeq(dd,prod,sup)) {
	(void) printf("TEST-CLIP: supersetting product not a superset.\n");
	return(0);
    }
    if (!Cudd_bddLeq(dd,psub,prod)) {
	(void) printf("TEST-CLIP: product of subsets not a subset.\n");
	return(0);
    }
    if (!Cudd_bddLeq(dd,prod,psup)) {
	(void) printf("TEST-CLIP: product of supersets not a superset.\n");
	return(0);
    }

    Cudd_RecursiveDeref(dd,prod);
    Cudd_RecursiveDeref(dd,sub);
    Cudd_RecursiveDeref(dd,sup);
    Cudd_RecursiveDeref(dd,psub);
    Cudd_RecursiveDeref(dd,psup);

    return(retval);

} /* end of ntrTestClippingAux */


/**Function********************************************************************

  Synopsis    [Try hard to squeeze a BDD.]

  Description [Try hard to squeeze a BDD.
  Returns a pointer to the squeezed BDD if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     [ntrTestDensityAux Cudd_SubsetCompress]

*****************************************************************************/
static DdNode *
ntrCompress1(
  DdManager * dd,
  DdNode * f,
  int  nvars,
  int  threshold)
{
    DdNode *res, *tmp1, *tmp2;
    int sizeI, size;

    tmp1 = Cudd_RemapUnderApprox(dd,f,nvars,0,1.0);
    if (tmp1 == NULL) return(NULL);
    Cudd_Ref(tmp1);
    sizeI = Cudd_DagSize(tmp1);
    size = (sizeI < threshold) ? sizeI : threshold;
    tmp2 = Cudd_SubsetShortPaths(dd, tmp1, nvars, size, 0);
    if (tmp2 == NULL) {
	Cudd_RecursiveDeref(dd,tmp1);
	return(NULL);
    }
    Cudd_Ref(tmp2);
    Cudd_RecursiveDeref(dd,tmp1);
    res = Cudd_bddSqueeze(dd,tmp2,f);
    if (res == NULL) {
	Cudd_RecursiveDeref(dd,tmp2);
	return(NULL);
    }
    Cudd_Ref(res);
    Cudd_RecursiveDeref(dd,tmp2);
    return(res);

} /* end of ntrCompress1 */


/**Function********************************************************************

  Synopsis    [Try hard to squeeze a BDD.]

  Description [Try hard to squeeze a BDD.
  Returns a pointer to the squeezed BDD if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     [ntrTestDensityAux Cudd_SubsetCompress]

*****************************************************************************/
static DdNode *
ntrCompress2(
  DdManager * dd,
  DdNode * f,
  int  nvars,
  int  threshold)
{
    DdNode *res, *tmp1, *tmp2;
    int sizeI;

    tmp1 = Cudd_RemapUnderApprox(dd,f,nvars,0,1.0);
    if (tmp1 == NULL) return(NULL);
    Cudd_Ref(tmp1);
    sizeI = Cudd_DagSize(tmp1);
    if (sizeI > threshold) {
	tmp2 = Cudd_SubsetShortPaths(dd, tmp1, nvars, threshold, 0);
	if (tmp2 == NULL) {
	    Cudd_RecursiveDeref(dd,tmp1);
	    return(NULL);
	}
	Cudd_Ref(tmp2);
	Cudd_RecursiveDeref(dd,tmp1);
    } else {
	tmp2 = tmp1;
    }
    res = Cudd_bddSqueeze(dd,tmp2,f);
    if (res == NULL) {
	Cudd_RecursiveDeref(dd,tmp2);
	return(NULL);
    }
    Cudd_Ref(res);
    if (Cudd_Density(dd,res,nvars) < Cudd_Density(dd,tmp2,nvars)) {
	Cudd_RecursiveDeref(dd,res);
	return(tmp2);
    } else {
	Cudd_RecursiveDeref(dd,tmp2);
	return(res);
    }

} /* end of ntrCompress2 */


/**Function********************************************************************

  Synopsis    [Checks whether node is a buffer.]

  Description [Checks whether node is a buffer. Returns a pointer to the
  input node if nd is a buffer; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static BnetNode *
ntrNodeIsBuffer(
  BnetNode *nd,
  st_table *hash)
{
    BnetNode *inpnd;

    if (nd->ninp != 1) return(0);
    if (!st_lookup(hash,(char *) nd->inputs[0], (char **) &inpnd)) return(0);

    return(nd->dd == inpnd->dd ? inpnd : NULL);

} /* end of ntrNodeIsBuffer */
