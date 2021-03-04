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
#include <fmt/format.h>

#include <sys/stat.h>
#include <time.h>
#define stat _stat
#define gmtime_r(a,b) _gmtime64_s(b,a)

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

static std::string ErrorToString(DWORD error)
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

static void SetIoError(DWORD error, Error &io_error)
{
  io_error.code = error;
  io_error.string = ErrorToString(error);
}

template<size_t N>
void SetIoError(DWORD error, const char (&error_string_prefix)[N], Error &io_error)
{
  io_error.code = error;
  io_error.string = std::string(error_string_prefix, N - 1) + ErrorToString(error);
}

bool FileView::SystemFileMappingObject::Open(SystemFileMappingObject** ppcFileMappingObject, File& file, Error& error)
{
  assert(ppcFileMappingObject && !*ppcFileMappingObject);

  HANDLE
    hFile = file.Handle();

  *ppcFileMappingObject = reinterpret_cast<SystemFileMappingObject*>(CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL));

  if (!*ppcFileMappingObject)
  {
    SetIoError(GetLastError(), error);
    return false;
  }
  return true;
}

void FileView::SystemFileMappingObject::Close(SystemFileMappingObject** ppcFileMappingObject)
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

  static DWORD StaticPageSize()
  {
    SYSTEM_INFO
      siSysInfo;

    GetSystemInfo(&siSysInfo);

    return siSysInfo.dwAllocationGranularity;
  }

  static PrefetchVirtualMemory_pf StaticGetPrefetchVirtualMemoryProcAddress()
  {
    HMODULE hKernel32 = LoadLibraryW(L"Kernel32");
    if (!hKernel32)
      return nullptr;

    FARPROC pfFARPROC = GetProcAddress(hKernel32, "PrefetchVirtualMemory");

    return reinterpret_cast<PrefetchVirtualMemory_pf>(pfFARPROC);
  }

public:
  SystemFileView(SystemFileMappingObject* pFileMappingObject, int64_t nPos, int64_t nSize, bool isPopulate, Error &error)
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

      SetIoError(GetLastError(), error);
      return;
    }

    if (isPopulate)
    {
      Prefetch(m_pData, m_nSize, error);
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

  bool Prefetch(const void* pData, int64_t nSize, Error &error ) const override
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
      SetIoError(GetLastError(), error);
      return false;
    }

    return true;
  }
};

SystemFileView::PrefetchVirtualMemory_pf SystemFileView::s_pfPrefetchVirtualMemory = StaticGetPrefetchVirtualMemoryProcAddress();
DWORD SystemFileView::s_dwPageSize = SystemFileView::StaticPageSize();

bool File::Exists(const std::string& filename)
{
  std::wstring native_name;
  s2ws(filename, native_name);

  DWORD result = GetFileAttributesW(native_name.c_str());

  return (result != INVALID_FILE_ATTRIBUTES) && ((result & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool File::Open(const std::string& filename, bool isCreate, bool isDestroyExisting, bool isWriteAccess, Error& error)
{
  assert(!IsOpen());
  assert(!isDestroyExisting || isCreate);
  assert(!isCreate || isWriteAccess || !"it is meaningless to demand creation with RO access");
  assert(!_pxPlatformHandleRead && "RawFileAccess_c::Open: file already open");
  assert(!_pxPlatformHandleReadWrite && "RawFileAccess_c::Open: file already open");

  _cFileName = filename;

  DWORD dwCreationDisposition = isCreate ? (isDestroyExisting ? CREATE_ALWAYS : CREATE_NEW) : OPEN_EXISTING;

  std::wstring native_name;
  s2ws(_cFileName, native_name);
  _pxPlatformHandleRead = CreateFileW(
    native_name.c_str(),
    GENERIC_READ | (isWriteAccess ? GENERIC_WRITE : 0),
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    dwCreationDisposition,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_OVERLAPPED,
    NULL);

  if (_pxPlatformHandleRead == INVALID_HANDLE_VALUE)
  {
    SetIoError(GetLastError(), "File::Open: ", error);

    _pxPlatformHandleRead = 0;
    _cFileName.clear();
    return false;
  }

  if(isWriteAccess)
  {
    _pxPlatformHandleReadWrite = _pxPlatformHandleRead;
  }

  return true;
}

void File::Close()
{
  assert(IsOpen());

  if(_pxPlatformHandleRead != _pxPlatformHandleReadWrite)
  {
    CloseHandle(_pxPlatformHandleRead);
  }
  if(_pxPlatformHandleReadWrite)
  {
    CloseHandle(_pxPlatformHandleReadWrite);
  }

  _pxPlatformHandleRead = 0;
  _pxPlatformHandleReadWrite = 0;

  _cFileName.clear();
}

bool File::EnableWriting(Error& error)
{
  assert(IsOpen());

  if(IsWriteable())
  {
    return true;
  }

  DWORD dwCreationDisposition = OPEN_EXISTING;

  std::wstring native_name;
  s2ws(_cFileName, native_name);
  _pxPlatformHandleReadWrite = CreateFileW(
    native_name.c_str(),
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    dwCreationDisposition,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_OVERLAPPED,
    NULL);

  if (_pxPlatformHandleReadWrite == INVALID_HANDLE_VALUE)
  {
    SetIoError(GetLastError(), "File::EnableWriting: ", error);

    _pxPlatformHandleReadWrite = 0;
    return false;
  }

  return true;
}

int64_t File::Size(Error& error) const
{
  LARGE_INTEGER
    li;

  if (!GetFileSizeEx(_pxPlatformHandleRead, &li))
  {
    SetIoError(GetLastError(), "GetFileSizeEx: ", error);
    return -1;
  }
  return int64_t(li.QuadPart);
}

std::string File::LastWriteTime(Error& error) const
{
  FILETIME lastWriteTime;

  if(!GetFileTime(_pxPlatformHandleRead, NULL, NULL, &lastWriteTime))
  {
    SetIoError(GetLastError(), "GetFileTime: ", error);
    return std::string();
  }

  SYSTEMTIME systemTime;

  if(!FileTimeToSystemTime(&lastWriteTime, &systemTime))
  {
    SetIoError(GetLastError(), "FileTimeToSystemTime: ", error);
    return std::string();
  }

  return fmt::format("{:04d}-{:02d}-{:02d}T{:02d}:{:02d}:{:02d}.{:03d}Z", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
}

bool File::Read(void* pxData, int64_t nOffset, int32_t nLength, Error& error) const
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

  BOOL isOK = ReadFile(_pxPlatformHandleRead, pxData, nLength, &uRead, &ol) && uRead == (DWORD)nLength;

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
      SetIoError(uLastError, "ReadSync::ReadFile: ", error);
    }
    else
    {
      // Wait for overlapped IO to finish.
      BOOL isOKGetOverlappedResult = GetOverlappedResult(_pxPlatformHandleRead, &ol, &uReadOverlapped, TRUE);

      assert(nLength == uReadOverlapped || !isOKGetOverlappedResult);

      if (!isOKGetOverlappedResult)
      {
        SetIoError(GetLastError(), "ReadSync::GetOverlappedResult: ", error);
      }

      isOK = isOKGetOverlappedResult;
    }
  }

  return isOK == TRUE;
}

bool File::Write(const void* pxData, int64_t nOffset, int32_t nLength, Error & error)
{
  assert(nOffset >= 0);

  if (!_pxPlatformHandleReadWrite)
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
    isOK = WriteFile(_pxPlatformHandleReadWrite, pxData, nLength, &uWritten, &ol) && uWritten == (DWORD)nLength;

  if (isOK)
  {
    assert(nLength == uWritten);
  }
  else
  {
    DWORD uLastError = GetLastError();

    if (uLastError != ERROR_IO_PENDING)
    {
      SetIoError(uLastError, "WriteSync::WriteFile: ", error);
    }
    else
    {
      // Wait for overlapped IO to finish.
      BOOL isOKGetOverlappedResult = GetOverlappedResult(_pxPlatformHandleReadWrite, &ol, &uWrittenOverlapped, TRUE);

      assert(uWrittenOverlapped == nLength);

      if (!isOKGetOverlappedResult)
      {
        SetIoError(GetLastError(), "WriteSync::GetOverlappedResult: ", error);
      }

      isOK = isOKGetOverlappedResult;
    }
  }

  return isOK == TRUE;
}

bool File::Flush()
{
  if(!IsWriteable()) return true;

  BOOL isOK = FlushFileBuffers(_pxPlatformHandleReadWrite);
  return isOK == TRUE;
}

FileView *File::CreateFileView(int64_t nPos, int64_t nSize, bool isPopulate, Error &error)
{
  if(!m_pFileMappingObject)
  {
    if (!FileView::SystemFileMappingObject::Open(&m_pFileMappingObject, *this, error))
      return nullptr;
  }
  FileView *ret = new SystemFileView(m_pFileMappingObject, nPos, nSize, isPopulate, error);
  if (error.code)
  {
    bool deleted = FileView::RemoveReference(ret);
    assert(deleted);
    ret = nullptr;
  }
  return ret;
}

} //namespace
