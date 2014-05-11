//
//  bind/bind_cc.hpp - support for different calling conventions
//
//  Do not include this header directly.
//
//  Copyright (c) 2001 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/bind/bind.html for documentation.
//

template<class R>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (), sc_bi::list0>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) ())
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) ();
    typedef sc_bi::list0 list_type;
    return sc_bi::bind_t<R, F, list_type> (f, list_type());
}

template<class R, class B1, class A1>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1), typename sc_bi::list_av_1<A1>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1), A1 a1)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1);
    typedef typename sc_bi::list_av_1<A1>::type list_type;
    return sc_bi::bind_t<R, F, list_type> (f, list_type(a1));
}

template<class R, class B1, class B2, class A1, class A2>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1, B2), typename sc_bi::list_av_2<A1, A2>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1, B2), A1 a1, A2 a2)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1, B2);
    typedef typename sc_bi::list_av_2<A1, A2>::type list_type;
    return sc_bi::bind_t<R, F, list_type> (f, list_type(a1, a2));
}

template<class R,
    class B1, class B2, class B3,
    class A1, class A2, class A3>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1, B2, B3), typename sc_bi::list_av_3<A1, A2, A3>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1, B2, B3), A1 a1, A2 a2, A3 a3)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1, B2, B3);
    typedef typename sc_bi::list_av_3<A1, A2, A3>::type list_type;
    return sc_bi::bind_t<R, F, list_type>(f, list_type(a1, a2, a3));
}

template<class R,
    class B1, class B2, class B3, class B4,
    class A1, class A2, class A3, class A4>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1, B2, B3, B4), typename sc_bi::list_av_4<A1, A2, A3, A4>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1, B2, B3, B4), A1 a1, A2 a2, A3 a3, A4 a4)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1, B2, B3, B4);
    typedef typename sc_bi::list_av_4<A1, A2, A3, A4>::type list_type;
    return sc_bi::bind_t<R, F, list_type>(f, list_type(a1, a2, a3, a4));
}

template<class R,
    class B1, class B2, class B3, class B4, class B5,
    class A1, class A2, class A3, class A4, class A5>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1, B2, B3, B4, B5), typename sc_bi::list_av_5<A1, A2, A3, A4, A5>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1, B2, B3, B4, B5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1, B2, B3, B4, B5);
    typedef typename sc_bi::list_av_5<A1, A2, A3, A4, A5>::type list_type;
    return sc_bi::bind_t<R, F, list_type>(f, list_type(a1, a2, a3, a4, a5));
}

template<class R,
    class B1, class B2, class B3, class B4, class B5, class B6,
    class A1, class A2, class A3, class A4, class A5, class A6>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1, B2, B3, B4, B5, B6), typename sc_bi::list_av_6<A1, A2, A3, A4, A5, A6>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1, B2, B3, B4, B5, B6), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1, B2, B3, B4, B5, B6);
    typedef typename sc_bi::list_av_6<A1, A2, A3, A4, A5, A6>::type list_type;
    return sc_bi::bind_t<R, F, list_type>(f, list_type(a1, a2, a3, a4, a5, a6));
}

template<class R,
    class B1, class B2, class B3, class B4, class B5, class B6, class B7,
    class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1, B2, B3, B4, B5, B6, B7), typename sc_bi::list_av_7<A1, A2, A3, A4, A5, A6, A7>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1, B2, B3, B4, B5, B6, B7), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1, B2, B3, B4, B5, B6, B7);
    typedef typename sc_bi::list_av_7<A1, A2, A3, A4, A5, A6, A7>::type list_type;
    return sc_bi::bind_t<R, F, list_type>(f, list_type(a1, a2, a3, a4, a5, a6, a7));
}

template<class R,
    class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8,
    class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1, B2, B3, B4, B5, B6, B7, B8), typename sc_bi::list_av_8<A1, A2, A3, A4, A5, A6, A7, A8>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1, B2, B3, B4, B5, B6, B7, B8), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1, B2, B3, B4, B5, B6, B7, B8);
    typedef typename sc_bi::list_av_8<A1, A2, A3, A4, A5, A6, A7, A8>::type list_type;
    return sc_bi::bind_t<R, F, list_type>(f, list_type(a1, a2, a3, a4, a5, a6, a7, a8));
}

template<class R,
    class B1, class B2, class B3, class B4, class B5, class B6, class B7, class B8, class B9,
    class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    sc_bi::bind_t<R, SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *) (B1, B2, B3, B4, B5, B6, B7, B8, B9), typename sc_bi::list_av_9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::type>
    SC_BOOST_BIND(SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *f) (B1, B2, B3, B4, B5, B6, B7, B8, B9), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
{
    typedef SC_BOOST_BIND_ST R (SC_BOOST_BIND_CC *F) (B1, B2, B3, B4, B5, B6, B7, B8, B9);
    typedef typename sc_bi::list_av_9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::type list_type;
    return sc_bi::bind_t<R, F, list_type>(f, list_type(a1, a2, a3, a4, a5, a6, a7, a8, a9));
}
