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

#ifndef OPENVDS_METADATAKEY_H
#define OPENVDS_METADATAKEY_H

#include <string>
#include <algorithm>
#include <cstring>

namespace OpenVDS
{

enum class MetadataType
{
  Int,
  IntVector2,
  IntVector3,
  IntVector4,
  Float,
  FloatVector2,
  FloatVector3,
  FloatVector4,
  Double,
  DoubleVector2,
  DoubleVector3,
  DoubleVector4,
  String,
  BLOB
};

/// <summary>
/// A metadata key uniquely identifies a metadata item
/// </summary>
class MetadataKey
{
  MetadataType  m_type;
  const char   *m_category;
  const char   *m_name;
public:
  /// <summary>
  /// Default constructor
  /// </summary>
  MetadataKey() : m_type(), m_category(), m_name() {}

  /// <summary>
  /// Constructor
  /// </summary>
  /// <param name="type">
  /// The type of the metadata that this key identifies. The possible types are (Int, Float, Double, {Int,Float,Double}Vector{2,3,4}, String or BLOB).
  /// </param>
  /// <param name="category">
  /// The category of the metadata that this key identifies. A category is a collection of related metadata items, e.g. SurveyCoordinateSystem consists of Origin, InlineSpacing, CrosslineSpacing and Unit metadata.
  /// </param>
  /// <param name="name">
  /// The name of the metadata that this key identifies
  /// </param>
  MetadataKey(MetadataType type, const char *category, const char *name) : m_type(type), m_category(category), m_name(name) {}

  /// <summary>
  /// Get the type of metadata that this key identifies
  /// </summary>
  /// <returns>
  /// The type of the metadata that this key identifies
  /// </returns>
  MetadataType GetType() const { return m_type; }

  /// <summary>
  /// Get the category of metadata that this key identifies
  /// </summary>
  /// <returns>
  /// The category of the metadata that this key identifies
  /// </returns>
  const char  *GetCategory() const { return m_category; }

  /// <summary>
  /// Get the name of metadata that this key identifies
  /// </summary>
  /// <returns>
  /// The name of the metadata that this key identifies
  /// </returns>
  const char  *GetName() const { return m_name; }

  bool operator==(const MetadataKey& other) const { return m_type == other.m_type && strcmp(m_category, other.m_category) == 0 && strcmp(m_name, other.m_name) == 0; }
  bool operator!=(const MetadataKey& other) const { return m_type != other.m_type || strcmp(m_category, other.m_category) != 0 || strcmp(m_name, other.m_name) != 0; }
};

/// \brief A range of metadata keys that can be iterated over using range-based 'for'
class MetadataKeyRange
{
  const MetadataKey *m_begin;
  const MetadataKey *m_end;
public:
  using const_iterator = const MetadataKey *;
  MetadataKeyRange(const_iterator begin, const_iterator end) : m_begin(begin), m_end(end) {}
  const_iterator begin() const { return m_begin; } ///< Returns a const iterator pointing to the first element in the MetadataKey collection
  const_iterator end() const   { return m_end; } ///< Returns a const iterator pointing to the past-the-end element in the MetadataKey collection
};

} // end namespace OpenVDS

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace std
{
template<>
struct hash<OpenVDS::MetadataKey>
{
  std::size_t operator()(const OpenVDS::MetadataKey &k) const
  {
    size_t const h1= std::hash<std::string>()(k.GetCategory());
    size_t const h2= std::hash<std::string>()(k.GetName());
    return h1 ^ (h2 << 1);
  }
};
} // end namespace std
#endif

#endif // OPENVDS_METADATAKEY_H
