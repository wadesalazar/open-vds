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

#include "File.h"
#include <assert.h>

#undef WIN32_LEAN_AND_MEAN // avoid warnings if defined on command line
#define WIN32_LEAN_AND_MEAN

#include <io.h>
#include <windows.h>

namespace OpenVDS
{
static void s2ws(const std::string& source, std::wstring& target)
{
  int len;
  int slength = (int)source.length() + 1;
  len = MultiByteToWideChar(CP_UTF8, 0, source.c_str(), slength, 0, 0);
  target.resize(len);
  MultiByteToWideChar(CP_UTF8, 0, source.c_str(), slength, &target[0], len);
}

static void sw2s(const std::wstring& source, std::string& target)
{
  int len;
  int slength = (int)source.length() + 1;
  len = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), slength, 0, 0, 0, false);
  target.resize(len);
  WideCharToMultiByte(CP_UTF8, 0, source.c_str(), slength, &target[0], len, 0, false);
}

static std::string error_to_string(DWORD error)
{
  LPVOID lpMsgBuf;

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    error,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)& lpMsgBuf,
    0, NULL);

  std::string ret((const char*)lpMsgBuf, strlen((const char*)lpMsgBuf));
  LocalFree(lpMsgBuf);
  return ret;
}

static void set_io_error(DWORD error, IOError &io_error)
{
  io_error.code = error;
  io_error.string = error_to_string(error);
}

static void set_io_error(DWORD error, std::string &error_string_prefix, IOError &io_error)
{
  io_error.code = error;
  io_error.string = error_string_prefix + error_to_string(error);
}

template<size_t N>
static void set_io_error(DWORD error, const char (&error_string_prefix)[N], IOError &io_error)
{
  io_error.code = error;
  io_error.string = std::string(error_string_prefix, N) + error_to_string(error);
}

bool FileView::SystemFileMappingObject::open(SystemFileMappingObject** ppcFileMappingObject, File& file, IOError& error)
{
  assert(ppcFileMappingObject && !*ppcFileMappingObject);

  HANDLE
    hFile = file.handle();

  *ppcFileMappingObject = reinterpret_cast<SystemFileMappingObject*>(CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL));

  if (!*ppcFileMappingObject)
  {
    set_io_error(GetLastError(), error);
    return false;
  }
  return true;
}

void FileView::SystemFileMappingObject::close(SystemFileMappingObject** ppcFileMappingObject)
{
  assert(ppcFileMappingObject && *ppcFileMappingObject);

  BOOL
    isSuccess = CloseHandle(reinterpret_cast<HANDLE>(*ppcFileMappingObject));

  if (!isSuccess)
  {
    DWORD
      dwError = GetLastError();

    return (void)dwError;
  }
}

class SystemFileView : public FileView
{
  typedef BOOL(*PrefetchVirtualMemory_pf)(HANDLE, ULONG_PTR, PWIN32_MEMORY_RANGE_ENTRY, ULONG);

  static PrefetchVirtualMemory_pf s_pfPrefetchVirtualMemory;

  static DWORD          s_dwPageSize;

  LPVOID                m_lpBaseAddress;

  static DWORD
    staticPageSize()
  {
    SYSTEM_INFO
      siSysInfo;

    GetSystemInfo(&siSysInfo);

    return siSysInfo.dwAllocationGranularity;
  }

  static PrefetchVirtualMemory_pf
    SystemFileView::staticGetPrefetchVirtualMemoryProcAddress()
  {
    HMODULE hKernel32 = LoadLibraryW(L"Kernel32");
    if (!hKernel32)
      return nullptr;

    FARPROC pfFARPROC = GetProcAddress(hKernel32, "PrefetchVirtualMemory");

    return reinterpret_cast<PrefetchVirtualMemory_pf>(pfFARPROC);
  }

public:
  SystemFileView(SystemFileMappingObject* pFileMappingObject, int64_t nPos, int64_t nSize, bool isPopulate, IOError &error)
  {
    int64_t nDelta = nPos % s_dwPageSize;

    LARGE_INTEGER
      lPos;

    lPos.QuadPart = nPos - nDelta;

    SIZE_T
      dwNumberOfBytes = (SIZE_T)(nSize + nDelta);

    m_lpBaseAddress = MapViewOfFile(reinterpret_cast<HANDLE>(pFileMappingObject), FILE_MAP_READ, lPos.HighPart, lPos.LowPart, dwNumberOfBytes);
    m_pData = m_lpBaseAddress ? reinterpret_cast<const unsigned char*>(m_lpBaseAddress) + nDelta : NULL;
    m_nPos = nPos;
    m_nSize = nSize;

    if (!m_lpBaseAddress)
    {
      m_pData = NULL;

      set_io_error(GetLastError(), error);
      return;
    }

    if (isPopulate)
    {
      prefetch(m_pData, m_nSize, error);
    }
  }

  ~SystemFileView()
  {
    BOOL isSuccess = UnmapViewOfFile(m_lpBaseAddress);

    if (!isSuccess)
    {
      DWORD
        dwError = GetLastError();

      (void)dwError; return;
    }
  }

  bool prefetch(const void* pData, int64_t nSize, IOError &error ) const override
  {
    if (!s_pfPrefetchVirtualMemory)
    {
      error.code = -256;
      error.string = "Prefetch: state error.";
      return false;
    }
    int64_t nDelta = (int64_t)((uintptr_t)pData % s_dwPageSize);

    pData = static_cast<const uint8_t*>(pData) - nDelta;
    nSize = ((nSize + nDelta + s_dwPageSize - 1) / s_dwPageSize) * s_dwPageSize;

    WIN32_MEMORY_RANGE_ENTRY
      sMemoryRange;

    sMemoryRange.VirtualAddress = (PVOID)pData;
    sMemoryRange.NumberOfBytes = (SIZE_T)nSize;

    BOOL isSuccess = s_pfPrefetchVirtualMemory(GetCurrentProcess(), 1, &sMemoryRange, 0);

    if (!isSuccess)
    {
      set_io_error(GetLastError(), error);
      return false;
    }

    return true;
  }
};

SystemFileView::PrefetchVirtualMemory_pf SystemFileView::s_pfPrefetchVirtualMemory = SystemFileView::staticGetPrefetchVirtualMemoryProcAddress();
DWORD SystemFileView::s_dwPageSize = SystemFileView::staticPageSize();

bool File::open(const std::string& filename, bool isCreate, bool isDestroyExisting, bool isWriteAccess, IOError& error)
{
  assert(!isDestroyExisting || isCreate);
  assert(!isCreate || isWriteAccess || !"it is meaningless to demand creation with RO access");
  assert(!_pxPlatformHandle || ("RawFileAccess_c::Open: file already open"));

  _cFileName = filename;

  DWORD dwCreationDisposition = isCreate ? (isDestroyExisting ? CREATE_ALWAYS : CREATE_NEW) : OPEN_EXISTING;

  std::wstring native_name;
  s2ws(_cFileName, native_name);
  _pxPlatformHandle = CreateFileW(
    native_name.c_str(),
    GENERIC_READ | (isWriteAccess ? GENERIC_WRITE : 0),
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    dwCreationDisposition,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_OVERLAPPED,
    NULL);

  if (_pxPlatformHandle == INVALID_HANDLE_VALUE)
  {
    set_io_error(GetLastError(), "File::Open: ", error);

    _pxPlatformHandle = 0;
    _cFileName.clear();
    return false;
  }

  _isWriteable = isWriteAccess;

  return true;
}

void File::close()
{
  assert(isOpen());

  BOOL isOK = CloseHandle(_pxPlatformHandle);

  _pxPlatformHandle = 0;
  _cFileName.clear();
}

int64_t File::size(IOError& error) const
{
  LARGE_INTEGER
    li;

  if (!GetFileSizeEx(_pxPlatformHandle, &li))
  {
    set_io_error(GetLastError(), "GetFileSizeEx: ", error);
    return -1;
  }
  return int64_t(li.QuadPart);
}

bool File::read(void* pxData, int64_t nOffset, int32_t nLength, IOError& error)
{
  assert(nOffset >= 0);

  OVERLAPPED ol;
  memset(&ol, 0, sizeof(ol));

  ol.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  ol.Offset = (DWORD)nOffset;
  ol.OffsetHigh = (DWORD)(nOffset >> 32);

  DWORD
    uRead,
    uReadOverlapped;

  BOOL isOK = ReadFile(_pxPlatformHandle, pxData, nLength, &uRead, &ol) && uRead == (DWORD)nLength;

  // isOK might be true if ReadFile finishes immediately
  if (isOK)
  {
    assert(nLength == uRead);
  }
  else
  {
    DWORD uLastError = GetLastError();

    if (uLastError != ERROR_IO_PENDING)
    {
      set_io_error(uLastError, "ReadSync::ReadFile: ", error);
    }
    else
    {
      // Wait for overlapped IO to finish.
      BOOL isOKGetOverlappedResult = GetOverlappedResult(_pxPlatformHandle, &ol, &uReadOverlapped, TRUE);

      assert(nLength == uReadOverlapped || !isOKGetOverlappedResult);

      if (!isOKGetOverlappedResult)
      {
        set_io_error(GetLastError(), "ReadSync::GetOverlappedResult: ", error);
      }

      isOK = isOKGetOverlappedResult;
    }
  }

  return isOK;
}

bool File::write(const void* pxData, int64_t nOffset, int32_t nLength, IOError & error)
{
  assert(nOffset >= 0);

  if (!_isWriteable)
  {
    error.code = -1;
    error.string = "RawFileAccess_c::WriteSync: file not writeable";
    return false;
  }

  OVERLAPPED ol;
  memset(&ol, 0, sizeof(ol));

  DWORD
    uWritten,
    uWrittenOverlapped;

  ol.Offset = (DWORD)nOffset;
  ol.OffsetHigh = (DWORD)(nOffset >> 32);

  BOOL
    isOK
    = WriteFile(_pxPlatformHandle, pxData, nLength, &uWritten, &ol) && uWritten == (DWORD)nLength;

  if (isOK)
  {
    assert(nLength == uWritten);
  }
  else
  {
    DWORD uLastError = GetLastError();

    if (uLastError != ERROR_IO_PENDING)
    {
      set_io_error(uLastError, "WriteSync::WriteFile: ", error);
    }
    else
    {
      // Wait for overlapped IO to finish.
      BOOL isOKGetOverlappedResult = GetOverlappedResult(_pxPlatformHandle, &ol, &uWrittenOverlapped, TRUE);

      assert(uWrittenOverlapped == nLength);

      if (!isOKGetOverlappedResult)
      {
        set_io_error(GetLastError(), "WriteSync::GetOverlappedResult: ", error);
      }

      isOK = isOKGetOverlappedResult;
    }
  }

  return isOK;
}

bool File::flush()
{
  if (!_isWriteable)
    return false;

  BOOL isOK = FlushFileBuffers(_pxPlatformHandle);
  return isOK;
}

FileView *File::createFileView(int64_t nPos, int64_t nSize, bool isPopulate, IOError &error)
{
  if(!m_pFileMappingObject)
  {
    if (!FileView::SystemFileMappingObject::open(&m_pFileMappingObject, *this, error))
      return false;
  }
  FileView *ret = new SystemFileView(m_pFileMappingObject, nPos, nSize, isPopulate, error);
  if (error.code)
  {
    bool deleted = FileView::removeReference(ret);
    assert(deleted);
    ret = nullptr;
  }
  return ret;
}

} //namespace
