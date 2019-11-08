/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
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

#include "SharedLibrary.h"

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
#else
  #include <dlfcn.h>
#endif

namespace PyOpenVDS {

SharedLibrary::SharedLibrary() : m_LibraryHandle(nullptr)
{
}

SharedLibrary::~SharedLibrary()
{
  unloadLibrary();
}

bool
SharedLibrary::loadLibrary(const char *libraryName)
{
  if (!m_LibraryHandle)
  {
#ifdef _WIN32
    m_LibraryHandle = (void*)LoadLibraryA(libraryName);
#else
    m_LibraryHandle = dlopen(libraryName, RTLD_NOW | RTLD_LOCAL);
#endif
  }
  return m_LibraryHandle != nullptr;
}

void
SharedLibrary::unloadLibrary()
{
  if (m_LibraryHandle)
  {
#ifdef _WIN32
    FreeLibrary((HMODULE)m_LibraryHandle);
#else
    dlclose(m_LibraryHandle);
#endif
    m_LibraryHandle = nullptr;
  }
}

void *  
SharedLibrary::resolveFunction(const char *functionName)
{
  if (m_LibraryHandle)
  {
#ifdef _WIN32
    return GetProcAddress((HMODULE)m_LibraryHandle, functionName);
#else
    return dlsym(m_LibraryHandle, functionName);
#endif
  }
  return nullptr;
}

}
