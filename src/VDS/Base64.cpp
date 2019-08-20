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

namespace OpenVDS
{
class Base64Table
{
  static const char alphabet[65];
  static unsigned char table[256];

  static Base64Table    instance;

  Base64Table()
  {
    memset(table, -1, sizeof(table));

    for(int i = 0; i < sizeof(alphabet) - 1; i++)
    {
      table[alphabet[i]] = i;
    }
  }

public:
  static int  decode(char a) { return table[(unsigned char)a]; }
  static char encode(unsigned char u) { return alphabet[u]; }
};

Base64Table
Base64Table::instance;

const char
Base64Table::alphabet[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char
Base64Table::table[256];

bool Base64Decode(const char *data, int len, std::vector<unsigned char> &result)
{
  bool error = false;

  int
    decode = 0;

  // skip leading whitespace
  while(len && isspace(*data)) len--, data++;
  if(len == 0) { error = true; }

  result.reserve(result.size() + len / 4 * 3);

  while(len != 0 && !isspace(*data))
  {
    int a = Base64Table::decode(*data++); len--; if(a == -1) { error = true; break; }

    if(len == 0 || isspace(*data)) { error = true; break; }
    int b = Base64Table::decode(*data++); len--; if(b == -1) { error = true; break; }
    result.push_back((a << 2) | (b >> 4));

    if(len == 0 || isspace(*data)) break; if(*data == '=') { data++; len--; if(len==0 || *data++ != '=') { error = true; break; } len--; break; }
    int c = Base64Table::decode(*data++); len--; if(c == -1) { error = true; break; }
    result.push_back(((b & 0xf) << 4) | (c >> 2));

    if(len == 0 || isspace(*data)) break; if(*data == '=') { data++; len--; break; }
    int d = Base64Table::decode(*data++); len--; if(d == -1) { error = true; break; }
    result.push_back(((c & 0x3) << 6) | d);
  }

  // skip trailing whitespace
  while(len && isspace(*data)) len--, data++;
  if(len != 0) { error = true; }

  return !error;
}
}
