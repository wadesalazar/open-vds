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

#ifndef VOLUMEDATASTORE_H
#define VOLUMEDATASTORE_H

#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeData.h>
#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAccess.h>

#include "MetadataManager.h"
#include "DataBlock.h"
#include "VolumeDataHash.h"
#include "ParsedMetadata.h"
#include "GlobalStateImpl.h"

#include <vector>

namespace OpenVDS
{

class VolumeDataStore
{
public:
           VolumeDataStore(OpenOptions::ConnectionType connectionType);
  virtual ~VolumeDataStore() {};

  virtual CompressionInfo
                        GetCompressionInfoForChunk(std::vector<uint8_t>& metadata, const VolumeDataChunk &volumeDataChunk, Error &error) = 0;
  virtual bool          PrepareReadChunk(const VolumeDataChunk &volumeDataChunk, Error &error) = 0;
  virtual bool          ReadChunk(const VolumeDataChunk& chunk, std::vector<uint8_t>& serializedData, std::vector<uint8_t>& metadata, CompressionInfo& compressionInfo, Error& error) = 0;
  virtual bool          CancelReadChunk(const VolumeDataChunk& chunk, Error& error) = 0;
  virtual bool          WriteChunk(const VolumeDataChunk& chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata) = 0;
  virtual bool          Flush(bool writeUpdatedLayerStatus) = 0;
  virtual bool          ReadSerializedVolumeDataLayout(std::vector<uint8_t>& serializedVolumeDataLayout, Error &error) = 0;
  virtual bool          WriteSerializedVolumeDataLayout(const std::vector<uint8_t>& serializedVolumeDataLayout, Error &error) = 0;
  virtual bool          AddLayer(VolumeDataLayer* volumeDataLayer, int chunkMetadataPageSize) = 0;
  virtual bool          RemoveLayer(VolumeDataLayer* volumeDataLayer) = 0;

  bool DeserializeVolumeData(const VolumeDataChunk &volumeDataChunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata, CompressionMethod compressionMethod, int32_t adaptiveLevel, VolumeDataChannelDescriptor::Format loadFormat, DataBlock &dataBlock, std::vector<uint8_t>& target, Error& error);

  static bool Verify(const VolumeDataChunk& volumeDataChunk, const std::vector<uint8_t>& serializedData, CompressionMethod compressionMethod, bool isFullyRead);
  static bool CreateConstantValueDataBlock(VolumeDataChunk const &volumeDataChunk, VolumeDataChannelDescriptor::Format format, float noValue, VolumeDataChannelDescriptor::Components components, VolumeDataHash const &constantValueVolumeDataHash, DataBlock &dataBlock, std::vector<uint8_t> &buffer, Error &error);
  static uint64_t
              SerializeVolumeData(const VolumeDataChunk& chunk, const DataBlock &dataBlock, const std::vector<uint8_t>& chunkData, CompressionMethod compressionMethod, std::vector<uint8_t>& destinationBuffer);

protected:
  GlobalStateVds        m_globalStateVds; 
};

}

#endif //VOLUMEDATASTORE_H
