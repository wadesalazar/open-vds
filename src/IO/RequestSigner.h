#ifndef _REQUESTSIGNER_H_
#define _REQUESTSIGNER_H_

#include <string>
#include <map>
#include <vector>
#include "fmt/core.h"



namespace OpenVDS
{
namespace RequestSigner {

  std::vector<unsigned char> Compute_HMAC_SHA256(const std::vector<unsigned char>& buffer, const std::vector<unsigned char>& key);
  std::vector<unsigned char> Compute_HMAC_SHA256(const std::string& s, const std::vector<unsigned char>& key);
  std::vector<unsigned char> Compute_SHA256(const std::vector<unsigned char>& buffer);
  std::vector<unsigned char> Compute_SHA256(const std::string &buffer);
  
  std::vector<unsigned char> NormalizeLineEndings(const std::string& s);
}

}
#endif // _REQUESTSIGNER_H_
