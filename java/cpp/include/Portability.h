/*
 * Copyright 2019 The Open Group
 * Copyright 2019 INT, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define __PLATFORM_SparcCompiler 0
#define __PLATFORM_SparcGcc 1
#define __PLATFORM_EgcsLinux 2
#define __PLATFORM_Win32MSVC 3
#define __PLATFORM_Win32MinGW 4
#define __PLATFORM_SgiCompiler 5
#define __PLATFORM_IbmCompiler 6
#define __PLATFORM_HpCompiler 7
#define __PLATFORM_LinuxIcc 8
#define __PLATFORM_MacIcc 9


#ifndef __PLATFORM
#if defined(__SUNPRO_CC)
#define __PLATFORM __PLATFORM_SparcCompiler
#define __PLATFORM_NAME  SparcCompiler
#elif defined(__INTEL_COMPILER)
#if defined(__linux__)
#define __PLATFORM __PLATFORM_LinuxIcc
#define __PLATFORM_NAME  LinuxIcc
#else
#define __PLATFORM __PLATFORM_MacxIcc
#define __PLATFORM_NAME  MacIcc
#endif
#elif defined(__GNUC__)
#if defined(__sparc__)
#define __PLATFORM __PLATFORM_SparcGcc
#define __PLATFORM_NAME  SparcGcc
#elif defined(__linux__)
#define __PLATFORM __PLATFORM_EgcsLinux
#define __PLATFORM_NAME  EgcsLinux
#elif defined(_WIN32)
#define __PLATFORM __PLATFORM_Win32MinGW
#define __PLATFORM_NAME  Win32-MinGW
#elif defined(__APPLE__)
#define __PLATFORM __PLATFORM_MacClang
#define __PLATFORM_NAME  MacClang
#elif defined(__EMSCRIPTEN__)
#define __PLATFORM __PLATFORM_Emscripten
#define __PLATFORM_NAME  Emscripten
#else
#error Unsupported platform for GNU C++ Compiler!
#endif
#elif defined(_MSC_VER)
#define __PLATFORM __PLATFORM_Win32MSVC
#define __PLATFORM_NAME  Win32-MSVC
#elif defined(__sgi)
#define __PLATFORM __PLATFORM_SgiCompiler
#define __PLATFORM_NAME  SgiCompiler
#elif defined(_AIX)
#define __PLATFORM __PLATFORM_IbmCompiler
#define __PLATFORM_NAME  IbmCompiler
#elif defined(__hpux)
#define __PLATFORM __PLATFORM_HpCompiler
#define __PLATFORM_NAME  HpCompiler
#else
#error Unknown platform!
#endif
#endif
