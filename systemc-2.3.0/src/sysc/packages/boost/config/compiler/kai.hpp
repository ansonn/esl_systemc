//  (C) Copyright John Maddock 2001. 
//  (C) Copyright David Abrahams 2002. 
//  (C) Copyright Aleksey Gurtovoy 2002. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Kai C++ compiler setup:

#include "sysc/packages/boost/config/compiler/common_edg.hpp"

#   if (__KCC_VERSION <= 4001) || !defined(SC_BOOST_STRICT_CONFIG)
      // at least on Sun, the contents of <cwchar> is not in namespace std
#     define SC_BOOST_NO_STDC_NAMESPACE
#   endif

// see also common_edg.hpp which needs a special check for __KCC
# if !defined(_EXCEPTIONS)
#     define SC_BOOST_NO_EXCEPTIONS
# endif

#define SC_BOOST_COMPILER "Kai C++ version " SC_BOOST_STRINGIZE(__KCC_VERSION)

//
// last known and checked version is 4001:
#if (__KCC_VERSION > 4001)
#  if defined(SC_BOOST_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif



