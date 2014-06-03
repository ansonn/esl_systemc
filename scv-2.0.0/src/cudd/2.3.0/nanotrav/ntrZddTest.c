/**CFile***********************************************************************
 
   FileName    [ntrZddTest.c]

   PackageName [ntr]

   Synopsis    [ZDD test functions.]

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
static char rcsid[] UTIL_UNUSED = "$Id: ntrZddTest.c,v 1.1 2002/09/17 21:59:08 dlm Exp $";
#endif

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int reorderZdd ARGS((BnetNetwork *net, DdManager *dd, NtrOptions *option));

/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Tests ZDDs.]

  Description [Tests ZDDs. Returns 1 if successful; 0 otherwise.]

  SideEffects [Creates ZDD variables in the manager.]

  SeeAlso     []

******************************************************************************/
int
Ntr_testZDD(
  DdManager * dd,
  BnetNetwork * net,
  NtrOptions * option)
{
    DdNode **zdd;		/* array of converted outputs */
    int result;
    int i, j;
    BnetNode *node;		/* auxiliary pointer to network node */
    int pr = option->verb;
    int level;			/* aux. var. used to print variable orders */
    char **names;		/* array used to print variable orders */
    int nvars;

    /* Build an array of ZDDs for the output functions. */
    Cudd_AutodynDisable(dd);
    Cudd_AutodynDisableZdd(dd);
    zdd = ALLOC(DdNode *,net->noutputs);
    result = Cudd_zddVarsFromBddVars(dd,1);
    if (result == 0) return(0);
    for (i = 0; i < net->noutputs; i++) {
	if (!st_lookup(net->hash,net->outputs[i],(char **)&node)) {
	    return(0);
	}
	zdd[i] = Cudd_zddPortFromBdd(dd, node->dd);
	if (zdd[i]) {
	    Cudd_Ref(zdd[i]);
	    (void) printf("%s", node->name);
	    result = Cudd_zddPrintDebug(dd,zdd[i],Cudd_ReadZddSize(dd),pr);
	    if (result == 0) return(0);
	} else {
	    (void) printf("Conversion to ZDD failed.\n");
	}
    }

#ifdef DD_DEBUG
    result = Cudd_CheckKeys(dd);
    if (result != 0) {
	(void) fprintf(stderr,"Error reported by Cudd_CheckKeys\n");
	return(0);
    }
#endif

    if (option->autoDyn & 1) {
	Cudd_AutodynEnable(dd,CUDD_REORDER_SAME);
    }
    if (option->autoDyn & 2) {
	Cudd_AutodynEnableZdd(dd,CUDD_REORDER_SAME);
    }

    /* Convert the ZDDs back to BDDs and check identity. */
    for (i = 0; i < net->noutputs; i++) {
	DdNode *checkBdd;
	checkBdd = Cudd_zddPortToBdd(dd,zdd[i]);
	if (checkBdd) {
	    Cudd_Ref(checkBdd);
	    if (!st_lookup(net->hash,net->outputs[i],(char **)&node)) {
		return(0);
	    }
	    if (checkBdd != node->dd) {
		(void) fprintf(stdout,"Equivalence failed at node %s",
			       node->name);
		result = Cudd_PrintDebug(dd,checkBdd,Cudd_ReadZddSize(dd),pr);
		if (result == 0) return(0);
	    }
	    Cudd_RecursiveDeref(dd,checkBdd);
	} else {
	    (void) printf("Conversion to BDD failed.\n");
	}
    }

#ifdef DD_DEBUG
    result = Cudd_CheckKeys(dd);
    if (result != 0) {
	(void) fprintf(stderr,"Error reported by Cudd_CheckKeys\n");
	return(0);
    }
#endif

    /* Play with the ZDDs a little. */
    if (net->noutputs > 2) {
	DdNode *f;
	DdNode *g1, *g2, *g;
	f = Cudd_zddIte(dd,zdd[0],zdd[1],zdd[2]);
	if (f == NULL) return(0);
	cuddRef(f);
	g1 = Cudd_zddIntersect(dd,zdd[0],zdd[1]);
	if (g1 == NULL) {
	    Cudd_RecursiveDerefZdd(dd,f);
	    return(0);
	}
	cuddRef(g1);
	g2 = Cudd_zddDiff(dd,zdd[2],zdd[0]);
	if (g2 == NULL) {
	    Cudd_RecursiveDerefZdd(dd,f);
	    Cudd_RecursiveDerefZdd(dd,g1);
	    return(0);
	}
	cuddRef(g2);
	g = Cudd_zddUnion(dd,g1,g2);
	if (g == NULL) {
	    Cudd_RecursiveDerefZdd(dd,f);
	    Cudd_RecursiveDerefZdd(dd,g1);
	    Cudd_RecursiveDerefZdd(dd,g2);
	    return(0);
	}
	cuddRef(g);
	Cudd_RecursiveDerefZdd(dd,g1);
	Cudd_RecursiveDerefZdd(dd,g2);
	if (g != f) {
	    (void) fprintf(stderr,"f != g!\n");
	}
	Cudd_RecursiveDerefZdd(dd,g);
	Cudd_RecursiveDerefZdd(dd,f);
    }

#ifdef DD_DEBUG
    result = Cudd_CheckKeys(dd);
    if (result != 0) {
	(void) fprintf(stderr,"Error reported by Cudd_CheckKeys\n");
	return(0);
    }
#endif

    /* Perform ZDD reordering. */
    result = reorderZdd(net,dd,option);
    if (result == 0) return(0);

    /* Print final ZDD order. */
    nvars = Cudd_ReadZddSize(dd);
    names = ALLOC(char *, nvars);
    if (names == NULL) return(0);
    for (i = 0; i < nvars; i++) {
	names[i] = NULL;
    }
    if (option->reordering != CUDD_REORDER_NONE) {
	for (i = 0; i < net->npis; i++) {
	    if (!st_lookup(net->hash,net->inputs[i],(char **)&node)) {
		FREE(names);
		return(0);
	    }
	    level = Cudd_ReadPermZdd(dd,node->var);
	    names[level] = node->name;
	}
	for (i = 0; i < net->nlatches; i++) {
	    if (!st_lookup(net->hash,net->latches[i][1],(char **)&node)) {
		FREE(names);
		return(0);
	    }
	    level = Cudd_ReadPermZdd(dd,node->var);
	    names[level] = node->name;
	}
	(void) printf("New order\n");
	for (i = 0, j = 0; i < nvars; i++) {
	    if (names[i] == NULL) continue;
	    if((j%8 == 0)&&j) (void) printf("\n");
	    (void) printf("%s ",names[i]);
	    j++;
	}
	(void) printf("\n");
    }
    FREE(names);

    /* Dispose of ZDDs. */
    for (i = 0; i < net->noutputs; i++) {
	Cudd_RecursiveDerefZdd(dd,zdd[i]);
    }
    FREE(zdd);
    return(1);

} /* end of Ntr_testZDD */


/**Function********************************************************************

  Synopsis    [Builds ZDD covers.]

  Description []

  SideEffects [Creates ZDD variables in the manager.]

  SeeAlso     []

******************************************************************************/
int
Ntr_testISOP(
  DdManager * dd,
  BnetNetwork * net,
  NtrOptions * option)
{
    DdNode **zdd;		/* array of converted outputs */
    DdNode *bdd;		/* return value of Cudd_zddIsop */
    int result;
    int i;
    BnetNode *node;		/* auxiliary pointer to network node */
    int pr = option->verb;

    /* Build an array of ZDDs for the output functions. */
    Cudd_zddRealignEnable(dd);
    Cudd_AutodynDisableZdd(dd);
    zdd = ALLOC(DdNode *,net->noutputs);
    result = Cudd_zddVarsFromBddVars(dd,2);
    if (result == 0) return(0);
    for (i = 0; i < net->noutputs; i++) {
	if (!st_lookup(net->hash,net->outputs[i],(char **)&node)) {
	    return(0);
	}
	bdd = Cudd_zddIsop(dd, node->dd, node->dd, &zdd[i]);
	if (bdd != node->dd) return(0);
	Cudd_Ref(bdd);
	Cudd_RecursiveDeref(dd,bdd);
	if (zdd[i]) {
	    Cudd_Ref(zdd[i]);
	    (void) printf("%s", node->name);
	    result = Cudd_zddPrintDebug(dd,zdd[i],Cudd_ReadZddSize(dd),pr);
	    if (result == 0) return(0);
#if 0
	    result = Cudd_zddPrintCover(dd,zdd[i]);
	    if (result == 0) return(0);
#endif
	} else {
	    (void) printf("Conversion to ISOP failed.\n");
	}
    }
    if (option->autoDyn) {
	Cudd_AutodynEnableZdd(dd,CUDD_REORDER_SAME);
    }

    /* Perform ZDD reordering. */
    result = reorderZdd(net,dd,option);
    if (result == 0) return(0);

    /* Dispose of ZDDs. */
    for (i = 0; i < net->noutputs; i++) {
	Cudd_RecursiveDerefZdd(dd,zdd[i]);
    }
    FREE(zdd);

    return(1);

} /* end of Ntr_testISOP */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Applies reordering to the ZDDs.]

  Description [Explicitly applies reordering to the ZDDs. Returns 1 if
  successful; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

*****************************************************************************/
static int
reorderZdd(
  BnetNetwork * net,
  DdManager * dd /* DD Manager */,
  NtrOptions * option)
{
    int result;			/* return value from functions */

    /* Perform the final reordering. */
    if (option->reordering != CUDD_REORDER_NONE) {
	(void) printf("Number of inputs = %d\n",net->ninputs);

	dd->siftMaxVar = 1000000; 
	result = Cudd_zddReduceHeap(dd,option->reordering,1);
	if (result == 0) return(0);

	/* Print symmetry stats if pertinent */
	if (option->reordering == CUDD_REORDER_SYMM_SIFT ||
	    option->reordering == CUDD_REORDER_SYMM_SIFT_CONV)
	    Cudd_zddSymmProfile(dd, 0, dd->sizeZ - 1);
    }

    return(1);

} /* end of reorderZdd */

