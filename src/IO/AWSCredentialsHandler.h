#ifndef AWSCREDENTIALSHANDLER_H
#define AWSCREDENTIALSHANDLER_H

#include <vector>
#include <string>

#include <unordered_map>

namespace OpenVDS
{
struct AWSCredentialsHandler
{
  AWSCredentialsHandler();

  void readParseProperties();

  std::unordered_map<std::string, std::string> m_properties;
};
}
#endif //AWSCREDENTIALSHANDLER_H
