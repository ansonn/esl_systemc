/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2014 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.accellera.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  scv_ver.h -- Version and copyright information.

  Original Author: Jerome Cornet, STMicroelectronics

 NO AUTOMATIC CHANGE LOG IS GENERATED, EXPLICIT CHANGE LOG AT END OF FILE
 *****************************************************************************/


#ifndef SCV_VER_H
#define SCV_VER_H


#define SCV_SHORT_RELEASE_DATE 20140417

#define SCV_VERSION_ORIGINATOR "Accellera"
#define SCV_VERSION_MAJOR      2
#define SCV_VERSION_MINOR      0
#define SCV_VERSION_PATCH      0
#define SCV_IS_PRERELEASE      0

// token stringification

#define SCV_STRINGIFY_HELPER_( Arg ) \
  SCV_STRINGIFY_HELPER_DEFERRED_( Arg )
#define SCV_STRINGIFY_HELPER_DEFERRED_( Arg ) \
  SCV_STRINGIFY_HELPER_MORE_DEFERRED_( Arg )
#define SCV_STRINGIFY_HELPER_MORE_DEFERRED_( Arg ) \
  #Arg 

#define SCV_VERSION_RELEASE_DATE \
  SCV_STRINGIFY_HELPER_( SCV_SHORT_RELEASE_DATE )

#if ( SCV_IS_PRERELEASE == 1 )
#  define SCV_VERSION_PRERELEASE "pub_rev"
#  define SCV_VERSION \
    SCV_STRINGIFY_HELPER_( SCV_VERSION_MAJOR.SCV_VERSION_MINOR.SCV_VERSION_PATCH ) \
    "_" SCV_VERSION_PRERELEASE "_" SCV_VERSION_RELEASE_DATE \
    "-" SCV_VERSION_ORIGINATOR
#else
#  define SCV_VERSION_PRERELEASE "" // nothing
#  define SCV_VERSION \
    SCV_STRINGIFY_HELPER_( SCV_VERSION_MAJOR.SCV_VERSION_MINOR.SCV_VERSION_PATCH ) \
    "-" SCV_VERSION_ORIGINATOR
#endif

#endif
