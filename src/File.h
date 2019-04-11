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

namespace OpenVDS
{

// This class is thread-safe except for the following methods:
// Open, Close
// The user must ensure that these methods are called from a single thread only.
class File
{
public:

  struct Error
  {
    int code = 0;
    std::string string;
  };

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

  File();
  ~File();

  bool open(const std::string& filename, bool isCreate, bool isDestroyExisting, bool isWriteAccess, Error& error);
  void close();

  int64_t size(Error& error) const;

  bool read(void* pxData, int64_t nOffset, int32_t nLength, Error& error);
  bool write(const void* pxData, int64_t nOffset, int32_t nLength, Error& error);

  bool flush();
  bool isWriteable() const;
  bool isOpen() const;
  std::string fileName() const;

private:
  void* _pxPlatformHandle;
  bool _isWriteable;
  std::string _cFileName;
};


} // namespace core

#endif // File_h_
