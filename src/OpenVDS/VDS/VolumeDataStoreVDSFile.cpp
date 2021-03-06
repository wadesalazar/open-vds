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

#include <BulkDataStore/VDSObjectParser.h>

#include "VolumeDataStoreVDSFile.h"

#include "VDS.h"
#include "ParseVDSJson.h"
#include "WaveletTypes.h"

#include <fmt/format.h>

#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <algorithm>

namespace OpenVDS
{

CompressionInfo VolumeDataStoreVDSFile::GetEffectiveAdaptiveLevel(VolumeDataLayer* volumeDataLayer, WaveletAdaptiveMode waveletAdaptiveMode, float tolerance, float ratio)
{
  CompressionMethod
    compressionMethod = CompressionMethod::None;

  float
    compressionTolerance = 0.0f;

  int
    adaptiveLevel = (waveletAdaptiveMode == WaveletAdaptiveMode::BestQuality) ? -1 : 0;

  LayerFile* layerFile = GetLayerFile(*volumeDataLayer);

  if(layerFile)
  {
    compressionMethod = CompressionMethod(layerFile->layerMetadata.m_compressionMethod);
    compressionTolerance = layerFile->layerMetadata.m_compressionTolerance;

    if(waveletAdaptiveMode == WaveletAdaptiveMode::Tolerance && layerFile->layerChunksWaveletAdaptive)
    {
      adaptiveLevel = Wavelet_GetEffectiveWaveletAdaptiveLoadLevel(tolerance, layerFile->layerMetadata.m_compressionTolerance);
    }
    else if(waveletAdaptiveMode == WaveletAdaptiveMode::Ratio && layerFile->layerChunksWaveletAdaptive)
    {
      adaptiveLevel = Wavelet_GetEffectiveWaveletAdaptiveLoadLevel(ratio, layerFile->layerMetadata.m_adaptiveLevelSizes, layerFile->layerMetadata.m_uncompressedSize);
    }
    else
    {
      assert(waveletAdaptiveMode == WaveletAdaptiveMode::BestQuality && "Illegal WaveletAdaptiveMode");
    }
  }

  return CompressionInfo(compressionMethod, compressionTolerance, adaptiveLevel);
}

VolumeDataStoreVDSFile::LayerFile *VolumeDataStoreVDSFile::GetLayerFile(std::string const &layerName) const
{
  std::unique_lock<std::mutex> lock(m_mutex);

  auto layerFileIterator = m_layerFiles.find(layerName);
  return (layerFileIterator != m_layerFiles.end()) ? const_cast<VolumeDataStoreVDSFile::LayerFile *>(&layerFileIterator->second) : nullptr;
}

bool VolumeDataStoreVDSFile::PrepareReadChunk(const VolumeDataChunk &volumeDataChunk, int adaptiveLevel, Error &error)
{
  return true;
}

bool VolumeDataStoreVDSFile::ReadChunk(const VolumeDataChunk& chunk, int adaptiveLevel, std::vector<uint8_t>& serializedData, std::vector<uint8_t>& metadata, CompressionInfo& compressionInfo, Error& error)
{
  error = Error();

  assert(chunk.layer);
  LayerFile* layerFile = GetLayerFile(*chunk.layer);

  if(!layerFile)
  {
    error.code = -1;
    error.string = "Trying to read from a layer that has not been added";
    compressionInfo = CompressionInfo();
    return false;
  }

  std::unique_lock<std::mutex> lock(m_mutex);
  HueBulkDataStore::FileInterface *fileInterface = layerFile->fileInterface;

  metadata.resize(fileInterface->GetChunkMetadataLength());
  IndexEntry indexEntry;

  bool success = fileInterface->ReadIndexEntry((int)chunk.index, &indexEntry, metadata.data());
  lock.unlock();

  if(success)
  {
    if(indexEntry.m_length > 0)
    {
      if(layerFile->layerChunksWaveletAdaptive)
      {
        assert(metadata.size() == sizeof(VDSWaveletAdaptiveLevelsChunkMetadata));
        auto waveletAdaptiveLevelsChunkMetadata = reinterpret_cast<VDSWaveletAdaptiveLevelsChunkMetadata *>(metadata.data());
        indexEntry.m_length = Wavelet_DecodeAdaptiveLevelsMetadata(indexEntry.m_length, adaptiveLevel, waveletAdaptiveLevelsChunkMetadata->m_levels);
      }

      HueBulkDataStore::Buffer *buffer = m_dataStore->ReadChunkData(indexEntry);

      if(buffer)
      {
        m_globalStateVds.addDownload(buffer->Size());
        auto bufferData = reinterpret_cast<const uint8_t *>(buffer->Data());
        serializedData.assign(bufferData, bufferData + buffer->Size());
        m_dataStore->ReleaseBuffer(buffer);
      }
      else
      {
        success = false;
      }
    }
  }

  if(!success)
  {
    error.code = -1;
    error.string = m_dataStore->GetErrorMessage();
    compressionInfo = CompressionInfo();
  }
  else
  {
    compressionInfo = CompressionInfo(CompressionMethod(layerFile->layerMetadata.m_compressionMethod), layerFile->layerMetadata.m_compressionTolerance, adaptiveLevel);
  }

  return success;
}

bool VolumeDataStoreVDSFile::CancelReadChunk(const VolumeDataChunk& chunk, Error& error)
{
  return true;
}

bool VolumeDataStoreVDSFile::WriteChunk(const VolumeDataChunk& chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata)
{
  Error error = Error();

  assert(chunk.layer);
  LayerFile* layerFile = GetLayerFile(*chunk.layer);

  if(!layerFile)
  {
    error.code = -1;
    error.string = "Trying to write to a layer that has not been added";
    m_vds.accessManager->AddUploadError(error, fmt::format("{}/{}", GetLayerName(*chunk.layer), chunk.index));
    return false;
  }

  std::unique_lock<std::mutex> lock(m_mutex);
  HueBulkDataStore::FileInterface *fileInterface = layerFile->fileInterface;

  if((int)metadata.size() != fileInterface->GetChunkMetadataLength())
  {
    throw std::runtime_error("Wrong metadata size for chunk");
  }

  VDSWaveletAdaptiveLevelsChunkMetadata const &newMetadata = *reinterpret_cast<const VDSWaveletAdaptiveLevelsChunkMetadata *>(metadata.data());
  VDSWaveletAdaptiveLevelsChunkMetadata oldMetadata;

  IndexEntry indexEntry = IndexEntry();

  if(!serializedData.empty())
  {
    m_dataStore->CreateChunkDataIndexEntry(indexEntry, (int)serializedData.size());
    lock.unlock();
    m_dataStore->WriteChunkData(indexEntry, serializedData.data(), (int)serializedData.size());
    lock.lock();
  }

  int oldSize;

  bool success = fileInterface->WriteIndexEntry((int)chunk.index, indexEntry, metadata.data(), &oldSize, &oldMetadata);

  if(!success)
  {
    error.code = -1;
    error.string = m_dataStore->GetErrorMessage();
    m_vds.accessManager->AddUploadError(error, fmt::format("{}/{}", GetLayerName(*chunk.layer), chunk.index));
    return false;
  }

  layerFile->dirty = true;

  if (newMetadata.m_hash != VolumeDataHash::UNKNOWN)
  {
    layerFile->layerMetadata.m_validChunkCount++;
  }

  if (oldMetadata.m_hash != VolumeDataHash::UNKNOWN)
  {
    layerFile->layerMetadata.m_validChunkCount--;
  }

  if(layerFile->layerChunksWaveletAdaptive)
  {
    int size[DataBlock::Dimensionality_Max];
    chunk.layer->GetChunkVoxelSize(chunk.index, size);
    int64_t uncompressedSize = GetByteSize(size, chunk.layer->GetFormat(), chunk.layer->GetComponents());

    if (newMetadata.m_hash != VolumeDataHash::UNKNOWN && !VolumeDataHash(newMetadata.m_hash).IsConstant())
    {
      if(layerFile->layerChunksWaveletAdaptive)
      {
        layerFile->layerMetadata.m_uncompressedSize += uncompressedSize;
        Wavelet_AccumulateAdaptiveLevelSizes(indexEntry.m_length, layerFile->layerMetadata.m_adaptiveLevelSizes, false, newMetadata.m_levels);
      }
    }

    if (oldMetadata.m_hash != VolumeDataHash::UNKNOWN && !VolumeDataHash(oldMetadata.m_hash).IsConstant())
    {
      if(layerFile->layerChunksWaveletAdaptive)
      {
        layerFile->layerMetadata.m_uncompressedSize -= uncompressedSize;
        Wavelet_AccumulateAdaptiveLevelSizes(oldSize, layerFile->layerMetadata.m_adaptiveLevelSizes, true, oldMetadata.m_levels);
      }
    }
  }

  assert(layerFile->layerMetadata.m_validChunkCount <= layerFile->fileInterface->GetChunkCount());
  return true;
}

bool VolumeDataStoreVDSFile::Flush(bool writeUpdatedLayerStatus)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  bool success = true;

  for(auto &layerFileEntry : m_layerFiles)
  {
    LayerFile *layerFile = &layerFileEntry.second;

    if(layerFile->dirty)
    {
      layerFile->fileInterface->WriteFileMetadata(&layerFile->layerMetadata);
      success = layerFile->fileInterface->Commit();

      if(!success)
      {
        std::string message = fmt::format("Commit on layer {} failed: {}", layerFile->fileInterface->GetFileName(), m_dataStore->GetErrorMessage());
      }
      else
      {
        layerFile->dirty = false;
      }
    }
  }

  return success;
}

bool VolumeDataStoreVDSFile::ReadSerializedVolumeDataLayout(std::vector<uint8_t>& serializedVolumeDataLayout, Error &error)
{
  bool isReadVDSObject = (m_isVDSObjectFilePresent && !m_isVolumeDataLayoutFilePresent);

  HueBulkDataStore::FileInterface *fileInterface = m_dataStore->OpenFile(isReadVDSObject ? "VDSObject" : "VolumeDataLayout");

  if(!fileInterface)
  {
    error.code = -1;
    error.string = m_dataStore->GetErrorMessage();
    return false;
  }

  HueBulkDataStore::Buffer *buffer = fileInterface->ReadChunk(0, nullptr);

  if(!buffer)
  {
    error.code = -1;
    error.string = m_dataStore->GetErrorMessage();
    m_dataStore->CloseFile(fileInterface);
    return false;
  }

  m_dataStore->CloseFile(fileInterface);

  // auto-release buffer when it goes out of scope
  std::unique_ptr< HueBulkDataStore::Buffer, decltype(&HueBulkDataStore::ReleaseBuffer)> bufferGuard(buffer, &HueBulkDataStore::ReleaseBuffer);

  if(isReadVDSObject)
  {
    serializedVolumeDataLayout = ParseVDSObject(std::string(reinterpret_cast<const char *>(buffer->Data()), buffer->Size()));
  }
  else
  {
    const char *data = reinterpret_cast<const char *>(buffer->Data());
    serializedVolumeDataLayout.assign(data, data + buffer->Size());
  }

  for(auto &layerFileEntry : m_layerFiles)
  {
    LayerFile *layerFile = &layerFileEntry.second;
    std::string layerName = layerFile->fileInterface->GetFileName();

    size_t lodIndex = layerName.rfind("LOD");
    size_t dimensionsIndex = layerName.rfind("Dimensions_");
    const int dimensionsLength = (int)strlen("Dimensions_");

    if(lodIndex == std::string::npos || dimensionsIndex == std::string::npos || lodIndex < dimensionsIndex)
    {
      continue;
    }

    std::string channelName    = layerName.substr(0, dimensionsIndex);
    int dimensions[3] = { -1, -1, -1 };
    bool valid = true;
    for(int i = 0; i < 3; i++)
    {
      if(dimensionsIndex + dimensionsLength + i == lodIndex) break;

      dimensions[i] = layerName[dimensionsIndex + dimensionsLength + i] - '0';
      if(dimensions[i] < 0 || dimensions[i] >= 6 || (i > 0 && dimensions[i] <= dimensions[i - 1]))
      {
        valid = false;
        break;
      }
    }
    if(!valid) continue;

    DimensionsND dimensionsND = DimensionGroupUtil::GetDimensionsNDFromDimensionGroup(DimensionGroupUtil::GetDimensionGroupFromDimensionIndices(dimensions[0], dimensions[1], dimensions[2]));
    int lod = std::stoi(layerName.substr(lodIndex + 3));

    if (lod == 0 && m_vds.produceStatuses[dimensionsND] == VolumeDataLayer::ProduceStatus_Unavailable)
    {
      m_vds.produceStatuses[dimensionsND] = VolumeDataLayer::ProduceStatus_Normal;
    }
  }

  return true;
}

std::vector<uint8_t> VolumeDataStoreVDSFile::ParseVDSObject(std::string const &parseString)
{
  std::vector<uint8_t> jsonStringBuffer;

  Parser *parser;
    
  InitializeParser(nullptr, &parser, parseString.c_str());

  if (ParseVDS(&parser))
  {
    jsonStringBuffer.resize(parseString.size());

    size_t size = GetVolumeDataLayoutJsonString(parser, reinterpret_cast<char *>(jsonStringBuffer.data()), jsonStringBuffer.size());
    bool retry = (size > jsonStringBuffer.size());
    jsonStringBuffer.resize(size);
    if(retry)
    {
      GetVolumeDataLayoutJsonString(parser, reinterpret_cast<char *>(jsonStringBuffer.data()), jsonStringBuffer.size());
    }
  }

  DeInitializeParser(&parser);

  return jsonStringBuffer;
}

bool VolumeDataStoreVDSFile::WriteSerializedVolumeDataLayout(const std::vector<uint8_t>& serializedVolumeDataLayout, Error &error)
{
  HueBulkDataStore::FileInterface *fileInterface = m_dataStore->AddFile("VolumeDataLayout", 1, 1, FILETYPE_JSON_OBJECT, 0, 0, true);

  if(!fileInterface)
  {
    error.code = -1;
    error.string = m_dataStore->GetErrorMessage();
    return false;
  }

  bool success = fileInterface->WriteChunk(0, serializedVolumeDataLayout.data(), (int)serializedVolumeDataLayout.size(), nullptr) && fileInterface->Commit();

  if(!success)
  {
    error.code = -1;
    error.string = m_dataStore->GetErrorMessage();
  }

  m_dataStore->CloseFile(fileInterface);
  return success;
}

bool VolumeDataStoreVDSFile::AddLayer(VolumeDataLayer* volumeDataLayer, int chunkMetadataPageSize)
{
  assert(volumeDataLayer);
  assert(chunkMetadataPageSize > 0);
  std::unique_lock<std::mutex> lock(m_mutex);

  if(volumeDataLayer->GetTotalChunkCount() > INT_MAX)
  {
    return false;
  }

  if(m_dataStore->IsReadOnly())
  {
    return false;
  }

  std::string layerName = GetLayerName(*volumeDataLayer);

  // Check if layer is already added
  if(m_layerFiles.find(layerName) != m_layerFiles.end())
  {
    return true;
  }

  int
    chunkMetadataLength = int(sizeof(VDSChunkMetadata)),
    fileMetadataLength  = int(sizeof(VDSLayerMetadata));

  bool
    layerChunksWaveletAdaptive = CompressionMethod_IsWavelet(volumeDataLayer->GetEffectiveCompressionMethod());

  if(layerChunksWaveletAdaptive)
  {
    chunkMetadataLength = int(sizeof(VDSWaveletAdaptiveLevelsChunkMetadata));
    fileMetadataLength  = int(sizeof(VDSLayerMetadataWaveletAdaptive));
  }

  HueBulkDataStore::FileInterface *fileInterface = m_dataStore->AddFile(layerName.c_str(), (int)volumeDataLayer->GetTotalChunkCount(), chunkMetadataPageSize, FILETYPE_VDS_LAYER, chunkMetadataLength, fileMetadataLength, true);
  assert(fileInterface);

  VDSLayerMetadataWaveletAdaptive layerMetadata = VDSLayerMetadataWaveletAdaptive();
  layerMetadata.m_compressionMethod = (int)volumeDataLayer->GetEffectiveCompressionMethod();
  layerMetadata.m_compressionTolerance = volumeDataLayer->GetEffectiveCompressionTolerance();
  fileInterface->WriteFileMetadata(&layerMetadata);
  fileInterface->Commit();

  m_layerFiles[layerName] = LayerFile(fileInterface, layerMetadata, layerChunksWaveletAdaptive, true);
  return true;
}

bool VolumeDataStoreVDSFile::GetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus) const
{
  LayerFile* layerFile = GetLayerFile(layerName);

  if(!layerFile)
  {
    return false;
  }

  HueBulkDataStore::FileInterface *fileInterface = layerFile->fileInterface;

  metadataStatus.m_chunkIndexCount = fileInterface->GetChunkCount();
  metadataStatus.m_chunkMetadataPageSize = 0;
  metadataStatus.m_chunkMetadataByteSize = fileInterface->GetChunkMetadataLength();
  metadataStatus.m_compressionTolerance = layerFile->layerMetadata.m_compressionTolerance;
  metadataStatus.m_compressionMethod = CompressionMethod(layerFile->layerMetadata.m_compressionMethod);
  metadataStatus.m_uncompressedSize = layerFile->layerMetadata.m_uncompressedSize;
  std::copy(layerFile->layerMetadata.m_adaptiveLevelSizes + 0, layerFile->layerMetadata.m_adaptiveLevelSizes + sizeof(layerFile->layerMetadata.m_adaptiveLevelSizes) / sizeof(layerFile->layerMetadata.m_adaptiveLevelSizes[0]), metadataStatus.m_adaptiveLevelSizes);

  return true;
}

void VolumeDataStoreVDSFile::SetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus, int pageLimit)
{
  assert(0 && "Not implemented");
}

VolumeDataStoreVDSFile::VolumeDataStoreVDSFile(VDS &vds, const std::string &vdsFileName, Mode mode, Error &error)
  : VolumeDataStore(OpenOptions::VDSFile)
  , m_vds(vds)
  , m_isVDSObjectFilePresent(false)
  , m_isVolumeDataLayoutFilePresent(false)
  , m_dataStore(nullptr, &HueBulkDataStore::Close)
{
  if (mode == Mode::Create)
  {
    m_dataStore.reset(HueBulkDataStore::CreateNew(vdsFileName.c_str(), true));
  }
  else
  {
    m_dataStore.reset(HueBulkDataStore::Open(vdsFileName.c_str()));
    if (mode == ReadWrite)
    {
      if (!m_dataStore->IsOpen())
      {
        m_dataStore.reset(HueBulkDataStore::CreateNew(vdsFileName.c_str(), false));
      }
      else
      {
        m_dataStore->EnableWriting();
      }
    }
  }

  if(m_dataStore->IsOpen())
  {
    int fileCount = m_dataStore->GetFileCount();
    for(int fileIndex = 0; fileIndex < fileCount; fileIndex++)
    {
      std::string fileName(m_dataStore->GetFileName(fileIndex));
      int fileType = m_dataStore->GetFileType(fileIndex);

      if(fileType == FILETYPE_VDS_LAYER)
      {
        HueBulkDataStore::FileInterface *
          fileInterface = m_dataStore->OpenFile(m_dataStore->GetFileName(fileIndex));
        assert(fileInterface);

        bool layerChunksWaveletAdaptive = (fileInterface->GetFileMetadataLength() == sizeof(VDSLayerMetadataWaveletAdaptive) &&
                                           fileInterface->GetChunkMetadataLength() == sizeof(VDSWaveletAdaptiveLevelsChunkMetadata));

        VDSLayerMetadataWaveletAdaptive layerMetadata = VDSLayerMetadataWaveletAdaptive();
        fileInterface->ReadFileMetadata(&layerMetadata);

        m_layerFiles[fileName] = LayerFile(fileInterface, layerMetadata, layerChunksWaveletAdaptive, false);
      }
      else if(fileType == FILETYPE_HUE_OBJECT)
      {
        m_isVDSObjectFilePresent = (fileName == "VDSObject");
      }
      else if(fileType == FILETYPE_JSON_OBJECT)
      {
        m_isVolumeDataLayoutFilePresent = (fileName == "VolumeDataLayout");
      }
    }
  }
  else
  {
    auto errormsg = m_dataStore->GetErrorMessage();
    if (strlen(errormsg))
    {
      error.string = errormsg;
      error.code = -1;
    }
  }
}

VolumeDataStoreVDSFile::~VolumeDataStoreVDSFile()
{
}

}
