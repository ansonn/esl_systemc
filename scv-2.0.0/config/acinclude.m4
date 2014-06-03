dnl ****************************************************************************
dnl
dnl  The following code is derived, directly or indirectly, from the SystemC
dnl  source code Copyright (c) 1996-2014 by all Contributors.
dnl  All Rights reserved.
dnl
dnl  The contents of this file are subject to the restrictions and limitations
dnl  set forth in the SystemC Open Source License (the "License");
dnl  You may not use this file except in compliance with such restrictions and
dnl  limitations. You may obtain instructions on how to receive a copy of the
dnl  License at http://www.accellera.org/. Software distributed by Contributors
dnl  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
dnl  ANY KIND, either express or implied. See the License for the specific
dnl  language governing rights and limitations under the License.
dnl
dnl ****************************************************************************

dnl
dnl Careful: don't use unquoted commas or double quotes in AC_MSG_ERROR,
dnl          quote with [ and ]
dnl

AC_DEFUN([SCV_SC_VERSION_CHECK],
  [AC_CACHE_CHECK([for SystemC version], [scv_cv_sc_version],
  [dnl prepare environment
   scv_save_cross_compiling="$cross_compiling"
   scv_save_CXXFLAGS="${CXXFLAGS}"
   scv_save_LDFLAGS="${LDFLAGS}"
   scv_save_LIBS="$LIBS"
   cross_compiling=no
   CXXFLAGS="${CXXFLAGS} ${PTHREAD_CFLAGS} -I[$1]"
   LIBS="-lsystemc ${PTHREAD_LIBS} $LIBS"
   LDFLAGS="-L[$2] ${LDFLAG_RPATH}[$2] ${LDFLAGS}"
   AC_LANG_PUSH([C++])
   AC_RUN_IFELSE([AC_LANG_SOURCE([[//
// A program to get the version info from SystemC
//
#include "systemc.h"
#include "stdio.h"
int sc_main(int, char *[])
{
  int maj=0, min=0, kit=0;
  FILE *fout = fopen( "conftest.out", "w" );
  if(!fout) return 1;
  sscanf(sc_version()," SystemC %d.%d.%d ",&maj,&min,&kit);
  fprintf(fout, "%d%03d%03d\n",maj,min,kit);
  fclose(fout);
  return 0;
}       ]])],
        [scv_cv_sc_version="`cat conftest.out`"],
        [AC_MSG_WARN([unable to determine SystemC version])
         scv_cv_sc_version="unknown"])
   dnl restore environment
   CXXFLAGS="$scv_save_CXXFLAGS"
   LDFLAGS="$scv_save_LDFLAGS"
   LIBS="$scv_save_LIBS"
   cross_compiling=$scv_save_cross_compiling
   AC_LANG_POP([C++])
  ]) dnl AC_CACHE_CHECK
])   dnl AC_DEFUN


AC_DEFUN([SCV_TEST_CC_SANITY],
  [AC_CACHE_CHECK([for working C++ compiler], [scv_cv_cc_sanity],
    [dnl prepare environment
     scv_save_CXXFLAGS="${CXXFLAGS}"
     CXXFLAGS="[$1]"
     AC_LANG_PUSH([C++])
     AC_RUN_IFELSE([AC_LANG_SOURCE([[//
// A program to determine if a C++ installation is basically sane
// Use some STL and standard library functions
//
/* HP-UX's aCC doesn't have std defined until it sees a std lib header file */
/* (unlike g++ and SunWorks CC.) */
/* So define it here empty.  Namespaces are extensible, so this is harmless. */
namespace std {}
using namespace std;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
int main()
{
#define STRING_SIZE 100
  char buf[STRING_SIZE];
  int i = 1;
  sprintf(buf, "hello world %i", i);
  if (strcmp(buf,"hello world 1") != 0)
  {
    exit(1);
  }

  ostringstream outString(buf);

  outString << "hello world " << i;
  if (strcmp(buf,"hello world 1") != 0)
  {
    exit(1);
  }

  string str = "hello world";
  str = str + " 1";
  if (str != "hello world 1")
  {
     exit(1);
  }
  //
  // OK!
  //
  exit(0);
}]])],
      [scv_cv_cc_sanity="ok"],
      [scv_cv_cc_sanity="bad"
       AC_MSG_WARN([A bad C++ compiler or library or linker found.])],
      [AC_MSG_WARN([cross-compiling, skipped C++ run-time check])]
      ) dnl AC_RUN_IFELSE
      dnl restore environment
      CXXFLAGS="$scv_save_CXXFLAGS"
      AC_LANG_POP([C++])
    ]) dnl AC_CACHE_CHECK
  ])   dnl AC_DEFUN
