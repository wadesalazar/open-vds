#include "AWSCredentialsHandler.h"

#include <IO/File.h>

#include <fmt/printf.h>

#include <unordered_map>

namespace OpenVDS
{
static std::string trimInBothEnds(const std::string &str)
{
  const char* start = str.data();
  const char* end = str.data() + str.size();
  while (std::isspace(*start) && start < end)
    start++;
  end--;
  while (end > start && std::isspace(*end))
    end--;
  return std::string(start, end + 1);
}

static std::unordered_map<std::string, std::string> readParsePropertiesStatic()
{
#ifdef WIN32
  const char *homeEnv = getenv("USERPROFILE");
#else
  const char *homeEnv = getenv("HOME");
#endif

  std::string home;
  if (homeEnv)
      home = homeEnv;
  std::string credentialsFile = home + "/.aws/credentials";
  IOError error;
  File file;
  if (!file.Open(credentialsFile , false, false, false, error))
  {
    fmt::print(stderr, "Failed to open Credentials file {}, got error: {}\n", credentialsFile, error.string);
    return {};
  }
  int64_t size = file.Size(error);
  if (error.code)
  {
    fmt::print(stderr, "Failed to get size of file {} - {}\n", credentialsFile, error.string);
    return {} ;
  }
  std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
  if (!file.Read(data.get(), 0, size, error))
  {
    fmt::print(stderr, "Failed to read file {} - {}\n", credentialsFile, error.string);
    return {};
  }

  std::vector<std::string> lines;
  {
    auto it = data.get();
    auto new_it = it;
    auto end = data.get() + size;
    for (;it < end && *it != 0; it = new_it + 1)
    {
      new_it = std::find_if(it, end, [](uint8_t a) { return a == '\n' || a == 0; });
      if (new_it == end)
        lines.emplace_back(it, end);
      else if (new_it == it)
      {
        continue;
      }
      else
      {
        lines.emplace_back(it, new_it);
      }
    }
  }

  std::string profile_name;
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> unsorted_properties;
  for (auto &line : lines)
  {
    if (line.empty())
      continue;
    auto it = std::find_if(line.begin(), line.end(), [](char a) {return a == '=';});
    if (it < line.end() - 1)
    {
      std::string name(line.begin(), it);
      std::string value(it + 1, line.end());
      name = trimInBothEnds(name);
      value = trimInBothEnds(value);
      unsorted_properties[profile_name][name] = value;
    }
    else
    {
      std::string trimmedLine = trimInBothEnds(line);
      if (trimmedLine.empty())
        continue;
      if (trimmedLine.front() == '[' && trimmedLine.back() == ']')
      {
        profile_name = std::string(trimmedLine.begin() + 1, trimmedLine.end() - 1);
      }
    }
  }

  std::string aws_profile;
  {
    const char *profile = getenv("AWS_PROFILE");
    if (profile)
      aws_profile = profile;
  }
  if (aws_profile.empty())
    aws_profile = "default";
  std::unordered_map<std::string, std::string> map;
  auto &propes_for_profile = unsorted_properties[aws_profile];
  for (auto &keyvalue : propes_for_profile)
    map.emplace(keyvalue.first, keyvalue.second);

  return map;
}
AWSCredentialsHandler::AWSCredentialsHandler()
{
  readParseProperties();
}

void AWSCredentialsHandler::readParseProperties()
{
  m_properties = readParsePropertiesStatic();
}

}
