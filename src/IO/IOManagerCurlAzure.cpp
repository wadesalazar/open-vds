#include "IOManagerCurlAzure.h"

#include "RequestSigner.h"
#include <VDS/Base64.h>

#include <chrono>
#include <fmt/chrono.h>


#define AZURE_DATE_FORMAT "{:%a, %d %b %Y %H:%M:%S GMT}"
#define AZURE_SERVICE_VERSION "2018-03-28"
// https://docs.microsoft.com/en-us/rest/api/storageservices/versioning-for-the-azure-storage-services
// Current recommended version

namespace OpenVDS
{

static std::string CurrentTimeAsUTC()
{
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  return fmt::format(AZURE_DATE_FORMAT, *std::gmtime(&in_time_t));
}

struct BlobRequestAuthenticationContext
{
  BlobRequestAuthenticationContext(std::string accountKey, std::string accountName, std::string containerName, std::string blobName)
    : accountKey(std::move(accountKey))
    , accountName(std::move(accountName))
    , containerName(std::move(containerName))
    , blobName(std::move(blobName))
  {
  }
  std::string accountKey;
  std::string accountName;
  std::string containerName;
  std::string blobName;
};

struct BlobHeaderFields
{
  BlobHeaderFields(std::string date = CurrentTimeAsUTC())
    : contentLength(0)
    , xMsDate(std::move(date))
    , xMsVersion(AZURE_SERVICE_VERSION)
  {}
  int contentLength;
  std::string contentType;
  std::string xMsDate;
  std::string xMsVersion;
  std::string xMsBlobType;
  std::vector<std::pair<std::string, std::string>> xMsMeta;
  std::string xMsBlobContentDisposition;
};

// Azure Shared Key Signature Helper Methods
// Refer: https://docs.microsoft.com/en-us/rest/api/storageservices/authorize-with-shared-key

static std::string GetAzureBaseStringToSign(const std::string& httpVerb, const BlobHeaderFields &headers)
{
  // TODONOTE: The Date and Range headers are empty here and are passed in as x-ms-* headers. The Range header specifically has 
  // a 4Gb limit and since block blobs can exceed that limit the custom x-ms-range is used instead. 
  std::string contentLength = headers.contentLength > 0 ? std::to_string(headers.contentLength) : std::string();

  std::string buffer;
  buffer.reserve(256);

  buffer += httpVerb;                       buffer.push_back('\n'); /*HTTP Verb*/ 
  buffer.push_back('\n');                                           /*Content-Encoding*/
  buffer.push_back('\n');                                           /*Content-Language*/
  buffer += contentLength;                  buffer.push_back('\n'); /*Content-Length (empty string when zero)*/
  buffer.push_back('\n');                                           /*Content-MD5*/
  buffer += headers.contentType;            buffer.push_back('\n'); /*Content-Type*/
  buffer.push_back('\n');                                           /*Date (passed in extraHeaders as x-ms-range*/
  buffer.push_back('\n');                                           /*If-Modified-Since */
  buffer.push_back('\n');                                           /*If-Match*/
  buffer.push_back('\n');                                           /*If-None-Match*/
  buffer.push_back('\n');                                           /*If-Unmodified-Since*/
                                                                       /*Range (passed in extraHeaders as x-ms-range)*/
  return buffer;
}

static std::string GetAzureCanonicalHeaders(const BlobHeaderFields &headers)
{
  std::string ret;
  ret.reserve(512);

  if (headers.xMsBlobContentDisposition.size())
  {
    ret += "x-ms-blob-content-disposition:";
    ret += headers.xMsBlobContentDisposition;
    ret.push_back('\n');
  }

  if (headers.xMsBlobType.size())
  {
    ret += "x-ms-blob-type:";
    ret += headers.xMsBlobType;
    ret.push_back('\n');
  }

  ret += "x-ms-date:";
  ret += headers.xMsDate;
  ret.push_back('\n');

  for (auto &metadata : headers.xMsMeta)
  {
    ret += metadata.first;
    ret += ":";
    ret += metadata.second;
    ret.push_back('\n');
  }

  ret += "x-ms-version:";
  ret += headers.xMsVersion;

  return ret;
}

static std::string GetAzureCanonicalizedResource(const std::string& accountName, const std::string& containerName, const std::string& blobName)
{
  std::string resourceString = "/" + accountName + "/" + containerName + "/" + blobName;
  return resourceString;
}

static std::string CreateAzureStringToSign(const std::string &httpVerb, const BlobRequestAuthenticationContext& context, const BlobHeaderFields &headers)
{
  std::string baseStringToSign = GetAzureBaseStringToSign(httpVerb, headers);
  std::string canonicalHeaders = GetAzureCanonicalHeaders(headers);
  std::string canonicalizedResource = GetAzureCanonicalizedResource(context.accountName, context.containerName, context.blobName);

  std::string stringToSign = baseStringToSign + '\n' + canonicalHeaders + '\n' + canonicalizedResource;

  return stringToSign;

}

std::vector<std::string> GetBlobGetRequestHeaders(const std::string &httpVerb, const std::string& url, const BlobRequestAuthenticationContext& blobContext, const BlobHeaderFields &headers)
{
  if (url.empty() || blobContext.accountKey.empty())
  {
    return std::vector<std::string>();
  }

  // NOTE: Signature = Base64(HMAC - SHA256(UTF8(StringToSign), Base64.decode(<your_azure_storage_account_shared_key>)))

  auto stringToSign = CreateAzureStringToSign(httpVerb, blobContext, headers);

  std::vector<unsigned char> signingKey;

  bool success = Base64Decode(blobContext.accountKey.data(), blobContext.accountKey.size(), signingKey);

  if (!success)
  {
    return std::vector<std::string>();
  }

  std::vector<unsigned char> signature = RequestSigner::Compute_HMAC_SHA256(stringToSign, signingKey);

  std::vector<char> charsig;

  Base64Encode(signature.data(), signature.size(), charsig);

  std::string authHeader;
  authHeader.reserve(512);
  authHeader += "SharedKey ";
  authHeader += blobContext.accountName;
  authHeader += ":";
  authHeader.insert(authHeader.end(), charsig.begin(), charsig.end());

  std::vector<std::string> headersOut;
  headersOut.push_back("Authorization: " + authHeader);
  headersOut.push_back("x-ms-date: " + headers.xMsDate);
  headersOut.push_back("x-ms-version: " + headers.xMsVersion);
  if (headers.contentLength)
    headersOut.push_back("Content-Length:"+ std::to_string(headers.contentLength));
  if (headers.contentType.size())
    headersOut.push_back("Content-Type:" + headers.contentType);
  if (headers.xMsBlobType.size())
    headersOut.push_back("x-ms-blob-type:" + headers.xMsBlobType);
  if (headers.xMsBlobContentDisposition.size())
    headersOut.push_back("x-ms-blob-content-disposition:" + headers.xMsBlobContentDisposition);
  for (auto &meta : headers.xMsMeta)
  {
    headersOut.push_back(meta.first + ":" + meta.second);
  }

  return headersOut;
}

void parseConnectionString(const AzureOpenOptions& openOptions, AzureParsedConnectionString &parsedData)
{
  parsedData.useHttps = true;
  if (openOptions.connectionString.empty())
    return;
  auto first = openOptions.connectionString.begin();
  while (first < openOptions.connectionString.end())
  {
    auto end = std::find(first, openOptions.connectionString.end(), ';');
    if (end == openOptions.connectionString.end())
      break;
    std::string keyvalue(first, end);
    first = end + 1;
    std::string key;
    std::string value;
    getKeyValueFromLine(keyvalue.c_str(), keyvalue.size(), key, value, '=');
    if (key == "DefaultEndpointsProtocol")
    {
      if (value == "http")
        parsedData.useHttps = false;
      else
        parsedData.useHttps = true;
    }
    else if (key == "AccountName")
    {
      parsedData.accountName = value;
    }
    else if (key == "AccountKey")
    {
      parsedData.accountKey = value;
    }
    else if (key == "EndpointSuffix")
    {
      parsedData.endpointSuffix = value;
    }
  }
  if (parsedData.endpointSuffix.empty())
    parsedData.endpointSuffix = "core.windows.net";
}

static std::string composeBaseUrl(const AzureOpenOptions& openOptions, const AzureParsedConnectionString &parsedConnectionString)
{
  std::string ret;
  ret.reserve(256);
  if (parsedConnectionString.useHttps)
    ret += "https://";
  else
    ret += "http://";

  ret += parsedConnectionString.accountName;
  ret += ".blob.";
  ret += parsedConnectionString.endpointSuffix;
  ret += "/";
  ret += openOptions.container;
  ret += "/";
  return ret;
}

static void headerNameTransform(std::string& name)
{
  static const std::string xamzmeta("x-ms-meta-");
  if (name.size() < xamzmeta.size())
    return;
  if (memcmp(name.data(), xamzmeta.data(), xamzmeta.size()) == 0)
  {
    name.erase(0, xamzmeta.size());
  }
}

IOManagerCurlAzure::IOManagerCurlAzure(const AzureOpenOptions& openOptions, Error& error)
  : IOManagerCurl(headerNameTransform, error)
  , m_openOptions(openOptions)
{
  fmt::print(stdout, "Initializing IOManagerCurlAzure\n");
  parseConnectionString(m_openOptions, m_parsedConnectionString);
  //Should we fail here.
  //if (m_parsedConnectionString.accountName.empty())
  //{
  //  error.code = -111;
  //  error.string = fmt::format("Failed to aprse Azure connection string '{}'.\n", openOptions.connectionString);
  //  return;
  //}
  m_baseUrl = composeBaseUrl(m_openOptions, m_parsedConnectionString);
}
IOManagerCurlAzure::~IOManagerCurlAzure()
{
}


std::shared_ptr<Request> IOManagerCurlAzure::Download(const std::string requestName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range)
{
  BlobHeaderFields headers;
  std::string url = m_baseUrl + requestName;
  BlobRequestAuthenticationContext context(m_parsedConnectionString.accountKey, m_parsedConnectionString.accountName, m_openOptions.container, requestName);

  auto curlHeaders = GetBlobGetRequestHeaders("GET", url, context, headers);

  std::shared_ptr<DownloadRequestCurl> request = std::make_shared<DownloadRequestCurl>(requestName,handler);
  addDownloadRequest(request, url, curlHeaders);
  return request;
}

std::shared_ptr<Request> IOManagerCurlAzure::Upload(const std::string requestName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeaders, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
{
  BlobHeaderFields headers;
  headers.contentLength = data->size();
  headers.contentType = contentType;
  if (contentDispostionFilename.size())
    headers.xMsBlobContentDisposition = std::string("attachment; filename=\"") + contentDispostionFilename + "\"";
  
  headers.xMsBlobType = "BlockBlob";
  headers.xMsMeta.reserve(metadataHeaders.size());
  std::string xmsmeta = "x-ms-meta-";
  for (auto &meta : metadataHeaders)
  {
    headers.xMsMeta.emplace_back(xmsmeta + meta.first, meta.second);
  }

  std::string url = m_baseUrl + requestName;
  BlobRequestAuthenticationContext context(m_parsedConnectionString.accountKey, m_parsedConnectionString.accountName, m_openOptions.container, requestName);
  auto curlHeaders = GetBlobGetRequestHeaders("PUT", url, context, headers);

  std::shared_ptr<UploadRequestCurl> request = std::make_shared<UploadRequestCurl>(requestName, completedCallback);
  addUploadRequest(request, url, curlHeaders, data);
  return request;
}

}
