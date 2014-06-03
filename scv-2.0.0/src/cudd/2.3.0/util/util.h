#ifndef UTIL_H
#define UTIL_H

#include "scv/scv_config.h"

/* These are potential duplicates. */
#ifndef EXTERN
#   ifdef __cplusplus
#	define EXTERN extern "C"
#   else
#	define EXTERN extern
#   endif
#endif
#ifndef ARGS
#   if defined(__STDC__) || defined(__cplusplus) || defined(_MSC_VER)
#	define ARGS(protos)    protos          /* ANSI C */
#   else /* !(__STDC__ || __cplusplus) */
#	define ARGS(protos)    ()              /* K&R C */
#   endif /* !(__STDC__ || __cplusplus) */
#endif
#ifndef NULLARGS
#   if defined(__STDC__) || defined(__cplusplus)
#       define NULLARGS    (void)
#   else
#       define NULLARGS    ()
#   endif
#endif
#ifndef const
#   if !defined(__STDC__) && !defined(__cplusplus)
#       define const
#   endif
#endif


#if defined(__GNUC__)
#   define UTIL_INLINE __inline__
#   if __GNUC__ > 2 || __GNUC_MINOR__ >= 7
#       define UTIL_UNUSED __attribute__ ((unused))
#   else
#       define UTIL_UNUSED
#   endif
#else
#   define UTIL_INLINE
#   define UTIL_UNUSED
#endif

#if SIZEOF_VOID_P == 8 && SIZEOF_INT == 4
typedef long util_ptrint;
#else
typedef int util_ptrint;
#endif

/* #define USE_MM */		/* choose libmm.a as the memory allocator */

/* these are too entrenched to get away with changing the name */
#define strsav		util_strsav

#ifndef _MSC_VER
#include <unistd.h>
extern char *optarg;
extern int optind, opterr;
#endif

#define NIL(type)		((type *) 0)

#if defined(USE_MM) || defined(MNEMOSYNE)
/*
 *  assumes the memory manager is either libmm.a or libmnem.a
 *	libmm.a:
 *	- allows malloc(0) or realloc(obj, 0)
 *	- catches out of memory (and calls MMout_of_memory())
 *	- catch free(0) and realloc(0, size) in the macros
 *	libmnem.a:
 *	- reports memory leaks
 *	- is used in conjunction with the mnemalyse postprocessor
 */
#ifdef MNEMOSYNE
#include "mnemosyne.h"
#define ALLOC(type, num)	\
    ((num) ? ((type *) malloc(sizeof(type) * (num))) : \
	    ((type *) malloc(sizeof(long))))
#else
#define ALLOC(type, num)	\
    ((type *) malloc(sizeof(type) * (num)))
#endif
#define REALLOC(type, obj, num)	\
    (obj) ? ((type *) realloc((char *) obj, sizeof(type) * (num))) : \
	    ((type *) malloc(sizeof(type) * (num)))
#define FREE(obj)		\
    ((obj) ? (free((char *) (obj)), (obj) = 0) : 0)
#else
/*
 *  enforce strict semantics on the memory allocator
 *	- when in doubt, delete the '#define USE_MM' above
 */
#define ALLOC(type, num)	\
    ((type *) MMalloc((long) sizeof(type) * (long) (num)))
#define REALLOC(type, obj, num)	\
    ((type *) MMrealloc((char *) (obj), (long) sizeof(type) * (long) (num)))
#define FREE(obj)		\
    ((obj) ? (free((char *) (obj)), (obj) = 0) : 0)
#endif


/* Ultrix (and SABER) have 'fixed' certain functions which used to be int */
#if defined(ultrix) || defined(SABER) || defined(aiws) || defined(hpux) || defined(apollo) || defined(__osf__) || defined(__SVR4) || defined(__GNUC__)
#define VOID_OR_INT void
#define VOID_OR_CHAR void
#else
#define VOID_OR_INT int
#define VOID_OR_CHAR char
#endif


/* No machines seem to have much of a problem with these */
#include <stdio.h>
#include <ctype.h>


/* Some machines fail to define some functions in stdio.h */
#if !defined(__STDC__) && !defined(__cplusplus) && !defined(_MSC_VER)
extern FILE *popen(), *tmpfile();
extern int pclose();
#ifndef clearerr		/* is a macro on many machines, but not all */
extern VOID_OR_INT clearerr();
#endif
#if !defined(rewind) && !defined(_IBMR2)
extern VOID_OR_INT rewind();
#endif
#endif


/* most machines don't give us a header file for these */
#if (defined(__STDC__) || defined(__cplusplus) || defined(ultrix)) && !defined(MNEMOSYNE) || defined(__SVR4)
# include <stdlib.h>
#else
# ifndef _IBMR2
    extern VOID_OR_INT abort(), exit();
# endif
# if !defined(__STDC__) && !defined(_IBMR2) && !defined(_MSC_VER)
    extern VOID_OR_INT perror();
#   ifdef __osf__
      extern int sprintf();
#   else
#     ifndef _IBMR2
        extern char *sprintf();
#     endif
#   endif
# endif
# if !defined(MNEMOSYNE) && !defined(_IBMR2)
    EXTERN VOID_OR_INT free ARGS((void *));
    extern VOID_OR_CHAR *malloc(), *realloc();
# endif
  extern char *getenv();
  extern int system();
  extern double atof();
#endif


/* some call it strings.h, some call it string.h; others, also have memory.h */
#if defined(__STDC__) || defined(__cplusplus) || defined(_IBMR2) || defined(ultrix) || defined(_MSC_VER)
#include <string.h>
#else
/* ANSI C string.h -- 1/11/88 Draft Standard */
extern char *strcpy(), *strncpy(), *strcat(), *strncat(), *strerror();
extern char *strpbrk(), *strtok(), *strchr(), *strrchr(), *strstr();
extern int strcoll(), strxfrm(), strncmp(), strlen(), strspn(), strcspn();
extern char *memmove(), *memccpy(), *memchr(), *memcpy(), *memset();
extern int memcmp(), strcmp();
#endif

#ifdef _MSC_VER
#include <search.h>
#endif

#if defined(__STDC__) || defined(__cplusplus) || defined(_MSC_VER)
#include <assert.h>
#else
#ifndef _HPUX_SOURCE
#ifndef NDEBUG
#define assert(ex) {\
    if (! (ex)) {\
	(void) fprintf(stderr,\
	    "Assertion failed: file %s, line %d\n\"%s\"\n",\
	    __FILE__, __LINE__, "ex");\
	(void) fflush(stdout);\
	abort();\
    }\
}
#else
#define assert(ex) ;
#endif
#endif
#endif


#define fail(why) {\
    (void) fprintf(stderr, "Fatal error: file %s, line %d\n%s\n",\
	__FILE__, __LINE__, why);\
    (void) fflush(stdout);\
    abort();\
}


#ifdef lint
#undef putc			/* correct lint '_flsbuf' bug */
#undef ALLOC			/* allow for lint -h flag */
#undef REALLOC
#define ALLOC(type, num)	(((type *) 0) + (num))
#define REALLOC(type, obj, num)	((obj) + (num))
#endif


/* These arguably do NOT belong in util.h */
#define ABS(a)			((a) < 0 ? -(a) : (a))

#ifndef _HPUX_SOURCE
#define MAX(a,b)		((a) > (b) ? (a) : (b))
#define MIN(a,b)		((a) < (b) ? (a) : (b))
#endif


#ifndef USE_MM
EXTERN char *MMalloc ARGS((long));
EXTERN void MMout_of_memory ARGS((long));
EXTERN void (*MMoutOfMemory) ARGS((long));
EXTERN char *MMrealloc ARGS((char *, long));
#endif

EXTERN long util_cpu_time ARGS((void));
EXTERN int util_getopt ARGS((int, char **, char *));
EXTERN void util_getopt_reset ARGS((void));
EXTERN char *util_path_search ARGS((char *));
EXTERN char *util_file_search ARGS((char *, char *, char *));
EXTERN int util_pipefork ARGS((char **, FILE **, FILE **, int *));
EXTERN void util_print_cpu_stats ARGS((FILE *));
EXTERN char *util_print_time ARGS((unsigned long));
EXTERN int util_save_image ARGS((char *, char *));
EXTERN char *util_strsav ARGS((char *));
EXTERN char *util_tilde_expand ARGS((char *));
EXTERN void util_restart ARGS((char *, char *, int));


/* util_getopt() global variables (ack !) */
EXTERN int util_optind;
EXTERN char *util_optarg;

EXTERN long getSoftDataLimit ARGS((void));

#endif /* UTIL_H */
