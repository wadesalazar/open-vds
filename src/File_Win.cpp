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

namespace OpenVDS {
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

  bool File::open(const std::string& filename, bool isCreate, bool isDestroyExisting, bool isWriteAccess, File::Error& error)
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

    if (_pxPlatformHandle == INVALID_HANDLE_VALUE) {
      error.string = "CreateFile";
      error.code = -1;

      _pxPlatformHandle = 0;
      _cFileName.clear();
      return -1;
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

  int64_t File::size(Error &error) const
  {
    LARGE_INTEGER
      li;

    if (!GetFileSizeEx(_pxPlatformHandle, &li))
    {
      error.string = "GetFileSizeEx" + error_to_string(GetLastError());
      error.code = -1;
      return -1;
    }
    return int64_t(li.QuadPart);
  }

  bool File::read(void* pxData, int64_t nOffset, int32_t nLength, File::Error& error)
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
    if (isOK) {
      assert(nLength == uRead);
    }
    else {
      DWORD uLastError = GetLastError();

      if (uLastError != ERROR_IO_PENDING) {
        error.string = std::string("ReadSync::ReadFile: ") + error_to_string(uLastError);
        error.code = uLastError;
      }
      else {
        // Wait for overlapped IO to finish.
        BOOL isOKGetOverlappedResult = GetOverlappedResult(_pxPlatformHandle, &ol, &uReadOverlapped, TRUE);

        assert(nLength == uReadOverlapped || !isOKGetOverlappedResult);

        if (!isOKGetOverlappedResult) {
          uLastError = GetLastError();
          error.string = std::string("ReadSync::GetOverlappedResult: ") + error_to_string(uLastError);
          error.code = uLastError;
        }

        isOK = isOKGetOverlappedResult;
      }
    }

    return isOK;
  }

  bool File::write(const void* pxData, int64_t nOffset, int32_t nLength, File::Error & error)
  {
    assert(nOffset >= 0);

    if (!_isWriteable) {
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

    if (isOK) {
      assert(nLength == uWritten);
    }
    else {
      DWORD uLastError = GetLastError();

      if (uLastError != ERROR_IO_PENDING) {
        error.string = std::string("WriteSync::WriteFile: ") + error_to_string(uLastError);
        error.code = uLastError;
      }
      else {
        // Wait for overlapped IO to finish.
        BOOL isOKGetOverlappedResult = GetOverlappedResult(_pxPlatformHandle, &ol, &uWrittenOverlapped, TRUE);

        assert(uWrittenOverlapped == nLength);

        if (!isOKGetOverlappedResult) {
          uLastError = GetLastError();
          error.string = std::string("WriteSync::GetOverlappedResult: ") + error_to_string(uLastError);
          error.code = uLastError;
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

} //namespace
