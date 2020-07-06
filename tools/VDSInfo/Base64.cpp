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

#include "Base64.h"

#include <cctype>
#include <cstring>

namespace OpenVDS
{
class Base64Table
{
  static const char Alphabet[65];
  static unsigned char Table[256];

  static Base64Table    Instance;

  Base64Table()
  {
    memset(Table, -1, sizeof(Table));

    for(int i = 0; i < int(sizeof(Alphabet)) - 1; i++)
    {
      Table[int(Alphabet[i])] = i;
    }
  }

public:
  static int  Decode(char a) { return Table[(unsigned char)a]; }
  static char Encode(unsigned char u) { return Alphabet[u]; }
};

Base64Table
Base64Table::Instance;

const char
Base64Table::Alphabet[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char
Base64Table::Table[256];

bool Base64Decode(const char *data, int64_t len, std::vector<unsigned char> &result)
{
  bool error = false;

  // skip leading whitespace
  while(len && isspace(*data)) len--, data++;
  if(len == 0) { return true; }

  result.reserve(result.size() + len / 4 * 3);

  while (len != 0 && !isspace(*data))
  {
    int a = Base64Table::Decode(*data++); len--;
    if (a == -1)
    {
      error = true;
      break;
    }

    if (len == 0 || isspace(*data))
    {
      error = true;
      break;
    }

    int b = Base64Table::Decode(*data++);
    len--;
    if (b == -1) {
      error = true;
      break;
    }
    result.push_back((a << 2) | (b >> 4));

    if (len == 0 || isspace(*data))
      break;
    if (*data == '=')
    {
      data++;
      len--;
      if(len==0 || *data++ != '=')
      {
        error = true; break;
      } len--;
      break;
    }
    int c = Base64Table::Decode(*data++);
    len--;
    if (c == -1)
    {
      error = true;
      break;
    }
    result.push_back(((b & 0xf) << 4) | (c >> 2));

    if (len == 0 || isspace(*data))
      break;
    if(*data == '=')
    {
      data++;
      len--;
      break;
    }
    int d = Base64Table::Decode(*data++);
    len--;
    if (d == -1)
    {
      error = true;
      break;
    }
    result.push_back(((c & 0x3) << 6) | d);
  }

  // skip trailing whitespace
  while (len && isspace(*data))
  {
    len--;
    data++;
  }

  if(len != 0)
  {
    error = true;
  }

  return !error;
}

void Base64Encode(const unsigned char *data, int64_t len, std::vector<char> &result)
{
  result.reserve(result.size() + ((len + 2) / 3) * 4);

  while(len > 0)
  {
    int a = data[0] >> 2, b = (data[0] & 0x03) << 4, c = 0, d = 0;

    if(len > 1) { b |= data[1] >> 4; c = (data[1] & 0x0f) << 2; }
    if(len > 2) { c |= data[2] >> 6; d = (data[2] & 0x3f) << 0; }

    result.push_back(Base64Table::Encode(a));
    result.push_back(Base64Table::Encode(b));
    result.push_back((len > 1) ? Base64Table::Encode(c) : '=');
    result.push_back((len > 2) ? Base64Table::Encode(d) : '=');

    len -= 3; data += 3;
  }
}
}
