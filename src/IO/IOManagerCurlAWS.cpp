#include "IOManagerCurlAWS.h"

#include "RequestSigner.h"

#include <chrono>
#include <fmt/chrono.h>

#define AWS_DATE_FORMAT "{:%Y%m%dT%H%M%SZ}"

namespace OpenVDS
{

static std::string CurrentTimeAsUTC()
{
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  return fmt::format(AWS_DATE_FORMAT, *std::gmtime(&in_time_t));
}

struct S3Headers
{
  S3Headers(std::string date = CurrentTimeAsUTC())
    : date(date)
    , xAmzContentSha256("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")
  {}
  std::string contentDisposition;
  std::string contentType;
  std::string contentLength;
  std::string date;
  std::string host;
  std::string xAmzContentSha256;
  std::string expect;
  std::vector<std::pair<std::string, std::string>> xAmzMeta;
};


static std::string FormatBufferAsHexString(const std::vector<unsigned char>& buffer)
{
  std::vector<char> charBuffer(2 * buffer.size() + 1);

  for (auto i = 0u; i < buffer.size(); i++)
  {
    snprintf(&charBuffer[i * 2], 3, "%02x", (int)buffer[i]);
  }

  std::string s(&charBuffer[0], charBuffer.size() - 1);
  return s;
}

struct S3RequestAuthentificationContext
{
  std::string awsAccessKeyID;
  std::string awsSecretKey;
  std::string awsSessionToken;
  std::string awsRegion;
};

static std::string CreateAWSStringToSign(std::string awsRegion, std::string awsService, std::string hexHash, std::string requestTime)
{
  std::string buffer;
  buffer.reserve(256); // 256 bytes should be enough for everyone

  buffer += "AWS4-HMAC-SHA256";
  buffer.push_back('\n');

  buffer += requestTime;
  buffer.push_back('\n');

  buffer += requestTime.substr(0, 8);
  buffer.push_back('/');
  buffer += awsRegion;
  buffer.push_back('/');
  buffer += awsService;
  buffer += "/aws4_request";
  buffer.push_back('\n');
  buffer += hexHash;

  return buffer;
}

static std::vector<unsigned char> ComputeRequestSigningKey(std::string awsSecretKey, std::string yyyymmdd, std::string region, std::string awsService)
{
  auto
    byteKey = RequestSigner::NormalizeLineEndings("AWS4" + awsSecretKey);

  auto
    dateKey = RequestSigner::Compute_HMAC_SHA256(yyyymmdd, byteKey),
    dateRegionKey = RequestSigner::Compute_HMAC_SHA256(region, dateKey),
    dateRegionServiceKey = RequestSigner::Compute_HMAC_SHA256(awsService, dateRegionKey),
    signingKey = RequestSigner::Compute_HMAC_SHA256("aws4_request", dateRegionServiceKey);

  return signingKey;
}

static std::string FormatAuthorizationHeader(const std::string& requestScope, const std::map<std::string, std::string> &headers, const std::string& signature)
{
  std::string ret;
  ret.reserve(512);

  ret += "AWS4-HMAC-SHA256 ";

  ret += "Credential=";
  ret += requestScope;
  ret.push_back(',');

  ret += "SignedHeaders=";

  bool first = true;
  for (auto pair : headers)
  {
    if (first)
      first = false;
    else
      ret.push_back(';');
    ret += pair.first;
  }
  ret.push_back(',');

  ret += "Signature=";
  ret += signature;

  return ret;
}

static std::string FormatScope(const std::string& aws_access_key, const std::string& yyyymmdd, const std::string& aws_region, const std::string& aws_service)
{
  std::string s = aws_access_key + "/" + yyyymmdd + "/" + aws_region + "/" + aws_service + "/aws4_request";
  return s;
}

std::string CreateCanonicalRequest(const std::string &httpVerb, const std::string &path, const std::map<std::string, std::string> &headers, std::string hashedPayload)
{
  std::string buffer;

  buffer.reserve(1024);

  buffer += httpVerb;
  buffer.push_back('\n');

  buffer += path;
  buffer.push_back('\n');

  buffer.push_back('\n');

  for (auto& pair : headers)
  {
    buffer += pair.first;
    buffer.push_back(':');
    buffer += pair.second;
    buffer.push_back('\n');
  }

  buffer.push_back('\n');

  bool first = true;
  for (auto &pair : headers)
  {
    if (first)
      first = false;
    else
      buffer.push_back(';');
    buffer += pair.first;
  }
  buffer.push_back('\n');

  buffer += hashedPayload;

  return buffer;
}

static std::vector<std::string> GetS3RequestHeaders(const std::string& verb, const std::string &host, const std::string &path, const S3RequestAuthentificationContext& awsContext, const S3Headers &s3headers)
{

  if (host.empty() || awsContext.awsAccessKeyID.empty() || awsContext.awsSecretKey.empty() || awsContext.awsRegion.empty())
  {
    return std::vector<std::string>();
  }

  std::map<std::string, std::string> headers;
  headers["host"] = host;
  headers["x-amz-content-sha256"] = s3headers.xAmzContentSha256;
  headers["x-amz-date"] = s3headers.date;
  if (!awsContext.awsSessionToken.empty())
  {
    headers["x-amz-security-token"] = awsContext.awsSessionToken;
  }
  if (s3headers.contentDisposition.size())
    headers["content-disposition"] = s3headers.contentDisposition;
  if (s3headers.contentType.size())
    headers["content-type"] = s3headers.contentType;
  if (s3headers.contentLength.size())
    headers["content-length"] = s3headers.contentLength;
  if (s3headers.expect.size())
    headers["expect"] = s3headers.expect;
  for (auto &meta : s3headers.xAmzMeta)
  {
    headers[meta.first] = meta.second;
  }

  auto yyyymmdd = s3headers.date.substr(0, 8);

  auto canonicalRequest = CreateCanonicalRequest(verb, path, headers, s3headers.xAmzContentSha256);

  auto canonicalRequestHash = FormatBufferAsHexString(RequestSigner::Compute_SHA256(canonicalRequest));

  auto stringToSign = CreateAWSStringToSign(awsContext.awsRegion, "s3", canonicalRequestHash, s3headers.date);

  auto signingKey = ComputeRequestSigningKey(awsContext.awsSecretKey, yyyymmdd, awsContext.awsRegion, "s3");

  auto signature = RequestSigner::Compute_HMAC_SHA256(stringToSign, signingKey);

  auto signatureHex = FormatBufferAsHexString(signature);

  auto requestScope = FormatScope(awsContext.awsAccessKeyID, yyyymmdd, awsContext.awsRegion, "s3");

  auto authHeader = FormatAuthorizationHeader(requestScope, headers, signatureHex);

  headers["authorization"] = authHeader;

  std::vector<std::string> headersOut;
  headersOut.reserve(headers.size());

  for (auto const& pair : headers)
  {
    headersOut.push_back(pair.first + ":" + pair.second);
  }

  return headersOut;
}

static std::string composeHostname(const AWSOpenOptions &openOptions)
{
  std::string ret;
  ret.reserve(256);
  ret += openOptions.bucket;
  ret += ".s3.";
  ret += openOptions.region;
  ret += ".amazonaws.com";
  return ret;
}

static std::string composeBaseUrl(const AWSOpenOptions& openOptions, const std::string &hostname)
{
  std::string ret;
  ret.reserve(256);
  //if (openOptions.useHttps)
    ret += "https://";
  //else
  //  ret += "http://";
  ret += hostname;
  return ret;
}

static void headerNameTransform(std::string& name)
{
  static const std::string xamzmeta("x-amz-meta-");
  if (name.size() < xamzmeta.size())
    return;
  if (memcmp(name.data(), xamzmeta.data(), xamzmeta.size()) == 0)
  {
    name.erase(0, xamzmeta.size());
  }
}

IOManagerCurlAWS::IOManagerCurlAWS(const AWSOpenOptions& openOptions, Error& error)
  : IOManagerCurl(headerNameTransform, error)
  , m_openOptions(openOptions)
  , m_hostname(composeHostname(openOptions))
  , m_baseUrl(composeBaseUrl(openOptions, m_hostname))
{
  fmt::print(stdout, "Initializing IOManagerCurlAWS\n");

  if (m_openOptions.key[0] != '/')
    m_openOptions.key = "/" + m_openOptions.key;
}
IOManagerCurlAWS::~IOManagerCurlAWS()
{
}

std::shared_ptr<Request> IOManagerCurlAWS::Download(const std::string requestName, std::shared_ptr<TransferDownloadHandler> handler, const IORange& range)
{
  S3Headers headers;
  std::string path = m_openOptions.key + "/" + requestName;
  std::string url = m_baseUrl + path;

  S3RequestAuthentificationContext s3RequestAuthentificationContext = { m_credentialsHandler.m_properties["aws_access_key_id"], m_credentialsHandler.m_properties["aws_secret_access_key"], "", m_openOptions.region };
  std::vector<std::string> curlHeaders = GetS3RequestHeaders("GET", m_hostname, path, s3RequestAuthentificationContext, headers);
  std::shared_ptr<DownloadRequestCurl> request = std::make_shared<DownloadRequestCurl>(requestName, handler);
  addDownloadRequest(request, url, curlHeaders);
  return request;
}

std::shared_ptr<Request> IOManagerCurlAWS::Upload(const std::string requestName, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeaders, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
{
  S3Headers headers;
  if (contentDispostionFilename.size())
   headers.contentDisposition = std::string("attachment; filename=\"") + contentDispostionFilename + "\"";
  if (contentType.size())
    headers.contentType = contentType;

  headers.xAmzContentSha256 = FormatBufferAsHexString(RequestSigner::Compute_SHA256(*data));
  headers.contentLength = std::to_string(data->size());
  headers.expect = "100-continue";

  headers.xAmzMeta.reserve(metadataHeaders.size());
  const std::string metaPrefix("x-amz-meta-");
  for (auto &metaHeader : metadataHeaders)
  {
    headers.xAmzMeta.emplace_back(metaPrefix + metaHeader.first, metaHeader.second);
  }

  std::string path = m_openOptions.key + "/" + requestName;
  std::string url = m_baseUrl + path;
  S3RequestAuthentificationContext s3RequestAuthentificationContext = {m_credentialsHandler.m_properties["aws_access_key_id"], m_credentialsHandler.m_properties["aws_secret_access_key"], "", m_openOptions.region};
  std::vector<std::string> curlHeaders = GetS3RequestHeaders("PUT", m_hostname, path,  s3RequestAuthentificationContext, headers);

  std::shared_ptr<UploadRequestCurl> request = std::make_shared<UploadRequestCurl>(requestName, completedCallback);
  addUploadRequest(request, url, curlHeaders, data);
  return request;
}
}
