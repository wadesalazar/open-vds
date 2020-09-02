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

#ifndef OPENVDS_METADATACONTAINER_H
#define OPENVDS_METADATACONTAINER_H

#include <OpenVDS/MetadataAccess.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstring>

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

  void        SetMetadataString(const char* category, const char* name, const char* value) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::String, category, name); m_stringData[key] = value; }
  void        SetMetadataBLOB(const char* category, const char* name, const void *data, size_t size) override { MetadataKey key = GetOrCreateMetadataKey(MetadataType::BLOB, category, name); const uint8_t *udata = static_cast<const uint8_t *>(data); auto &vec = m_blobData[key]; vec.clear(); vec.insert(vec.begin(), udata, udata + size); }

  void        CopyMetadata(const char* category, MetadataReadAccess const *metadataReadAccess) override
  {
    for (auto &key : metadataReadAccess->GetMetadataKeys())
    {
      if (strcmp(key.GetCategory(), category) == 0)
      {
        switch(key.GetType())
        {
        case MetadataType::Int:
          SetMetadataInt(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataInt(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::IntVector2:
          SetMetadataIntVector2(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataIntVector2(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::IntVector3:
          SetMetadataIntVector3(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataIntVector3(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::IntVector4:
          SetMetadataIntVector4(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataIntVector4(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::Float:
          SetMetadataFloat(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataFloat(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::FloatVector2:
          SetMetadataFloatVector2(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataFloatVector2(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::FloatVector3:
          SetMetadataFloatVector3(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataFloatVector3(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::FloatVector4:
          SetMetadataFloatVector4(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataFloatVector4(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::Double:
          SetMetadataDouble(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataDouble(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::DoubleVector2:
          SetMetadataDoubleVector2(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataDoubleVector2(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::DoubleVector3:
          SetMetadataDoubleVector3(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataDoubleVector3(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::DoubleVector4:
          SetMetadataDoubleVector4(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataDoubleVector4(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::String:
          SetMetadataString(key.GetCategory(), key.GetName(), metadataReadAccess->GetMetadataString(key.GetCategory(), key.GetName()));
          break;
        case MetadataType::BLOB:
          std::vector<uint8_t> data;
          metadataReadAccess->GetMetadataBLOB(key.GetCategory(), key.GetName(), data);
          SetMetadataBLOB(key.GetCategory(), key.GetName(), data);
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
      if (strcmp(category, key.GetCategory()) == 0 && strcmp(name, key.GetName()) == 0)
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
      if (strcmp(category, key.GetCategory()) == 0)
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

private:
  MetadataKey GetOrCreateMetadataKey(MetadataType type, const char *category, const char *name)
  {
    auto category_it = m_categories.emplace(category).first;
    auto name_it = m_names.emplace(name).first;
    MetadataKey key(type, category_it->c_str(), name_it->c_str());
    if (std::find(m_keys.begin(), m_keys.end(), key) == m_keys.end())
      m_keys.push_back(key);
    return key;
  }

  void RemoveMetadataForKey(const MetadataKey &key)
  {
    switch(key.GetType())
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

#endif // OPENVDS_METADATACONTAINER_H
