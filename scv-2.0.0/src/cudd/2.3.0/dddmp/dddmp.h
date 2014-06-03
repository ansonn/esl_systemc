/**CHeaderFile*****************************************************************

  FileName    [dddmp.h]

  PackageName [dddmp]

  Synopsis    [Functions to read in and write out bdds to file]

  Description []

  Author      [Gianpiero Cabodi & Stefano Quer]

  Copyright   [Politecnico di Torino(Italy)]
******************************************************************************/


#ifndef _DDDMP
#define _DDDMP

#include "util.h"
#include "cudd.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/* 
 * Dddmp format version 
 */

#define DDDMP_VERSION           "DDDMP-1.0"

/* 
 * format modes for dd files
 */

#define DDDMP_MODE_TEXT           ((int)'A')
#define DDDMP_MODE_BINARY         ((int)'B')
#define DDDMP_MODE_DEFAULT        ((int)'D')


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Enum************************************************************************

  Synopsis    [Type for variable extra info.]

  Description [Type for variable extra info. Used to specify info stored
  in text mode.]

******************************************************************************/
typedef enum {
    DDDMP_VARIDS,
    DDDMP_VARPERMIDS,
    DDDMP_VARAUXIDS,
    DDDMP_VARNAMES,
    DDDMP_VARDEFAULT
} Dddmp_VarInfoType;


/**Enum************************************************************************

  Synopsis    [Type for variable matching in BDD load.]

  Description []

******************************************************************************/
typedef enum {
    DDDMP_VAR_MATCHIDS,
    DDDMP_VAR_MATCHPERMIDS,
    DDDMP_VAR_MATCHAUXIDS,
    DDDMP_VAR_MATCHNAMES,
    DDDMP_VAR_COMPOSEIDS
} Dddmp_VarMatchType;


/**Enum************************************************************************

  Synopsis    [Type for BDD root matching in BDD load.]

  Description []

******************************************************************************/
typedef enum {
    DDDMP_ROOT_MATCHNAMES,
    DDDMP_ROOT_MATCHLIST
} Dddmp_RootMatchType;

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN int Dddmp_Text2Bin(char *filein, char *fileout);
EXTERN int Dddmp_Bin2Text(char *filein, char *fileout);
EXTERN int Dddmp_cuddBddDisplayBinary(char *filein, char *fileout);
EXTERN int Dddmp_cuddBddStore(DdManager *dd, char *ddname, DdNode *f, char **varnames, int *auxids, int mode, Dddmp_VarInfoType varinfo, char *fname, FILE *fp);
EXTERN int Dddmp_cuddBddArrayStore(DdManager *dd, char *ddname, int nroots, DdNode **f, char **rootnames, char **varnames, int *auxids, int mode, Dddmp_VarInfoType varinfo, char *fname, FILE *fp);
EXTERN DdNode * Dddmp_cuddBddLoad(DdManager *dd, Dddmp_VarMatchType varmatchmode, char **varmatchnames, int *varmatchauxids, int *varcomposeids, int mode, char *file, FILE *fp);
EXTERN int Dddmp_cuddBddArrayLoad(DdManager *dd, Dddmp_RootMatchType rootmatchmode, char **rootmatchnames, Dddmp_VarMatchType varmatchmode, char **varmatchnames, int *varmatchauxids, int *varcomposeids, int mode, char *file, FILE *fp, DdNode ***pproots);

/**AutomaticEnd***************************************************************/

#endif
