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
#include <BulkDataStore/HueBulkDataStoreFileTypes.h>

#include "VolumeDataStoreVDSFile.h"

#include "VDS.h"
#include "ParseVDSJson.h"

#include <fmt/format.h>

#include <stdlib.h>
#include <assert.h>
#include <algorithm>

namespace OpenVDS
{

CompressionInfo VolumeDataStoreVDSFile::GetCompressionInfoForChunk(std::vector<uint8_t>& metadata, const VolumeDataChunk &volumeDataChunk, Error &error)
{
  assert(0 && "Not implemented");
  return CompressionInfo();
}

bool VolumeDataStoreVDSFile::PrepareReadChunk(const VolumeDataChunk &volumeDataChunk, Error &error)
{
  return true;
}

bool VolumeDataStoreVDSFile::ReadChunk(const VolumeDataChunk& chunk, std::vector<uint8_t>& serializedData, std::vector<uint8_t>& metadata, CompressionInfo& compressionInfo, Error& error)
{
  error = Error();
  auto layerFileEntryIterator = m_layerFiles.find(GetLayerName(*chunk.layer));

  if(layerFileEntryIterator == m_layerFiles.end())
  {
    error.code = -1;
    error.string = "Trying to read from a layer that has not been added";
    return false;
  }

  HueBulkDataStore::FileInterface *fileInterface = layerFileEntryIterator->second;
  metadata.resize(fileInterface->GetChunkMetadataLength());
  IndexEntry indexEntry;
  bool success = fileInterface->ReadIndexEntry((int)chunk.index, &indexEntry, metadata.data());

  if(success)
  {
    if(indexEntry.m_length > 0)
    {
      HueBulkDataStore::Buffer *buffer = m_dataStore->ReadBuffer(indexEntry);

      if(buffer)
      {
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

  auto layerFileEntryIterator = m_layerFiles.find(GetLayerName(*chunk.layer));

  if(layerFileEntryIterator == m_layerFiles.end())
  {
    error.code = -1;
    error.string = "Trying to write to a layer that has not been added";
    m_vds.accessManager->AddUploadError(error, fmt::format("{}/{}", GetLayerName(*chunk.layer), chunk.index));
    return false;
  }

  HueBulkDataStore::FileInterface *fileInterface = layerFileEntryIterator->second;

  if(metadata.size() != fileInterface->GetChunkMetadataLength())
  {
    throw std::runtime_error("Wrong metadata size for chunk");
  }

  VDSWaveletAdaptiveLevelsChunkMetadata
    oldMetadata;

  bool success = fileInterface->WriteChunk((int)chunk.index, serializedData.data(), (int)serializedData.size(), metadata.data(), &oldMetadata);

  if(!success)
  {
    error.code = -1;
    error.string = m_dataStore->GetErrorMessage();
    m_vds.accessManager->AddUploadError(error, fmt::format("{}/{}", GetLayerName(*chunk.layer), chunk.index));
    return false;
  }

  return true;
}

bool VolumeDataStoreVDSFile::Flush()
{
  bool success = true;

  for(auto &layerFile : m_layerFiles)
  {
    success = layerFile.second->Commit();

    if(!success)
    {
      std::string message = fmt::format("Commit on layer {} failed: {}", layerFile.second->GetFileName(), m_dataStore->GetErrorMessage());
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

  bool success = fileInterface->WriteChunk(0, serializedVolumeDataLayout.data(), (int)serializedVolumeDataLayout.size(), nullptr);

  if(!success)
  {
    error.code = -1;
    error.string = m_dataStore->GetErrorMessage();
    m_dataStore->CloseFile(fileInterface);
    return false;
  }

  m_dataStore->CloseFile(fileInterface);
  return true;
}

bool VolumeDataStoreVDSFile::AddLayer(VolumeDataLayer* volumeDataLayer)
{
  assert(0 && "Not implemented");
  return true;
}

bool VolumeDataStoreVDSFile::GetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus) const
{
  auto it = m_layerFiles.find(layerName);
  if(it == m_layerFiles.end())
  {
    return false;
  }

  HueBulkDataStore::FileInterface *layerFile = it->second;

  VDSLayerMetadataWaveletAdaptive layerFileMetadata;
  layerFile->ReadFileMetadata(&layerFileMetadata);

  size_t lodIndex = layerName.rfind("LOD");
  size_t dimensionsIndex = layerName.rfind("Dimensions_");

  if(lodIndex == std::string::npos || dimensionsIndex == std::string::npos || lodIndex < dimensionsIndex)
  {
    return false;
  }

  metadataStatus.m_chunkIndexCount = layerFile->GetChunkCount();
  metadataStatus.m_chunkMetadataPageSize = 0;
  metadataStatus.m_chunkMetadataByteSize = layerFile->GetChunkMetadataLength();
  metadataStatus.m_compressionTolerance = layerFileMetadata.m_compressionTolerance;
  metadataStatus.m_compressionMethod = CompressionMethod(layerFileMetadata.m_compressionMethod);
  metadataStatus.m_uncompressedSize = layerFileMetadata.m_uncompressedSize;
  std::copy(layerFileMetadata.m_adaptiveLevelSizes + 0, layerFileMetadata.m_adaptiveLevelSizes + sizeof(layerFileMetadata.m_adaptiveLevelSizes) / sizeof(layerFileMetadata.m_adaptiveLevelSizes[0]), metadataStatus.m_adaptiveLevelSizes);

  return true;
}

void VolumeDataStoreVDSFile::SetMetadataStatus(std::string const &layerName, MetadataStatus &metadataStatus, int pageLimit)
{
  assert(0 && "Not implemented");
}

VolumeDataStoreVDSFile::VolumeDataStoreVDSFile(VDS &vds, const std::string &fileName, Mode mode, Error &error)
  : m_vds(vds)
  , m_isVDSObjectFilePresent(false)
  , m_isVolumeDataLayoutFilePresent(false)
  , m_dataStore(HueBulkDataStore::Open(fileName.c_str()), &HueBulkDataStore::Close)
{
  if(!m_dataStore->IsOpen() && mode == ReadWrite)
  {
    m_dataStore.reset(HueBulkDataStore::CreateNew(fileName.c_str(), false));
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
        m_layerFiles[fileName] = m_dataStore->OpenFile(m_dataStore->GetFileName(fileIndex));
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
}

VolumeDataStoreVDSFile::~VolumeDataStoreVDSFile()
{
}

}
