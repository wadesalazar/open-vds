/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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

#ifndef CONNECTIONSTRINGPARSER_H
#define CONNECTIONSTRINGPARSER_H

#include <OpenVDS/OpenVDS.h>
#include <string>
#include <algorithm>
#include <map>
#include <fmt/format.h>

namespace OpenVDS
{

inline char asciitolower(char in) {
  if (in <= 'Z' && in >= 'A')
    return in - ('Z' - 'z');
  return in;
}

inline std::string trim(const char *start, const char *end)
{
  while (std::isspace(*start) && start < end)
    start++;
  end--;
  while (end > start && std::isspace(*end))
    end--;
  return std::string(start, end + 1);
}

inline std::map<std::string, std::string> ParseConnectionString(const char* connectionString, size_t connectionStringSize, Error &error)
{
  std::map<std::string, std::string> ret;
  auto it = connectionString;
  auto end = connectionString + connectionStringSize;
 
  const char* name_begin = nullptr;
  const char* name_end = nullptr;
  while (it < end)
  {
    auto keyValueEnd = std::find(it, end, ';');
    auto equals = std::find(it, keyValueEnd, '=');
    name_begin = it;
    name_end = equals;
    it = equals + 1;

    std::string name = trim(name_begin, name_end);
    if (name.empty() && it < keyValueEnd)
    {
      error.code = - 1;
      error.string = fmt::format("Empty name in connection string. Name must consist of more than empty spaces.");
      return ret;
    }
   
    if (it > keyValueEnd)
      continue;

    std::string value = trim(it, keyValueEnd);
    
    std::transform(name.begin(), name.end(), name.begin(), asciitolower);
    ret.emplace(std::move(name), std::move(value));
    it = keyValueEnd + 1;
  }
  return ret;
}
}

#endif //CONNECTIONSTRINGPARSER_H
