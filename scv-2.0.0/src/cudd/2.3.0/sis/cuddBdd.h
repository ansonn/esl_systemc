/**CHeaderFile*****************************************************************

  FileName    [cuddBdd.h]

  PackageName [cudd]

  Synopsis    [Defines interface for the CU package to work with the
  ucb interface.]

  Description []

  Author      [Abelardo Pardo]

  Copyright   [Copyright (c) 1994-1996 The Univ. of Colorado.
  All rights reserved.

  Permission is hereby granted, without written agreement and without license
  or royalty fees, to use, copy, modify, and distribute this software and its
  documentation for any purpose, provided that the above copyright notice and
  the following two paragraphs appear in all copies of this software.

  IN NO EVENT SHALL THE UNIVERSITY OF COLORADO BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
  COLORADO HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  THE UNIVERSITY OF COLORADO SPECIFICALLY DISCLAIMS ANY WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
  "AS IS" BASIS, AND THE UNIVERSITY OF COLORADO HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.]

  Revision    [$Id: cuddBdd.h,v 1.1 2002/09/17 21:59:11 dlm Exp $]

******************************************************************************/

#ifndef _BDD
#define _BDD

/*---------------------------------------------------------------------------*/
/* Nested includes                                                           */
/*---------------------------------------------------------------------------*/

#include "var_set.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

#define boolean		int
/*
 *    foreach macro in the most misesque tradition
 *    bdd_gen_free always returns 0
 *
 *    CAUTION: in the context of the port to the CUDD package, it is assumed that
 *    dynamic reordering will not occur while there are open generators.  It is 
 *    the user's responsibility to make sure dynamic reordering doesn't occur.
 *    As long as new nodes are not created during generation, and you don't 
 *    explicitly call dynamic reordering, you should be okay.
 */

/*
 *    foreach_bdd_cube(fn, gen, cube)
 *    bdd_t *fn;
 *    bdd_gen *gen;
 *    array_t *cube;	- return
 *
 *    foreach_bdd_cube(fn, gen, cube) {
 *        ...
 *    }
 */
#define foreach_bdd_cube(fn, gen, cube)\
  for((gen) = bdd_first_cube(fn, &cube);\
      bdd_is_gen_empty(gen) ? bdd_gen_free(gen) : TRUE;\
      (void) bdd_next_cube(gen, &cube))

/*
 *    foreach_bdd_node(fn, gen, node)
 *    bdd_t *fn;
 *    bdd_gen *gen;
 *    bdd_node *node;	- return
 */
#define foreach_bdd_node(fn, gen, node)\
  for((gen) = bdd_first_node(fn, &node);\
      bdd_is_gen_empty(gen) ? bdd_gen_free(gen) : TRUE;\
      (void) bdd_next_node(gen, &node))

/*
 * Default settings.
 */
#define BDD_NO_LIMIT                      ((1<<30)-2)
#define BDD_DFLT_ITE_ON                   TRUE
#define BDD_DFLT_ITE_RESIZE_AT            75
#define BDD_DFLT_ITE_MAX_SIZE             1000000
#define BDD_DFLT_ITE_CONST_ON             TRUE
#define BDD_DFLT_ITE_CONST_RESIZE_AT      75
#define BDD_DFLT_ITE_CONST_MAX_SIZE       1000000
#define BDD_DFLT_ADHOC_ON                 TRUE
#define BDD_DFLT_ADHOC_RESIZE_AT          0
#define BDD_DFLT_ADHOC_MAX_SIZE           10000000
#define BDD_DFLT_GARB_COLLECT_ON          TRUE
#define BDD_DFLT_DAEMON                   NIL(void)
#define BDD_DFLT_MEMORY_LIMIT             BDD_NO_LIMIT   
#define BDD_DFLT_NODE_RATIO               2.0
#define BDD_DFLT_INIT_BLOCKS              10


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef struct DdManager bdd_manager;	        /* referenced via a pointer only */
typedef unsigned int bdd_variableId;		/* the id of the variable in a bdd node */
typedef struct DdNode bdd_node;                 /* referenced via a pointer only */
typedef int bdd_literal;	                /* integers in the set { 0, 1, 2 } */

/* This is to avoid problems with the mnemosyne library, which redefines
** free.
*/
#ifdef MNEMOSYNE
#undef free
#endif

typedef struct bdd_t {
    boolean free;	                        /* TRUE if this is free, FALSE otherwise ... */
    bdd_node *node;                             /* ptr to the top node of the function */
    bdd_manager *mgr;                           /* the manager */
} bdd_t;	

/* 
 * Initialization data structure.   Not supported in CMU package.
 */
typedef struct bdd_mgr_init {
    struct {
        boolean on;                   /* TRUE/FALSE: is the cache on */
        unsigned int resize_at;       /* percentage at which to resize (e.g. 85% is 85); doesn't apply to adhoc */
        unsigned int max_size;        /* max allowable number of buckets; for adhoc, max allowable number of entries */
    } ITE_cache,
      ITE_const_cache,
      adhoc_cache;
    struct {
      boolean on;                     /* TRUE/FALSE: is the garbage collector on */
    } garbage_collector;
    struct {
      void (*daemon)();               /* used for callback when memory limit exceeded */
      unsigned int limit;             /* upper bound on memory allocated by the manager; in megabytes */
    } memory;
    struct {
      float ratio;                    /* allocate new bdd_nodes to achieve ratio of used to unused nodes */
      unsigned int init_blocks;       /* number of bdd_nodeBlocks initially allocated */
    } nodes;
} bdd_mgr_init;

/*
 * Match types for BDD minimization.
 */
typedef enum {
    BDD_MIN_TSM,		/* two-side match */
    BDD_MIN_OSM,		/* one-side match */
    BDD_MIN_OSDM		/* one-side DC match */
} bdd_min_match_type_t;

/*
 * Statistics and Other Queries
 */
typedef struct bdd_cache_stats {
    unsigned int hits;
    unsigned int misses;
    unsigned int collisions;
    unsigned int inserts;
} bdd_cache_stats;

typedef struct bdd_stats {
    struct {
	bdd_cache_stats hashtable;   /* the unique table; collisions and inserts fields not used */ 
	bdd_cache_stats itetable;
	bdd_cache_stats consttable;
	bdd_cache_stats adhoc;
    } cache;		/* various cache statistics */
    struct {
	unsigned int calls;
	struct {
	    unsigned int trivial;
	    unsigned int cached;
	    unsigned int full;
	} returns;
    } ITE_ops,
      ITE_constant_ops,
      adhoc_ops;
    struct {
	unsigned int total;
    } blocks;		/* bdd_nodeBlock count */
    struct {
	unsigned int used;
	unsigned int unused;
	unsigned int total;
        unsigned int peak;
    } nodes;		/* bdd_node count */
    struct {
	unsigned int used;
	unsigned int unused;
	unsigned int total;
        unsigned int blocks; 
    } extptrs;		/* bdd_t count */
    struct {
	unsigned int times;     /* the number of times the garbage-collector has run */
        unsigned int nodes_collected; /* cumulative number of nodes collected over life of manager */
        long runtime;           /* cumulative CPU time spent garbage collecting */
    } gc;
    struct {
        int first_sbrk;         /* value of sbrk at start of manager; used to analyze memory usage */
        int last_sbrk;          /* value of last sbrk (see "man sbrk") fetched; used to analyze memory usage */
        unsigned int manager;
	unsigned int nodes;
	unsigned int hashtable;
	unsigned int ext_ptrs;
	unsigned int ITE_cache;
	unsigned int ITE_const_cache;
	unsigned int adhoc_cache;
	unsigned int total;
    } memory;           /* memory usage */
} bdd_stats;

/*
 * Traversal of BDD Formulas
 */

typedef struct bdd_gen bdd_gen;

/*
 *    These are the hooks for stuff that uses bdd's
 *
 *    There are three hooks, and users may use them in whatever
 *    way they wish; these hooks are guaranteed to never be used
 *    by the bdd package.
 */
typedef struct bdd_external_hooks {
    char *network;
    char *mdd;
    char *undef1;
} bdd_external_hooks;

/*
 * Dynamic reordering.
 */
typedef enum {
    BDD_REORDER_SIFT,
    BDD_REORDER_WINDOW,
    BDD_REORDER_NONE,
    BDD_REORDER_SAME,
    BDD_REORDER_RANDOM,
    BDD_REORDER_RANDOM_PIVOT,
    BDD_REORDER_SIFT_CONVERGE,
    BDD_REORDER_SYMM_SIFT,
    BDD_REORDER_SYMM_SIFT_CONV,
    BDD_REORDER_WINDOW2,
    BDD_REORDER_WINDOW3,
    BDD_REORDER_WINDOW4,
    BDD_REORDER_WINDOW2_CONV,
    BDD_REORDER_WINDOW3_CONV,
    BDD_REORDER_WINDOW4_CONV,
    BDD_REORDER_GROUP_SIFT,
    BDD_REORDER_GROUP_SIFT_CONV,
    BDD_REORDER_ANNEALING,
    BDD_REORDER_GENETIC
} bdd_reorder_type_t;


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*
 * BDD Manager Allocation And Destruction
 */
EXTERN void bdd_end ARGS((bdd_manager *));
EXTERN void bdd_register_daemon ARGS((bdd_manager *, void (*daemon)()));
EXTERN void bdd_set_mgr_init_dflts ARGS((bdd_mgr_init *));
EXTERN bdd_manager *bdd_start ARGS((int));
EXTERN bdd_manager *bdd_start_with_params ARGS((int, bdd_mgr_init *));

/*
 * BDD Variable Allocation
 */
EXTERN bdd_t *bdd_create_variable ARGS((bdd_manager *));		
EXTERN bdd_t *bdd_get_variable ARGS((bdd_manager *, bdd_variableId));	

/*
 * BDD Formula Management
 */
EXTERN bdd_t *bdd_dup ARGS((bdd_t *));
EXTERN void bdd_free ARGS((bdd_t *));

/* 
 * Operations on BDD Formulas
 */
EXTERN bdd_t *bdd_and ARGS((bdd_t *, bdd_t *, boolean, boolean));
EXTERN bdd_t *bdd_and_smooth ARGS((bdd_t *, bdd_t *, array_t *));
EXTERN bdd_t *bdd_between ARGS((bdd_t *, bdd_t *));
EXTERN bdd_t *bdd_cofactor ARGS((bdd_t *, bdd_t *));
EXTERN bdd_t *bdd_compose ARGS((bdd_t *, bdd_t *, bdd_t *));
EXTERN bdd_t *bdd_consensus ARGS((bdd_t *, array_t *));
EXTERN bdd_t *bdd_cproject ARGS((bdd_t *, array_t *));
EXTERN bdd_t *bdd_else ARGS((bdd_t *));
EXTERN bdd_t *bdd_ite ARGS((bdd_t *, bdd_t *, bdd_t *, boolean, boolean, boolean));
EXTERN bdd_t *bdd_minimize ARGS((bdd_t *, bdd_t *));
EXTERN bdd_t *bdd_minimize_with_params ARGS((bdd_t *, bdd_t *, bdd_min_match_type_t, boolean, boolean, boolean));
EXTERN bdd_t *bdd_not ARGS((bdd_t *));
EXTERN bdd_t *bdd_one ARGS((bdd_manager *));
EXTERN bdd_t *bdd_or ARGS((bdd_t *, bdd_t *, boolean, boolean));
EXTERN bdd_t *bdd_smooth ARGS((bdd_t *, array_t *));
EXTERN bdd_t *bdd_substitute ARGS((bdd_t *, array_t *, array_t *));
EXTERN bdd_t *bdd_then ARGS((bdd_t *));
EXTERN bdd_t *bdd_top_var ARGS((bdd_t *));
EXTERN bdd_t *bdd_xnor ARGS((bdd_t *, bdd_t *));
EXTERN bdd_t *bdd_xor ARGS((bdd_t *, bdd_t *));
EXTERN bdd_t *bdd_zero ARGS((bdd_manager *));

/*
 * Queries about BDD Formulas
 */
EXTERN boolean bdd_equal ARGS((bdd_t *, bdd_t *));
EXTERN boolean bdd_is_cube ARGS((bdd_t *));
EXTERN boolean bdd_is_tautology ARGS((bdd_t *, boolean));
EXTERN boolean bdd_leq ARGS((bdd_t *, bdd_t *, boolean, boolean));

EXTERN double bdd_count_onset ARGS((bdd_t *, array_t *));
EXTERN bdd_manager *bdd_get_manager ARGS((bdd_t *));
EXTERN bdd_node *bdd_get_node ARGS((bdd_t *, boolean *));
EXTERN void bdd_get_stats ARGS((bdd_manager *, bdd_stats *));
EXTERN var_set_t *bdd_get_support ARGS((bdd_t *));
EXTERN array_t *bdd_get_varids ARGS((array_t *));
EXTERN unsigned int bdd_num_vars ARGS((bdd_manager *));
EXTERN void bdd_print ARGS((bdd_t *));
EXTERN void bdd_print_stats ARGS((bdd_stats, FILE *));
EXTERN int bdd_size ARGS((bdd_t *));
EXTERN bdd_variableId bdd_top_var_id ARGS((bdd_t *));
EXTERN bdd_t *bdd_create_variable_after ARGS((bdd_manager *, bdd_variableId));
EXTERN bdd_variableId bdd_get_id_from_level ARGS((bdd_manager *, long));
EXTERN long bdd_top_var_level ARGS((bdd_manager *, bdd_t *));

EXTERN int bdd_gen_free ARGS((bdd_gen *));

/*
 * These are NOT to be used directly; only indirectly in the macros.
 */
EXTERN bdd_gen *bdd_first_cube ARGS((bdd_t *, array_t **));
EXTERN boolean bdd_next_cube ARGS((bdd_gen *, array_t **));
EXTERN bdd_gen *bdd_first_node ARGS((bdd_t *, bdd_node **));
EXTERN boolean bdd_next_node ARGS((bdd_gen *, bdd_node **));
EXTERN boolean bdd_is_gen_empty ARGS((bdd_gen *));

/* 
 * Miscellaneous
 */
EXTERN void bdd_set_gc_mode ARGS((bdd_manager *, boolean));

EXTERN bdd_external_hooks *bdd_get_external_hooks ARGS((bdd_manager *));

EXTERN void bdd_dynamic_reordering ARGS((bdd_manager *, bdd_reorder_type_t));

EXTERN int bdd_read_reordering_flag ARGS((bdd_manager *));

#endif /* _BDD */
