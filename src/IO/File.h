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

#ifndef File_h_
#define File_h_

#include <string>
#include <vector>

#include <OpenVDS/openvds_export.h>
#include <OpenVDS/OpenVDS.h>

namespace OpenVDS
{

class File;

class FileView
{
  int m_nReferenceCount;
protected:
  const void * m_pData;
  int64_t m_nPos;
  int64_t m_nSize;

  FileView()
    : m_nReferenceCount(1)
    , m_pData(nullptr)
    , m_nPos(0)
    , m_nSize(0)
  {
  }
  virtual ~FileView()
  {
  }
public:
  class SystemFileMappingObject
  {
  public:
    static bool Open(SystemFileMappingObject** ppcFileMappingObject, File &file, Error &error);
    static void Close(SystemFileMappingObject** pcFileMappingObject);
  };

  OPENVDS_EXPORT
  const void * Pointer() const
  {
    return m_pData;
  }

  OPENVDS_EXPORT
  int64_t Pos() const
  {
    return m_nPos;
  }

  OPENVDS_EXPORT
  int64_t Size() const
  {
    return m_nSize;
  }

  OPENVDS_EXPORT
  virtual bool Prefetch(const void *pData, int64_t nSize, Error &error) const = 0;

  OPENVDS_EXPORT
  static FileView* AddReference(FileView* pcFileView);

  OPENVDS_EXPORT
  static bool RemoveReference(FileView* pcFileView);
};

// This class is thread-safe except for the following methods:
// open, close, createFileView
// The user must ensure that these methods are called from a single thread only.
class File
{
public:
  OPENVDS_EXPORT File();
  OPENVDS_EXPORT ~File();

  OPENVDS_EXPORT static bool Exists(const std::string& filename);
  OPENVDS_EXPORT bool Open(const std::string& filename, bool isCreate, bool isDestroyExisting, bool isWriteAccess, Error& error);
  OPENVDS_EXPORT void Close();

  OPENVDS_EXPORT int64_t Size(Error& error) const;

  OPENVDS_EXPORT bool Read(void* pxData, int64_t nOffset, int32_t nLength, Error& error) const;
  OPENVDS_EXPORT bool Write(const void* pxData, int64_t nOffset, int32_t nLength, Error& error);

  OPENVDS_EXPORT bool Flush();
  OPENVDS_EXPORT bool IsWriteable() const;
  OPENVDS_EXPORT bool IsOpen() const;
  OPENVDS_EXPORT std::string FileName() const;

  OPENVDS_EXPORT FileView *CreateFileView(int64_t iPos, int64_t nSize, bool isPopulate, Error &error);

  OPENVDS_EXPORT void *Handle() const;
private:
  void* _pxPlatformHandle;
  bool _isWriteable;
  std::string _cFileName;
  FileView::SystemFileMappingObject * m_pFileMappingObject;
};

} // namespace core

#ifdef _WIN32
// From "Reading and Writing From a File View" - https://msdn.microsoft.com/en-us/library/windows/desktop/aa366801(v=vs.85).aspx
//#define FILEVIEW_SIGNAL_HANDLER
#define FILEVIEW_TRY   __try
#define FILEVIEW_CATCH __except (GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
#define FILEVIEW_FINALLY
#else
// From "SIGBUS handling" - https://www.linuxprogrammingblog.com/code-examples/SIGBUS-handling
#include <setjmp.h>

namespace OpenVDS
{
extern "C" void SystemFileView_SetSigBusJmpEnv(sigjmp_buf* pSigjmpEnv);
}
#define FILEVIEW_TRY     { sigjmp_buf sj_env; OpenVDS::SystemFileView_SetSigBusJmpEnv(&sj_env); if (sigsetjmp(sj_env, 1) == 0)
#define FILEVIEW_CATCH   else
#define FILEVIEW_FINALLY OpenVDS::SystemFileView_SetSigBusJmpEnv(NULL); }
#endif

#endif // File_h_
