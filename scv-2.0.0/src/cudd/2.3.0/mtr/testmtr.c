/**CFile***********************************************************************
 
   FileName    [testmtr.c]

   PackageName [mtr]

   Synopsis    [Test program for the mtr package.]

   Description []

   SeeAlso     []

   Author      [Fabio Somenzi]
		   
   Copyright [ This file was created at the University of Colorado at
   Boulder.  The University of Colorado at Boulder makes no warranty
   about the suitability of this software for any purpose.  It is
   presented on an AS IS basis.]

******************************************************************************/

#ifndef lint
static char rcsid[] = "$Id: testmtr.c,v 1.1 2002/09/17 21:59:06 dlm Exp $";
#endif

#include <stdio.h>
#include "util.h"
#include "mtr.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define TESTMTR_VERSION\
    "TestMtr Version #0.4, Release date 1/2/97"

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void usage ARGS((char *prog));
static FILE * open_file ARGS((char *filename, char *mode));

/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Main program for testmtr.]

  Description [Main program for testmtr.  Performs initialization.
  Reads command line options and network(s).  Builds some simple trees
  and prints them out.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
int
main(
  int  argc,
  char ** argv)
{
    MtrNode *root,
	    *node;
    int	    i,
	    c,
	    pr = 0;
    FILE    *fp;
    char    *file = NULL;
    
    (void) printf("# %s\n", TESTMTR_VERSION);
    /* Echo command line and arguments. */
    (void) printf("#");
    for(i = 0; i < argc; i++) {
	(void) printf(" %s", argv[i]);
    }
    (void) printf("\n");
    (void) fflush(stdout);

    while ((c = getopt(argc, argv, "Mhp:")) != EOF) {
	switch(c) {
	case 'M':
#ifdef MNEMOSYNE
	    (void) mnem_setrecording(0);
#endif
	    break;
	case 'p':
	    pr = atoi(optarg);
	    break;
	case 'h':
	default:
	    usage(argv[0]);
	    break;
	}
    }

    if (argc - optind == 0) {
	file = "-";
    } else if (argc - optind == 1) {
	file = argv[optind];
    } else {
	usage(argv[0]);
    }

    /* Create and print a simple tree. */
    root = Mtr_InitTree();
    root->flags = 0;
    node = Mtr_CreateFirstChild(root);
    node->flags = 1;
    node = Mtr_CreateLastChild(root);
    node->flags = 2;
    node = Mtr_CreateFirstChild(root);
    node->flags = 3;
    node = Mtr_AllocNode();
    node->flags = 4;
    Mtr_MakeNextSibling(root->child,node);
    Mtr_PrintTree(root); (void) printf("#------------------------\n");

    Mtr_FreeTree(root);

    /* Create an initial tree in which all variables belong to one group. */
    root = Mtr_InitGroupTree(0,12);
    Mtr_PrintTree(root); (void) printf("#  ");
    Mtr_PrintGroups(root,pr == 0); (void) printf("\n");
    node = Mtr_MakeGroup(root,0,6,MTR_DEFAULT);
    node = Mtr_MakeGroup(root,6,6,MTR_DEFAULT);
    Mtr_PrintTree(root); (void) printf("#  ");
    Mtr_PrintGroups(root,pr == 0); (void) printf("\n");
    for (i = 0; i < 6; i+=2) {
	node = Mtr_MakeGroup(root,(unsigned) i,(unsigned) 2,MTR_DEFAULT);
    }
    node = Mtr_MakeGroup(root,0,12,MTR_FIXED);
    Mtr_PrintTree(root); (void) printf("#  ");
    Mtr_PrintGroups(root,pr == 0); (void) printf("\n");
    /* Print a partial tree. */
    (void) printf("#  ");
    Mtr_PrintGroups(root->child,pr == 0); (void) printf("\n");
    node = Mtr_FindGroup(root,0,6);
    node = Mtr_DissolveGroup(node);
    Mtr_PrintTree(root); (void) printf("#  ");
    Mtr_PrintGroups(root,pr == 0); (void) printf("\n");
    node = Mtr_FindGroup(root,4,2);
    if (!Mtr_SwapGroups(node,node->younger)) {
	(void) printf("error in Mtr_SwapGroups\n");
	exit(3);
    }
    Mtr_PrintTree(root); (void) printf("#  ");
    Mtr_PrintGroups(root,pr == 0); (void) printf("\n");

    Mtr_FreeTree(root);

    /* Open input file. */
    fp = open_file(file, "r");
    root = Mtr_ReadGroups(fp,12);
    Mtr_PrintTree(root); (void) printf("#  ");
    Mtr_PrintGroups(root,pr == 0); (void) printf("\n");

    Mtr_FreeTree(root);

#ifdef MNEMOSYNE
    mnem_writestats();
#endif

    exit(0);
    /* NOTREACHED */

} /* end of main */


/**Function********************************************************************

  Synopsis    [Prints usage message and exits.]

  Description []

  SideEffects [none]

  SeeAlso     []

******************************************************************************/
static void
usage(
  char * prog)
{
    (void) fprintf(stderr, "usage: %s [options] [file]\n", prog);
    (void) fprintf(stderr, "   -M\t\tturns off memory allocation recording\n");
    (void) fprintf(stderr, "   -h\t\tprints this message\n");
    (void) fprintf(stderr, "   -p n\t\tcontrols verbosity\n");
    exit(2);

} /* end of usage */


/**Function********************************************************************

  Synopsis    [Opens a file.]

  Description [Opens a file, or fails with an error message and exits.
  Allows '-' as a synonym for standard input.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static FILE *
open_file(
  char * filename,
  char * mode)
{
    FILE *fp;

    if (strcmp(filename, "-") == 0) {
        return mode[0] == 'r' ? stdin : stdout;
    } else if ((fp = fopen(filename, mode)) == NULL) {
        perror(filename);
        exit(1);
    }
    return(fp);

} /* end of open_file */

