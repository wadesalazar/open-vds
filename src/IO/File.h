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

namespace OpenVDS
{

class File;

struct IOError
{
  int code = 0;
  std::string string;
};

class FileView
{
  int m_nReferenceCount;
protected:
  const void * m_pData;
  int64_t m_nPos;
  int64_t m_nSize;

  FileView()
    : m_nReferenceCount(1)
    , m_nPos(0)
    , m_nSize(0)
    , m_pData(nullptr)
  {
  }
  virtual ~FileView()
  {
  }
public:
  class SystemFileMappingObject
  {
  public:
    static bool open(SystemFileMappingObject** ppcFileMappingObject, File &file, IOError &error);
    static void close(SystemFileMappingObject** pcFileMappingObject);
  };

  OPENVDS_EXPORT
  const void * pointer() const
  {
    return m_pData;
  }

  OPENVDS_EXPORT
  int64_t pos() const
  {
    return m_nPos;
  }

  OPENVDS_EXPORT
  int64_t size() const
  {
    return m_nSize;
  }

  OPENVDS_EXPORT
  virtual bool prefetch(const void *pData, int64_t nSize, IOError &error) const = 0;

  OPENVDS_EXPORT
  static FileView* addReference(FileView* pcFileView);

  OPENVDS_EXPORT
  static bool removeReference(FileView* pcFileView);
};

// This class is thread-safe except for the following methods:
// open, close, createFileView
// The user must ensure that these methods are called from a single thread only.
class File
{
public:
  class CloseGuard
  {
  public:
    CloseGuard(File& file) : _file(file)
    {
    }

    ~CloseGuard()
    {
      if (_file.isOpen())
        _file.close();
    }
  private:
    File& _file;
  };

  OPENVDS_EXPORT File();
  OPENVDS_EXPORT ~File();

  OPENVDS_EXPORT static bool exists(const std::string& filename);
  OPENVDS_EXPORT bool open(const std::string& filename, bool isCreate, bool isDestroyExisting, bool isWriteAccess, IOError& error);
  OPENVDS_EXPORT void close();

  OPENVDS_EXPORT int64_t size(IOError& error) const;

  OPENVDS_EXPORT bool read(void* pxData, int64_t nOffset, int32_t nLength, IOError& error) const;
  OPENVDS_EXPORT bool write(const void* pxData, int64_t nOffset, int32_t nLength, IOError& error);

  OPENVDS_EXPORT bool flush();
  OPENVDS_EXPORT bool isWriteable() const;
  OPENVDS_EXPORT bool isOpen() const;
  OPENVDS_EXPORT std::string fileName() const;

  OPENVDS_EXPORT FileView *createFileView(int64_t iPos, int64_t nSize, bool isPopulate, IOError &error);

  OPENVDS_EXPORT void *handle() const;
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
