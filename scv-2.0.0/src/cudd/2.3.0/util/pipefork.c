/*
 * Revision Control Information
 *
 * $Source: /d2/pit/dlm/cvs-root/scv/src/cudd/2.3.0/util/pipefork.c,v $
 * $Author: dlm $
 * $Revision: 1.1 $
 * $Date: 2002/09/17 21:59:12 $
 *
 */
/* LINTLIBRARY */

#if ! ((defined _MSC_VER) || (defined _WIN32))
#include <sys/wait.h>
#endif

#include "util.h"

#if ! defined(__GNUC__) && ! defined(_MSC_VER)
extern pid_t wait3 ARGS((int *statusp, int options, struct rusage *rusage));
#endif

/*
 * util_pipefork - fork a command and set up pipes to and from
 *
 * Rick L Spickelmier, 3/23/86
 * Richard Rudell, 4/6/86
 * Rick L Spickelmier, 4/30/90, got rid of slimey vfork semantics
 *
 * Returns:
 *   1 for success, with toCommand and fromCommand pointing to the streams
 *   0 for failure
 */

/* ARGSUSED */
int
util_pipefork(
  char **argv,		/* normal argv argument list */
  FILE **toCommand,	/* pointer to the sending stream */
  FILE **fromCommand,	/* pointer to the reading stream */
  int *pid)
{
#ifdef UNIX
    int forkpid, waitPid;
    int topipe[2], frompipe[2];
    char buffer[1024];
#if (defined __hpux) || (defined __osf__) || (defined _IBMR2) || (defined __SVR4) || (defined __CYGWIN32__)
    int status;
#else
    union wait status;
#endif

    /* create the PIPES...
     * fildes[0] for reading from command
     * fildes[1] for writing to command
     */
    (void) pipe(topipe);
    (void) pipe(frompipe);

#ifdef __CYGWIN32__
    if ((forkpid = fork()) == 0) {
#else
    if ((forkpid = vfork()) == 0) {
#endif
	/* child here, connect the pipes */
	(void) dup2(topipe[0], fileno(stdin));
	(void) dup2(frompipe[1], fileno(stdout));

	(void) close(topipe[0]);
	(void) close(topipe[1]);
	(void) close(frompipe[0]);
	(void) close(frompipe[1]);

	(void) execvp(argv[0], argv);
	(void) sprintf(buffer, "util_pipefork: can not exec %s", argv[0]);
	perror(buffer);
	(void) _exit(1);
    }

    if (pid) {
        *pid = forkpid;
    }

#ifdef __CYGWIN32__
    waitPid = waitpid(-1, &status, WNOHANG);
#else
    waitPid = wait3(&status, WNOHANG, NULL);
#endif

    /* parent here, use slimey vfork() semantics to get return status */
    if (waitPid == forkpid && WIFEXITED(status)) {
	return 0;
    }
    if ((*toCommand = fdopen(topipe[1], "w")) == NULL) {
	return 0;
    }
    if ((*fromCommand = fdopen(frompipe[0], "r")) == NULL) {
	return 0;
    }
    (void) close(topipe[0]);
    (void) close(frompipe[1]);
    return 1;
#else
    (void) fprintf(stderr, 
	"util_pipefork: not implemented on your operating system\n");
    return 0;
#endif
}
