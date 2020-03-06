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

#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

#include <OpenVDS/OpenVDS.h>
#include "IO/IOManager.h"
#include "IO/File.h"
#include <map>
#include <memory>
#include <mutex>

struct SynchronousDataTransfer : public OpenVDS::TransferDownloadHandler
{
  void HandleMetadata(const std::string &key, const std::string &header) override
  {
  }

  void HandleData(std::vector<uint8_t> &&data) override
  {
    this->data = std::move(data);
  }

  void Completed(const OpenVDS::Request &request, const OpenVDS::Error &error) override
  {
    this->error = error;
  }

  std::vector<uint8_t> data;
  OpenVDS::Error error;
};

struct DataProvider
{
  DataProvider(OpenVDS::File *file)
    : m_file(file)
    , m_ioManager(nullptr)
  {
  }

  DataProvider(OpenVDS::IOManager *ioManager, const std::string &objectName)
    : m_file(nullptr)
    , m_ioManager(ioManager)
    , m_objectName(objectName)
  {
    if (m_ioManager)
    {
      OpenVDS::Error error;
      auto info = m_ioManager->Head(objectName, error);
      if (error.code == 0)
      {
        m_size = info.contentLength;
      }
    }
  }

  bool Read(void* data, int64_t offset, int32_t length, OpenVDS::Error& error) const
  {
    if (m_file)
    {
      return m_file->Read(data, offset, length, error);
    }
    else if (m_ioManager)
    {
      auto dataTransfer = std::make_shared<SynchronousDataTransfer>();
      auto request = m_ioManager->Download(m_objectName, dataTransfer, { offset, offset + length});
      request->WaitForFinish();
      if (dataTransfer->error.code)
      {
        error = dataTransfer->error;
        return false;
      }
      memcpy(data, dataTransfer->data.data(), std::min(size_t(length), dataTransfer->data.size()));
      return true;
    }
    error.code = -1;
    error.string = "Not implemented";
    return false;
  }

  bool Size(OpenVDS::Error &error) const
  {
    if (m_file)
      return m_file->Size(error);

    if (m_ioManager)
      return m_size;
    error.code = -1;
    error.string = "Not implemented";
    return 0;
  }

  std::unique_ptr<OpenVDS::File> m_file;
  std::unique_ptr<OpenVDS::IOManager> m_ioManager;
  const std::string m_objectName;
  int64_t m_size;
};

struct DataView
{

  DataView(DataProvider &dataProvider, int64_t pos, int64_t size, bool isPopulate, OpenVDS::Error &error)
    : m_fileView(nullptr)
    , m_pos(0)
    , m_size(0)
    , m_ref(1)
  {
    if (dataProvider.m_file)
    {
      m_fileView = dataProvider.m_file->CreateFileView(pos, size, isPopulate, error);
    }
    else if (dataProvider.m_ioManager)
    {
      auto dataTransfer = std::make_shared<SynchronousDataTransfer>();
      auto request = dataProvider.m_ioManager->Download(dataProvider.m_objectName, dataTransfer, { pos, pos + size});
      request->WaitForFinish();
      if (dataTransfer->error.code)
      {
        error = dataTransfer->error;
        return;
      }
      m_data = std::move(dataTransfer->data);
      m_pos = pos;
      m_size = size;
    }
    else
    {
      error.code = 2;
      error.string = "Missing data provider";
    }
  }
  ~DataView()
  {
    if (m_fileView)
      OpenVDS::FileView::RemoveReference(m_fileView);
  }

  const void * Pointer() const
  {
    if (m_fileView)
      return m_fileView->Pointer();
    return m_data.data();
  }

  int64_t Pos() const
  {
    if (m_fileView)
      return m_fileView->Pos();
    return m_pos;
  }

  int64_t Size() const
  {
    if (m_fileView)
      return m_fileView->Size();
    return m_size;
  }

  void ref()
  {
    m_ref++;
  }

  bool deref()
  {
    m_ref--;
    return m_ref == 0;
  }

  OpenVDS::FileView *m_fileView;
  std::vector<uint8_t> m_data;
  int64_t m_pos;
  int64_t m_size;
  int m_ref;
};

class DataViewManager
{
public:
  DataViewManager(DataProvider &dataProvider)
    : m_dataProvider(dataProvider)
  {}

  std::shared_ptr<DataView> acquireDataView(int64_t pos, int64_t size, bool isPopulate, OpenVDS::Error& error)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    DataView* dataView = nullptr;

    auto key = DataViewMap::key_type(pos, size);
    auto it = m_dataViewMap.lower_bound(key);
    if (it == m_dataViewMap.end() || it->first != key)
    {
      auto dataView = new DataView(m_dataProvider, pos, size, isPopulate, error);
      it = m_dataViewMap.insert(it, {key, dataView});
    }
    else
    {
      it->second->ref();
    }

    auto ptr = std::shared_ptr<DataView>(it->second, [this](DataView* dataView) { if (dataView) this->releaseDataView(dataView); });
    return ptr;
  }
private:
  typedef std::map<std::pair<int64_t, int64_t>, DataView *> DataViewMap;

  std::mutex m_mutex;
  DataViewMap m_dataViewMap;
  DataProvider &m_dataProvider;

  void releaseDataView(DataView *dataView)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    if (dataView->deref())
    {
      auto it = m_dataViewMap.find(DataViewMap::key_type(dataView->Pos(), dataView->Size()));
      assert(it != m_dataViewMap.end());
      delete it->second;
      m_dataViewMap.erase(it);
    }
  }
};

#endif //DATA_PROVIDER_H
