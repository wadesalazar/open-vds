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

#ifndef OPTIONAL_H
#define OPTIONAL_H

#if __cplusplus >=201703L
#include <optional>
#endif

namespace OpenVDS
{

template<typename T>
struct optional
{
  using value_type = T;
  value_type m_Value;
  bool m_HasValue;

  optional() : m_Value(), m_HasValue(false)
  {
  }

  optional(const value_type& value) : m_Value(value), m_HasValue(true)
  {
  }

#if __cplusplus >=201703L

  optional(const std::optional& opt) : m_Value(opt.has_value() ? opt.value() : value_type()), m_HasValue(opt.has_value())
  {
  }

#endif

  value_type value() const
  {
    return m_Value;
  }

  value_type value_or(value_type const& value) const
  {
    return m_HasValue ? m_Value : value;
  }

  value_type& emplace(value_type&& value)
  {
    m_HasValue = true;
    m_Value = std::move(value);
    return m_Value;
  }

  bool has_value() const
  {
    return m_HasValue;
  }

  operator bool () const
  {
    return m_HasValue;
  }
};

} /* namespace OpenVDS */

#endif //OPTIONAL_H
