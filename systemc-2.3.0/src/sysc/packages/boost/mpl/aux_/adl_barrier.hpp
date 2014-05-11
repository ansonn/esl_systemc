
#ifndef SC_BOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED
#define SC_BOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Source: /Users/acg/CVSROOT/systemc-2.3/src/sysc/packages/boost/mpl/aux_/adl_barrier.hpp,v $
// $Date: 2009/10/14 19:11:05 $
// $Revision: 1.3 $

#include <sysc/packages/boost/mpl/aux_/config/adl.hpp>
#include <sysc/packages/boost/mpl/aux_/config/gcc.hpp>
#include <sysc/packages/boost/mpl/aux_/config/workaround.hpp>

#if !defined(SC_BOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE)

#   define SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE sc_mpl_
#   define SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN namespace sc_mpl_ {
#   define SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE }
#   define SC_BOOST_MPL_AUX_ADL_BARRIER_DECL(type) \
    namespace sc_boost { namespace sc_mpl { \
    using ::SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::type; \
    } } \
/**/

#if !defined(SC_BOOST_MPL_PREPROCESSING_MODE)
namespace SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE { namespace sc_aux {} }
namespace sc_boost { namespace sc_mpl { using namespace SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE; 
namespace sc_aux { using namespace SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::sc_aux; }
}}
#endif

#else // SC_BOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE

#   define SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE sc_boost::sc_mpl
#   define SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN namespace sc_boost { namespace sc_mpl {
#   define SC_BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE }}
#   define SC_BOOST_MPL_AUX_ADL_BARRIER_DECL(type) /**/

#endif

#endif // SC_BOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED
