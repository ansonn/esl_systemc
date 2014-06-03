/**CFile**********************************************************************

  FileName    [dddmpDbg.c]

  PackageName [dddmp]

  Synopsis    [Functions to display BDD files]

  Description [Functions to display BDD files in binary format
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


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Display a binary dump file in a text file]

  Description [Display a binary dump file in a text file
               ]

  SideEffects [None]

  SeeAlso     [Dddmp_cuddBddStore , Dddmp_cuddBddLoad ]

******************************************************************************/
int
Dddmp_cuddBddDisplayBinary(
 char *filein,   /* name of binary file */ 
 char *fileout   /* name of text file */ )
{
  FILE *fp, *fpo; 
  int id, size;
  struct binary_dd_code code;
  char buf[1000];
  int nnodes = 0, i;

  fp = fopen (filein, "rb");
  if (fp == 0)
    return (0);

  fpo = fopen (fileout, "w");
  if (fpo == 0)
    return (0);

  while (fgets(buf, 999,fp)!=NULL) {
    fprintf (fpo, "%s", buf);
    if (strncmp(buf, ".nnodes", 7) == 0)    
      sscanf (buf, "%*s %d", &nnodes);
    if (strncmp(buf, ".rootids", 8) == 0)    
      break;
  }

  for (i=1; i<=nnodes; i++) {
    if (feof(fp))
      return (0);
    if (DddmpReadCode(fp,&code) == 0)
      return 0;                        
    fprintf (fpo, "c  : v %d | T %d | E %d\n",
            (int)code.V, (int)code.T, 
            (code.Ecompl ? -(int)(code.E) : (int)(code.E)));
    if (code.V == DDDMP_TERMINAL) 
      continue;
    if (code.V <= DDDMP_RELATIVE_ID) {
      size = DddmpReadInt(fp,&id);
      if (size == 0)
        return 0;
      fprintf(fpo, "v(%d): %d\n", size, id);
    }
    if (code.T <= DDDMP_RELATIVE_ID) {
      size = DddmpReadInt(fp,&id);
      if (size == 0)
        return 0;
      fprintf(fpo, "T(%d): %d\n", size, id);
    }
    if (code.E <= DDDMP_RELATIVE_ID) {
      size = DddmpReadInt(fp,&id);
      if (size == 0)
        return 0;
      fprintf(fpo, "E(%d): %d\n", size, id);
    }

  }

  fgets(buf, 999,fp);
  if (strncmp(buf, ".end", 4) != 0)    
    return 0;

  fprintf(fpo, ".end");

  fclose(fp);
  fclose(fpo);

  return 1;

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


