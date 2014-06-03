/**CFile**********************************************************************

  FileName    [dddmpConvert.c]

  PackageName [dddmp]

  Synopsis    [Conversion between ASCII and binary formats]

  Description [Conversion between ASCII and binary formats is presently 
               supported by loading a BDD in the source format and storing it 
               in the target one. We plan to introduce ad hoc procedures
               avoiding explicit BDD node generation.
              ]

  Author      [Gianpiero Cabodi and Stefano Quer]

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


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************
  Synopsis    [Converts from ASCII to binary format]
  Description [Converts from ASCII to binary format. A BDD array is loaded and
               and stored to the target file.]
  SideEffects [None]
  SeeAlso     [Dddmp_Bin2Text()]
******************************************************************************/
int
Dddmp_Text2Bin (
  char *filein,
  char *fileout
)
{
  DdManager *dd;      /* pointer to DD manager */
  DdNode **roots;        /* array of BDD roots to be loaded */
  int    nroots;         /* number of BDD roots */
 
  dd = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
  if (dd == NULL)
    return 0;

  nroots = Dddmp_cuddBddArrayLoad(dd,DDDMP_ROOT_MATCHLIST,NULL,
             DDDMP_VAR_MATCHIDS,NULL,NULL,NULL,
             DDDMP_MODE_TEXT,filein,NULL,&roots);

  if (nroots<=0)
    goto failure;

  if (Dddmp_cuddBddArrayStore (dd,NULL,nroots,roots,NULL,
      NULL,NULL,DDDMP_MODE_BINARY,DDDMP_VARIDS,fileout,NULL) <= 0)
    goto failure;
   
  Cudd_Quit(dd);
  return 1;

  failure:
    printf("error converting BDD format\n");
    Cudd_Quit(dd);
    return 0;

}

/**Function********************************************************************
  Synopsis    [Converts from binary to ASCII format]
  Description [Converts from binary to ASCII format. A BDD array is loaded and
               and stored to the target file.]
  SideEffects [None]
  SeeAlso     [Dddmp_Text2Bin()]
******************************************************************************/
int
Dddmp_Bin2Text (
  char *filein,
  char *fileout
)
{
  DdManager *dd;      /* pointer to DD manager */
  DdNode **roots;        /* array of BDD roots to be loaded */
  int    nroots;         /* number of BDD roots */
 
  dd = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
  if (dd == NULL)
    return 0;

  nroots = Dddmp_cuddBddArrayLoad(dd,DDDMP_ROOT_MATCHLIST,NULL,
             DDDMP_VAR_MATCHIDS,NULL,NULL,NULL,
             DDDMP_MODE_BINARY,filein,NULL,&roots);

  if (nroots<=0)
    goto failure;

  if (Dddmp_cuddBddArrayStore (dd,NULL,nroots,roots,NULL,
      NULL,NULL,DDDMP_MODE_TEXT,DDDMP_VARIDS,fileout,NULL) <= 0)
    goto failure;
   
  Cudd_Quit(dd);
  return 1;

  failure:
    printf("error converting BDD format\n");
    Cudd_Quit(dd);
    return 0;

}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


