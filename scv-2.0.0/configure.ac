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
dnl  configure.in --
dnl  Process this file with autoconf to produce a configure script.
dnl 
dnl ****************************************************************************
dnl
dnl  Original Authors:
dnl  Norris Ip, Dean Shea, John Rose, Jasvinder Singh, William Paulsen,
dnl  John Pierce, Rachida Kebichi, Ted Elkind, David Bailey,
dnl  Cadence Design Systems, Inc., 2002-09-23
dnl
dnl ****************************************************************************
dnl  
dnl  
dnl  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
dnl  changes you are making here.
dnl
dnl      Name, Affiliation, Date: Eric Paire, Jerome Cornet,
dnl                               STMicroelectronics, 2012-04-27
dnl  Description of Modification: Revamp of autotool build system for 64-bit
dnl                               support, 32-bit compilation on 64-bit host.
dnl
dnl      Name, Affiliation, Date: Stephan Schulz,
dnl                               Fraunhofer IIS-EAS, 2013-02-21
dnl  Description of Modification: Added support for mingw32 and fixed missing
dnl                               '$' in SystemC check.
dnl
dnl      Name, Affiliation, Date: Stephan Schulz,
dnl                               Fraunhofer IIS-EAS, 2013-04-23
dnl  Description of Modification: Added check for Solaris 9/10/11,
dnl                               removed -m8v flag which unsupported by newer
dnl                               gcc compilers.
dnl
dnl      Name, Affiliation, Date: Torsten Maehne,
dnl                               Universite Pierre et Marie Curie, 2013-05-04
dnl  Description of Modification: Added support for Darwin platform (Mac OS X);
dnl                               fixed SCV version number handling, autotools
dnl                               warnings, test execution, and typos.
dnl
dnl ****************************************************************************

AC_INIT([SCV],[2.0.0])
AC_CONFIG_SRCDIR([NEWS])
AC_CONFIG_MACRO_DIR([config])
AC_CONFIG_AUX_DIR([config])

cfgdir=`dirname $0`
CFGDIR=`cd $cfgdir; pwd`

dnl
dnl If dangerous option selected, print warning early, before lots of messages are printed out.
dnl
AC_ARG_ENABLE(compiler-check, [  --disable-compiler-check  set to override the check for supported compilers.  Default is off.],
[case "$enableval" in
no)
	AC_MSG_WARN([Not checking for a supported compiler.  Try this at your own risk.])
	CHECK_COMPILER=no;;
*)
	CHECK_COMPILER=yes;;
esac]
,
CHECK_COMPILER=yes)

dnl
dnl setup relocate option for Beach-head kit
default_beach_dir=/tmp/SCV
AC_ARG_ENABLE(relocate, [  --enable-relocate       enable relocation by placing in /tmp/SCV. Default is off. ],
[case "$enableval" in
yes)
    PATH=${default_beach_dir}:$PATH
    export PATH
    relocate=yes;;
*)
    relocate="";;
esac]
,
relocate="")

dnl
dnl Determine location of extra documentation to be copied into the kit.
dnl
if test "${with_external_documents:-no}" = "no"; then
  pdfDocSrcDir=`pwd`/docs/scv;
else
  pdfDocSrcDir=${with_external_documents}
fi

dnl
dnl Determine location of extra examples to be copied into the kit.
dnl
if test "${with_external_examples:-no}" = "no"; then
  extraExamples="";
else
  extraExamples=${with_external_examples}
fi

dnl
dnl add bottom lines in config.h
dnl
AH_TOP([/*
 * The following code is derived, directly or indirectly, from the SystemC
 * source code Copyright (c) 1996-2014 by all Contributors.
 * All Rights reserved.
 *
 * The contents of this file are subject to the restrictions and limitations
 * set forth in the SystemC Open Source License (the "License");
 * You may not use this file except in compliance with such restrictions and
 * limitations. You may obtain instructions on how to receive a copy of the
 * License at http://www.accellera.org/. Software distributed by Contributors
 * under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 * ANY KIND, either express or implied. See the License for the specific
 * language governing rights and limitations under the License.
 */])
AH_BOTTOM([
#if defined(_MSC_VER)
// int/long are both 32-bit on Win32/Win64 (LLP64)
# if defined(WIN64) || defined(_WIN64)
#   define SIZEOF_INT    4
#   define SIZEOF_LONG   4
#   define SIZEOF_VOID_P 8
# else // Win32
#   define SIZEOF_INT    4
#   define SIZEOF_LONG   4
#   define SIZEOF_VOID_P 4
# endif // Win32/64
#endif // _MSC_VER
])

dnl
dnl This will create in $target something like:
dnl sparc-sun-solaris2.5.1
dnl Currently inca has these directories:  tools.hppa tools.hppa11 tools.sun4v tools.ibmrs
dnl This macro has to be called before AM_INIT_AUTOMAKE
dnl
AC_CANONICAL_SYSTEM

dnl require at least Automake 1.5 to work around bug on HPPA
AM_INIT_AUTOMAKE([1.5])

DEF_GNU_OPT=-O2
DEF_SUN_CC_OPT=-xO1
DEF_HP_ACC_OPT=+O2

DEF_GNU_DBG="-O0 -g"
DEF_SUN_CC_DBG="-xs -g"
DEF_HP_ACC_DBG=-g0

if test "$target_os" = "hpux11.11"; then DEF_HP_ACC_OPT=+O1; fi

if test "$target_os" = "mingw32"; then
	DEF_GNU_OPT+=" -D__MSVCRT_VERSION__=0x601";
	DEF_GNU_DBG+=" -D__MSVCRT_VERSION__=0x601";
fi

dnl
dnl Parameters used in the definition of CXXFLAGS
dnl
dnl The default for DEBUG is -g (for GNU and HP's aCC compiler.)
dnl HP's aCC has -g0, but I can't see what advantage it has, perhaps with instances of objects declared in external libraries
dnl not compiled with -g...
dnl -g on HP aCC is the same as -g1.
dnl
AC_ARG_ENABLE(debug, [  --enable-debug          set to enable debug symbol table production by the C++ compiler.  Default is on.], 
[case "$enableval" in
yes)
	GNU_DBG="$DEF_GNU_DBG"
	SUN_CC_DBG="$DEF_SUN_CC_DBG"
	HP_ACC_DBG="$DEF_HP_ACC_DBG";;
no)
	GNU_DBG=
	SUN_CC_DBG=
	HP_ACC_DBG=;;
*)
	GNU_DBG="$enableval"
	SUN_CC_DBG="$enableval"
	HP_ACC_DBG="$enableval";;
esac]
,
dnl defaults if nothing is specified on the configure command line
GNU_DBG="-g"
SUN_CC_DBG="-xs -g"
HP_ACC_DBG="-g"
)

dnl
dnl I think for gcc the default is -O0 (minimal)
dnl For aCC the default is +O0 (minimal)
dnl
AC_ARG_ENABLE(opt, [  --enable-opt            set to no (O0), O1, yes (O2), etc.  Default is O2.],
[case "$enableval" in
yes)
	GNU_OPT="$DEF_GNU_OPT"
	SUN_CC_OPT="$DEF_SUN_CC_OPT"
	HP_ACC_OPT="$DEF_HP_ACC_OPT";;
no)    
        GNU_OPT=-O0
	SUN_CC_OPT=
	HP_ACC_OPT=;;
*)
        GNU_OPT="$enableval"
	SUN_CC_OPT="$enableval"
	HP_ACC_OPT="$enableval";;
esac]
,
dnl defaults if nothing is specified on the configure command line
GNU_OPT="$DEF_GNU_OPT"
SUN_CC_OPT="$DEF_SUN_CC_OPT"
HP_ACC_OPT="$DEF_HP_ACC_OPT"
)

dnl
dnl Handle option to turn on using insure for program analysis
dnl 
AC_ARG_ENABLE(insure, [  --enable-insure         set to enable the insure run-time analysis system from Parasoft.  Default is off.],
if test "$enableval" = yes; then
	CXX=insure
fi
,
)

dnl
dnl Handle option to turn on using Rational analysis tools (doesn't work for NC)
dnl 
purify_enab=no
quantify_enab=no
purecov_enab=no

AC_ARG_ENABLE(purify, [  --enable-purify         set to enable the purify run-time memory analysis system from Rational.  Default is off.],
  if test "$enableval" = yes; then
    AC_MSG_CHECKING(purify version) 
    purify_ver=`purify -version | tail -1 | sed -e 's/Purify//' |sed -e 's/Version//' |awk '{print $1}' | awk -F. '{print $1"."$2}'`
    if test -z "$purify_ver"; then
      AC_MSG_ERROR(purify executable not found check your path)
    else
dnl NOTE - newer Rational tool versions will result in purify_ver=2001a.04
dnl Allow purify_ver string starting with "200" without further checking.
      new_ver_check=`echo $purify_ver | sed -e 's/^200.*$//'`
      if test -z "$new_ver_check"; then
        AC_MSG_RESULT(yes)
        purify_enab=$enableval
      else
        purify_ok=`echo "{if ($purify_ver >= 5.0) 1;}" | bc`
        if test -n "$purify_ok" -a "$purify_ok" = "1" ; then
          AC_MSG_RESULT(yes)
          purify_enab=$enableval
        else
          AC_MSG_ERROR(purify version later than 5.0 necessary for SCV - version found is $purify_ver)
        fi
      fi
    fi
  fi
,
)
AC_ARG_ENABLE(quantify, [  --enable-quantify       set to enable the quantify run-time memory analysis system from Rational.  Default is off.],
  if test "$enableval" = yes; then
    AC_MSG_CHECKING(quantify version) 
    quantify_ver=`quantify -version | tail -1 | sed -e 's/Quantify//' | sed -e 's/Version//' |awk '{print $1}' | awk -F. '{print $1"."$2}'`
    if test -z "$quantify_ver"; then
      AC_MSG_ERROR(quantify executable not found check your path)
    else
dnl NOTE - newer Rational tool versions will result in quantify_ver=2001a.04
dnl Allow quantify_ver string starting with "200" without further checking.
      new_ver_check=`echo $quantify_ver | sed -e 's/^200.*$//'`
      if test -z "$new_ver_check"; then
        AC_MSG_RESULT(yes)
        quantify_enab=$enableval
      else
        quantify_ok=`echo "{if ($quantify_ver >= 5.0) 1;}" | bc`
        if test -n "$quantify_ok" -a "$quantify_ok" = "1" ; then
          AC_MSG_RESULT(yes)
          quantify_enab=$enableval
        else
          AC_MSG_ERROR(quantify version later than 5.0 necessary for SCV - version found is $quantify_ver)
        fi
      fi
    fi
  fi
,
)

AC_ARG_ENABLE(purecov, [  --enable-purecov        set to enable the purecov run-time memory analysis system from Rational.  Default is off.],
  if test "$enableval" = yes; then
    AC_MSG_CHECKING(purecov version) 
    purecov_ver=`purecov -version | tail -1 | sed -e 's/PureCoverage//' | sed -e 's/Version//' |awk '{print $1}' | awk -F. '{print $1"."$2}'`
    if test -z "$purecov_ver"; then
      AC_MSG_ERROR(purecov executable not found check your path)
    else
dnl NOTE - newer Rational tool versions will result in purecov_ver=2001a.04
dnl Allow purecov_ver string starting with "200" without further checking.
      new_ver_check=`echo $purecov_ver | sed -e 's/^200.*$//'`
      if test -z "$new_ver_check"; then
        AC_MSG_RESULT(yes)
        purecov_enab=$enableval
      else
        purecov_ok=`echo "{if ($purecov_ver >= 5.0) 1;}" | bc`
        if test -n "$purecov_ok" -a "$purecov_ok" = "1" ; then
          AC_MSG_RESULT(yes)
          purecov_enab=$enableval
        else
          AC_MSG_ERROR(purecov version later than 5.0 necessary for SCV - version found is $purecov_ver)
        fi
      fi
    fi
  fi
,
)

dnl
dnl On Linux, producing a 32-bit objects from 64-bit build system requires
dnl "--host=i686-pc-linux-gnu" as configure option. Notice that this setup
dnl must be done before the call to AC_PROG_LIBTOOL.
dnl
AC_MSG_CHECKING(support 32-bit host compilation using 64-bit tools)
case "$build" in
x86_64-*linux*)
	case "$host" in
	i[[34567]]86-*linux*)
		linux_64_32="-m32"
		build_64_32="yes"
		;;
	*)
		linux_64_32=""
		build_64_32="no"
		;;
	esac
	;;
*)
	linux_64_32=""
	build_64_32="no"
	;;
esac
AC_MSG_RESULT(${build_64_32})

AC_CONFIG_HEADER([config/config.h])

AC_PROG_CC
if test -n "${GCC}" -a -n "${linux_64_32}"; then
    CC="${CC} ${linux_64_32}"
fi

AC_PROG_CXX
if test -n "${GXX}" -a -n "${linux_64_32}"; then
    CXX="${CXX} ${linux_64_32}"
fi

dnl
dnl check for pthreads compiler flags.
dnl
AX_PTHREAD

AC_ENABLE_STATIC
AC_ENABLE_SHARED
AC_PROG_LIBTOOL

case "${host}" in
*)
    # On UNIX systems, versioning is done through library name`
    LIBTOOL_LDFLAGS_RELEASE="-release ${PACKAGE_VERSION}"
    LIBTOOL_LDFLAGS=""
    ;;
esac
AC_SUBST([LIBTOOL_LDFLAGS])
AC_SUBST([LIBTOOL_LDFLAGS_RELEASE])

dnl
dnl setup Rational linker options, do this after AC_PROG_CXX because program may be changed
dnl
purecache=`pwd`/purecache
if test "$purify_enab" = "yes" -o "$quantify_enab" = "yes" -o "$purecov_enab" = "yes"; then
  if test "$ac_cv_prog_gxx" = "yes" -a "$CXX" != "g++"; then
dnl rational only understand g++ as the gnu compiler
    AC_MSG_WARN(Rational tools do not understand $CXX as a gnu compiler - setting CXX=g++)
    CXX=g++
  fi
  mkdir -p $purecache
fi
if test -n "$collector"; then
  pure_ld_opts="-best-effort -always-use-cache-dir -thread_stack_change=0x4000 -max-threads=200 -collector=$collector -cache-dir=$purecache"
else
  pure_ld_opts="-best-effort -always-use-cache-dir -thread_stack_change=0x4000 -max-threads=200 -cache-dir=$purecache"
fi

dnl
dnl Set the proper linker switch to get dlopen/dlsym to work with the
dnl GNU linker.  Sun's /usr/ccs/bin/ld does not need a switch.  HP's 
dnl /usr/ccs/bin/ld does need a switch: -E
dnl
dnl Also, need to set the proper option for allowing undefined symbols
dnl in the executable. For aCC the option is -B nonfatal, for
dnl gnu_ld the option is -noinhibit-exec
dnl
dnl On Solaris if /usr/ccs/bin/ld is used with g++, then -shared doesn't
dnl work.  You get messages like:
dnl Text relocation remains                 	referenced
dnl    against symbol		    offset	in file
dnl istream::get(char *, int, char)     0x237c    	/usr/local/lib/gcc-lib/sparc-sun-solaris2.5.1/2.95.1/libstdc++.a(iostream.o)
dnl ...
dnl Use -G instead.  Note that the man page for gcc recommends using -shared instead of -G if possible.
dnl

dnl if MAKE set then check its version; else if make version OK
dnl then set MAKE=make; else if gmake version OK then set
dnl MAKE=gmake

if test -n "$MAKE"; then
  AC_MSG_CHECKING(MAKE for GNU make)
  dnl Extract GNU make version number
  gnumakeversion=`${MAKE} --version 2>&1 | grep "GNU Make" | sed -e 's/[[^0-9]]*\([[0-9.]]*\).*/\1/'`
  gnumakemajorrev=`echo "$gnumakeversion" | sed -e 's/\./ /g' | awk '{print $1}'`
  gnumakeminorrev=`echo "$gnumakeversion" | sed -e 's/\./ /g' | awk '{print $2}'`
  version_error=""
  dnl For non-GNU makes, version string won't contain GNU, so 
  dnl gnumakeversion will be zero-length.
  if test -z "$gnumakeversion"; then
    version_error="non-GNU make"
  else
    if test $gnumakemajorrev -lt 3; then
      version_error="$gnumakeversion"
    elif test $gnumakemajorrev -eq 3 -a $gnumakeminorrev -lt 76; then
      version_error="$gnumakeversion"
    fi
  fi
  if test -z "$version_error"; then
    AC_MSG_RESULT(found $gnumakeversion which is OK)
  else
    AC_MSG_RESULT(found $version_error)
    AC_MSG_ERROR(GNU make 3.76.1 or higher required to build TestBuilder)
  fi
fi

if test -z "$MAKE"; then
  AC_MSG_CHECKING(make for GNU make)
  gnumakeversion=`make --version 2>&1 | grep "GNU Make" | sed -e 's/[[^0-9]]*\([[0-9.]]*\).*/\1/'`
  gnumakemajorrev=`echo "$gnumakeversion" | sed -e 's/\./ /g' | awk '{print $1}'`
  gnumakeminorrev=`echo "$gnumakeversion" | sed -e 's/\./ /g' | awk '{print $2}'`
  version_error=""
  if test -z "$gnumakeversion"; then
    version_error="non-GNU make"
  else
    if test $gnumakemajorrev -lt 3; then
      version_error="$gnumakeversion"
    elif test $gnumakemajorrev -eq 3 -a $gnumakeminorrev -lt 76; then
      version_error="$gnumakeversion"
    fi
  fi
  if test -z "$version_error"; then
    AC_MSG_RESULT(found $gnumakeversion which is OK)
    MAKE=make
  else
    AC_MSG_RESULT(found $version_error)
  fi
fi

if test -z "$MAKE"; then
  AC_MSG_CHECKING(gmake for GNU make)
  gnumakeversion=`gmake --version 2>&1 | grep "GNU Make" | sed -e 's/[[^0-9]]*\([[0-9.]]*\).*/\1/'`
  gnumakemajorrev=`echo "$gnumakeversion" | sed -e 's/\./ /g' | awk '{print $1}'`
  gnumakeminorrev=`echo "$gnumakeversion" | sed -e 's/\./ /g' | awk '{print $2}'`
  version_error=""
  if test -z "$gnumakeversion"; then
    version_error="non-GNU make"
  else
    if test $gnumakemajorrev -lt 3; then
      version_error="$gnumakeversion"
    elif test $gnumakemajorrev -eq 3 -a $gnumakeminorrev -lt 76; then
      version_error="$gnumakeversion"
    fi
  fi
  if test -z "$version_error"; then
    AC_MSG_RESULT(found $gnumakeversion which is OK)
    MAKE=gmake
  else
    AC_MSG_RESULT(found $version_error)
    AC_MSG_ERROR(GNU make 3.76.1 or higher required to build TestBuilder)
  fi
fi

dnl
dnl Set architecture flags
dnl
is_hppa="no"
is_linux="no"
is_sparc="no"
case "$target_cpu" in
sparc)
	is_sparc="yes";;
hppa*)
	is_hppa="yes";;
powerpc*)
	is_linux="yes";;
i[[3456]]86*)
	is_linux="yes";;
x86_64*)
	is_linux="yes";;
*)
	AC_MSG_ERROR(Target CPU type not supported: "$target_cpu");;
esac

AC_MSG_CHECKING(for GNU ld)
if test "$ac_cv_prog_gxx" = "yes"; then
  gxx_archive_lib=`$CXX -print-file-name=libstdc++.a`
  gcclib=`$CXX -print-libgcc-file-name`
  gxx_lib_dirs=`$CXX -print-search-dirs | grep libraries: | sed -e 's/libraries: //'`
  ld_cmd=`$CXX -print-prog-name=ld`
  ld_cmd_out=`$ld_cmd -v 2>&1`
else
  if test "$is_hppa" = "yes"; then
    dnl
    dnl We would only get to here on HP using aCC.
    dnl
    ld_cmd_out=`$CXX -Wl,-v 2>&1`
    ld_cmd=/usr/ccs/bin/ld
  else
    touch foo.o
    ld_cmd_out=`$CXX -Qoption ld -v foo.o 2>&1`
    rm foo.o
  fi
fi

if echo $ld_cmd_out | egrep '(GNU|with BFD)' >/dev/null; then
  AC_MSG_RESULT(yes)
  EXPORT_DYNAMIC=-export-dynamic
  UNDEF_SYMBOLS="-Wl,-noinhibit-exec"
  gnu_ld=yes
else
  AC_MSG_RESULT(no)
  gnu_ld=no
  if test "$is_hppa" = "yes"; then
    UNDEF_SYMBOLS="-Wl,-B,nonfatal"
    EXPORT_DYNAMIC="-Wl,-E"
  else
    EXPORT_DYNAMIC=""
    UNDEF_SYMBOLS=""
  fi
fi

dnl get compiler versions
rh_gxx_prog=no
case "`basename $CXX`" in
  aCC)
    cxx_version=`$CXX -V 2>&1 | head | awk '{print $6;}'`;;
  g++* | *-g++* | c++*)
    cxx_version=`$CXX -dumpversion`
    if test "$is_linux" = "yes"; then
       rh_version=`/usr/bin/g++ -dumpversion`
       if test "$cxx_version" = "$rh_version"; then
         rh_gxx_prog=yes;
       fi
    fi
    ;;
  CC*)
    cxx_version=`$CXX -V 2>&1 | sed "s/CC:.*C++ \([[0-9]]*\.[[0-9]]*\).*/\1/"`;;
  *)
    AC_MSG_WARN(Unrecognized C++ compiler $CXX);;
esac
case "`basename $CC`" in
  gcc*|*-gcc*)
    cc_version=`$CC -dumpversion`;;
  cc|acc)
    cc_version=`$CC -V 2>&1 | sed "s/cc:.*C \([[0-9]]*\.[[0-9]]*\).*/\1/"`;;
  *)
    AC_MSG_WARN(Unrecognized C compiler $CC);;
esac

CXX_VERSION=""
compiler_prefix=""
if test "$CHECK_COMPILER" = "yes"; then
  AC_MSG_CHECKING([for supported C++ compiler])
  case "`basename $CXX`" in
  aCC|insure)
    dnl
    dnl double square brackets are required since one gets eaten by autoconf
    dnl
    SUPPORTED_CXX_VERSIONS="(A.03.39)"
    CXX_VERSION=`$CXX -V 2>&1 | head | sed 's/.*\(A\.[[0-9]]*\.[[0-9]]*\).*/\1/'`
    compiler_prefix="acc"
    ;;
  g++*|*-g++*|c++)
    dnl
    dnl Use $CXX here instead of g++ or c++ in case -V2.95.2 or other options
    dnl are set in the environment variable CXX.
    dnl
    SUPPORTED_CXX_VERSIONS="(3.2|3.4|4.1|4.2|4.3|4.4|4.5|4.6|4.7|4.8)"
    CXX_VERSION=`$CXX -v 2>&1 | grep "gcc version" | sed "s/gcc version \([[0-9]]*\.[[0-9]]*\).*/\1/"`
    compiler_prefix="gcc"
    ;;
  CC)
    SUPPORTED_CXX_VERSIONS="(5.2|5.4)"
    CXX_VERSION=`$CXX -V 2>&1 | sed "s/CC:.*C++ \([[0-9]]*\.[[0-9]]*\).*/\1/"`
    compiler_prefix="sunw"
    ;;
  *)
    AC_MSG_WARN(CXX set to $CXX which is not a supported compiler.  Beware.);;
  esac
fi

if test "$CXX_VERSION" != ""; then
  if `echo $CXX_VERSION | egrep "$SUPPORTED_CXX_VERSIONS" > /dev/null`; then
    AC_MSG_RESULT($compiler_prefix $CXX_VERSION is supported)
  else
    AC_MSG_WARN($compiler_prefix $CXX_VERSION is not a supported version (i.e. $SUPPORTED_CXX_VERSIONS).  Beware.)
  fi
fi

dnl
dnl We support:
dnl   Solaris 2.5, 2.6, 2.7, 2.8, 2.9, 2.10, 2.11
dnl   HP 11
dnl   Linux
dnl   Darwin (Mac OS X >= 10.5)
dnl   MinGW32
dnl
AC_MSG_CHECKING([for supported operating system])
case "$target_os" in solaris2.5*|solaris2.6|solaris2.7|solaris2.8|solaris2.9|solaris2.10|solaris2.11|hpux11*|linux-gnu|darwin*|mingw32)
  AC_MSG_RESULT($target_os is supported);;
*)
  AC_MSG_ERROR($target_os is not supported);;
esac

dnl Enable v2 standard library for HP.
AA_OPT="-AA"

dnl
dnl The switch -mv8 is really Sparc-related, not Solaris-related, since
dnl Solaris can be run on x86 CPUs.
dnl
HPPA_ACC_CXXFLAGS="-ext ${AA_OPT} -Aa -z"
HPPA_GNU_CXXFLAGS="-Wall -mlong-load-store -Wformat"
LINUX_GNU_CXXFLAGS="-Wall -Wformat"
SPARC_CC_CXXFLAGS="-pto"
SPARC_GNU_CXXFLAGS="-Wall -Wformat"

HPPA_ACC_CFLAGS="-ext ${AA_OPT} -Aa -z"
HPPA_GNU_CFLAGS="-Wall -mlong-load-store -Wformat"
LINUX_GNU_CFLAGS="-Wall -Wformat"
SPARC_CC_CFLAGS="-pto"
SPARC_GNU_CFLAGS="-Wall -Wformat"

dnl
dnl In the Bourne shell ;; means "jump to the esac", like "break" in C
dnl
case "$target_cpu" in
sparc)
	echo 'setting compiler flags for sparc'
	muldefs="-Wl,-z,muldefs"
	case "$GXX" in
	yes) echo 'setting compiler flags for g++'
	     AM_CXXFLAGS="$SPARC_GNU_CXXFLAGS"
	     CXXFLAGS="$GNU_OPT $GNU_DBG"
	     CXXFLAGS_DEBUG="$DEF_GNU_DBG"
	     CXXFLAGS_OPT="$DEF_GNU_OPT"
	     AM_CFLAGS="$SPARC_GNU_CFLAGS"
	     CFLAGS="$GNU_OPT $GNU_DBG"
	     CFLAGS_DEBUG="$DEF_GNU_DBG"
	     CFLAGS_OPT="$DEF_GNU_OPT"
	     OPTFLAGS="$GNU_OPT"
	     DBGFLAGS="$GNU_DBG"
	     CXX_NAME=gcc
	     CXX_FULL_VER=`$CXX -dumpversion`
	     CXX_VER=`echo $CXX_FULL_VER | awk -F. '{print $1"."$2}'`
	     collector=`$CXX -print-prog-name=collect2`
	     echo 'setting library options for g++ on Solaris'
	     ;;
	'')  echo 'setting compiler flags for NOT g++'
	     dnl
	     dnl Use -xs to get the stabs (debug symbols) in the object libraries and executables instead of 
	     dnl just in the object files, so that gdb and stack traces work.  dbx uses .stab.index to look 
	     dnl up .stab in the object files and therefore doesn't need this switch (unless you move or 
	     dnl delete your objects!)
	     dnl
	     AM_CXXFLAGS="$SPARC_CC_CXXFLAGS"
	     CXXFLAGS="$SUN_CC_OPT $SUN_CC_DBG"
	     CXXFLAGS_DEBUG="$DEF_SUN_CC_DBG"
	     CXXFLAGS_OPT="$DEF_SUN_CC_OPT"
	     AM_CFLAGS="$SPARC_CC_CFLAGS"
	     CFLAGS="$SUN_CC_OPT $SUN_CC_DBG"
	     CFLAGS_DEBUG="$DEF_SUN_CC_DBG"
	     CFLAGS_OPT="$DEF_SUN_CC_OPT"
	     OPTFLAGS="$SUN_CC_OPT"
	     DBGFLAGS="$SUN_CC_DBG"
	     scvLinkerOpts="$SHARED -library=Cstd -library=Crun $EXPORT_DYNAMIC $UNDEF_SYMBOLS"
	     CXX_NAME=sunw
	     CXX_VER=5.2
             collector=""
	     ;;
        esac;;
hppa*)
	echo 'setting compiler flags for hppa*'
	case "$GXX" in
        yes) echo 'setting compiler flags for g++'
	     AM_CXXFLAGS="$HPPA_GNU_CXXFLAGS"
	     CXXFLAGS="$GNU_OPT $GNU_DBG"
	     CXXFLAGS_DEBUG="$DEF_GNU_DBG"
	     CXXFLAGS_OPT="$DEF_GNU_OPT"
	     AM_CFLAGS="$HPPA_GNU_CFLAGS"
	     CFLAGS="$GNU_OPT $GNU_DBG"
	     CFLAGS_DEBUG="$DEF_GNU_DBG"
	     CFLAGS_OPT="$DEF_GNU_OPT"
             OPTFLAGS="$GNU_OPT"
             DBGFLAGS="$GNU_DBG"
             scvCXXopts="$HPPA_GNU_CXXFLAGS $GNU_DBG"
	     scvLinkerOpts="$HPPA_GNU_CXXFLAGS $SHARED $EXPORT_DYNAMIC $UNDEF_SYMBOLS -Wl,-B,nonfatal -Wl,+s"
             cxxDepFlags="-M -MMD"
	     CXX_NAME=gcc
             collector=`$CXX -print-prog-name=collect2`
	     CXX_FULL_VER=`$CXX -dumpversion`
	     CXX_VER=`echo $CXX_FULL_VER | awk -F. '{print $1"."$2}'`
	     ;;
        '')  echo 'setting compiler flags and defines for HP aCC'
	     AM_CXXFLAGS="$HPPA_ACC_CXXFLAGS"
	     CXXFLAGS="$HP_ACC_OPT $HP_ACC_DBG"
	     CXXFLAGS_DEBUG="$DEF_HP_ACC_DBG"
	     CXXFLAGS_OPT="$DEF_HP_ACC_OPT"
	     AM_CFLAGS="$HPPA_ACC_XFLAGS"
	     CFLAGS="$HP_ACC_OPT $HP_ACC_DBG"
	     CFLAGS_DEBUG="$DEF_HP_ACC_DBG"
	     CFLAGS_OPT="$DEF_HP_ACC_OPT"
             OPTFLAGS=
             DBGFLAGS="$HP_ACC_DBG"
             HP_EXT_OPT="-ext ${AA_OPT} -Aa"
             scvCXXopts="$HPPA_ACC_CXXFLAGS $HP_ACC_DBG"
             cxxDepFlags="+md -P"
	     scvLinkerOpts="$SHARED $EXPORT_DYNAMIC $UNDEF_SYMBOLS -Wl,+s"
	     CXX_NAME=acc
             collector=""
	     CXX_VER=`$CXX -V 2> ver.txt; cat ver.txt | awk '{print $6}' | awk -F. '{print $2"."$3}'; rm ver.txt`
	     echo 'setting library options for HP aCC'
	     ;;
	esac;;
powerpc*)
	echo 'setting compiler flags for Darwin/powerpc)'
	case "$GXX" in
        yes) echo 'setting compiler flags for g++'
	     AM_CXXFLAGS="$LINUX_GNU_CXXFLAGS"
	     CXXFLAGS="$GNU_OPT $GNU_DBG"
	     CXXFLAGS_DEBUG="$DEF_GNU_DBG"
	     CXXFLAGS_OPT="$DEF_GNU_OPT"
	     AM_CFLAGS="$LINUX_GNU_CFLAGS"
	     CFLAGS="$GNU_OPT $GNU_DBG"
	     CFLAGS_DEBUG="$DEF_GNU_DBG"
	     CFLAGS_OPT="$DEF_GNU_OPT"
             OPTFLAGS="$GNU_OPT"
             DBGFLAGS="$GNU_DBG"
             scvCXXopts="$LINUX_GNU_CXXFLAGS $GNU_DBG"
             cxxDepFlags="-M -MMD"
	     scvLinkerOpts="$SHARED $EXPORT_DYNAMIC $UNDEF_SYMBOLS"
	     CXX_NAME=gcc
	     CXX_FULL_VER=`$CXX -dumpversion`
	     CXX_VER=`echo $CXX_FULL_VER | awk -F. '{print $1"."$2}'`
	     echo 'setting library options for g++ on Darwin/powerpc'
	     ;;
        '')  echo 'setting compiler flags for NOT g++'
	     AM_CXXFLAGS="$LINUX_GNU_CXXFLAGS"
	     CXXFLAGS="$GNU_OPT $GNU_DBG"
	     CXXFLAGS_DEBUG="$DEF_GNU_DBG"
	     CXXFLAGS_OPT="$DEF_GNU_OPT"
	     AM_CFLAGS="$LINUX_GNU_CFLAGS"
	     CFLAGS="$GNU_OPT $GNU_DBG"
	     CFLAGS_DEBUG="$DEF_GNU_DBG"
	     CFLAGS_OPT="$DEF_GNU_OPT"
             OPTFLAGS="$GNU_OPT"
             DBGFLAGS="$GNU_DBG"
	     scvCXXopts="$LINUX_GNU_CXXFLAGS $GNU_DBG"
             cxxDepFlags=""
	     scvLinkerOpts="-G $EXPORT_DYNAMIC $UNDEF_SYMBOLS"
	     CXX_NAME=gcc
	     CXX_VER=`$CXX -dumpversion | awk -F. '{print $1"."$2}'`
	     ;;
        esac;;
i[[3456]]86*)
	echo 'setting compiler flags for Linux/i386)'
	case "$GXX" in
        yes) echo 'setting compiler flags for g++'
	     AM_CXXFLAGS="$LINUX_GNU_CXXFLAGS"
	     CXXFLAGS="$GNU_OPT $GNU_DBG"
	     CXXFLAGS_DEBUG="$DEF_GNU_DBG"
	     CXXFLAGS_OPT="$DEF_GNU_OPT"
	     AM_CFLAGS="$LINUX_GNU_CFLAGS"
	     CFLAGS="$GNU_OPT $GNU_DBG"
	     CFLAGS_DEBUG="$DEF_GNU_DBG"
	     CFLAGS_OPT="$DEF_GNU_OPT"
             OPTFLAGS="$GNU_OPT"
             DBGFLAGS="$GNU_DBG"
             scvCXXopts="$LINUX_GNU_CXXFLAGS $GNU_DBG"
             cxxDepFlags="-M -MMD"
	     scvLinkerOpts="$SHARED $EXPORT_DYNAMIC $UNDEF_SYMBOLS"
	     CXX_NAME=gcc
	     CXX_FULL_VER=`$CXX -dumpversion`
	     CXX_VER=`echo $CXX_FULL_VER | awk -F. '{print $1"."$2}'`
	     echo 'setting library options for g++ on Linux/i386'
	     ;;
        '')  echo 'setting compiler flags for NOT g++'
	     AM_CXXFLAGS="$LINUX_GNU_CXXFLAGS"
	     CXXFLAGS="$GNU_OPT $GNU_DBG"
	     CXXFLAGS_DEBUG="$DEF_GNU_DBG"
	     CXXFLAGS_OPT="$DEF_GNU_OPT"
	     AM_CFLAGS="$LINUX_GNU_CFLAGS"
	     CFLAGS="$GNU_OPT $GNU_DBG"
	     CFLAGS_DEBUG="$DEF_GNU_DBG"
	     CFLAGS_OPT="$DEF_GNU_OPT"
             OPTFLAGS="$GNU_OPT"
             DBGFLAGS="$GNU_DBG"
	     scvCXXopts="$LINUX_GNU_CXXFLAGS $GNU_DBG"
             cxxDepFlags=""
	     scvLinkerOpts="-G $EXPORT_DYNAMIC $UNDEF_SYMBOLS"
	     CXX_NAME=gcc
	     CXX_VER=`$CXX -dumpversion | awk -F. '{print $1"."$2}'`
	     ;;
        esac;;
x86_64*)
	echo 'setting compiler flags for Linux/x86_64'
	case "$GXX" in
        yes) echo 'setting compiler flags for g++'
	     AM_CXXFLAGS="$LINUX_GNU_CXXFLAGS"
	     CXXFLAGS="$GNU_OPT $GNU_DBG"
	     CXXFLAGS_DEBUG="$DEF_GNU_DBG"
	     CXXFLAGS_OPT="$DEF_GNU_OPT"
	     AM_CFLAGS="$LINUX_GNU_CFLAGS"
	     CFLAGS="$GNU_OPT $GNU_DBG"
	     CFLAGS_DEBUG="$DEF_GNU_DBG"
	     CFLAGS_OPT="$DEF_GNU_OPT"
             OPTFLAGS="$GNU_OPT"
             DBGFLAGS="$GNU_DBG"
             scvCXXopts="$LINUX_GNU_CXXFLAGS $GNU_DBG"
             cxxDepFlags="-M -MMD"
	     scvLinkerOpts="$SHARED $EXPORT_DYNAMIC $UNDEF_SYMBOLS"
	     CXX_NAME=gcc
	     CXX_FULL_VER=`$CXX -dumpversion`
	     CXX_VER=`echo $CXX_FULL_VER | awk -F. '{print $1"."$2}'`
	     echo 'setting library options for g++ on Linux/x86_64'
	     ;;
        '')  echo 'setting compiler flags for NOT g++'
	     AM_CXXFLAGS="$LINUX_GNU_CXXFLAGS"
	     CXXFLAGS="$GNU_OPT $GNU_DBG"
	     CXXFLAGS_DEBUG="$DEF_GNU_DBG"
	     CXXFLAGS_OPT="$DEF_GNU_OPT"
	     AM_CFLAGS="$LINUX_GNU_CFLAGS"
	     CFLAGS="$GNU_OPT $GNU_DBG"
	     CFLAGS_DEBUG="$DEF_GNU_DBG"
	     CFLAGS_OPT="$DEF_GNU_OPT"
             OPTFLAGS="$GNU_OPT"
             DBGFLAGS="$GNU_DBG"
	     scvCXXopts="$LINUX_GNU_CXXFLAGS $GNU_DBG"
             cxxDepFlags=""
	     scvLinkerOpts="-G $EXPORT_DYNAMIC $UNDEF_SYMBOLS"
	     CXX_NAME=gcc
	     CXX_VER=`$CXX -dumpversion | awk -F. '{print $1"."$2}'`
	     ;;
        esac;;
*)
	AC_MSG_ERROR(Target CPU type not supported: "$target_cpu");;
esac

dnl
dnl Check for basic C++ sanity (hello world in several forms)
dnl
SCV_TEST_CC_SANITY([$scvCXXopts])

dnl
dnl Check C basic type lengths
dnl
old_cflags=${CFLAGS}
CFLAGS="${AM_CFLAGS} ${CFLAGS}"
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([void *])
CFLAGS="${old_cflags}"

dnl
dnl determine which simulators to build for
dnl
AC_ARG_WITH([systemc],
  [AS_HELP_STRING([--with-systemc],
    [path to SystemC])],
  [],
  [with_systemc=""])
case "${with_systemc:-}" in
  "no")  systemc_enab="no";    systemc_path="";;
  "yes") systemc_enab="yes";   systemc_path="";;
  "")    systemc_enab="maybe"; systemc_path="";;
  *)     systemc_enab="yes";   systemc_path="${with_systemc}";;
esac

dnl it is required for the user to configure with --with-systemc
dnl otherwise, will look at the directory above the configure dir
if test -z "$systemc_path"; then
  AC_MSG_CHECKING(for systemc directory)
  if test -d "`pwd`/systemc"; then
    builddir=`pwd`;
    systemc_path=`cd $builddir/systemc; pwd`
  elif test -d "`pwd`/../systemc"; then
    builddir=`pwd`;
    systemc_path=`cd $builddir/../systemc; pwd`
  elif test -d "$CFGDIR/../systemc"; then
    systemc_path=`cd $CFGDIR/../systemc; pwd`
  fi
  if test -z "$systemc_path"; then
    AC_MSG_WARN(SystemC directory was not found)
    AC_MSG_RESULT(searched `pwd`/systemc)
    AC_MSG_RESULT(searched `pwd`/../systemc)
    AC_MSG_RESULT(searched $CFGDIR/../systemc)
    AC_MSG_RESULT(unable to find systemc directory. use --with-systemc=<systemc_dir>)
  else
    AC_MSG_RESULT($systemc_path)
  fi
fi

dnl
dnl Make sure path to SystemC installation exists and is absolute.
dnl

if test "$systemc_enab" != "no"; then
  if test -r "$systemc_path"; then
    systemc_path=`cd $systemc_path;pwd`;
  else
    AC_MSG_ERROR(SystemC installation root directory does not exist.
       Was SystemC installed correctly?)
  fi
fi

dnl
dnl Check for correct SystemC installation
dnl

LDFLAG_RPATH="-Wl,-rpath="
EXTRA_GNU_DIR=""
SYSC_TARGET=""
case "$target" in 
  sparc-sun-solaris*)
    LDFLAG_RPATH="-Wl,-R"
    case "$CXX" in
      CC) SYSC_TARGET="sparcOS5";;
      c++ | g++*) SYSC_TARGET="gccsparcOS5"; EXTRA_GNU_DIR="/gnu";;
      *) SYSC_TARGET="gccsparcOS5";;
    esac
    ;;
  *linux*)
    if test "$rh_gxx_prog" = "no"; then EXTRA_GNU_DIR="/gnu"; fi
    case "$target_cpu" in
      i[[3456]]86*)
        SYSC_TARGET="linux";;
      x86_64*)
        SYSC_TARGET="linux64";;
    esac;;
  *darwin*)
    LDFLAG_RPATH="-Wl,-rpath -Wl,"
    case "$target_cpu" in
      i[[3456]86*])
        SYSC_TARGET="macosx";;
      x86_64*)
        SYSC_TARGET="macosx64";;
      powerpc)
        SYSC_TARGET="macosxppc";;
      powerpc64)
        SYSC_TARGET="macosxppc64";;
    esac;;
  *cygwin*)
    SYSC_TARGET="cygwin";;
  *hpux11*)
    case "$CXX" in
      aCC) SYSC_TARGET="hpux11";;
      *) SYSC_TARGET="gcchpux11"; EXTRA_GNU_DIR="/gnu";;
    esac
    ;;
  *mingw32*)
    SYSC_TARGET="mingw";;
esac

if test "$systemc_enab" != "no"; then
  SYSC_SRC_PREFIX=$systemc_path/src
  lib_found=0
  header_found=0
  if test "$SYSC_TARGET" = ""; then
    AC_MSG_ERROR(Unable to determine the SystemC target architecture from $target)
  fi
  if test $lib_found = 0; then
    SYSC_LIB_PREFIX="$systemc_path/lib-$SYSC_TARGET"
    AC_MSG_CHECKING([for $SYSC_LIB_PREFIX/libsystemc.a])
    if test -r "$SYSC_LIB_PREFIX/libsystemc.a"; then
      lib_found=1
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
  if test $lib_found = 0; then
    SYSC_LIB_PREFIX="$systemc_path/lib$EXTRA_GNU_DIR"
    AC_MSG_CHECKING([for $SYSC_LIB_PREFIX/libsystemc.a])
    if test -r "$SYSC_LIB_PREFIX/libsystemc.a"; then
      lib_found=1
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
  if test $lib_found = 0; then
    SYSC_LIB_PREFIX="$systemc_path/lib-$SYSC_TARGET"
    AC_MSG_CHECKING("for $SYSC_LIB_PREFIX/libsystemc.so")
    if test -r "$SYSC_LIB_PREFIX/libsystemc.so"; then
      lib_found=1
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
  if test $lib_found = 0; then
    SYSC_LIB_PREFIX="$systemc_path/lib$EXTRA_GNU_DIR"
    AC_MSG_CHECKING("for $SYSC_LIB_PREFIX/libsystemc.so")
    if test -r "$SYSC_LIB_PREFIX/libsystemc.so"; then
      lib_found=1
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
  if test $lib_found = 0; then
    SYSC_LIB_PREFIX="$systemc_path/lib-$SYSC_TARGET"
    AC_MSG_CHECKING("for $SYSC_LIB_PREFIX/libsystemc.dylib")
    if test -r "$SYSC_LIB_PREFIX/libsystemc.dylib"; then
      lib_found=1
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
  if test $lib_found = 0; then
    SYSC_LIB_PREFIX="$systemc_path/lib$EXTRA_GNU_DIR"
    AC_MSG_CHECKING("for $SYSC_LIB_PREFIX/libsystemc.dylib")
    if test -r "$SYSC_LIB_PREFIX/libsystemc.dylib"; then
      lib_found=1
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
  if test $header_found = 0; then
    SYSC_INC_PREFIX="$systemc_path/include"
    AC_MSG_CHECKING([for $SYSC_INC_PREFIX/systemc.h])
    if test -r "$SYSC_INC_PREFIX/systemc.h"; then
      header_found=1
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
  if test $header_found = 0; then
    SYSC_INC_PREFIX=`cd $systemc_path/../include; pwd`
    AC_MSG_CHECKING([for $SYSC_INC_PREFIX/systemc.h])
    if test -r "$SYSC_INC_PREFIX/systemc.h"; then
      header_found=1
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
  if test $lib_found = 0 -o $header_found = 0; then
    AC_MSG_ERROR(Could not find all required files for SystemC. 
       Was SystemC installed correctly?)
  fi
fi

dnl
dnl Warn if the detected SystemC version has not been tested with SCV.
dnl
SCV_SC_VERSION_CHECK($SYSC_INC_PREFIX, $SYSC_LIB_PREFIX)
if test ! "$scv_cv_sc_version" = "unknown" -a ! -z "$scv_cv_sc_version"; then
  if test "$scv_cv_sc_version" -lt "2002000" -o "$scv_cv_sc_version" -gt "2003001"; then
    AC_MSG_WARN([SCV has only been tested with Accellera SystemC 2.2.0--2.3.1 (2002000--2003001)])
  fi

  scv_sysc_major=$(( $scv_cv_sc_version/1000000 ))
  scv_sysc_minor=$(( ($scv_cv_sc_version%1000000) / 1000 ))
  scv_sysc_patch=$(( $scv_cv_sc_version%1000 ))
  SCV_REQUIRES_SC_VERSION="= $scv_sysc_major.$scv_sysc_minor.$scv_sysc_patch"
fi
AC_SUBST(SCV_REQUIRES_SC_VERSION)

dnl
dnl set the default prefix and make sure the prefix is absolute
dnl

if test "$prefix" != "NONE"; then
  mkdir -p $prefix
  prefix=`cd $prefix; pwd`
  libdir=$prefix/lib-$SYSC_TARGET
elif test "$systemc_enab" = "no"; then
  prefix=`pwd`
  libdir=$prefix/lib-$SYSC_TARGET
else
  prefix=`cd $systemc_path; pwd`
  libdir=$SYSC_LIB_PREFIX
  includedir=$SYSC_INC_PREFIX
fi

SCV_INC_PREFIX="`dirname $libdir`/include"
SCV_LIB_PREFIX="$libdir" 

dnl
dnl substitution variables
dnl

AC_SUBST(AM_CXXFLAGS)
AC_SUBST(AM_CFLAGS)
AC_SUBST(CFLAGS_DEBUG)
AC_SUBST(CFLAGS_OPT)
AC_SUBST(CXXFLAGS_DEBUG)
AC_SUBST(CXXFLAGS_OPT)
AC_SUBST(DBGFLAGS)
AC_SUBST(EXPORT_DYNAMIC)
AC_SUBST(OPTFLAGS)
AC_SUBST(SCV_INC_PREFIX)
AC_SUBST(SCV_LIB_PREFIX)
AC_SUBST(SYSC_INC_PREFIX)
AC_SUBST(SYSC_LIB_PREFIX)
AC_SUBST(SYSC_SRC_PREFIX)
AC_SUBST(UNDEF_SYMBOLS)
AC_SUBST(cxxDepFlags)
AC_SUBST(extraExamples)
AC_SUBST(pdfDocSrcDir)
AC_SUBST(pure_ld_opts)
AC_SUBST(purecache)
AC_SUBST(relocate)
AC_SUBST(scvLinkerOpts)


dnl
dnl define documentation configuration data
dnl
if test "$prefix" = "NONE"; then
  if test "$relocate" = "yes"; then
    prefixTmp=/tmp/SCV
    if test ! -d $prefixTmp; then mkdir $prefixTmp; fi
  else
    prefixTmp="$ac_default_prefix"
  fi
else
  if test "$relocate" = "yes" -a ! "$default_beach_dir" = "$prefix" ; then
    AC_MSG_ERROR(It is illegal to specify --prefix with --enable-relocate option)
  fi
  AC_MSG_CHECKING(for absolute path prefix and that it exists)
  leading_slash="`echo $prefix | egrep '^/'`"
  if test -z "$leading_slash"; then
    AC_MSG_ERROR(Prefix must be an absolute path)
  else
    if test -d $prefix; then
      AC_MSG_RESULT([yes, OK])
    else
      AC_MSG_ERROR(Prefix must be an existing directory)
    fi;
  fi;
  prefixTmp="$prefix"
fi

AC_MSG_CHECKING([whether documentation needs to be installed])
abs_srcdir=`cd "$srcdir" && pwd`
abs_builddir=`pwd`
AM_CONDITIONAL([SCV_INSTALL_SOURCEDIR],
  [test "x$prefix" = "x$abs_srcdir"])
AM_CONDITIONAL([SCV_INSTALL_BUILDDIR],
  [test "x$prefix" = "x$abs_builddir"])
AM_CONDITIONAL([SCV_INSTALL_NOTSELF],
  [test "x$prefix" != "x$abs_srcdir" -a "x$prefix" != "x$abs_builddir" ])

# manual AM_COND_IF checks for old Automake versions
if test -z "${SCV_INSTALL_SOURCEDIR_TRUE}" ; then
  AC_MSG_RESULT([not needed, install is source dir])
else
  if test -z "${SCV_INSTALL_BUILDDIR_TRUE}" ; then
    AC_MSG_RESULT([not needed, install is build dir])
  else
    AC_MSG_RESULT([yes])
  fi
fi

dnl
dnl list generated Makefiles
dnl

AC_CONFIG_FILES( \
  Makefile \
  docs/Makefile \
  docs/scv/Makefile \
  examples/Makefile \
  examples/data_structures/Makefile \
  examples/data_structures/scv_bag/Makefile \
  examples/data_structures/scv_sparse_array/Makefile \
  examples/extensions/Makefile \
  examples/extensions/ex_01_userdata/Makefile \
  examples/extensions/ex_02_enumdata/Makefile \
  examples/extensions/ex_03_nestdata/Makefile \
  examples/extensions/introspection1/Makefile \
  examples/extensions/private/Makefile \
  examples/general/Makefile \
  examples/general/hello/Makefile \
  examples/randomization/Makefile \
  examples/randomization/constraints/Makefile \
  examples/randomization/debug/Makefile \
  examples/randomization/ex_04_simplerand/Makefile \
  examples/randomization/ex_05_disablerand/Makefile \
  examples/randomization/ex_07_seedfile/Makefile \
  examples/randomization/ex_08_dist/Makefile \
  examples/randomization/ex_09_distrange/Makefile \
  examples/randomization/ex_10_basicconstraint/Makefile \
  examples/randomization/ex_11_softconstraint/Makefile \
  examples/randomization/ex_12_hier_constraint/Makefile \
  examples/randomization/ex_13_custom_next/Makefile \
  examples/randomization/pythagoras/Makefile \
  examples/transactions/Makefile \
  examples/transactions/overview/Makefile \
  src/Makefile \
  src/scv.pc \
  src/cudd/Makefile \
  src/cudd/2.3.0/Makefile \
  src/cudd/2.3.0/cudd/Makefile \
  src/cudd/2.3.0/cudd/doc/Makefile \
  src/cudd/2.3.0/cudd/doc/icons/Makefile \
  src/cudd/2.3.0/dddmp/Makefile \
  src/cudd/2.3.0/dddmp/doc/Makefile \
  src/cudd/2.3.0/mtr/Makefile \
  src/cudd/2.3.0/mtr/doc/Makefile \
  src/cudd/2.3.0/util/Makefile \
  src/cudd/2.3.0/st/Makefile \
  src/cudd/2.3.0/st/doc/Makefile \
  src/cudd/2.3.0/obj/Makefile \
  src/cudd/2.3.0/nanotrav/Makefile \
  src/cudd/2.3.0/nanotrav/doc/Makefile \
  src/cudd/2.3.0/mnemosyne/Makefile \
  src/cudd/2.3.0/sis/Makefile \
  src/scv/Makefile \
)

dnl
dnl Test for rational tools with quickthreads
dnl
if test "$purify_enab" = "yes" -o "$quantify_enab" = "yes" -o "$purecov_enab" = "yes"; then
  AC_MSG_WARN(Rational tools do not work well with quickthreads)
fi

AC_OUTPUT
