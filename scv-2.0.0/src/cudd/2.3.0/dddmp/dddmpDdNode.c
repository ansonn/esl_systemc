/**CFile**********************************************************************

  FileName    [dddmpDdNode.c]

  PackageName [dddmp]

  Synopsis    [Functions to handle BDD node infos and numbering]

  Description [Functions to handle BDD node infos and numbering
              ]

  Author      [Gianpiero Cabodi]

  Copyright   [Politecnico di Torino(Italy)]

******************************************************************************/

#include "dddmpInt.h"


/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void RemoveFromUniqueRecur(DdManager *dd, DdNode *f);
static void RestoreInUniqueRecur(DdManager *dd, DdNode *f);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************
  Synopsis    [Removes nodes from unique table and number them]
  Description [Node numbering is required to convert pointers to integers.
               Since nodes are removed from unique, no new nodes shoulld be 
               generated before re-inserting nodes in unique
               (DddmpUnnumberDdNodes()).
              ]
  SideEffects [Nodes are temporarily removed from unique table]
  SeeAlso     [RemoveFromUniqueRecur(), NumberNodeRecur(), 
               DddmpUnnumberDdNodes()]
******************************************************************************/
int
DddmpNumberDdNodes(
  DdManager *dd,  /* manager */
  DdNode **f,
  int n)
{
  int id=0, i;

  for (i=0; i<n; i++)
    RemoveFromUniqueRecur(dd,f[i]);
  for (i=0; i<n; i++)
    id = NumberNodeRecur(f[i],id);

  return id;
}


/**Function********************************************************************
  Synopsis    [Restores nodes in unique table, loosing numbering]
  Description [Node indexes are no more needed. Nodes are re-linked in the unique
               table.
              ]
  SideEffects [None]
  SeeAlso     [ DddmpNumberDdNode() ]
******************************************************************************/
void
DddmpUnnumberDdNodes(
  DdManager *dd,
  DdNode **f,
  int n )
{
  int i;

  for (i=0; i<n; i++)
    RestoreInUniqueRecur(dd,f[i]);
}


/**Function********************************************************************
  Synopsis    [Write index to node]
  Description [The index of the node is written in the "next" field of
               a DdNode struct. LSB is not used (set to 0). It is used as 
               "visited" flag in DD traversals.
              ]
  SideEffects [None]
  SeeAlso     [DddmpReadNodeIndex(), DddmpSetVisited(), DddmpVisited()]
******************************************************************************/
void 
DddmpWriteNodeIndex(
  DdNode *f,
  int id )
{
  f->next = (struct DdNode *)((ptruint)((id)<<1));
}


/**Function********************************************************************
  Synopsis    [Reads the index of a node]
  Description [Reads the index of a node. LSB is skipped (used as visited flag).
              ]
  SideEffects [None]
  SeeAlso     [DddmpWriteNodeIndex(), DddmpSetVisited(), DddmpVisited()]
******************************************************************************/
int
DddmpReadNodeIndex (
DdNode *f )
{
  return ((int)(((ptruint)(f->next))>>1));
}


/**Function********************************************************************
  Synopsis    [Returns true if node is visited]
  Description [Returns true if node is visited]
  SideEffects [None]
  SeeAlso     [DddmpSetVisited(), DddmpClearVisited()]
******************************************************************************/
int
DddmpVisited (
DdNode *f )
{
  f = Cudd_Regular(f);
  return ((int)((ptruint)(f->next)) & (01));
}    /* Mark node as visited. */


/**Function********************************************************************
  Synopsis    [Marks a node as visited]
  Description [Marks a node as visited]
  SideEffects [None]
  SeeAlso     [DddmpVisited(), DddmpClearVisited()]
******************************************************************************/
void
DddmpSetVisited(
DdNode *f )
{

  f = Cudd_Regular(f);

  f->next = (DdNode *)(ptruint)((int)((ptruint)(f->next))|01);

}


/**Function********************************************************************
  Synopsis    [Marks a node as not visited]
  Description [Marks a node as not visited]
  SideEffects [None]
  SeeAlso     [DddmpVisited(), DddmpSetVisited()]
******************************************************************************/
void
DddmpClearVisited(
DdNode *f )
{

  f = Cudd_Regular(f);

  f->next = (DdNode *)(ptruint)((int)((ptruint)(f->next)) & (~01));

}

/**Function********************************************************************
  Synopsis    [Number nodes recursively in post-order]
  Description [Number nodes recursively in post-order. The "visited flag is 
               used with inverse polarity, because all nodes were set "visited"
               when removing them from unique. 
              ]
  SideEffects ["visited" flags are reset.]
  SeeAlso     []
******************************************************************************/
int
NumberNodeRecur(
  DdNode *f,
  int id )
{
  f = Cudd_Regular(f);

  if (!DddmpVisited(f))
    return (id);

  if (!cuddIsConstant(f)) {
    id = NumberNodeRecur(cuddT(f),id);
    id = NumberNodeRecur(cuddE(f),id);
  }

  DddmpWriteNodeIndex(f,++id);
  DddmpClearVisited(f);

  return (id);
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************
  Synopsis    [Removes a node from unique table]
  Description [Removes a node from the unique table by locating the proper
subtable and unlinking the node from it. It recurs on son nodes]
  SideEffects [Nodes are left with the "visited" flag true.]
  SeeAlso     [RestoreInUniqueRecur()]
******************************************************************************/
static void
RemoveFromUniqueRecur (
  DdManager *dd, 
  DdNode *f )
{
  DdNode *node, *last, *next;

  DdNode *sentinel = &(dd->sentinel);
  DdNodePtr *nodelist;

  DdSubtable *subtable;
  int pos, level;

  f = Cudd_Regular(f);

  if (DddmpVisited(f))
    return;

  if (!cuddIsConstant(f)) {

    RemoveFromUniqueRecur(dd,cuddT(f));
    RemoveFromUniqueRecur(dd,cuddE(f));

    level = dd->perm[f->index];
    subtable = &(dd->subtables[level]);

    nodelist = subtable->nodelist;

    pos = ddHash(cuddT(f), cuddE(f), subtable->shift);
    node = nodelist[pos];
    last = NULL;
    while (node != sentinel) {
      next = node->next;
      if (node == f) {
        if (last != NULL)
  	  last->next = next;
        else 
          nodelist[pos] = next;
        break;
      }
      else {
        last = node;
        node = next;
      }
    }

    f->next = NULL;

  }

  DddmpSetVisited(f);

}

/**Function********************************************************************
  Synopsis    [Restores a node in unique table]
  Description [Restores a node in unique table (recursive)]
  SideEffects [Nodes are not restored in the same order as before removal]
  SeeAlso     [RemoveFromUnique()]
******************************************************************************/
static void
RestoreInUniqueRecur (
  DdManager *dd,
  DdNode *f )
{
#ifdef DDDMP_DEBUG
  DdNode *node;
#endif

  DdNodePtr *nodelist;
#ifdef DDDMP_DEBUG
  DdNode *sentinel = &(dd->sentinel);
#endif
  DdNode *T, *E, *looking;
  DdNodePtr *previousP;

  DdSubtable *subtable;
  int pos, level;

  f = Cudd_Regular(f);

  if (!Cudd_IsComplement(f->next))
    return;

  if (cuddIsConstant(f)) {
    f->next = NULL;
    return;
  }

  RestoreInUniqueRecur(dd,cuddT(f));
  RestoreInUniqueRecur(dd,cuddE(f));

  level = dd->perm[f->index];
  subtable = &(dd->subtables[level]);

  nodelist = subtable->nodelist;

  pos = ddHash(cuddT(f), cuddE(f), subtable->shift);

#ifdef DDDMP_DEBUG
  /* verify uniqueness to avoid duplicate nodes in unique table */
  for (node=nodelist[pos]; node != sentinel; node=node->next)
    assert(node!=f);
#endif

  T = cuddT(f);
  E = cuddE(f);
  previousP = &(nodelist[pos]);
  looking = *previousP;

  while (T < cuddT(looking)) {
    previousP = &(looking->next);
    looking = *previousP;
  }
  while (T == cuddT(looking) && E < cuddE(looking)) {
    previousP = &(looking->next);
    looking = *previousP;
  }
  f->next = *previousP;
  *previousP = f;

}

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void RemoveFromUniqueRecur(DdManager *dd, DdNode *f);
static void RestoreInUniqueRecur(DdManager *dd, DdNode *f);

