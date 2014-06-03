/*
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
 */


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
