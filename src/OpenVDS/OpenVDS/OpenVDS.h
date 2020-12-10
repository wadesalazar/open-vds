/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
** Copyright 2020 Microsoft Corp.
** Copyright 2020 Google, Inc.
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

#ifndef OPENVDS_H
#define OPENVDS_H

#include <OpenVDS/openvds_export.h>
#include <OpenVDS/MetadataAccess.h>
#include <OpenVDS/VolumeData.h>


#include <cstdint>
#include <string>
#include <vector>

namespace OpenVDS
{
class VolumeDataLayoutDescriptor;
class VolumeDataAxisDescriptor;
class VolumeDataChannelDescriptor;
class GlobalState;
class IOManager;

struct OpenOptions
{
  enum ConnectionType
  {
    AWS,
    Azure,
    AzurePresigned,
    GoogleStorage,
    Http,
    VDSFile,
    InMemory,
    Other,
    ConnectionTypeCount
  };

  ConnectionType connectionType;

protected:
  OpenOptions(ConnectionType connectionType) : connectionType(connectionType) {}
public:
  OPENVDS_EXPORT virtual ~OpenOptions();
};

/// <summary>
/// Options for opening a VDS in AWS
/// </summary>
struct AWSOpenOptions : OpenOptions
{
  std::string bucket;
  std::string key;
  std::string region;
  std::string endpointOverride;
  std::string accessKeyId;
  std::string secretKey;
  std::string sessionToken;
  std::string expiration;
  std::string logFilenamePrefix;
  std::string loglevel;
  int connectionTimeoutMs;
  int requestTimeoutMs;

  AWSOpenOptions() : OpenOptions(AWS), connectionTimeoutMs(3000), requestTimeoutMs(6000) {}
  /// <summary>
  /// AWSOpenOptions constructor
  /// </summary>
  /// <param name="bucket">
  /// The bucket of the VDS
  /// </param>
  /// <param name="key">
  /// The key prefix of the VDS
  /// </param>
  /// <param name="region">
  /// The region of the bucket of the VDS
  /// </param>
  /// <param name="endpointOverride">
  /// This parameter allows to override the endpoint url
  /// </param>
  /// <param name="connectionTimeoutMs">
  /// This parameter allows to override the time a connection can spend on connecting to AWS
  /// </param>
  /// <param name="requestTimeoutMs">
  /// This paramter allows to override the time a request can take
  /// </param>
  AWSOpenOptions(std::string const & bucket, std::string const & key, std::string const & region = std::string(), std::string const & endpointOverride = std::string(), int connectionTimeoutMs = 3000, int requestTimeoutMs = 6000) : OpenOptions(AWS), bucket(bucket), key(key), region(region), endpointOverride(endpointOverride), connectionTimeoutMs(connectionTimeoutMs), requestTimeoutMs(requestTimeoutMs) {}
};

/// <summary>
/// Options for opening a VDS in Azure
/// </summary>
struct AzureOpenOptions : OpenOptions
{
  std::string connectionString;
  std::string accountName;
  std::string bearerToken;
  std::string container;
  std::string blob;

  int parallelism_factor = 4;
  int max_execution_time = 100000;

  AzureOpenOptions() : OpenOptions(Azure) {}

  /// <summary>
  /// AzureOpenOptions constructor
  /// </summary>
  /// <param name="connectionString">
  /// The connectionString for the VDS
  /// </param>
  /// <param name="container">
  /// The container of the VDS
  /// </param>
  /// <param name="blob">
  /// The blob prefix of the VDS
  /// </param>
  AzureOpenOptions(std::string const& connectionString, std::string const& container, std::string const& blob) : OpenOptions(Azure), connectionString(connectionString), container(container), blob(blob) {}

  /// <summary>
  /// AzureOpenOptions constructor
  /// </summary>
  /// <param name="connectionString">
  /// The connectionString for the VDS
  /// </param>
  /// <param name="container">
  /// The container of the VDS
  /// </param>
  /// <param name="blob">
  /// The blob prefix of the VDS
  /// </param>
  /// <param name="parallelism_factor">
  /// The parallelism factor setting for the Azure Blob Storage library
  /// </param>
  /// <param name="max_execution_time">
  /// The max execution time setting for the Azure Blob Storage library
  /// </param>
  AzureOpenOptions(std::string const& connectionString, std::string const& container, std::string const& blob, int& parallelism_factor, int& max_execution_time) : OpenOptions(Azure), connectionString(connectionString), container(container), blob(blob), parallelism_factor(parallelism_factor), max_execution_time(max_execution_time) {}
 

  /// <summary>
  /// AzureOpenOptions factory function for bearer token based authentication 
  /// </summary>
  /// <param name="bearerToken">
  /// The bearer token
  /// </param>
  /// <param name="container">
  /// The container of the VDS
  /// </param>
  /// <param name="blob">
  /// The blob prefix of the VDS
  /// </param>
  /// <returns> A valid AzureOpenOptions </returns>
  static AzureOpenOptions AzureOpenOptionsBearer(std::string const &accountName, std::string const &bearerToken, std::string const &container, std::string const &blob) { AzureOpenOptions ret; ret.accountName = accountName; ret.bearerToken = bearerToken; ret.container = container; ret.blob = blob; return ret; }
};

/// <summary>
/// Options for opening a VDS with presigned Azure url
/// </summary>
struct AzurePresignedOpenOptions : OpenOptions
{
  std::string baseUrl;
  std::string urlSuffix;

  AzurePresignedOpenOptions() : OpenOptions(AzurePresigned) {}

  /// <summary>
  /// AzurePresignedOpenOptions constructor
  /// </summary>
  /// <param name="baseUrl">
  /// The base url for the VDS
  /// </param>
  /// <param name="urlSuffix">
  /// The suffix of the presigned url
  /// </param>
  AzurePresignedOpenOptions(const std::string &baseUrl, const std::string &urlSuffix) : OpenOptions(AzurePresigned), baseUrl(baseUrl), urlSuffix(urlSuffix) {}
};

/// <summary>
/// Credentials for opening a VDS in Google Cloud Storage
/// by using the string containing an access token
/// Using OAuth
/// </summary>
class GoogleCredentialsToken
{
    friend struct GoogleOpenOptions;
    
    std::string token;

public:
    /// <summary>
    /// GoogleCredentialsToken constructor
    /// </summary>
    /// <param name="token">
    /// The string containing an access token
    /// </param>
    explicit GoogleCredentialsToken(std::string const& token) : token(token) {}
    explicit GoogleCredentialsToken(std::string&& token) noexcept : token(std::move(token)) {}
};

/// <summary>
/// Credentials for opening a VDS in Google Cloud Storage
/// by path to the service account json file
/// Using OAuth
/// </summary>
class GoogleCredentialsPath
{
    friend struct GoogleOpenOptions;

    std::string path;

public:
    /// <summary>
    /// GoogleCredentialsPath constructor
    /// </summary>
    /// <param name="path">
    /// The path to the service account json file
    /// </param>
    explicit GoogleCredentialsPath(std::string const& path) : path(path) {}
    explicit GoogleCredentialsPath(std::string&& path) noexcept : path(std::move(path)) {}
};

/// <summary>
/// Credentials for opening a VDS in Google Cloud Storage
/// by the string containing json with credentials
/// Using OAuth
/// </summary>
class GoogleCredentialsJson
{
    friend struct GoogleOpenOptions;

    std::string json;

public:
    /// <summary>
    /// GoogleCredentialsJson constructor
    /// </summary>
    /// <param name="json">
    /// The string containing json with credentials
    /// </param>
    explicit GoogleCredentialsJson(std::string const& json) : json(json) {}
    explicit GoogleCredentialsJson(std::string&& json) noexcept : json(std::move(json)) {}
};

/// <summary>
/// Credentials for opening a VDS in Google Cloud Storage
/// by using the default credentials
/// Using signed URL mechanism
/// </summary>
class GoogleCredentialsSignedUrl
{
    friend struct GoogleOpenOptions;

    std::string region;

public:
    /// <summary>
    /// GoogleCredentialsSignedUrl constructor
    /// </summary>
    /// <param name="region">
    /// The string containing the region required for signature generation
    /// </param>
    explicit GoogleCredentialsSignedUrl(std::string const& region) : region(region) {}
    explicit GoogleCredentialsSignedUrl(std::string&& region) noexcept : region(std::move(region)) {}
};

/// <summary>
/// Credentials for opening a VDS in Google Cloud Storage
/// by path to the service account json file
/// Using signed URL mechanism
/// </summary>
class GoogleCredentialsSignedUrlPath
{
    friend struct GoogleOpenOptions;

    std::string region;
    std::string path;

public:
    /// <summary>
    /// GoogleCredentialsSignedUrlPath constructor
    /// </summary>
    /// <param name="region">
    /// The string containing the region required for signature generation
    /// </param>
    /// <param name="path">
    /// The path to the service account json file
    /// </param>
    explicit GoogleCredentialsSignedUrlPath(std::string const& region, std::string const& path) : region(region), path(path) {}
    explicit GoogleCredentialsSignedUrlPath(std::string&& region, std::string const& path) : region(std::move(region)), path(path) {}
    explicit GoogleCredentialsSignedUrlPath(std::string const& region, std::string&& path) : region(region), path(std::move(path)) {}
    explicit GoogleCredentialsSignedUrlPath(std::string&& region, std::string&& path) noexcept : region(std::move(region)), path(std::move(path)) {}
};

/// <summary>
/// Credentials for opening a VDS in Google Cloud Storage
/// by the string containing json with credentials
/// Using signed URL mechanism
/// </summary>
class GoogleCredentialsSignedUrlJson
{
    friend struct GoogleOpenOptions;

    std::string region;
    std::string json;

public:
    /// <summary>
    /// GoogleCredentialsSignedUrlJson constructor
    /// </summary>
    /// <param name="region">
    /// The string containing the region required for signature generation
    /// </param>
    /// <param name="json">
    /// The string containing json with credentials
    /// </param>
    explicit GoogleCredentialsSignedUrlJson(std::string const& region, std::string const& json) : region(region), json(json) {}
    explicit GoogleCredentialsSignedUrlJson(std::string&& region, std::string const& json) : region(std::move(region)), json(json) {}
    explicit GoogleCredentialsSignedUrlJson(std::string const& region, std::string&& json) : region(region), json(std::move(json)) {}
    explicit GoogleCredentialsSignedUrlJson(std::string&& region, std::string&& json) noexcept : region(std::move(region)), json(std::move(json)) {}
};

/// <summary>
/// Options for opening a VDS in Google Cloud Storage
/// </summary>
struct GoogleOpenOptions : OpenOptions
{
  using CredentialsIntType = unsigned int;
  enum class CredentialsType : CredentialsIntType
  {
    Default       = 0x0000,
    AccessToken   = 0x0001,
    Path          = 0x0002,
    Json          = 0x0003,
    SignedUrl     = 0x0010,
    SignedUrlPath = SignedUrl | Path,
    SignedUrlJson = SignedUrl | Json
  };

  CredentialsType credentialsType = CredentialsType::Default;

  std::string bucket;
  std::string pathPrefix;
  std::string credentials;
  std::string storageClass;
  std::string region;

  GoogleOpenOptions() : OpenOptions(GoogleStorage) {}
  /// <summary>
  /// GoogleOpenOptions constructor
  /// </summary>
  /// <param name="bucket">
  /// The bucket of the VDS
  /// </param>
  /// <param name="pathPrefix">
  /// The prefix of the VDS
  /// </param>
  /// <param name="credentials">
  /// Google Cloud Storage access credentials
  /// </param>
  GoogleOpenOptions(std::string const& bucket, std::string const& pathPrefix) 
      : OpenOptions(GoogleStorage)
      , bucket(bucket)
      , pathPrefix(pathPrefix)
  {}
  GoogleOpenOptions(std::string const& bucket, std::string const& pathPrefix, GoogleCredentialsToken const& credentials)
      : OpenOptions(GoogleStorage)
      , credentialsType(CredentialsType::AccessToken)
      , bucket(bucket)
      , pathPrefix(pathPrefix)
      , credentials(credentials.token)
  {}
  GoogleOpenOptions(std::string const& bucket, std::string const& pathPrefix, GoogleCredentialsPath const& credentials)
      : OpenOptions(GoogleStorage)
      , credentialsType(CredentialsType::Path)
      , bucket(bucket)
      , pathPrefix(pathPrefix)
      , credentials(credentials.path)
  {}
  GoogleOpenOptions(std::string const& bucket, std::string const& pathPrefix, GoogleCredentialsJson const& credentials)
      : OpenOptions(GoogleStorage)
      , credentialsType(CredentialsType::Json)
      , bucket(bucket)
      , pathPrefix(pathPrefix)
      , credentials(credentials.json)
  {}
  GoogleOpenOptions(std::string const& bucket, std::string const& pathPrefix, GoogleCredentialsSignedUrl const& credentials)
      : OpenOptions(GoogleStorage)
      , credentialsType(CredentialsType::SignedUrl)
      , bucket(bucket)
      , pathPrefix(pathPrefix)
      , region(credentials.region)
  {}
  GoogleOpenOptions(std::string const& bucket, std::string const& pathPrefix, GoogleCredentialsSignedUrlPath const& credentials)
      : OpenOptions(GoogleStorage)
      , credentialsType(CredentialsType::SignedUrlPath)
      , bucket(bucket)
      , pathPrefix(pathPrefix)
      , credentials(credentials.path)
      , region(credentials.region)
  {}
  GoogleOpenOptions(std::string const& bucket, std::string const& pathPrefix, GoogleCredentialsSignedUrlJson const& credentials)
      : OpenOptions(GoogleStorage)
      , credentialsType(CredentialsType::SignedUrlJson)
      , bucket(bucket)
      , pathPrefix(pathPrefix)
      , credentials(credentials.json)
      , region(credentials.region)
  {}

  bool SetSignedUrl()
  {
      if (CredentialsType::AccessToken == credentialsType)
      {
          return false;
      }

      credentialsType = static_cast<CredentialsType>(static_cast<CredentialsIntType>(credentialsType) | static_cast<CredentialsIntType>(CredentialsType::SignedUrl));
      return true;
  }
};

/// <summary>
/// Options for opening a VDS with a plain http url.
/// If there are query parameters in then they will be appended to the different sub urls.
/// The resulting IO backend will not support uploading data.
/// </summary>
struct HttpOpenOptions : OpenOptions
{
  std::string url;

  HttpOpenOptions() : OpenOptions(Http) {}
  /// <summary>
  /// HttpOpenOptions constructor
  /// </summary>
  /// <param name="url">
  /// The http base url of the VDS
  /// </param>
  HttpOpenOptions(std::string const &url) : OpenOptions(Http), url(url) {}
};

/// <summary>
/// Options for opening a VDS which is stored in memory (for testing)
/// </summary>
struct InMemoryOpenOptions : OpenOptions
{
  InMemoryOpenOptions() : OpenOptions(InMemory) {}
  InMemoryOpenOptions(const char *name)
    : OpenOptions(InMemory)
    , name(name)
  {}
  InMemoryOpenOptions(const std::string &name)
    : OpenOptions(InMemory)
    , name(name)
  {}
  std::string name;
};

/// <summary>
/// Options for opening a VDS file
/// </summary>
struct VDSFileOpenOptions : OpenOptions
{
  std::string fileName;

  VDSFileOpenOptions() : OpenOptions(VDSFile) {}

  /// <summary>
  /// VDSFileOpenOptions constructor
  /// </summary>
  /// <param name="fileName">
  /// The name of the VDS file
  /// </param>
  VDSFileOpenOptions(const std::string &fileName) : OpenOptions(VDSFile), fileName(fileName) {}
};

struct Error
{
  int code = 0;
  std::string string;
};

template<typename T>
struct VectorWrapper
{
  VectorWrapper(const std::vector<T> &toWrap)
    : data(toWrap.data())
    , size(toWrap.size())
  {}

  const T *data;
  size_t size;
};

struct StringWrapper
{
  StringWrapper(const std::string& toWrap)
    : data(toWrap.c_str())
    , size(toWrap.size())
  {}

  const char* data;
  size_t size;
};

typedef struct VDS *VDSHandle;
class VolumeDataLayout;
class VolumeDataAccessManager;
class VolumeDataPageAccessor;

/// <summary>
/// Create an OpenOptions struct from a url and connection string
/// </summary>
/// <param name="url">
/// The url scheme specific to each cloud provider
/// Available schemes are s3:// azure://
/// </param>
/// <param name="connectionString">
/// The cloud provider specific connection string
/// Specifies additional arguments for the cloud provider
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// This function news a OpenOptions struct that has to be deleted by
/// the caller. This is a helper function to allow applications modify the
/// OpenOption before passing it to Open. Use the Open and Create functions
/// with url and string instead if this is not needed.
/// </returns>
OPENVDS_EXPORT OpenOptions* CreateOpenOptions(StringWrapper url, StringWrapper connectionString, Error& error);

/// <summary>
/// Open an existing VDS
/// </summary>
/// <param name="url">
/// The url scheme specific to each cloud provider
/// Available schemes are s3:// azure://
/// </param>
/// <param name="connectionString">
/// The cloud provider specific connection string
/// Specifies additional arguments for the cloud provider
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Open(StringWrapper url, StringWrapper connectionString, Error& error);

/// <summary>
/// Open an existing VDS.
/// This is a simple wrapper that uses an empty connectionString
/// </summary>
/// <param name="url">
/// The url scheme specific to each cloud provider
/// Available schemes are s3:// azure://
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
inline VDSHandle Open(StringWrapper url, Error& error) { std::string connectionString; return Open(url, connectionString, error); }

/// <summary>
/// Open an existing VDS
/// </summary>
/// <param name="options">
/// The options for the connection
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Open(const OpenOptions& options, Error& error);

/// <summary>
/// Open an existing VDS
/// </summary>
/// <param name="ioManager">
/// The IOManager for the connection, it will be deleted automatically when the VDS handle is closed
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Open(IOManager*ioManager, Error &error);

/// <summary>
/// Create a new VDS
/// </summary>
/// <param name="url">
/// The url scheme specific to each cloud provider
/// Available schemes are s3:// azure://
/// </param>
/// <param name="connectionString">
/// The cloud provider specific connection string
/// Specifies additional arguments for the cloud provider
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Create(StringWrapper url, StringWrapper connectionString, VolumeDataLayoutDescriptor const& layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const& metadata, Error& error);

/// <summary>
/// Create a new VDS
/// This is a simple wrapper that uses an empty connectionString
/// </summary>
/// <param name="url">
/// The url scheme specific to each cloud provider
/// Available schemes are s3:// azure://
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
inline VDSHandle Create(StringWrapper url, VolumeDataLayoutDescriptor const& layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const& metadata, Error& error)
{
  std::string connectionString;
  return Create(url, connectionString, layoutDescriptor, axisDescriptors, channelDescriptors, metadata, error);
}

/// <summary>
/// Create a new VDS
/// </summary>
/// <param name="options">
/// The options for the connection
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Create(const OpenOptions& options, VolumeDataLayoutDescriptor const& layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const& metadata, Error& error);

/// <summary>
/// Create a new VDS

/// </summary>
/// <param name="ioManager">
/// The IOManager for the connection, it will be deleted automatically when the VDS handle is closed
/// </param>
/// <param name="error">
/// If an error occured, the error code and message will be written to this output parameter
/// </param>
/// <returns>
/// The VDS handle that can be used to get the VolumeDataLayout and the VolumeDataAccessManager
/// </returns>
OPENVDS_EXPORT VDSHandle Create(IOManager* ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const &metadata, Error &error);

/// <summary>
/// Get the VolumeDataLayout for a VDS
/// </summary>
/// <param name="handle">
/// The handle of the VDS
/// </param>
/// <returns>
/// The VolumeDataLayout of the VDS
/// </returns>
OPENVDS_EXPORT VolumeDataLayout *GetLayout(VDSHandle handle);

/// <summary>
/// Get the VolumeDataAccessManager for a VDS
/// </summary>
/// <param name="handle">
/// The handle of the VDS
/// </param>
/// <returns>
/// The VolumeDataAccessManager of the VDS
/// </returns>
OPENVDS_EXPORT VolumeDataAccessManager *GetAccessManager(VDSHandle handle);

/// <summary>
/// Close a VDS and free up all associated resources
/// </summary>
/// <param name="handle">
/// The handle of the VDS
/// </param>
OPENVDS_EXPORT void Close(VDSHandle handle);

/// <summary>
/// Get the GlobalState interface 
/// </summary>
/// <returns>
/// A pointer to the GlobalState interface
/// </returns>
OPENVDS_EXPORT GlobalState *GetGlobalState();

}

#endif //OPENVDS_H
