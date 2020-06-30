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

#ifndef IOMANAGERVDSFILE_H
#define IOMANAGERVDSFILE_H

#include "IOManager.h"
#include <VDS/ThreadPool.h>
#include <BulkDataStore/HueBulkDataStore.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

namespace OpenVDS
{
  class IOManagerVDSFile : public IOManager
  {
  public:
    enum Mode
    {
      ReadOnly,
      ReadWrite
    };

    IOManagerVDSFile(const VDSFileOpenOptions &openOptions, Mode mode, Error &error);
    ~IOManagerVDSFile() override;

    std::shared_ptr<Request> ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler) override;
    std::shared_ptr<Request> ReadObject(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range = IORange()) override;
    std::shared_ptr<Request> WriteObject(const std::string &objectName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeaders, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback = nullptr) override;
  private:
    std::mutex m_mutex;
    ThreadPool m_threadPool;

    bool m_isVDSObjectFilePresent;
    bool m_isVolumeDataLayoutFilePresent;
    std::map<std::string, HueBulkDataStore::FileInterface *> m_layerFiles;
    std::unique_ptr<HueBulkDataStore, void (*)(HueBulkDataStore *)> m_dataStore;

    std::vector<uint8_t> ReadVolumeDataLayout(Error &error);
    std::vector<uint8_t> ParseVDSObject(std::string const &parseString);
    std::vector<uint8_t> ReadLayerStatus(Error &error);

    bool WriteVolumeDataLayout(std::shared_ptr<std::vector<uint8_t> > data, Error &error);
  };
}

#endif //IOMANAGERVDSFILE_H
