#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN // Needed for cryptography API.
  #undef WIN32_LEAN_AND_MEAN
  #endif

  #include <windows.h>
  #include <bcrypt.h>

  #ifndef STATUS_SUCCESS
  #define STATUS_SUCCESS 0x0
  #endif

  #if _MSC_VER < 1900 // VS 2015
  #define snprintf _snprintf
  #endif
#else
  #include <openssl/sha.h>
  #include <openssl/hmac.h>
  #include <openssl/evp.h>
#endif

#include "RequestSigner.h"
#include <VDS/Base64.h>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip> // for std::put_time
#include <cassert>
#include <regex>

namespace OpenVDS 
{

namespace RequestSigner {
 
#ifdef WIN32
  // Based on https://msdn.microsoft.com/en-us/library/windows/desktop/aa376217(v=vs.85).aspx
static std::vector<unsigned char> ComputeSHA256_HMAC_Internal(const void *buffer, size_t buffer_size, const std::vector<unsigned char>& key, bool isHMAC)
{
  DWORD              cbData       = 0,
                     cbHash       = 0,
                     cbHashObject = 0;
  BCRYPT_ALG_HANDLE  hAlg         = NULL;
  BCRYPT_HASH_HANDLE hHash        = NULL;
  PBYTE              pbHashObject = NULL,
                     pbHash       = NULL;
  DWORD              dwFlagsAlg   = 0;

  if (isHMAC) {
    dwFlagsAlg |= BCRYPT_ALG_HANDLE_HMAC_FLAG;
  }

  auto 
    status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, dwFlagsAlg);

  switch (status)
  {
  default:
    assert("Unhandled error");
  case STATUS_SUCCESS:
    break;
  }

  status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0);
  switch (status)
  {
  default:
    assert("Unhandled error");
  case STATUS_SUCCESS:
    break;
  }

  pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
  assert(pbHashObject != NULL);

  status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHash, sizeof(DWORD), &cbData, 0);
  switch (status)
  {
  default:
    assert("Unhandled error");
  case STATUS_SUCCESS:
    break;
  }

  pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);
  assert(pbHash != NULL);

  PBYTE pbSecret = isHMAC ? (PBYTE)&key[0] : NULL;
  ULONG cbSecret = isHMAC ? (ULONG)key.size() : 0;

  status = BCryptCreateHash(hAlg, &hHash, pbHashObject, cbHashObject, pbSecret, cbSecret, 0);
  switch (status)
  {
  default:
    assert("Unhandled error");
  case STATUS_SUCCESS:
    break;
  }

  //status = BCryptHashData(hHash, (PBYTE)&buffer[0], (ULONG)buffer.size(), 0);
  status = BCryptHashData(hHash, (PBYTE)buffer, (ULONG)buffer_size, 0);
  switch (status)
  {
  default:
    assert("Unhandled error");
  case STATUS_SUCCESS:
    break;
  }

  status = BCryptFinishHash(hHash, pbHash, cbHash, 0);
  switch (status)
  {
  default:
    assert("Unhandled error");
  case STATUS_SUCCESS:
    break;
  }

  std::vector<unsigned char>
    hash(pbHash, pbHash + cbHash);

  // Cleanup
   if(hAlg)
    {
        BCryptCloseAlgorithmProvider(hAlg,0);
    }

    if (hHash)    
    {
        BCryptDestroyHash(hHash);
    }

    if(pbHashObject)
    {
        HeapFree(GetProcessHeap(), 0, pbHashObject);
    }

    if(pbHash)
    {
        HeapFree(GetProcessHeap(), 0, pbHash);
    }

  return hash;
}
#else
static std::vector<unsigned char> ComputeSHA256_HMAC_Internal(const void *buffer, size_t buffer_size, const std::vector<unsigned char>& key, bool isHMAC) {
  if (!isHMAC)
  {
    std::vector<unsigned char>
      hash(SHA256_DIGEST_LENGTH);

    SHA256((const unsigned char *)buffer, buffer_size, &hash[0]);
    return hash;
  }
  else
  {
    std::vector<unsigned char>
      hash(EVP_MAX_MD_SIZE);

    unsigned int
      mdLength = 0;

    unsigned char
      *pzHash = HMAC(EVP_sha256(), &key[0], key.size(), (const unsigned char *)buffer, buffer_size, &hash[0], &mdLength);

    assert(mdLength == SHA256_DIGEST_LENGTH);
    
    hash.resize(mdLength);
    return hash;
  }
}
#endif

  void AppendStringToBuffer(std::vector<unsigned char>& buffer, std::string s)
  {
    buffer.insert(buffer.end(), s.begin(), s.end());
  }

  
  std::vector<unsigned char> Compute_HMAC_SHA256(const std::vector<unsigned char>& buffer, const std::vector<unsigned char>& key)
  {
    return ComputeSHA256_HMAC_Internal(buffer.data(), buffer.size(), key, true);
  }

  std::vector<unsigned char> Compute_HMAC_SHA256(const std::string & s, const std::vector<unsigned char>& key)
  {
    auto
      buffer = NormalizeLineEndings(s);

    return ComputeSHA256_HMAC_Internal(buffer.data(), buffer.size(), key, true);
  }

  std::vector<unsigned char> Compute_SHA256(const std::vector<unsigned char>& buffer)
  {
    return ComputeSHA256_HMAC_Internal(buffer.data(), buffer.size(), std::vector<unsigned char>(), false);
  }
  std::vector<unsigned char> Compute_SHA256(const std::string &buffer)
  {
    return ComputeSHA256_HMAC_Internal(buffer.data(), buffer.size(), std::vector<unsigned char>(), false);
  }

  std::vector<unsigned char> NormalizeLineEndings(const std::string & s)
  {
    std::vector<unsigned char>
      buffer;

    buffer.reserve(s.size());

    for (auto i = 0; i < s.size(); ++i)
    {
      unsigned char
        c = s[i];

      if (c != '\r') {
        buffer.push_back(c);
      }
    }

    return buffer;
  }
 

}// namespace RequestSigner
}
