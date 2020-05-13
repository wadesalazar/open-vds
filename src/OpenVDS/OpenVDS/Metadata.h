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

#ifndef OPENVDS_METADATA_H
#define OPENVDS_METADATA_H

#include <unordered_map>
#include <OpenVDS/Vector.h>

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <cassert>
#include <cstring>

class PyMetadata;

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
  MetadataType Type() const { return m_type; }

  /// <summary>
  /// Get the category of metadata that this key identifies
  /// </summary>
  /// <returns>
  /// The category of the metadata that this key identifies
  /// </returns>
  const char  *Category() const { return m_category; }

  /// <summary>
  /// Get the name of metadata that this key identifies
  /// </summary>
  /// <returns>
  /// The name of the metadata that this key identifies
  /// </returns>
  const char  *Name() const { return m_name; }

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

/// \brief Interface for read access to Metadata
class MetadataReadAccess
{
protected:
  virtual ~MetadataReadAccess() {}

  virtual void        GetMetadataBLOB(const char* category, const char* name, const void **data, size_t *size) const = 0; ///< Returns the metadata BLOB with the given category and name
public:
  // These functions provide access to Metadata
  virtual bool        IsMetadataIntAvailable(const char* category, const char* name) const = 0;           ///< Returns true if a metadata int with the given category and name is available
  virtual bool        IsMetadataIntVector2Available(const char* category, const char* name) const = 0;    ///< Returns true if a metadata IntVector2 with the given category and name is available
  virtual bool        IsMetadataIntVector3Available(const char* category, const char* name) const = 0;    ///< Returns true if a metadata IntVector3 with the given category and name is available
  virtual bool        IsMetadataIntVector4Available(const char* category, const char* name) const = 0;    ///< Returns true if a metadata IntVector4 with the given category and name is available
  virtual bool        IsMetadataFloatAvailable(const char* category, const char* name) const = 0;         ///< Returns true if a metadata float with the given category and name is available
  virtual bool        IsMetadataFloatVector2Available(const char* category, const char* name) const = 0;  ///< Returns true if a metadata FloatVector2 with the given category and name is available
  virtual bool        IsMetadataFloatVector3Available(const char* category, const char* name) const = 0;  ///< Returns true if a metadata FloatVector3 with the given category and name is available
  virtual bool        IsMetadataFloatVector4Available(const char* category, const char* name) const = 0;  ///< Returns true if a metadata FloatVector4 with the given category and name is available
  virtual bool        IsMetadataDoubleAvailable(const char* category, const char* name) const = 0;        ///< Returns true if a metadata double with the given category and name is available
  virtual bool        IsMetadataDoubleVector2Available(const char* category, const char* name) const = 0; ///< Returns true if a metadata DoubleVector2 with the given category and name is available
  virtual bool        IsMetadataDoubleVector3Available(const char* category, const char* name) const = 0; ///< Returns true if a metadata DoubleVector3 with the given category and name is available
  virtual bool        IsMetadataDoubleVector4Available(const char* category, const char* name) const = 0; ///< Returns true if a metadata DoubleVector4 with the given category and name is available
  virtual bool        IsMetadataStringAvailable(const char* category, const char* name) const = 0;        ///< Returns true if a metadata string with the given category and name is available
  virtual bool        IsMetadataBLOBAvailable(const char* category, const char* name) const = 0;          ///< Returns true if a metadata BLOB with the given category and name is available

  virtual int         GetMetadataInt(const char* category, const char* name) const = 0;        ///< Returns the metadata int with the given category and name
  virtual IntVector2  GetMetadataIntVector2(const char* category, const char* name) const = 0; ///< Returns the metadata IntVector2 with the given category and name
  virtual IntVector3  GetMetadataIntVector3(const char* category, const char* name) const = 0; ///< Returns the metadata IntVector3 with the given category and name
  virtual IntVector4  GetMetadataIntVector4(const char* category, const char* name) const = 0; ///< Returns the metadata IntVector4 with the given category and name

  virtual float        GetMetadataFloat(const char* category, const char* name) const = 0;        ///< Returns the metadata float with the given category and name
  virtual FloatVector2 GetMetadataFloatVector2(const char* category, const char* name) const = 0; ///< Returns the metadata FloatVector2 with the given category and name
  virtual FloatVector3 GetMetadataFloatVector3(const char* category, const char* name) const = 0; ///< Returns the metadata FloatVector3 with the given category and name
  virtual FloatVector4 GetMetadataFloatVector4(const char* category, const char* name) const = 0; ///< Returns the metadata FloatVector4 with the given category and name

  virtual double        GetMetadataDouble(const char* category, const char* name) const = 0;        ///< Returns the metadata double with the given category and name
  virtual DoubleVector2 GetMetadataDoubleVector2(const char* category, const char* name) const = 0; ///< Returns the metadata DoubleVector2 with the given category and name
  virtual DoubleVector3 GetMetadataDoubleVector3(const char* category, const char* name) const = 0; ///< Returns the metadata DoubleVector3 with the given category and name
  virtual DoubleVector4 GetMetadataDoubleVector4(const char* category, const char* name) const = 0; ///< Returns the metadata DoubleVector4 with the given category and name

  virtual const char* GetMetadataString(const char* category, const char* name) const = 0; ///< Returns the metadata string with the given category and name
  template <typename T>
  inline void         GetMetadataBLOB(const char* category, const char* name, std::vector<T> &value) const
                      {
                        const void* data; size_t size;
                        GetMetadataBLOB(category, name, &data, &size);
                        value.assign(reinterpret_cast<const T *>(data), reinterpret_cast<const T *>(data) + size/sizeof(T));
                      }
  virtual MetadataKeyRange
                      GetMetadataKeys() const = 0; ///< Returns a range of metadata keys that can be iterated over using range-based 'for'

  friend PyMetadata;
};

/// \brief Interface for write access to Metadata
class MetadataWriteAccess
{
protected:
  virtual ~MetadataWriteAccess() {}

  virtual void        SetMetadataString(const char* category, const char* name, const char* value) = 0;   ///< Sets a metadata string with the given category and name to the given value
  virtual void        SetMetadataBLOB(const char* category, const char* name, const void *data, size_t size) = 0; ///< Sets a metadata BLOB with the given category and name to the given value
public:
  virtual void        SetMetadataInt(const char* category, const char* name, int value) = 0;               ///< Sets a metadata int with the given category and name to the given value
  virtual void        SetMetadataIntVector2(const char* category, const char* name, IntVector2 value) = 0; ///< Sets a metadata IntVector2 with the given category and name to the given value
  virtual void        SetMetadataIntVector3(const char* category, const char* name, IntVector3 value) = 0; ///< Sets a metadata IntVector3 with the given category and name to the given value
  virtual void        SetMetadataIntVector4(const char* category, const char* name, IntVector4 value) = 0; ///< Sets a metadata IntVector4 with the given category and name to the given value

  virtual void        SetMetadataFloat(const char* category, const char* name, float value) = 0;               ///< Sets a metadata float with the given category and name to the given value
  virtual void        SetMetadataFloatVector2(const char* category, const char* name, FloatVector2 value) = 0; ///< Sets a metadata FloatVector2 with the given category and name to the given value
  virtual void        SetMetadataFloatVector3(const char* category, const char* name, FloatVector3 value) = 0; ///< Sets a metadata FloatVector3 with the given category and name to the given value
  virtual void        SetMetadataFloatVector4(const char* category, const char* name, FloatVector4 value) = 0; ///< Sets a metadata FloatVector4 with the given category and name to the given value

  virtual void        SetMetadataDouble(const char* category, const char* name, double value) = 0;               ///< Sets a metadata double with the given category and name to the given value
  virtual void        SetMetadataDoubleVector2(const char* category, const char* name, DoubleVector2 value) = 0; ///< Sets a metadata DoubleVector2 with the given category and name to the given value
  virtual void        SetMetadataDoubleVector3(const char* category, const char* name, DoubleVector3 value) = 0; ///< Sets a metadata DoubleVector3 with the given category and name to the given value
  virtual void        SetMetadataDoubleVector4(const char* category, const char* name, DoubleVector4 value) = 0; ///< Sets a metadata DoubleVector4 with the given category and name to the given value

  inline  void        SetMetadataString(const char* category, const char* name, std::string const &value) ///< Sets a metadata string with the given category and name to the given value
                      {
                        SetMetadataString(category, name, value.c_str());
                      }
  template <typename T>
  inline void         SetMetadataBLOB(const char* category, const char* name, std::vector<T> const &value) ///< Sets a metadata BLOB with the given category and name to the given value
                      {
                        SetMetadataBLOB(category, name, value.empty() ? NULL : &value.front(), value.size() * sizeof(T));
                      }

  /// Copy the metadata from another MetadataContainer
  /// \param category the metadata category to copy
  /// \param metadataReadAccess the MetadataReadAccess interface to copy from
  virtual void        CopyMetadata(const char* category, MetadataReadAccess const *metadataReadAccess) = 0;

  virtual void        ClearMetadata(const char* category, const char* name) = 0; ///< Clear the metadata with the given category and name
  virtual void        ClearMetadata(const char* category) = 0; ///< Clear the metadata with the given category
};

} // end namespace OpenVDS

namespace std
{
template<>
struct hash<OpenVDS::MetadataKey>
{
  std::size_t operator()(const OpenVDS::MetadataKey &k) const
  {
    size_t const h1= std::hash<std::string>()(k.Category());
    size_t const h2= std::hash<std::string>()(k.Name());
    return h1 ^ (h2 << 1);
  }
};
} // end namespace std

namespace OpenVDS
{
class MetadataContainer : public MetadataReadAccess, public MetadataWriteAccess
{
public:
  bool        IsMetadataIntAvailable(const char* category, const char* name) const override           { MetadataKey key = { MetadataType::Int,           category, name }; return m_intData.find(key) != m_intData.end(); }
  bool        IsMetadataIntVector2Available(const char* category, const char* name) const override    { MetadataKey key = { MetadataType::IntVector2,    category, name }; return m_intVector2Data.find(key) != m_intVector2Data.end(); }
  bool        IsMetadataIntVector3Available(const char* category, const char* name) const override    { MetadataKey key = { MetadataType::IntVector3,    category, name }; return m_intVector3Data.find(key) != m_intVector3Data.end(); }
  bool        IsMetadataIntVector4Available(const char* category, const char* name) const override    { MetadataKey key = { MetadataType::IntVector4,    category, name }; return m_intVector4Data.find(key) != m_intVector4Data.end(); }
  bool        IsMetadataFloatAvailable(const char* category, const char* name) const override         { MetadataKey key = { MetadataType::Float,         category, name }; return m_floatData.find(key) != m_floatData.end(); }
  bool        IsMetadataFloatVector2Available(const char* category, const char* name) const override  { MetadataKey key = { MetadataType::FloatVector2,  category, name }; return m_floatVector2Data.find(key) != m_floatVector2Data.end(); }
  bool        IsMetadataFloatVector3Available(const char* category, const char* name) const override  { MetadataKey key = { MetadataType::FloatVector3,  category, name }; return m_floatVector3Data.find(key) != m_floatVector3Data.end(); }
  bool        IsMetadataFloatVector4Available(const char* category, const char* name) const override  { MetadataKey key = { MetadataType::FloatVector4,  category, name }; return m_floatVector4Data.find(key) != m_floatVector4Data.end(); }
  bool        IsMetadataDoubleAvailable(const char* category, const char* name) const override        { MetadataKey key = { MetadataType::Double,        category, name }; return m_doubleData.find(key) != m_doubleData.end(); }
  bool        IsMetadataDoubleVector2Available(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector2, category, name }; return m_doubleVector2Data.find(key) != m_doubleVector2Data.end(); }
  bool        IsMetadataDoubleVector3Available(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector3, category, name }; return m_doubleVector3Data.find(key) != m_doubleVector3Data.end(); }
  bool        IsMetadataDoubleVector4Available(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector4, category, name }; return m_doubleVector4Data.find(key) != m_doubleVector4Data.end(); }
  bool        IsMetadataStringAvailable(const char* category, const char* name) const override        { MetadataKey key = { MetadataType::String,        category, name }; return m_stringData.find(key) != m_stringData.end(); }
  bool        IsMetadataBLOBAvailable(const char* category, const char* name) const override          { MetadataKey key = { MetadataType::BLOB,          category, name }; return m_blobData.find(key) != m_blobData.end(); }

  int         GetMetadataInt(const char* category, const char* name) const override             { MetadataKey key = { MetadataType::Int,           category, name }; auto it = m_intData.find(key);           return it != m_intData.end() ? it->second : int(); }
  IntVector2  GetMetadataIntVector2(const char* category, const char* name) const override      { MetadataKey key = { MetadataType::IntVector2,    category, name }; auto it = m_intVector2Data.find(key);    return it != m_intVector2Data.end() ? it->second : IntVector2(); }
  IntVector3  GetMetadataIntVector3(const char* category, const char* name) const override      { MetadataKey key = { MetadataType::IntVector3,    category, name }; auto it = m_intVector3Data.find(key);    return it != m_intVector3Data.end() ? it->second : IntVector3(); }
  IntVector4  GetMetadataIntVector4(const char* category, const char* name) const override      { MetadataKey key = { MetadataType::IntVector4,    category, name }; auto it = m_intVector4Data.find(key);    return it != m_intVector4Data.end() ? it->second : IntVector4(); }
  float        GetMetadataFloat(const char* category, const char* name) const override          { MetadataKey key = { MetadataType::Float,         category, name }; auto it = m_floatData.find(key);         return it != m_floatData.end() ? it->second : float(); }
  FloatVector2 GetMetadataFloatVector2(const char* category, const char* name) const override   { MetadataKey key = { MetadataType::FloatVector2,  category, name }; auto it = m_floatVector2Data.find(key);  return it != m_floatVector2Data.end() ? it->second : FloatVector2(); }
  FloatVector3 GetMetadataFloatVector3(const char* category, const char* name) const override   { MetadataKey key = { MetadataType::FloatVector3,  category, name }; auto it = m_floatVector3Data.find(key);  return it != m_floatVector3Data.end() ? it->second : FloatVector3(); }
  FloatVector4 GetMetadataFloatVector4(const char* category, const char* name) const override   { MetadataKey key = { MetadataType::FloatVector4,  category, name }; auto it = m_floatVector4Data.find(key);  return it != m_floatVector4Data.end() ? it->second : FloatVector4(); }
  double        GetMetadataDouble(const char* category, const char* name) const override        { MetadataKey key = { MetadataType::Double,        category, name }; auto it = m_doubleData.find(key);        return it != m_doubleData.end() ? it->second : double(); }
  DoubleVector2 GetMetadataDoubleVector2(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector2, category, name }; auto it = m_doubleVector2Data.find(key); return it != m_doubleVector2Data.end() ? it->second : DoubleVector2(); }
  DoubleVector3 GetMetadataDoubleVector3(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector3, category, name }; auto it = m_doubleVector3Data.find(key); return it != m_doubleVector3Data.end() ? it->second : DoubleVector3(); }
  DoubleVector4 GetMetadataDoubleVector4(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector4, category, name }; auto it = m_doubleVector4Data.find(key); return it != m_doubleVector4Data.end() ? it->second : DoubleVector4(); }
  const char* GetMetadataString(const char* category, const char* name) const override          { MetadataKey key = { MetadataType::String,        category, name }; auto it = m_stringData.find(key);        return it != m_stringData.end() ? it->second.c_str() : ""; }

  void        SetMetadataInt(const char* category, const char* name, int value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::Int, category, name); m_intData[key] = value; }
  void        SetMetadataIntVector2(const char* category, const char* name, IntVector2 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::IntVector2, category, name); m_intVector2Data[key] = value; }
  void        SetMetadataIntVector3(const char* category, const char* name, IntVector3 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::IntVector3, category, name); m_intVector3Data[key] = value; }
  void        SetMetadataIntVector4(const char* category, const char* name, IntVector4 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::IntVector4, category, name); m_intVector4Data[key] = value; }

  void        SetMetadataFloat(const char* category, const char* name, float value) override      { MetadataKey key = GetOrCreateMetadataKey(MetadataType::Float, category, name); m_floatData[key] = value; }
  void        SetMetadataFloatVector2(const char* category, const char* name, FloatVector2 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::FloatVector2, category, name); m_floatVector2Data[key] = value; }
  void        SetMetadataFloatVector3(const char* category, const char* name, FloatVector3 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::FloatVector3, category, name); m_floatVector3Data[key] = value; }
  void        SetMetadataFloatVector4(const char* category, const char* name, FloatVector4 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::FloatVector4, category, name); m_floatVector4Data[key] = value; }

  void        SetMetadataDouble(const char* category, const char* name, double value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::Double, category, name); m_doubleData[key] = value; }
  void        SetMetadataDoubleVector2(const char* category, const char* name, DoubleVector2 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::DoubleVector2, category, name); m_doubleVector2Data[key] = value; } ///< Sets a metadata DoubleVector2 with the given category and name to the given value
  void        SetMetadataDoubleVector3(const char* category, const char* name, DoubleVector3 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::DoubleVector3, category, name); m_doubleVector3Data[key] = value; } ///< Sets a metadata DoubleVector3 with the given category and name to the given value
  void        SetMetadataDoubleVector4(const char* category, const char* name, DoubleVector4 value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::DoubleVector4, category, name); m_doubleVector4Data[key] = value; } ///< Sets a metadata DoubleVector4 with the given category and name to the given value

  using MetadataWriteAccess::SetMetadataBLOB;
  using MetadataWriteAccess::SetMetadataString;
  using MetadataReadAccess::GetMetadataBLOB;
  using MetadataReadAccess::GetMetadataString;

  void        CopyMetadata(const char* category, MetadataReadAccess const *metadataReadAccess) override
  {
    for (auto &key : metadataReadAccess->GetMetadataKeys())
    {
      if (strcmp(key.Category(), category) == 0)
      {
        switch(key.Type())
        {
        case MetadataType::Int:
          SetMetadataInt(key.Category(), key.Name(), metadataReadAccess->GetMetadataInt(key.Category(), key.Name()));
          break;
        case MetadataType::IntVector2:
          SetMetadataIntVector2(key.Category(), key.Name(), metadataReadAccess->GetMetadataIntVector2(key.Category(), key.Name()));
          break;
        case MetadataType::IntVector3:
          SetMetadataIntVector3(key.Category(), key.Name(), metadataReadAccess->GetMetadataIntVector3(key.Category(), key.Name()));
          break;
        case MetadataType::IntVector4:
          SetMetadataIntVector4(key.Category(), key.Name(), metadataReadAccess->GetMetadataIntVector4(key.Category(), key.Name()));
          break;
        case MetadataType::Float:
          SetMetadataFloat(key.Category(), key.Name(), metadataReadAccess->GetMetadataFloat(key.Category(), key.Name()));
          break;
        case MetadataType::FloatVector2:
          SetMetadataFloatVector2(key.Category(), key.Name(), metadataReadAccess->GetMetadataFloatVector2(key.Category(), key.Name()));
          break;
        case MetadataType::FloatVector3:
          SetMetadataFloatVector3(key.Category(), key.Name(), metadataReadAccess->GetMetadataFloatVector3(key.Category(), key.Name()));
          break;
        case MetadataType::FloatVector4:
          SetMetadataFloatVector4(key.Category(), key.Name(), metadataReadAccess->GetMetadataFloatVector4(key.Category(), key.Name()));
          break;
        case MetadataType::Double:
          SetMetadataDouble(key.Category(), key.Name(), metadataReadAccess->GetMetadataDouble(key.Category(), key.Name()));
          break;
        case MetadataType::DoubleVector2:
          SetMetadataDoubleVector2(key.Category(), key.Name(), metadataReadAccess->GetMetadataDoubleVector2(key.Category(), key.Name()));
          break;
        case MetadataType::DoubleVector3:
          SetMetadataDoubleVector3(key.Category(), key.Name(), metadataReadAccess->GetMetadataDoubleVector3(key.Category(), key.Name()));
          break;
        case MetadataType::DoubleVector4:
          SetMetadataDoubleVector4(key.Category(), key.Name(), metadataReadAccess->GetMetadataDoubleVector4(key.Category(), key.Name()));
          break;
        case MetadataType::String:
          SetMetadataString(key.Category(), key.Name(), metadataReadAccess->GetMetadataString(key.Category(), key.Name()));
          break;
        case MetadataType::BLOB:
          std::vector<uint8_t> data;
          metadataReadAccess->GetMetadataBLOB(key.Category(), key.Name(), data);
          SetMetadataBLOB(key.Category(), key.Name(), data);
          break;
        }
      }
    }
  }

  void        ClearMetadata(const char* category, const char* name) override
  {
    std::vector<MetadataKey> localKeys;
    localKeys.reserve(m_keys.size());
    for (auto &key : m_keys)
    {
      if (strcmp(category, key.Category()) == 0 && strcmp(name, key.Name()) == 0)
      {
        localKeys.push_back(key);
      }
    }
    for (auto &key : localKeys)
    {
        RemoveMetadataForKey(key);
    }
  }

  void        ClearMetadata(const char* category) override
  {
    std::vector<MetadataKey> localKeys;
    localKeys.reserve(m_keys.size());
    for (auto &key : m_keys)
    {
      if (strcmp(category, key.Category()) == 0)
      {
        localKeys.push_back(key);
      }
    }
    for (auto &key : localKeys)
    {
        RemoveMetadataForKey(key);
    }
  }

  void        GetMetadataBLOB(const char* category, const char* name, const void **data, size_t *size)  const override
  {
    MetadataKey key = { MetadataType::BLOB, category, name };
    auto it = m_blobData.find(key);
    *data = (it != m_blobData.end()) ? it->second.data() : nullptr;
    *size = (it != m_blobData.end()) ? it->second.size() : 0;
  }

  MetadataKeyRange
  GetMetadataKeys() const override
  {
    return MetadataKeyRange(m_keys.data(), m_keys.data() + m_keys.size());
  }

protected:
  void        SetMetadataString(const char* category, const char* name, const char* value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::String, category, name); m_stringData[key] = value; }
  void        SetMetadataBLOB(const char* category, const char* name, const void *data, size_t size) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::BLOB, category, name); const uint8_t *udata = static_cast<const uint8_t *>(data); auto &vec = m_blobData[key]; vec.clear(); vec.insert(vec.begin(), udata, udata + size); }

private:
  MetadataKey GetOrCreateMetadataKey(MetadataType type, const char *category, const char *name)
  {
    auto category_it = m_categories.emplace(category).first;
    auto name_it = m_names.emplace(name).first;
    MetadataKey key(type, category, name);
    if (std::find(m_keys.begin(), m_keys.end(), key) == m_keys.end())
      m_keys.push_back(key);
    return key;
  }

  void RemoveMetadataForKey(const MetadataKey &key)
  {
    switch(key.Type())
    {
    case MetadataType::Int:
      m_intData.erase(key);
      break;
    case MetadataType::IntVector2:
      m_intVector2Data.erase(key);
      break;
    case MetadataType::IntVector3:
      m_intVector3Data.erase(key);
      break;
    case MetadataType::IntVector4:
      m_intVector4Data.erase(key);
      break;
    case MetadataType::Float:
      m_floatData.erase(key);
      break;
    case MetadataType::FloatVector2:
      m_floatVector2Data.erase(key);
      break;
    case MetadataType::FloatVector3:
      m_floatVector3Data.erase(key);
      break;
    case MetadataType::FloatVector4:
      m_floatVector4Data.erase(key);
      break;
    case MetadataType::Double:
      m_doubleData.erase(key);
      break;
    case MetadataType::DoubleVector2:
      m_doubleVector2Data.erase(key);
      break;
    case MetadataType::DoubleVector3:
      m_doubleVector3Data.erase(key);
      break;
    case MetadataType::DoubleVector4:
      m_doubleVector4Data.erase(key);
      break;
    case MetadataType::String:
      m_stringData.erase(key);
      break;
    case MetadataType::BLOB:
      m_blobData.erase(key);
      break;
    }
    m_keys.erase(std::find(m_keys.begin(), m_keys.end(), key));
  }

  std::unordered_map<MetadataKey, int>        m_intData;
  std::unordered_map<MetadataKey, IntVector2> m_intVector2Data;
  std::unordered_map<MetadataKey, IntVector3> m_intVector3Data;
  std::unordered_map<MetadataKey, IntVector4> m_intVector4Data;

  std::unordered_map<MetadataKey, float>        m_floatData;
  std::unordered_map<MetadataKey, FloatVector2> m_floatVector2Data;
  std::unordered_map<MetadataKey, FloatVector3> m_floatVector3Data;
  std::unordered_map<MetadataKey, FloatVector4> m_floatVector4Data;

  std::unordered_map<MetadataKey, double>        m_doubleData;
  std::unordered_map<MetadataKey, DoubleVector2> m_doubleVector2Data;
  std::unordered_map<MetadataKey, DoubleVector3> m_doubleVector3Data;
  std::unordered_map<MetadataKey, DoubleVector4> m_doubleVector4Data;

  std::unordered_map<MetadataKey, std::string> m_stringData;

  std::unordered_map<MetadataKey, std::vector<uint8_t>> m_blobData;

  std::vector<MetadataKey> m_keys;
  std::unordered_set<std::string> m_categories;
  std::unordered_set<std::string> m_names;

};

} // end namespace OpenVDS

#endif // OPENVDS_METADATA_H
