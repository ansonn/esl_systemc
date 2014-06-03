/**CFile**********************************************************************

  FileName    [dddmpDump.c]

  PackageName [dddmp]

  Synopsis    [Functions to read in and write out bdds to file]

  Description [Functions to read in and write out bdds to file.  BDDs
  are represended on file either in text or binary format under the
  following rules.  A file contains a forest of BDDs (a vector of
  Boolean functions).  BDD nodes are numbered with contiguous numbers,
  from 1 to NNodes (total number of nodes on a file). 0 is not used to
  allow negative node indexes for complemented edges.  A file contains
  a header, including information about variables and roots to BDD
  functions, followed by the list of nodes.  BDD nodes are listed
  according to their numbering, and in the present implementation
  numbering follows a post-order strategy, in such a way that a node
  is never listed before its Then/Else children.  ]

  Author      [Gianpiero Cabodi & Stefano Quer]

  Copyright   [Politecnico di Torino (Italy)]

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

#define matchkeywd(str,key) (strncmp(str,key,strlen(key))==0)

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int StoreNodeRecur(DdManager *dd, DdNode *f, int mode, int *supportids, char **varnames, int *outids, FILE *fp);
static int QsortStrcmp(const void *ps1, const void *ps2);
static int FindVarname(char *name, char **array, int n);

/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Writes a dump file representing the argument BDD.]

  Description [Dumps the argument BDD to file. Dumping is done through
               Dddmp_cuddBddArrayStore, And a dummy array of 1 BDD root is
               used for this purpose.
               ]

  SideEffects [Nodes are temporarily removed from unique hash. They are 
re-linked after the store operation in a modified order.]

  SeeAlso     [Dddmp_cuddBddLoad Dddmp_cuddBddArrayLoad]

******************************************************************************/
int
Dddmp_cuddBddStore (
  DdManager *dd          /* manager */,
  char *ddname           /* dd name (or NULL) */,
  DdNode *f              /* BDD root to be stored */,
  char **varnames        /* array of variable names (or NULL) */,
  int *auxids            /* array of converted var ids */,
  int mode               /* storing mode selector */,
  Dddmp_VarInfoType varinfo /* extra info for variables in text mode */,
  char *fname            /* file name */,
  FILE *fp               /* pointer to the store file */ 
)
{
  DdNode *FArray[1];

  FArray[0] = f;
  return (Dddmp_cuddBddArrayStore (dd,ddname,1,FArray,NULL,
          varnames,auxids,mode,varinfo,fname,fp));

}

/**Function********************************************************************

  Synopsis    [Writes a dump file representing the argument Array of BDDs.]

  Description [Dumps the argument array of BDDs to file. Dumping is
  either in text or binary form.  BDDs are stored to the fp (already
  open) file if not NULL. Otherwise the file whose name is fname is opened
  in write mode.  The header has the same format for both textual and
  binary dump.  Names are allowed for input variables (vnames) and for
  represented functions (rnames).  For sake of generality and because
  of dynamic variable ordering both variable IDs and permuted IDs are
  included. New IDs are also supported (auxids).  Variables are identified with incremental       
  numbers. according with their positiom in the support set.
  In text mode, an extra info may be added, chosen among the following options:
  name, ID, PermID, or an auxiliary id.  Since conversion from DD pointers to integers is
  required, DD nodes are temporarily removed from the unique
  hash table. This allows the use of the next field to store node IDs.  ]

  SideEffects [Nodes are temporarily removed from the unique hash
  table. They are re-linked after the store operation in a modified
  order.]

  SeeAlso     [Dddmp_cuddBddStore, Dddmp_cuddBddLoad, Dddmp_cuddBddArrayLoad]

******************************************************************************/
int
Dddmp_cuddBddArrayStore (
  DdManager *dd             /* manager */,
  char *ddname              /* dd name (or NULL) */,
  int nroots                /* number of output BDD roots to be stored */,
  DdNode **f                /* array of BDD roots to be stored */,
  char **rootnames          /* array of root names (or NULL) */,
  char **varnames           /* array of variable names (or NULL) */,
  int *auxids               /* array of converted var IDs */,
  int mode                  /* storing mode selector */,
  Dddmp_VarInfoType varinfo /* extra info for variables in text mode */,
  char *fname               /* file name */,
  FILE *fp                  /* pointer to the store file */ 
)
{
  DdNode      *support = NULL;
  DdNode      *scan;
  int         *ids = NULL;
  int         *permids = NULL;
  int         *invpermids = NULL;
  int         *supportids = NULL;
  int         *outids = NULL;
  char        **outvarnames = NULL;
  int         nvars = dd->size;
  int         nnodes;
  int         retval;
  int         i, var;
  int         close_fp = 0;


  if (fp == NULL) {
    /* 
     * File needs to be opened in the proper mode.
     */
    fp = fopen (fname, "w");
    if (fp == NULL) {
      (void) fprintf (stdout,"DdStore: Error opening %s\n",fname);
      goto failure;
    }
    close_fp = 1;
  }
  /* 
   * Force binary mode if automatic.
   */
  switch (mode) {
    case DDDMP_MODE_TEXT:
    case DDDMP_MODE_BINARY:
         break;
    case DDDMP_MODE_DEFAULT:
         mode = DDDMP_MODE_BINARY;
         break;
  }

  /* 
   * Alloc vectors for variable IDs, perm IDs and support IDs.
   *  +1 to include a slot for terminals.
   */
  ids = DDDMP_ALLOC(int,nvars);
  permids = DDDMP_ALLOC(int,nvars);
  invpermids = DDDMP_ALLOC(int,nvars);
  supportids = DDDMP_ALLOC(int,nvars+1);
  if ((ids == NULL)||(permids == NULL)||
      (invpermids == NULL)||(supportids == NULL)) {
    (void) fprintf (stdout,"DdStore: Error allocating memory\n");
    goto failure;
  }
    
  for (i = 0; i < nvars; i++) {
    ids[i] = permids[i] = invpermids[i] = supportids[i] = -1;
  }

  /* 
   * Take the union of the supports of each output function.
   * skip NULL functions.
   * Set permids and invpermids of support variables to the proper values.
   */
  for (i=0; i < nroots; i++) {
    if (f[i] == NULL) continue;
    support = Cudd_Support(dd,f[i]);
    if (support == NULL) {
      (void) fprintf (stdout,"DdStore Error: NULL support returned\n");
      goto failure;
    }
    cuddRef(support);
    scan = support;
    while (!cuddIsConstant(scan)) {
	ids[scan->index] = scan->index;
	permids[scan->index] = dd->perm[scan->index];
      invpermids[dd->perm[scan->index]] = scan->index;
	scan = cuddT(scan);
    }
    Cudd_RecursiveDeref(dd,support);
  }
  support = NULL; /* so that we do not try to free it in case of failure */

  /*
   * Set supportids to incremental (shrinked) values following the ordering.
   */
  for (i=0, var=0; i < nvars; i++) {
    if (invpermids[i] >= 0)
      supportids[invpermids[i]] = var++;
  }
  supportids[nvars] = var; /* set a dummy id for terminal nodes */

  /*
   * select conversion array for extra var info
   */
  switch (mode) {
    case DDDMP_MODE_TEXT:
      switch (varinfo) {
        case DDDMP_VARIDS:
          outids = ids;
          break;
        case DDDMP_VARPERMIDS:
          outids = permids;
          break;
        case DDDMP_VARAUXIDS:
          outids = auxids;
          break;
        case DDDMP_VARNAMES:
          outvarnames = varnames;
          break;
        case DDDMP_VARDEFAULT:
          break;
      }
      break;
    case DDDMP_MODE_BINARY:
      outids = NULL;
      break;
  }

  /* 
   * number dd nodes and count them (numbering is from 1 to nnodes)
   */
  nnodes = DddmpNumberDdNodes(dd,f,nroots);

  /* 
   * START HEADER 
   */

#ifdef DDDMP_VERSION
  retval = fprintf(fp,".ver %s\n", DDDMP_VERSION);
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }
#endif

  retval = fprintf(fp,".mode %c\n", mode);
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  if (mode == DDDMP_MODE_TEXT) {
    retval = fprintf(fp,".varinfo %d\n", varinfo);
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }
  }

  if (ddname != NULL) {
    retval = fprintf(fp,".dd %s\n",ddname);
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }
  }

  retval = fprintf(fp,".nnodes %d\n", nnodes);
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  retval = fprintf(fp,".nvars %d\n", nvars);
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  retval = fprintf(fp,".nsuppvars %d\n", var);
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  if (varnames != NULL) {

    /* 
     * Write the var names by scanning the ids array.
     */

    retval = fprintf(fp,".varnames");
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }

    for (i = 0; i < nvars; i++) {
	if (ids[i] >= 0) {
        if (varnames[ids[i]] == NULL) {
          (void) fprintf (stdout,"DdStore Warning: null variable name. DUMMY%d generated\n",i);
          varnames[ids[i]] = DDDMP_ALLOC(char,10);
          if (varnames[ids[i]] == NULL) {
            (void) fprintf (stdout,"DdStore: Error allocating memory\n");
            goto failure;
          }
          sprintf(varnames[ids[i]],"DUMMY%d",i);
        }
        retval = fprintf(fp," %s", varnames[ids[i]]);
        if (retval == EOF) {
          (void) fprintf (stdout,"DdStore: Error writing to file\n");
          goto failure;
        }
	}
    }

    retval = fprintf(fp,"\n");
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }

  }

  /*
   * Write the var ids by scanning the ids array. 
   */
  retval = fprintf(fp,".ids");
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }
  for (i = 0; i < nvars; i++) {
    if (ids[i] >= 0) {
	retval = fprintf(fp," %d", i);
      if (retval == EOF) {
        (void) fprintf (stdout,"DdStore: Error writing to file\n");
        goto failure;
      }
    }
  }
  retval = fprintf(fp,"\n");
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  /*
   * Write the var permids by scanning the permids array. 
   */
  retval = fprintf(fp,".permids");
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }
  for (i = 0; i < nvars; i++) {
    if (permids[i] >= 0) {
      retval = fprintf(fp," %d", permids[i]);
      if (retval == EOF) {
        (void) fprintf (stdout,"DdStore: Error writing to file\n");
        goto failure;
      }
    }
  }
  retval = fprintf(fp,"\n");
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  if (auxids != NULL) {
  
    /*
     * Write the var auxids by scanning the ids array. 
     */
    retval = fprintf(fp,".auxids");
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }
    for (i = 0; i < nvars; i++) {
      if (ids[i] >= 0) {
        retval = fprintf(fp," %d", auxids[i]);
        if (retval == EOF) {
          (void) fprintf (stdout,"DdStore: Error writing to file\n");
          goto failure;
        }
      }
    }
    retval = fprintf(fp,"\n");
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }
  }

  /* 
   * Write the roots info. 
   */
  retval = fprintf(fp,".nroots %d\n", nroots);
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  if (rootnames != NULL) {

    /* 
     * Write the root names. 
     */

    retval = fprintf(fp,".rootnames");
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }
    for (i = 0; i < nroots; i++) {
      if (rootnames[i] == NULL) {
        (void) fprintf (stdout,"DdStore Warning: null variable name. ROOT%d generated\n",i);
        rootnames[i] = DDDMP_ALLOC(char,10);
        if (rootnames[i] == NULL) {
          (void) fprintf (stdout,"DdStore: Error allocating memory\n");
          goto failure;
        }
        sprintf(rootnames[ids[i]],"ROOT%d",i);
      }
      retval = fprintf(fp," %s", rootnames[i]);
      if (retval == EOF) {
        (void) fprintf (stdout,"DdStore: Error writing to file\n");
        goto failure;
      }
    }

    retval = fprintf(fp,"\n");
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }

  }

  retval = fprintf(fp,".rootids");
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  /* 
   * Write BDD indexes of function roots.
   * Use negative integers for complemented edges. 
   */

  for (i = 0; i < nroots; i++) {
    if (f[i] == NULL) {
      (void) fprintf (stdout,"DdStore Warning: %d-th root is NULL\n",i);
      retval = fprintf(fp," 0");
    }
    if (Cudd_IsComplement(f[i])) {
      retval = fprintf(fp," -%d", DddmpReadNodeIndex(Cudd_Regular(f[i])));
    } 
    else {
      retval = fprintf(fp," %d", DddmpReadNodeIndex(Cudd_Regular(f[i])));
    }
    if (retval == EOF) {
      (void) fprintf (stdout,"DdStore: Error writing to file\n");
      goto failure;
    }
  }

  retval = fprintf(fp,"\n");
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  retval = fprintf(fp,".nodes\n");
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  /* 
   * END HEADER
   */

  /* 
   * Call the function that really gets the job done.
   */

  for (i = 0; i < nroots; i++) {
    if (f[i] != NULL) {
      retval = StoreNodeRecur(dd,Cudd_Regular(f[i]),
                              mode,supportids,outvarnames,outids,fp);
      if (retval == 0) {
        (void) fprintf (stdout,"DdStore: Error in recursive node store\n");
        goto failure;
      }
    }
  }

  /* 
   * Write trailer and return.
   */

  retval = fprintf(fp,".end\n");
  if (retval == EOF) {
    (void) fprintf (stdout,"DdStore: Error writing to file\n");
    goto failure;
  }

  if (close_fp)
    fclose (fp);

  DddmpUnnumberDdNodes(dd,f,nroots);
  DDDMP_FREE(ids);
  DDDMP_FREE(permids);
  DDDMP_FREE(invpermids);
  DDDMP_FREE(supportids);

  return(1);

  failure:

  if (ids != NULL) DDDMP_FREE(ids);
  if (permids != NULL) DDDMP_FREE(permids);
  if (invpermids != NULL) DDDMP_FREE(invpermids);
  if (supportids != NULL) DDDMP_FREE(supportids);
  if (support != NULL) Cudd_RecursiveDeref(dd,support);

  return(0);

}


/**Function********************************************************************

  Synopsis    [Reads a dump file representing the argument BDD.]

  Description [Reads a dump file representing the argument BDD.
  Dddmp_cuddBddArrayLoad is used through a dummy array.  ]

  SideEffects [A vector of pointers to DD nodes is allocated and freed.]

  SeeAlso     [Dddmp_cuddBddStore, Dddmp_cuddBddArrayLoad]

******************************************************************************/
DdNode *
Dddmp_cuddBddLoad (
  DdManager *dd           /* manager */,
  Dddmp_VarMatchType varmatchmode /* storing mode selector */,
  char **varmatchnames    /* array of variable names (accessed by IDs) */,
  int  *varmatchauxids    /* array of variable auxids (accessed by IDs) */,
  int  *varcomposeids     /* array of new ids (accessed by ids) */,
  int mode      /* requested input file format (checked against file format)*/,
  char *file		  /* file name */,
  FILE *fp                /* file pointer */
)
{
  DdNode *f , **FArray;
  int i, nroots;

  nroots = Dddmp_cuddBddArrayLoad(dd,DDDMP_ROOT_MATCHLIST,NULL,
             varmatchmode,varmatchnames,varmatchauxids,varcomposeids,
             mode,file,fp,&FArray);

  if (nroots == 0)
    return (NULL);
  else {
    f = FArray[0];
    if (nroots > 1) {
      printf ("Warning: %d BDD roots found in file. Only first retrieved.\n",
              nroots);
      for (i=1; i<nroots; i++)
        Cudd_RecursiveDeref(dd,FArray[i]);
    } 
    DDDMP_FREE(FArray);
    return f;
  }

}

/**Function********************************************************************

  Synopsis    [Reads a dump file representing the argument BDDs.]

  Description [Reads a dump file representing the argument BDDs. The header is
  common to both text and binary mode. The node list is either 
  in text or binary format. A dynamic vector of DD pointers 
  is allocated to support conversion from DD indexes to pointers.
  Several criteria are supported for variable match between file
  and dd manager. Several changes/permutations/compositions are allowed
  for variables while loading DDs. Variable of the dd manager are allowed 
  to match with variables on file on ids, permids, varnames, 
  varauxids; also direct composition between ids and 
  composeids is supported. More in detail:
  <ol>
  <li> varmatchmode=DDDMP_VAR_MATCHIDS <p>
  allows the loading of a DD keeping variable IDs unchanged
  (regardless of the variable ordering of the reading manager); this
  is useful, for example, when swapping DDs to file and restoring them
  later from file, after possible variable reordering activations.
  
  <li> varmatchmode=DDDMP_VAR_MATCHPERMIDS <p>
  is used to allow variable match according to the position in the ordering.
  
  <li> varmatchmode=DDDMP_VAR_MATCHNAMES <p>
  requires a non NULL varmatchnames parameter; this is a vector of
  strings in one-to-one correspondence with variable IDs of the
  reading manager. Variables in the DD file read are matched with
  manager variables according to their name (a non NULL varnames
  parameter was required while storing the DD file).

  <li> varmatchmode=DDDMP_VAR_MATCHIDS <p>
  has a meaning similar to DDDMP_VAR_MATCHNAMES, but integer auxiliary
  IDs are used instead of strings; the additional non NULL
  varmatchauxids parameter is needed.

  <li> varmatchmode=DDDMP_VAR_COMPOSEIDS <p>
  uses the additional varcomposeids parameter is used as array of
  variable ids to be composed with ids stored in file.
  </ol>

  In the present implementation, the array varnames (3), varauxids (4)
  and composeids (5) need to have one entry for each variable in the 
  DD manager (NULL pointers are allowed for unused variables
  in varnames). Hence variables need to be already present in the 
  manager. All arrays are sorted according to IDs.
  ]

  SideEffects [A vector of pointers to DD nodes is allocated and freed.]

  SeeAlso     [Dddmp_cuddBddArrayStore]

******************************************************************************/
int
Dddmp_cuddBddArrayLoad (
  DdManager *dd           /* manager */,
  Dddmp_RootMatchType rootmatchmode /* storing mode selector */,
  char **rootmatchnames   /* sorted names for loaded roots */,
  Dddmp_VarMatchType varmatchmode /* storing mode selector */,
  char **varmatchnames    /* array of variable names (accessed by ids) */,
  int  *varmatchauxids    /* array of variable auxids (accessed by ids) */,
  int  *varcomposeids     /* array of new ids (accessed by ids) */,
  int mode                /* requested input file format (checked against file format)*/,
  char *file		  /* file name */,
  FILE *fp                /* file pointer */,
  DdNode ***pproots       /* array of returned BDD roots (by reference) */
)
{
DdNode *f, *T = NULL, *E = NULL;
struct binary_dd_code code;
char buf[DDDMP_MAXSTRLEN];
int varinfo;
int id, size, maxv;
int nnodes = 0, i, j, k, nsuppvars = 0, nroots = 0, maxaux, 
    var = 0, vT, vE, idT = 0, idE = 0;
int  *permsupport = NULL;
int  *ids = NULL;
int  *permids = NULL;
int  *auxids = NULL;
int  *convertids = NULL;
int  *invconvertids = NULL;
int  *rootids = NULL;
int  *invauxids = NULL;
char rmode[3];
char *ddname = NULL;
char **varnames = NULL;
char **sortedvarnames = NULL;
char **rootnames = NULL;
int  nvars = 0, nddvars;
DdNode **pnodes = NULL;
unsigned char *pvars1byte = NULL;
unsigned short *pvars2byte = NULL;
DdNode **proots = NULL;       /* array of BDD roots to be loaded */
int close_fp = 0;

  *pproots = NULL;

  if (fp == NULL) {
    fp = fopen (file, "r");
    if (fp == NULL) {
      (void) fprintf (stdout,"DdLoad: Error opening %s\n",file);
      goto failure;
    }
    close_fp = 1;
  }

  nddvars = dd->size;

  /* START HEADER */

  while (fscanf(fp,"%s",buf)!=EOF) {

    /* comment */
    if (buf[0] == '#') {
      fgets(buf,DDDMP_MAXSTRLEN,fp);
      continue;
    }

    if (buf[0] != '.') {
      (void) fprintf (stdout,"DdLoad Error at\n%s\n", buf);
      (void) fprintf (stdout,"line must begin with '.' or '#'\n");
      goto failure;
    }

    if matchkeywd(buf, ".ver") {    
      /* this not checked so far: only read */
      if (fscanf (fp, "%*s")==EOF) {
        (void) fprintf (stdout,"DdLoad: Error reading file - EOF found\n");
        goto failure;
      }
      continue;
    }

    if matchkeywd(buf, ".mode") {    
      if (fscanf (fp, "%s", rmode)==EOF) {
        (void) fprintf (stdout,"DdLoad: Error reading file - EOF found\n");
        goto failure;
      }
      if (rmode[0] != mode) {
        if (mode == DDDMP_MODE_DEFAULT) {
          mode = rmode[0];
        }
        else {
          (void) fprintf (stdout,"DdLoad Error: mode mismatch\n");
          goto failure;
        }
      }
      continue;
    }
    if matchkeywd(buf, ".varinfo") {    
      if (fscanf (fp, "%d", &varinfo)==EOF) {
        (void) fprintf (stdout,"DdLoad: Error reading file - EOF found\n");
        goto failure;
      }
      continue;
    }
    if matchkeywd(buf, ".dd") {    
      if (fscanf (fp, "%s", buf)==EOF) {
        (void) fprintf (stdout,"DdLoad: Error reading file - EOF found\n");
        goto failure;
      }
      ddname = DDDMP_ALLOC(char,sizeof(buf)+1);
      strcpy(ddname,buf);
      continue;
    }
    if matchkeywd(buf, ".nnodes") {
      if (fscanf (fp, "%d", &nnodes)==EOF) {
        (void) fprintf (stdout,"DdLoad: Error reading file - EOF found\n");
        goto failure;
      }
      continue;
    }
    if matchkeywd(buf, ".nvars") {   
      if (fscanf (fp, "%d", &nvars)==EOF) {
        (void) fprintf (stdout,"DdLoad: Error reading file - EOF found\n");
        goto failure;
      }
      if (dd->size != nvars) {
        printf ("Warning: total number of dd manager vars doesn't match with writing manager\n");
        printf ("DDM: %d / BDD: %d\n", dd->size, nvars);
      }
      continue;
    }
    if matchkeywd(buf, ".nsuppvars") {
      if (fscanf (fp, "%d", &nsuppvars)==EOF) {
        (void) fprintf (stdout,"DdLoad: Error reading file - EOF found\n");
        goto failure;
      }
      continue;
    }

    if matchkeywd(buf, ".varnames") {
      varnames = DDDMP_ALLOC(char *,nsuppvars);
      if (varnames == NULL) {
        (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
        goto failure;
      }
      for (i=0; i < nsuppvars; i++) { 
        if (fscanf (fp, "%s", buf)==EOF) {
          (void) fprintf (stdout,"DdLoad: Error reading file - EOF found\n");
          goto failure;
        }
        varnames[i] = DDDMP_ALLOC(char,strlen(buf)+1);
        if (varnames[i] == NULL) {
          (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
          goto failure;
        }
        strcpy(varnames[i],buf);
      }
      continue;
    }

    if matchkeywd(buf, ".ids") {
      ids = DDDMP_ALLOC(int,nsuppvars);
      if (ids == NULL) {
        (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
        goto failure;
      }
      for (i=0; i < nsuppvars; i++) { 
        if (fscanf (fp, "%d", &ids[i])==EOF) {
          (void) fprintf (stdout,"DdLoad: Error reading file\n");
          goto failure;
        }
      }
      continue;
    }

    if matchkeywd(buf, ".permids") {
      permids = DDDMP_ALLOC(int,nsuppvars);
      if (permids == NULL) {
        (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
        goto failure;
      }
      for (i=0; i < nsuppvars; i++) { 
        if (fscanf (fp, "%d", &permids[i])==EOF) {
          (void) fprintf (stdout,"DdLoad: Error reading file\n");
          goto failure;
        }
      }
      continue;
    }

    if matchkeywd(buf, ".auxids") {
      auxids = DDDMP_ALLOC(int,nsuppvars);
      if (auxids == NULL) {
        (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
        goto failure;
      }
      for (i=0; i < nsuppvars; i++) { 
        if (fscanf (fp, "%d", &auxids[i])==EOF) {
          (void) fprintf (stdout,"DdLoad: Error reading file\n");
          goto failure;
        }
      }
      continue;
    }

    if matchkeywd(buf, ".nroots") {
      if (fscanf (fp, "%d", &nroots)==EOF) {
        (void) fprintf (stdout,"DdLoad: Error reading file\n");
        goto failure;
      }
      continue;
    }

    if matchkeywd(buf, ".rootids") {
      rootids = DDDMP_ALLOC(int,nroots);
      if (rootids == NULL) {
        (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
        goto failure;
      }
      for (i=0; i < nroots; i++) {
        if (fscanf (fp, "%d", &rootids[i])==EOF) {
          (void) fprintf (stdout,"DdLoad: Error reading file\n");
          goto failure;
        }
      }
      continue;
    }

    if matchkeywd(buf, ".rootnames") {
      rootnames = DDDMP_ALLOC(char *,nroots);
      if (rootnames == NULL) {
        (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
        goto failure;
      }
      for (i=0; i < nroots; i++) { 
        if (fscanf (fp, "%s", buf)==EOF) {
          (void) fprintf (stdout,"DdLoad: Error reading file\n");
          goto failure;
        }
        rootnames[i] = DDDMP_ALLOC(char,strlen(buf)+1);
        if (rootnames[i] == NULL) {
          (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
          goto failure;
        }
        strcpy(rootnames[i],buf);
      }
      continue;
    }

    if matchkeywd(buf, ".nodes") {
      if (fgets(buf,999,fp)== NULL) {
        (void) fprintf (stdout,"DdLoad: Error reading file\n");
        goto failure;
      }
      break;
    }

  }

  /* END HEADER */

  /*
   * for each variable in the support, the relative position in the ordering
   * (within the support only) is computed
   */

  permsupport = DDDMP_ALLOC(int,nsuppvars);
  if (permsupport == NULL) {
    (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
    goto failure;
  }
  for (i=0,k=0; i < nvars; i++) { 
    for (j=0; j < nsuppvars; j++) { 
      if (permids[j] == i) {
        permsupport[j] = k++;
      }
    }
  }
  assert (k==nsuppvars);

  if (varnames != NULL) {
    /*
     *  Varnames are sorted for binary search
     */
    sortedvarnames = DDDMP_ALLOC(char *,nsuppvars);
    if (sortedvarnames == NULL) {
      (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
      goto failure;
    }
    for (i=0; i < nsuppvars; i++) {
      if (varnames[i] == NULL) {
        (void) fprintf (stdout,"DdLoad Error: support variable name missing in file\n");
        goto failure;
      } 
      sortedvarnames[i] = varnames[i];
    }    
    
    qsort((void *)sortedvarnames,nsuppvars,sizeof(char *),QsortStrcmp);
    
  }

  /*
   * convertids is the array used to vonvert variable ids from positional (shrinked)
   * ids used within the DD file. Positions in the file are from 0 to nsuppvars-1.
   */ 

  convertids = DDDMP_ALLOC(int,nsuppvars);
  if (convertids == NULL) {
    (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
    goto failure;
  }

  again_matchmode:
  switch (varmatchmode) {
    case DDDMP_VAR_MATCHIDS:
      for (i=0; i<nsuppvars; i++)
        convertids[permsupport[i]] = ids[i];
      break;
    case DDDMP_VAR_MATCHPERMIDS:
      for (i=0; i<nsuppvars; i++)
        convertids[permsupport[i]] = Cudd_ReadInvPerm(dd,permids[i]);
      break;
    case DDDMP_VAR_MATCHAUXIDS:
      if (auxids == NULL) {
        (void) fprintf (stdout,"DdLoad Error: variable auxids matching requested\n");
        (void) fprintf (stdout,"but .auxids not found in BDD file\n");
        (void) fprintf (stdout,"Matching IDs forced.\n");
        varmatchmode = DDDMP_VAR_MATCHIDS;
        goto again_matchmode;
      }
      /* find max auxid value to alloc invaux array */
      for (i=0,maxaux= -1; i<nddvars; i++)
        if (varmatchauxids[i]>maxaux)
          maxaux = varmatchauxids[i];
      /* generate invaux array */
      invauxids = DDDMP_ALLOC(int,maxaux+1);
      if (invauxids == NULL) {
        (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
        goto failure;
      }
      for (i=0; i<=maxaux; i++)
        invauxids[i] = -1;
      for (i=0; i<nsuppvars; i++)
        invauxids[varmatchauxids[ids[i]]] = ids[i];
      /* generate convertids array */
      for (i=0; i<nsuppvars; i++) {
        if ((auxids[i]>maxaux) || (invauxids[auxids[i]]<0)) {
          (void) fprintf (stdout,
                   "DdLoad Error: auxid %d not found in DD manager. ID matching forced (%d)\n", 
                   auxids[i], i);
          (void) fprintf (stdout,"Beware of possible overlappings with other variables\n"); 
          convertids[permsupport[i]]=i;
        }
        else
          convertids[permsupport[i]] = invauxids[auxids[i]];
      }
      break;
    case DDDMP_VAR_MATCHNAMES:
      if (varnames == NULL) {
        (void) fprintf (stdout,"DdLoad Error: variable names matching requested\n");
        (void) fprintf (stdout,"but .varnames not found in BDD file\n");
        (void) fprintf (stdout,"Matching IDs forced.\n");
        varmatchmode = DDDMP_VAR_MATCHIDS;
        goto again_matchmode;
      }
      /* generate invaux array */
      invauxids = DDDMP_ALLOC(int,nsuppvars);
      if (invauxids == NULL) {
        (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
        goto failure;
      }
      for (i=0; i<nsuppvars; i++)
        invauxids[i] = -1;
      for (i=0; i<nddvars; i++) {
        if (varmatchnames[i]==NULL) {
          (void) fprintf (stdout,"DdLoad Warning: NULL match variable name (id: %d). Ignored.\n",
                                 i);
        }
        else
          if ((j=FindVarname(varmatchnames[i],sortedvarnames,nsuppvars))>=0) {
            assert(j<nsuppvars);
            invauxids[j] = i;
          }
      }
      /* generate convertids array */
      for (i=0; i<nsuppvars; i++) {
        assert (varnames[i] != NULL);
        j=FindVarname(varnames[i],sortedvarnames,nsuppvars);
        assert((j>=0)&&(j<nsuppvars));
        if (invauxids[j]<0) {
          (void) fprintf (stdout,
              "DdLoad Error: varname %s not found in DD manager. ID matching forced (%d)\n", 
               varnames[i],i);
          convertids[permsupport[i]]=i;
        }
        else
          convertids[permsupport[i]] = invauxids[j];
      }
      break;
    case DDDMP_VAR_COMPOSEIDS:
      for (i=0; i<nsuppvars; i++)
        convertids[permsupport[i]] = varcomposeids[ids[i]];
      break;
  }

  maxv= -1;
  for (i=0; i<nsuppvars; i++)
    if (convertids[i] > maxv)
      maxv = convertids[i];
 
  invconvertids = DDDMP_ALLOC(int,maxv+1);
  if (invconvertids == NULL) {
    (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
    goto failure;
  }
  for (i=0; i<=maxv; i++)
    invconvertids[i]= -1;
  for (i=0; i<nsuppvars; i++)
    invconvertids[convertids[i]] = i;

  pnodes = DDDMP_ALLOC(DdNode *,(nnodes+1));
  if (pnodes == NULL) {
    (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
    goto failure;
  }

  if (nsuppvars < 256) {
    pvars1byte = DDDMP_ALLOC(unsigned char,(nnodes+1));
    if (pvars1byte == NULL) {
      (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
      goto failure;
    }
  }
  else if (nsuppvars < 0xffff) {
    pvars2byte = DDDMP_ALLOC(unsigned short,(nnodes+1));
    if (pvars2byte == NULL) {
      (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
      goto failure;
    }
  }
  else {
    printf ("DdLoad Error: more than %d variables. Not supported.\n", 0xffff);
    goto failure;
  }

  pnodes[1] = DD_ONE(dd);

  for (i=1; i<=nnodes; i++) {

    if (feof(fp)) {
      (void) fprintf (stdout,"DdLoad Error: unexpected EOF while reading DD nodes\n");
      goto failure;
    }

    switch (mode) {

      case DDDMP_MODE_TEXT:

        switch (varinfo) {
          case DDDMP_VARIDS:
          case DDDMP_VARPERMIDS:
          case DDDMP_VARAUXIDS:
          case DDDMP_VARNAMES:
            if (fscanf(fp,"%d %*s %d %d %d\n", 
                           &id, &var, &idT, &idE) < 4) {
              (void) fprintf (stdout,"DdLoad: Error reading nodes in text mode\n");
              goto failure;
            }
            break;
          case DDDMP_VARDEFAULT:
            if (fscanf(fp,"%d %d %d %d\n", 
                           &id, &var, &idT, &idE) < 4) {
              (void) fprintf (stdout,"DdLoad: Error reading nodes in text mode\n");
              goto failure;
            }
            break;
        }
#ifdef DDDMP_DEBUG
        assert (id == i);
#endif
        if (var==1 && idT==0 && idE==0)
        {
          /* the 1 leaf */
          continue;
        }
        else
        {
#ifdef DDDMP_DEBUG
          assert (idT > 0);
#endif
          T = pnodes[idT];
          if(idE<0) {
            idE = -idE;
            E = pnodes[idE];
            E = Cudd_Not(E);
          }
          else
            E = pnodes[idE];
        }

      break;

      case DDDMP_MODE_BINARY:

        if (DddmpReadCode(fp,&code) == 0)
          goto failure;

        switch (code.V) {
        case DDDMP_TERMINAL:     
          /* only 1 terminal presently supported: do nothing */    
          continue; 
          break;
        case DDDMP_RELATIVE_1:
          break;
        case DDDMP_RELATIVE_ID:
        case DDDMP_ABSOLUTE_ID:
          size = DddmpReadInt(fp,&var);
          if (size == 0)
            goto failure;
          break;
        }
        switch (code.T) {
        case DDDMP_TERMINAL:     
          idT = 1;
          break;
        case DDDMP_RELATIVE_1:
          idT = i-1;
          break;
        case DDDMP_RELATIVE_ID:
          size = DddmpReadInt(fp,&id);
          if (size == 0)  goto failure;
          idT = i-id;
          break;
        case DDDMP_ABSOLUTE_ID:
          size = DddmpReadInt(fp,&idT);
          if (size == 0)  goto failure;
          break;
        }
        switch (code.E) {
        case DDDMP_TERMINAL:     
          idE = 1;
          break;
        case DDDMP_RELATIVE_1:
          idE = i-1;
          break;
        case DDDMP_RELATIVE_ID:
          size = DddmpReadInt(fp,&id);
          if (size == 0)  goto failure;
          idE = i-id;
          break;
        case DDDMP_ABSOLUTE_ID:
          size = DddmpReadInt(fp,&idE);
          if (size == 0)  goto failure;
          break;
        }

#ifdef DDDMP_DEBUG
      assert(idT<i);
#endif
      T = pnodes[idT];
      if (cuddIsConstant(T))
        vT = nsuppvars;
      else {
        if (pvars1byte != NULL)
          vT = pvars1byte[idT];
        else if (pvars2byte != NULL)
          vT = pvars2byte[idT];
        else
          vT = invconvertids[T->index];
      }
#ifdef DDDMP_DEBUG
      assert (vT>0);
      assert (vT<=nsuppvars);
#endif

#ifdef DDDMP_DEBUG
      assert(idE<i);
#endif
      E = pnodes[idE];
      if (cuddIsConstant(E))
        vE = nsuppvars;
      else {
        if (pvars1byte != NULL)
          vE = pvars1byte[idE];
        else if (pvars2byte != NULL)
          vE = pvars2byte[idE];
        else
          vE = invconvertids[E->index];
      }
#ifdef DDDMP_DEBUG
      assert (vE>0);
      assert (vE<=nsuppvars);
#endif
  
      switch (code.V) {
        case DDDMP_TERMINAL:     
        case DDDMP_ABSOLUTE_ID:
          break;
        case DDDMP_RELATIVE_1:
          var = (vT<vE) ? vT-1 : vE-1;
          break;
        case DDDMP_RELATIVE_ID:
          var = (vT<vE) ? vT-var : vE-var;
          break;
      }

      if (code.Ecompl)
        E = Cudd_Not(E);

#ifdef DDDMP_DEBUG
      assert (var<nsuppvars);
#endif

      break;

    }

    if (pvars1byte != NULL)
      pvars1byte[i] = (unsigned char) var;
    else if (pvars2byte != NULL)
      pvars2byte[i] = (unsigned short) var;

    var = convertids[var]; 
    pnodes[i] = Cudd_bddIte(dd, Cudd_bddIthVar(dd,var), T, E);
    cuddRef(pnodes[i]);

  }

  fgets(buf, 999,fp);
  if (!matchkeywd(buf, ".end")) {
    (void) fprintf (stdout,"DdLoad Error: .end not found\n");
    goto failure;
  }

  if (close_fp)
    fclose (fp);

  /* BDD Roots */
  proots = DDDMP_ALLOC(DdNode *,nroots);
  if (proots == NULL) {
    (void) fprintf (stdout,"DdLoad: Error allocating memory\n");
    goto failure;
  }

  for(i=0; i<nroots; ++i) {
    switch (rootmatchmode) {
      case DDDMP_ROOT_MATCHNAMES:
        /* The CUDD implementation assumes that at this point the
        ** rootnames array has been properly allocated. Make this
        ** explicit for static code analysis.
        */
        assert(rootnames);
        for (j=0; j<nroots; j++) {
          if (strcmp(rootmatchnames[i],rootnames[j])==0)
            break;
        }
        if (j>=nroots) { /* rootname not found */
          printf ("Warning: unable to match root name <%s>\n",
                  rootmatchnames[i]);
        }
        break; 
      case DDDMP_ROOT_MATCHLIST:
        j = i;
        break;
    }
    /* The CUDD implementation assumes that at this point the rootids
    ** array has been properly allocated. Make this explicit for
    ** static code analysis.
    */
    assert(rootids);
    id = rootids[i];
    if (id==0) {
      (void) fprintf (stdout,"DdLoad Warning: NULL root found in file\n");
      f = NULL;
    }
    else if (id<0) 
      f = Cudd_Not(pnodes[-id]);
    else
      f = pnodes[id];
    proots[i] = f;
    cuddRef(f);
  }

  for(i=2; i<=nnodes; ++i) { 
    f = pnodes[i];
    Cudd_RecursiveDeref(dd, f);
  }

  /*
   * now free everithing was allocated within this function
   */

load_end:

  DDDMP_FREE(pnodes);
  DDDMP_FREE(pvars1byte);
  DDDMP_FREE(pvars2byte);

  DDDMP_FREE(ddname);
  if (varnames!=NULL)
    for (i=0;i<nsuppvars;i++)
      DDDMP_FREE(varnames[i]);
  DDDMP_FREE(varnames);
   /* variable names are not freed because they were shared with varnames */
  DDDMP_FREE(sortedvarnames);

  DDDMP_FREE(ids);
  DDDMP_FREE(permids);
  DDDMP_FREE(auxids);
  DDDMP_FREE(rootids);

  if (rootnames!=NULL)
    for (i=0;i<nroots;i++)
      DDDMP_FREE(rootnames[i]);
  DDDMP_FREE(rootnames);

  DDDMP_FREE(permsupport);
  DDDMP_FREE(convertids);
  DDDMP_FREE(invconvertids);
  DDDMP_FREE(invauxids);

  *pproots = proots;
  return nroots;

failure:

  if (close_fp)
    fclose (fp);

  nroots = 0; /* return 0 on error ! */

  DDDMP_FREE(proots);

  goto load_end; /* this is done to free memory */

}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Dddmp_bddStore.]

  Description [Stores a node to file in either text or
  binary mode.<l>
  In text mode a node is represented (on a text line basis) as
  <UL>
  <LI> node-index \[var-extrainfo\] var-index Then-index Else-index
  </UL>
  
  where all indexes are integer numbers and var-extrainfo (optional
  redundant field) is either an integer or a string (variable name).
  Node-index is redundant (due to the node ordering) but we keep it
  for readability.<p>

  In binary mode nodes are represented as a sequence of bytes,
  representing var-index, Then-index, and Else-index in an optimized
  way.  Only the first byte (code) is mandatory. Integer indexes are
  represented in absolute or relative mode, where relative means
  offset wrt. a Then/Else node info.  Suppose Var(NodeId),
  Then(NodeId) and Else(NodeId) represent infos about a given node.<p>

  The generic "NodeId" node is stored as 

  <UL>
  <LI> code-byte
  <LI> \[var-info\]
  <LI> \[Then-info\]
  <LI> \[Else-info\]
  </UL>

  where code-byte contains bit fields

  <UL>
  <LI>Unused  : 1 bit
  <LI>Variable: 2 bits, one of the following codes
    <UL>
    <LI>DDDMP_ABSOLUTE_ID   var-info = Var(NodeId) follows
    <LI>DDDMP_RELATIVE_ID   Var(NodeId) is represented in relative form as
    var-info = Min(Var(Then(NodeId)),Var(Else(NodeId))) -Var(NodeId)
    <LI>DDDMP_RELATIVE_1    No var-info follows, because
    Var(NodeId) = Min(Var(Then(NodeId)),Var(Else(NodeId)))-1
    <LI>DDDMP_TERMINAL      Node is a terminal, no var info required
    </UL>
  <LI>T       : 2 bits, with codes similar to V
    <UL>
    <LI>DDDMP_ABSOLUTE_ID   Then-info = Then(NodeId) follows
    <LI>DDDMP_RELATIVE_ID   Then(NodeId) is represented in relative form as
    Then-info = Nodeid-Then(NodeId)
    <LI>DDDMP_RELATIVE_1    No info on Then(NodeId) follows, because
    Then(NodeId) = NodeId-1
    <LI>DDDMP_TERMINAL      Then Node is a terminal, no info required (for
    BDDs)
    </UL>
  <LI>Ecompl  : 1 bit, if 1 means complemented edge
  <LI>E       : 2 bits, with codes and meanings as for the Then edge
  </UL>
var-info, Then-info, Else-info (if required) are represented as unsigned 
integer values on a sufficient set of bytes (MSByte first).
              ]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static int
StoreNodeRecur(
DdManager *dd     /* dd manager */,
DdNode *f         /* dd node to be stored */,
int mode          /* store mode */,
int *supportids   /* internal ids for variables */,
char **varnames   /* names of variables: to be stored with nodes */,
int *outids       /* output ids for variables */,
FILE *fp          /* store file */
)
{
  DdNode      *T = NULL, *E = NULL; /* SCV: ensure pointer initialization */
  int idf, idT = -1, idE = -1, vf = -1, vT = -1, vE = -1; /* SCV: enable check for proper initialization in the range of from 0 to CUDD_MAXINDEX */
  int retval, diff, var;
  int nvars = dd->size;
  struct binary_dd_code code;
  code.Unused = 0;  /* initialize unused field to avoid compiler warning */

#ifdef DDDMP_DEBUG
  assert(!Cudd_IsComplement(f));
  assert(f!=NULL);
  assert(supportids!=NULL);
#endif

  /* If already visited, nothing to do. */
  if (DddmpVisited(f))
    return(1);

  /* Mark node as visited. */
  DddmpSetVisited(f);

  if (f == DD_ONE(dd)) {
    /* Check for special case: don't recur */
    idf = DddmpReadNodeIndex(f);
  }
  else {

#ifdef DDDMP_DEBUG
    /* BDDs! Only one constant supported */
    assert(!cuddIsConstant(f));
#endif

    /* 
     * Recursive call for Then edge
     */
    T = cuddT(f);
#ifdef DDDMP_DEBUG
    /* ROBDDs! No complemented Then edge */
    assert(!Cudd_IsComplement(T)); 
#endif
    /* recur */
    retval = StoreNodeRecur(dd,T,mode,supportids,varnames,outids,fp);
    if (retval != 1) return(retval);

    /* 
     * Recursive call for Else edge
     */
    E = Cudd_Regular(cuddE(f));
    retval = StoreNodeRecur(dd,E,mode,supportids,varnames,outids,fp);
    if (retval != 1) return(retval);

    /* 
     * Obtain nodeids and variable ids of f, T, E 
     */

    idf = DddmpReadNodeIndex(f);
    vf = f->index;

    idT = DddmpReadNodeIndex(T);
    if Cudd_IsConstant(T)
      vT = nvars;
    else
      vT = T->index;

    idE = DddmpReadNodeIndex(E);
    if Cudd_IsConstant(E)
	vE = nvars;
    else
	vE = E->index;

  }

  switch (mode) {

    case DDDMP_MODE_TEXT:

      if (f == DD_ONE(dd)) {
	  if ((varnames != NULL)||(outids != NULL))
          retval = fprintf (fp,"%d T 1 0 0\n",idf);
        else
          retval = fprintf (fp,"%d 1 0 0\n",idf);
	}
	else {
          /* SCV: assert that idE and idT have been initialized upon first usage */
          assert((idE >= 0) && (idE <= CUDD_MAXINDEX));
          assert((idT >= 0) && (idT <= CUDD_MAXINDEX));
          if (Cudd_IsComplement(cuddE(f)))
            idE = -idE;
          /* SCV: assert that vf has been initialized upon first usage */
          assert((vf >= 0) && (vf <= CUDD_MAXINDEX));
          if (varnames != NULL) {
            retval = fprintf (fp,"%d %s %d %d %d\n",
                              idf,varnames[vf],supportids[vf],idT,idE);
          }
	  else {
	    if (outids != NULL) {
              retval = fprintf (fp,"%d %d %d %d %d\n",
                                idf,outids[vf],supportids[vf],idT,idE);
            }
            else
              retval = fprintf (fp,"%d %d %d %d\n",
                                idf,supportids[vf],idT,idE);
	  }
	}

	break;

    case DDDMP_MODE_BINARY:

	/* only integer ids used, varnames ignored */

	if (f == DD_ONE(dd)) {
        /*
         * Terminal one is coded as DDDMP_TERMINAL, all other fields are 0
         */
        code.Unused = 0;
        code.V = DDDMP_TERMINAL;
        code.T = 0;
        code.E = 0;
        code.Ecompl = 0;
        retval = DddmpWriteCode (fp,code);
	  if (retval == EOF) return(0);
	}
	else {
        /*
         * Non terminal: output variable id
         */
	  var = supportids[vf];
          /* SCV: assert proper initialization of vT and vE upon first usage */
          assert((vT >= 0) && (vT <= CUDD_MAXINDEX));
          assert((vE >= 0) && (vE <= CUDD_MAXINDEX));
	  diff = (supportids[vT]<supportids[vE]) ? 
			  (supportids[vT]-var) : (supportids[vE]-var);
        code.V = DDDMP_ABSOLUTE_ID;
        if (diff <= var) {
	    if (diff == 1)
              code.V = DDDMP_RELATIVE_1;
	    else {
              code.V = DDDMP_RELATIVE_ID;
              var = diff;
	    } 
	  }

	  if (T == DD_ONE(dd))
            code.T = DDDMP_TERMINAL;
	  else {
	    /* compute displacement */
	    diff = idf-idT;
            code.T = DDDMP_ABSOLUTE_ID;
	    if (diff <= idT) {
	      if (diff == 1)
                code.T = DDDMP_RELATIVE_1;
	      else {
                code.T = DDDMP_RELATIVE_ID;
		idT = diff;
	      } 
	    }
	  }

	  if (E == DD_ONE(dd))
            code.E = DDDMP_TERMINAL;
	  else {
	    /* compute displacement */
	    diff = idf-idE;
            code.E = DDDMP_ABSOLUTE_ID;
	    if (diff <= idE) {
	      if (diff == 1)
                code.E = DDDMP_RELATIVE_1;
	      else {
                code.E = DDDMP_RELATIVE_ID;
		idE = diff;
	      } 
	    }
	  }
        if (Cudd_IsComplement(cuddE(f)))
          code.Ecompl = 1;
        else
          code.Ecompl = 0;

        retval = DddmpWriteCode (fp,code);
	  if (retval == EOF)
 	    return(0);

        if ((code.V == DDDMP_ABSOLUTE_ID) || 
            (code.V == DDDMP_RELATIVE_ID)) { 
          retval = DddmpWriteInt (fp,var);
	    if (retval == EOF)
	      return(0);
        }

        if ((code.T == DDDMP_ABSOLUTE_ID) || 
            (code.T == DDDMP_RELATIVE_ID)) { 
  	    retval = DddmpWriteInt(fp,idT);
	    if (retval == EOF)
	      return(0);
        }

        if ((code.E == DDDMP_ABSOLUTE_ID) || 
            (code.E == DDDMP_RELATIVE_ID)) { 
	    retval = DddmpWriteInt(fp,idE);
  	    if (retval == EOF)
	      return(0);
        }

	}

	break;

      default:
	return(0);
    }

    if (retval == EOF) {
	return(0);
    } else {
	return(1);
    }

} /* end of StoreNodeRecur */


/**Function********************************************************************

  Synopsis    [String compare for qsort]

  Description [String compare for qsort]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static int
QsortStrcmp(
const void *ps1,
const void *ps2
)
{
  return strcmp (*((char**)ps1),*((char **)ps2));
}

/**Function********************************************************************

  Synopsis    [Performs binary search of a name within a sorted array]

  Description [Binary search of a name within a sorted array of strings.
               used when matching names of variables.
              ]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static int
FindVarname(
char *name,
char **array,
int n
)
{
  int d, m, u, t;

  d = 0; u = n-1;

  while (u>=d) {
    m = (u+d)/2;
    t=strcmp(name,array[m]);
    if (t==0)
      return m;
    if (t<0)
      u=m-1;
    else
      d=m+1;
  }
  return -1;
}

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int StoreNodeRecur(DdManager *dd, DdNode *f, int mode, int *supportids, char **varnames, int *outids, FILE *fp);
static int QsortStrcmp(const void *ps1, const void *ps2);
static int FindVarname(char *name, char **array, int n);

