/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#ifndef COMPILER_DEFINES
#define COMPILER_DEFINES

#ifdef _DEBUG
#   define force_inline inline
#else
#   if defined _MSC_VER
#      define force_inline __forceinline
#   elif defined __GNUC__
#      define force_inline __attribute__((always_inline)) inline
#   else
#      define force_inline inline
#   endif
#endif

#define CALC_BIT_SHIFT(IVAL)       (FIND_SHIFT<IVAL>::RET)

#if defined(__GNUC__) && __GNUC__ >= 7
#define FALLTHROUGH __attribute__ ((fallthrough))
#else
#define FALLTHROUGH ((void)0)
#endif /* __GNUC__ >= 7 */

#endif