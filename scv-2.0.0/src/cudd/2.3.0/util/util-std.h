#if defined (__STDC__) || defined(_MSV_VER)
#include <stdlib.h>
#else
/* ANSI C stdlib.h -- 1/11/88 Draft Standard */
extern double atof(), strtod();
extern long strtol(), strtoul(), labs();
extern int atoi(), rand(), atexit(), system(), abs();
extern void srand(), free(), abort(), exit(), qsort();
extern char *calloc(), *realloc(), *malloc(), *bsearch(), *getenv();
#endif

#ifdef _MSC_VER
#include <search.h>
#endif
