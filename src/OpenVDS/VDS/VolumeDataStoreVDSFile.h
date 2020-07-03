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

#ifndef VOLUMEDATASTOREVDSFILE_H
#define VOLUMEDATASTOREVDSFILE_H

#include <BulkDataStore/HueBulkDataStore.h>
#include <BulkDataStore/HueBulkDataStoreFileTypes.h>

#include "VDS.h"
#include "MetadataManager.h"
#include "VolumeDataStore.h"

#include <mutex>

namespace OpenVDS
{

class VolumeDataStoreVDSFile : public VolumeDataStore, public LayerMetadataContainer
{
  struct LayerFile
  {
    HueBulkDataStore::FileInterface *fileInterface;
    VDSLayerMetadataWaveletAdaptive layerMetadata;
    bool dirty;

    LayerFile() = default;
    LayerFile(HueBulkDataStore::FileInterface *fileInterface, VDSLayerMetadataWaveletAdaptive const &layerMetadata, bool dirty)
      : fileInterface(fileInterface), layerMetadata(layerMetadata), dirty(dirty)
    {}
  };

  VDS &m_vds;

  mutable std::mutex m_mutex;

  bool m_isVDSObjectFilePresent;
  bool m_isVolumeDataLayoutFilePresent;
  std::map<std::string, LayerFile> m_layerFiles;
  std::unique_ptr<HueBulkDataStore, void (*)(HueBulkDataStore *)> m_dataStore;

  std::vector<uint8_t> ParseVDSObject(std::string const &parseString);

  LayerFile *GetLayerFile(std::string const &layerName) const;
  LayerFile *GetLayerFile(const VolumeDataLayer &volumeDataLayer) const { return GetLayerFile(GetLayerName(volumeDataLayer)); }

public:
  enum Mode
  {
    ReadOnly,
    ReadWrite
  };

  CompressionInfo
                GetCompressionInfoForChunk(std::vector<uint8_t>& metadata, const VolumeDataChunk &volumeDataChunk, Error &error) override;
  bool          PrepareReadChunk(const VolumeDataChunk &volumeDataChunk, Error &error) override;
  bool          ReadChunk(const VolumeDataChunk& chunk, std::vector<uint8_t>& serializedData, std::vector<uint8_t>& metadata, CompressionInfo& compressionInfo, Error& error) override;
  bool          CancelReadChunk(const VolumeDataChunk& chunk, Error& error) override;
  bool          WriteChunk(const VolumeDataChunk& chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata) override;
  bool          Flush() override;
  bool          ReadSerializedVolumeDataLayout(std::vector<uint8_t>& serializedVolumeDataLayout, Error &error) override;
  bool          WriteSerializedVolumeDataLayout(const std::vector<uint8_t>& serializedVolumeDataLayout, Error &error) override;
  bool          AddLayer(VolumeDataLayer* volumeDataLayer) override;
  bool          RemoveLayer(VolumeDataLayer* volumeDataLayer) override { return false; }

  bool          GetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus) const override;
  void          SetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus, int pageLimit) override;

  VolumeDataStoreVDSFile(VDS &vds, const std::string &fileName, Mode mode, Error &error);
 ~VolumeDataStoreVDSFile();
};

}

#endif //VOLUMEDATASTOREVDSFILE_H
