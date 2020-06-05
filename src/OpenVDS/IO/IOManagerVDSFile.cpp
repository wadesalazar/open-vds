#include "IOManagerVDSFile.h"
#include <BulkDataStore/VDSObjectParser.h>
#include <BulkDataStore/HueBulkDataStoreFileTypes.h>
#include <VDS/Base64.h>

#include <fmt/format.h>
#include <json/json.h>

namespace OpenVDS
{
  class VDSFileRequest : public Request
  {
  public:
    VDSFileRequest(const std::string &objectName)
      : Request(objectName)
      , m_done(false)
    {

    }
    ~VDSFileRequest() override
    {

    }
    void WaitForFinish() override
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_wait.wait(lock, [this] { return m_done; });
    }
    bool IsDone() const override
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      return m_done;
    }
    bool IsSuccess(Error &error) const override
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      error = m_error;
      return m_error.code == 0;
    }
    void Cancel() override
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_done = true;
      m_wait.notify_all();
    }


    bool m_done;
    std::condition_variable m_wait;
    mutable std::mutex m_mutex;
    Error m_error;
  };

  IOManagerVDSFile::IOManagerVDSFile(const VDSFileOpenOptions &openOptions, Mode mode, Error &error)
    : m_isVDSObjectFilePresent(false)
    , m_isVolumeDataLayoutFilePresent(false)
    , m_threadPool(std::thread::hardware_concurrency())
    , m_dataStore(HueBulkDataStore::Open(openOptions.fileName.c_str()), &HueBulkDataStore::Close)
  {
    if(!m_dataStore->IsOpen() && mode == ReadWrite)
    {
      m_dataStore.reset(HueBulkDataStore::CreateNew(openOptions.fileName.c_str(), false));
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

  IOManagerVDSFile::~IOManagerVDSFile()
  {
  }

  std::shared_ptr<Request> IOManagerVDSFile::ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler)
  {
    auto request = std::make_shared<VDSFileRequest>(objectName);
    m_threadPool.Enqueue([this, objectName, handler, request]
      {
/*
        std::unique_lock<std::mutex> lock(m_mutex);
        auto it = m_data.find(objectName);
        Error error;
        if (it != m_data.end())
        {
          auto object = it->second;
          lock.unlock();
          handler->HandleObjectSize(int64_t(object.data.size()));
          for (auto& meta : object.metaHeader)
          {
            handler->HandleMetadata(meta.first, meta.second);
          }
        }
        else
        {
          lock.unlock();
          error.string = std::string("Object: ") + objectName + " not found.";
          error.code = 404;
        }

        {
          std::unique_lock<std::mutex> request_lock(request->m_mutex);
          request->m_done = true;
          request->m_error = error;
        }
        */

        Error error;
        error.string = "Not implemented";
        error.code = 404;

        handler->Completed(*request, error);
        request->m_wait.notify_all();
      });
    std::shared_ptr<OpenVDS::Request>  retRequest = request;
    return retRequest;
  }

  std::shared_ptr<OpenVDS::Request> IOManagerVDSFile::ReadObject(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range)
  {
    auto request = std::make_shared<VDSFileRequest>(objectName);

    if(objectName == "VolumeDataLayout" || objectName == "LayerStatus")
    {
      Error error;

      std::vector<uint8_t>
        result = (objectName == "VolumeDataLayout") ? ReadVolumeDataLayout(error) : ReadLayerStatus(error);

      if (!result.empty())
      {
        handler->HandleObjectSize(int64_t(result.size()));
        handler->HandleData(std::move(result));
      }

      handler->Completed(*request, error); 

      request->m_done = true;
      request->m_error = error;
      return request;
    }

    std::string layerName;
    int chunk;

    size_t delimiterIndex = objectName.find('/');
    if(delimiterIndex != std::string::npos)
    {
      layerName = objectName.substr(0, delimiterIndex);
      chunk = std::stoi(objectName.substr(delimiterIndex + 1));
    }

    Error error;
    auto layerFileEntryIterator = m_layerFiles.find(layerName);

    if(layerFileEntryIterator != m_layerFiles.end())
    {
      HueBulkDataStore::FileInterface *fileInterface = layerFileEntryIterator->second;
      std::vector<uint8_t> metadata(fileInterface->GetChunkMetadataLength());
      IndexEntry indexEntry;
      bool success = fileInterface->ReadIndexEntry(chunk, &indexEntry, metadata.data());

      if(success)
      {
        std::vector<char> base64EncodedMetadata;
        Base64Encode(metadata.data(), metadata.size(), base64EncodedMetadata);
        handler->HandleMetadata("vdschunkmetadata", std::string(base64EncodedMetadata.data(), base64EncodedMetadata.size()));

        if(indexEntry.m_length > 0)
        {
          HueBulkDataStore::Buffer *buffer = m_dataStore->ReadBuffer(indexEntry);

          if(buffer)
          {
            const char *data = reinterpret_cast<const char *>(buffer->Data());
            handler->HandleData(std::vector<uint8_t>(data, data + buffer->Size()));
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
    }
    handler->Completed(*request, error); 

    request->m_done = true;
    request->m_error = error;
    return request;

    /*
    m_threadPool.Enqueue([this, objectName, handler, request]
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        auto it = m_data.find(objectName);
        Error error;
        if (it != m_data.end())
        {
          auto object = it->second;
          lock.unlock();
          handler->HandleObjectSize(int64_t(object.data.size()));
          for (auto& meta : object.metaHeader)
          {
            handler->HandleMetadata(meta.first, meta.second);
          }
          handler->HandleData(std::move(object.data));
        }
        else
        {
          lock.unlock();
          error.string = std::string("Object: ") + objectName + " not found.";
          error.code = 404;
        }

        {
          std::unique_lock<std::mutex> request_lock(request->m_mutex);
          request->m_done = true;
          request->m_error = error;
        }

        Error error;
        error.string = "Not implemented";
        error.code = 404;

        handler->Completed(*request, error); 

        request->m_wait.notify_all();
      });
    */
  }

  std::shared_ptr<Request> IOManagerVDSFile::WriteObject(const std::string &objectName, const std::string &contentDispostionFilename, const std::string &contentType, const std::vector<std::pair<std::string, std::string> > &metadataHeaders, std::shared_ptr<std::vector<uint8_t> > data, std::function<void (const Request &, const Error &)> completedCallback)
  {
    auto request = std::make_shared<VDSFileRequest>(objectName);

    {
      Error error;
      bool success = m_dataStore->EnableWriting();
      if(!success)
      {
        error.code = -1;
        error.string = m_dataStore->GetErrorMessage();

        if (completedCallback)
          completedCallback(*request, error);

        request->m_done = true;
        request->m_error = error;
        return request;
      }
    }

    if(objectName == "VolumeDataLayout" || objectName == "LayerStatus")
    {
      Error error;

      if(objectName == "VolumeDataLayout")
      {
        WriteVolumeDataLayout(data, error);
      }
      else if(objectName == "LayerStatus")
      {
        assert(0 && "Not implemented");
      }

      if (completedCallback)
        completedCallback(*request, error);

      request->m_done = true;
      request->m_error = error;
      return request;
    }

    std::string layerName;
    int chunk;

    size_t delimiterIndex = objectName.find('/');
    if(delimiterIndex != std::string::npos)
    {
      layerName = objectName.substr(0, delimiterIndex);
      chunk = std::stoi(objectName.substr(delimiterIndex + 1));
    }

    Error error;
    auto layerFileEntryIterator = m_layerFiles.find(layerName);

    if(layerFileEntryIterator != m_layerFiles.end())
    {
      HueBulkDataStore::FileInterface *fileInterface = layerFileEntryIterator->second;
      std::vector<uint8_t> metadata(fileInterface->GetChunkMetadataLength());

      for(auto &metadataHeader : metadataHeaders)
      {
        if(metadataHeader.first == "vdschunkmetadata")
        {
          Base64Decode(metadataHeader.second.data(), (int)metadataHeader.second.size(), metadata);
          break;
        }
      }

      bool success = fileInterface->WriteChunk(chunk, data->data(), (int)data->size(), metadata.data());

      if(!success)
      {
        error.code = -1;
        error.string = m_dataStore->GetErrorMessage();
      }
    }
    else
    {
      error.code = -1;
      error.string = "Non-existent layer";
    }

    /*
    m_threadPool.Enqueue([objectName, metadataHeader, data, completedCallback, request, this]
    {
    Object object;
    object.metaHeader = metadataHeader;
    object.data = *data;
    std::unique_lock<std::mutex> lock(m_mutex);
    m_data[objectName] = std::move(object);
    lock.unlock();

    Error error;
    {
    std::unique_lock<std::mutex> request_lock(request->m_mutex);
    request->m_done = true;
    request->m_error = error;
    }

    if (completedCallback)
    completedCallback(*request, error);

    request->m_wait.notify_all();
    });
    */

    if (completedCallback)
      completedCallback(*request, error);

    request->m_done = true;
    request->m_error = error;
    return request;
  }

  std::vector<uint8_t> IOManagerVDSFile::ReadVolumeDataLayout(Error &error)
  {
    bool isReadVDSObject = (m_isVDSObjectFilePresent && !m_isVolumeDataLayoutFilePresent);

    HueBulkDataStore::FileInterface *fileInterface = m_dataStore->OpenFile(isReadVDSObject ? "VDSObject" : "VolumeDataLayout");

    if(!fileInterface)
    {
      error.code = -1;
      error.string = m_dataStore->GetErrorMessage();
      return std::vector<uint8_t>();
    }

    HueBulkDataStore::Buffer *buffer = fileInterface->ReadChunk(0, nullptr);

    if(!buffer)
    {
      error.code = -1;
      error.string = m_dataStore->GetErrorMessage();
      m_dataStore->CloseFile(fileInterface);
      return std::vector<uint8_t>();
    }

    m_dataStore->CloseFile(fileInterface);

    // auto-release buffer when it goes out of scope
    std::unique_ptr< HueBulkDataStore::Buffer, decltype(&HueBulkDataStore::ReleaseBuffer)> bufferGuard(buffer, &HueBulkDataStore::ReleaseBuffer);

    if(isReadVDSObject)
    {
      return ParseVDSObject(std::string(reinterpret_cast<const char *>(buffer->Data()), buffer->Size()));
    }
    else
    {
      const char *data = reinterpret_cast<const char *>(buffer->Data());
      return std::vector<uint8_t>(data, data + buffer->Size());
    }
  }

  std::vector<uint8_t> IOManagerVDSFile::ParseVDSObject(std::string const &parseString)
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

  std::string ToString(VDSLayerMetadata::CompressionMethod compressionMethod)
  {
    switch(compressionMethod)
    {
    case VDSLayerMetadata::COMPRESSIONMETHOD_NONE:                             return "None";
    case VDSLayerMetadata::COMPRESSIONMETHOD_WAVELET:                          return "Wavelet";
    case VDSLayerMetadata::COMPRESSIONMETHOD_RLE:                              return "RLE";
    case VDSLayerMetadata::COMPRESSIONMETHOD_ZIP:                              return "Zip";
    case VDSLayerMetadata::COMPRESSIONMETHOD_WAVELET_NORMALIZE_BLOCK:          return "WaveletNormalizeBlock";
    case VDSLayerMetadata::COMPRESSIONMETHOD_WAVELET_LOSSLESS:                 return "WaveletLossless";
    case VDSLayerMetadata::COMPRESSIONMETHOD_WAVELET_NORMALIZE_BLOCK_LOSSLESS: return "WaveletNormalizeBlockLossless";

    default: assert(0 && "Illegal compression method"); return "";
    };
  }

  std::vector<uint8_t> IOManagerVDSFile::ReadLayerStatus(Error &error)
  {
    Json::Value layerStatusArrayJson(Json::arrayValue);

    for(auto const &layerFileEntry : m_layerFiles)
    {
      HueBulkDataStore::FileInterface *layerFile = layerFileEntry.second;
      std::string layerName = layerFileEntry.first;

      VDSLayerMetadataWaveletAdaptive layerFileMetadata;
      layerFile->ReadFileMetadata(&layerFileMetadata);

      size_t lodIndex = layerName.rfind("LOD");
      size_t dimensionsIndex = layerName.rfind("Dimensions_");

      if(lodIndex == std::string::npos || dimensionsIndex == std::string::npos || lodIndex < dimensionsIndex)
      {
        continue;
      }

      std::string channelName    = layerName.substr(0, dimensionsIndex);
      std::string dimensionGroup = layerName.substr(dimensionsIndex, lodIndex - dimensionsIndex);
      int lod = std::stoi(layerName.substr(lodIndex + 3));

      Json::Value layerStatusJson;

      layerStatusJson["layerName"] = layerName;
      layerStatusJson["channelName"] = channelName;
      layerStatusJson["dimensionGroup"] =dimensionGroup;
      layerStatusJson["lod"] = lod;
      layerStatusJson["produceStatus"] = "Normal";
      layerStatusJson["compressionMethod"] = ToString(VDSLayerMetadata::CompressionMethod(layerFileMetadata.m_compressionMethod));
      layerStatusJson["compressionTolerance"] = layerFileMetadata.m_compressionTolerance;
      layerStatusArrayJson.append(layerStatusJson);
    }

    std::vector<uint8_t>
      result;

    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "    ";
    std::string document = Json::writeString(wbuilder, layerStatusArrayJson);

    // strip carriage return
    result.reserve(document.length());
    for(char c : document)
    {
      if(c != '\r')
      {
        result.push_back(c);
      }
    }

    return result;
  }

  bool IOManagerVDSFile::WriteVolumeDataLayout(std::shared_ptr<std::vector<uint8_t> > data, Error &error)
  {
    HueBulkDataStore::FileInterface *fileInterface = m_dataStore->AddFile("VolumeDataLayout", 1, 1, FILETYPE_JSON_OBJECT, 0, 0, true);

    if(!fileInterface)
    {
      error.code = -1;
      error.string = m_dataStore->GetErrorMessage();
      return false;
    }

    bool success = fileInterface->WriteChunk(0, data->data(), (int)data->size(), nullptr);

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
}
