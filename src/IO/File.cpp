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

namespace OpenVDS
{

FileView * FileView::addReference(FileView *pcFileView)
{
  assert(pcFileView);

  pcFileView->m_nReferenceCount++;

  assert(pcFileView->m_nReferenceCount > 0);
  return pcFileView;
}

bool FileView::removeReference(FileView *pcFileView)
{
  assert(pcFileView);
  assert(pcFileView->m_nReferenceCount > 0);

  if(--pcFileView->m_nReferenceCount == 0)
  {
    delete pcFileView;
    return true;
  }

  return false;
}

File::File()
  : _pxPlatformHandle(nullptr)
  , _isWriteable(false)
  , m_pFileMappingObject(nullptr)
{
}

File::~File()
{
  if (isOpen())
  {
    close();
  }
}

bool File::isWriteable() const
{
  return _isWriteable;
}

bool File::isOpen() const
{
  assert(!!_pxPlatformHandle == !_cFileName.empty());
  return _pxPlatformHandle;
}

std::string File::fileName() const
{
  return _cFileName;
}

void *File::handle() const
{
  return _pxPlatformHandle;
}

} // namespace OpenVDS
