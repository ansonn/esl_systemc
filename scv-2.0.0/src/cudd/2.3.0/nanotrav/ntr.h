/**CHeaderFile*****************************************************************

  FileName    [ntr.h]

  PackageName [ntr]

  Synopsis    [Simple-minded package to do traversal.]

  Description []

  SeeAlso     []

  Author      [Fabio Somenzi]

  Copyright [This file was created at the University of Colorado at
  Boulder.  The University of Colorado at Boulder makes no warranty
  about the suitability of this software for any purpose.  It is
  presented on an AS IS basis.]

  Revision    [$Id: ntr.h,v 1.1 2002/09/17 21:59:08 dlm Exp $]

******************************************************************************/

#ifndef _NTR
#define _NTR

/*---------------------------------------------------------------------------*/
/* Nested includes                                                           */
/*---------------------------------------------------------------------------*/

#include "dddmp.h"
#include "bnet.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define PI_PS_FROM_FILE	0
#define PI_PS_DFS	1
#define PI_PS_GIVEN	2

#define NTR_IMAGE_MONO 0
#define NTR_IMAGE_PART 1
#define NTR_IMAGE_CLIP 2
#define NTR_IMAGE_DEPEND 3

#define NTR_UNDER_APPROX 0
#define NTR_OVER_APPROX 1

#define NTR_FROM_NEW 0
#define NTR_FROM_REACHED 1
#define NTR_FROM_RESTRICT 2
#define NTR_FROM_COMPACT 3
#define NTR_FROM_SQUEEZE 4
#define NTR_FROM_UNDERAPPROX 5
#define NTR_FROM_OVERAPPROX 6

#define NTR_GROUP_NONE 0
#define NTR_GROUP_DEFAULT 1
#define NTR_GROUP_FIXED 2

/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef	struct	NtrOptions {
    long	initialTime;	/* this is here for convenience */
    int		verify;		/* read two networks and compare them */
    char	*file1;		/* first network file name */
    char	*file2;		/* second network file name */
    int		second;		/* a second network is given */
    int		traverse;	/* do reachability analysis */
    int		depend;		/* do latch dependence analysis */
    int		image;		/* monolithic, partitioned, or clip */
    double	imageClip;	/* clipping depth in image computation */
    int		approx;		/* under or over approximation */
    int		threshold;	/* approximation threshold */
    int		from;		/* method to compute from states */
    int		groupnsps;	/* group present state and next state vars */
    int		closure;	/* use transitive closure */
    double	closureClip;	/* clipping depth in closure computation */
    int		envelope;	/* compute outer envelope */
    int		scc;		/* compute strongly connected components */
    int		zddtest;	/* do zdd test */
    int		maxflow;	/* compute maximum flow in network */
    char	*sinkfile;	/* file for externally provided sink node */
    int		partition;	/* test McMillan conjunctive partitioning */
    int		char2vect;	/* test char-to-vect decomposition */
    int		density;	/* test density-related functions */
    double	quality;	/* quality parameter for density functions */
    int		decomp;		/* test decomposition functions */
    int		cofest;		/* test cofactor estimation */
    double	clip;		/* test clipping functions */
    int		noBuild;	/* do not build BDDs; just echo order */
    int		stateOnly;	/* ignore primary outputs */
    char	*node;		/* only node for which to build BDD */
    int		locGlob;	/* build global or local BDDs */
    int		progress;	/* report output names while building BDDs */
    int		cacheSize;	/* computed table initial size */
    unsigned long maxMemory;	/* computed table maximum size */
    int		slots;		/* unique subtable initial slots */
    int		ordering;	/* FANIN DFS ... */
    char	*orderPiPs;	/* file for externally provided order */
    Cudd_ReorderingType	reordering; /* NONE RANDOM PIVOT SIFTING ... */
    int		autoDyn;	/* ON OFF */
    Cudd_ReorderingType autoMethod; /* RANDOM PIVOT SIFTING CONVERGE ... */
    char 	*treefile;	/* file name for variable tree */
    int		firstReorder;	/* when to do first reordering */
    int		countDead;	/* count dead nodes toward triggering
				   reordering */
    int		maxGrowth;	/* maximum growth during reordering (%) */
    Cudd_AggregationType groupcheck; /* grouping function */
    int		arcviolation;   /* percent violation of arcs in
				   extended symmetry check */
    int		symmviolation;  /* percent symm violation in
				   extended symmetry check */
    int		recomb;		/* recombination parameter for grouping */
    int		nodrop;		/* don't drop intermediate BDDs ASAP */
    int		signatures;	/* computation of signatures */
    int		gaOnOff;	/* whether to run GA at the end */
    int		populationSize;	/* population size for GA */
    int		numberXovers;	/* number of crossovers for GA */
    int		bdddump;	/* ON OFF */
    int		dumpFmt;	/* 0 -> dot 1 -> blif 2 ->daVinci 3 -> DDcal
				** 4 -> factored form */
    char	*dumpfile;	/* filename for dump */
    int		store;		/* iteration at which to store Reached */
    char	*storefile;	/* filename for storing Reached */
    int		load;		/* load initial states from file */
    char	*loadfile;	/* filename for loading states */
    int		verb;		/* level of verbosity */
} NtrOptions;

typedef struct NtrHeapSlot {
    void *item;
    int key;
} NtrHeapSlot;

typedef struct NtrHeap {
    int size;
    int nslots;
    NtrHeapSlot *slots;
} NtrHeap;

typedef struct NtrPartTR {
    int nparts;			/* number of parts */
    DdNode **part;		/* array of parts */
    DdNode **icube;		/* quantification cubes for image */
    DdNode **pcube;		/* quantification cubes for preimage */
    DdNode **nscube;		/* next state variables in each part */
    DdNode *preiabs;		/* present state vars and inputs in no part */
    DdNode *prepabs;		/* inputs in no part */
    DdNode *xw;			/* cube of all present states and PIs */
    NtrHeap *factors;		/* factors extracted from the image */
    int nlatches;		/* number of latches */
    DdNode **x;			/* array of present state variables */
    DdNode **y;			/* array of next state variables */
} NtrPartTR;

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/* These are potential duplicates. */
#ifndef EXTERN
#   ifdef __cplusplus
#	define EXTERN extern "C"
#   else
#	define EXTERN extern
#   endif
#endif
#ifndef ARGS
#   if defined(__STDC__) || defined(__cplusplus)
#	define ARGS(protos)	protos		/* ANSI C */
#   else /* !(__STDC__ || __cplusplus) */
#	define ARGS(protos)	()		/* K&R C */
#   endif /* !(__STDC__ || __cplusplus) */
#endif

#ifndef TRUE
#   define TRUE 1
#endif
#ifndef FALSE
#   define FALSE 0
#endif

/**Macro***********************************************************************

  Synopsis     [Returns 1 if the two arguments are identical strings.]

  Description  []

  SideEffects  [none]

  SeeAlso      []
		   
******************************************************************************/
#define STRING_EQUAL(s1,s2) (strcmp((s1),(s2)) == 0)


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN int Ntr_buildDDs ARGS((BnetNetwork *net, DdManager *dd, NtrOptions *option, BnetNetwork *net2));
EXTERN NtrPartTR * Ntr_buildTR ARGS((DdManager *dd, BnetNetwork *net, NtrOptions *option, int image));
EXTERN DdNode * Ntr_TransitiveClosure ARGS((DdManager *dd, NtrPartTR *TR, NtrOptions *option));
EXTERN int Ntr_Trav ARGS((DdManager *dd, BnetNetwork *net, NtrOptions *option));
EXTERN int Ntr_SCC ARGS((DdManager *dd, BnetNetwork *net, NtrOptions *option));
EXTERN int Ntr_ClosureTrav ARGS((DdManager *dd, BnetNetwork *net, NtrOptions *option));
EXTERN void Ntr_freeTR ARGS((DdManager *dd, NtrPartTR *TR));
EXTERN NtrPartTR * Ntr_cloneTR ARGS((NtrPartTR *TR));
EXTERN DdNode * Ntr_initState ARGS((DdManager *dd, BnetNetwork *net, NtrOptions *option));
EXTERN DdNode * Ntr_getStateCube ARGS((DdManager *dd, BnetNetwork *net, char *filename, int pr));
EXTERN int Ntr_Envelope ARGS((DdManager *dd, NtrPartTR *TR, FILE *dfp, NtrOptions *option));
EXTERN int Ntr_TestMinimization ARGS((DdManager *dd, BnetNetwork *net1, BnetNetwork *net2, NtrOptions *option));
EXTERN int Ntr_TestDensity ARGS((DdManager *dd, BnetNetwork *net1, NtrOptions *option));
EXTERN int Ntr_TestDecomp ARGS((DdManager *dd, BnetNetwork *net1, NtrOptions *option));
EXTERN int Ntr_VerifyEquivalence ARGS((DdManager *dd, BnetNetwork *net1, BnetNetwork *net2, NtrOptions *option));
EXTERN int Ntr_TestCofactorEstimate ARGS((DdManager * dd, BnetNetwork * net, NtrOptions * option));
EXTERN int Ntr_TestClipping ARGS((DdManager *dd, BnetNetwork *net1, BnetNetwork *net2, NtrOptions *option));
EXTERN int Ntr_maxflow ARGS((DdManager *dd, BnetNetwork *net, NtrOptions *option));
EXTERN double Ntr_maximum01Flow ARGS((DdManager *bdd, DdNode *sx, DdNode *ty, DdNode *E, DdNode **F, DdNode **cut, DdNode **x, DdNode **y, DdNode **z, int n, int pr));
EXTERN int Ntr_testZDD ARGS((DdManager *dd, BnetNetwork *net, NtrOptions *option));
EXTERN int Ntr_testISOP ARGS((DdManager *dd, BnetNetwork *net, NtrOptions *option));
EXTERN NtrHeap * Ntr_InitHeap ARGS((int size));
EXTERN void Ntr_FreeHeap ARGS((NtrHeap *heap));
EXTERN int Ntr_HeapInsert ARGS((NtrHeap *heap, void *item, int key));
EXTERN int Ntr_HeapExtractMin ARGS((NtrHeap *heap, void **item, int *key));
EXTERN int Ntr_HeapCount ARGS((NtrHeap *heap));
EXTERN NtrHeap * Ntr_HeapClone ARGS((NtrHeap *source));
EXTERN int Ntr_TestHeap ARGS((NtrHeap *heap, int i));

/**AutomaticEnd***************************************************************/

#endif /* _NTR */
