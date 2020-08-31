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

#ifndef OPENVDS_GLOBAL_STATE_IMPL_H
#define OPENVDS_GLOBAL_STATE_IMPL_H

#include <OpenVDS/GlobalState.h>

#include <atomic>

namespace OpenVDS
{
  class GlobalStateImpl : public GlobalState
  {
  public:
    GlobalStateImpl()
    {
      for (auto &i : downloaded)
        i = 0;
      for (auto &i : downloadedChunks)
        i = 0;
      for (auto &i : decompressed)
        i = 0;
      for (auto &i : decompressedChunks)
        i = 0;
    }
    std::atomic<uint64_t> downloaded[OpenOptions::ConnectionTypeCount];
    std::atomic<uint64_t> downloadedChunks[OpenOptions::ConnectionTypeCount];
    std::atomic<uint64_t> decompressed[OpenOptions::ConnectionTypeCount];
    std::atomic<uint64_t> decompressedChunks[OpenOptions::ConnectionTypeCount];

    uint64_t GetBytesDownloaded(OpenOptions::ConnectionType connectionType) override
    {
      return downloaded[connectionType];
    }
    uint64_t GetChunksDownloaded(OpenOptions::ConnectionType connectionType) override
    {
      return downloadedChunks[connectionType];
    }
    uint64_t GetBytesDecompressed(OpenOptions::ConnectionType connectionType) override
    {
      return decompressed[connectionType];
    }
    uint64_t GetChunksDecompressed(OpenOptions::ConnectionType connectionType) override
    {
      return decompressedChunks[connectionType];
    }
  };

  class GlobalStateVds
  {
  public:
    GlobalStateVds(std::atomic<uint64_t> &downloaded, std::atomic<uint64_t> &downloadedChunks, std::atomic<uint64_t> &decompressed, std::atomic<uint64_t> &decompressedChunks)
      : downloaded(downloaded)
      , downloadedChunks(downloadedChunks)
      , decompressed(decompressed)
      , decompressedChunks(decompressedChunks)
    {}
    void addDownload(uint64_t download)
    {
      downloaded += download;
      downloadedChunks++;
    }
    void addDecompressed(uint64_t decompress)
    {
      decompressed += decompress;
      decompressedChunks++;
    }
  private:
    std::atomic<uint64_t> &downloaded;
    std::atomic<uint64_t> &downloadedChunks;
    std::atomic<uint64_t> &decompressed;
    std::atomic<uint64_t> &decompressedChunks;
  };
}
#endif