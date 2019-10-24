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

struct MetadataKey
{
  MetadataType type;
  std::string  category;
  std::string  name;
};

inline bool operator==(const MetadataKey& a, const MetadataKey& b) { return a.type == b.type && a.category == b.category && a.name == b.name; }

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
    size_t const h1= std::hash<std::string>()(k.category);
    size_t const h2= std::hash<std::string>()(k.name);
    return h1 ^ (h2 << 1);
  }
};
} // end namespace std

namespace OpenVDS
{
struct MetadataContainer : public MetadataReadAccess /*, public MetadataWriteAccess */
{
  std::unordered_map<MetadataKey, int>        intData;
  std::unordered_map<MetadataKey, IntVector2> intVector2Data;
  std::unordered_map<MetadataKey, IntVector3> intVector3Data;
  std::unordered_map<MetadataKey, IntVector4> intVector4Data;

  std::unordered_map<MetadataKey, float>        floatData;
  std::unordered_map<MetadataKey, FloatVector2> floatVector2Data;
  std::unordered_map<MetadataKey, FloatVector3> floatVector3Data;
  std::unordered_map<MetadataKey, FloatVector4> floatVector4Data;

  std::unordered_map<MetadataKey, double>        doubleData;
  std::unordered_map<MetadataKey, DoubleVector2> doubleVector2Data;
  std::unordered_map<MetadataKey, DoubleVector3> doubleVector3Data;
  std::unordered_map<MetadataKey, DoubleVector4> doubleVector4Data;
 
  std::unordered_map<MetadataKey, std::string> stringData;

  std::unordered_map<MetadataKey, std::vector<uint8_t>> blobData;

  std::vector<MetadataKey> keys;

  bool        IsMetadataIntAvailable(const char* category, const char* name) const override           { MetadataKey key = { MetadataType::Int,           category, name }; return intData.find(key) != intData.end(); }
  bool        IsMetadataIntVector2Available(const char* category, const char* name) const override    { MetadataKey key = { MetadataType::IntVector2,    category, name }; return intVector2Data.find(key) != intVector2Data.end(); }
  bool        IsMetadataIntVector3Available(const char* category, const char* name) const override    { MetadataKey key = { MetadataType::IntVector3,    category, name }; return intVector3Data.find(key) != intVector3Data.end(); }
  bool        IsMetadataIntVector4Available(const char* category, const char* name) const override    { MetadataKey key = { MetadataType::IntVector4,    category, name }; return intVector4Data.find(key) != intVector4Data.end(); }
  bool        IsMetadataFloatAvailable(const char* category, const char* name) const override         { MetadataKey key = { MetadataType::Float,         category, name }; return floatData.find(key) != floatData.end(); }
  bool        IsMetadataFloatVector2Available(const char* category, const char* name) const override  { MetadataKey key = { MetadataType::FloatVector2,  category, name }; return floatVector2Data.find(key) != floatVector2Data.end(); }
  bool        IsMetadataFloatVector3Available(const char* category, const char* name) const override  { MetadataKey key = { MetadataType::FloatVector3,  category, name }; return floatVector3Data.find(key) != floatVector3Data.end(); }
  bool        IsMetadataFloatVector4Available(const char* category, const char* name) const override  { MetadataKey key = { MetadataType::FloatVector4,  category, name }; return floatVector4Data.find(key) != floatVector4Data.end(); }
  bool        IsMetadataDoubleAvailable(const char* category, const char* name) const override        { MetadataKey key = { MetadataType::Double,        category, name }; return doubleData.find(key) != doubleData.end(); }
  bool        IsMetadataDoubleVector2Available(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector2, category, name }; return doubleVector2Data.find(key) != doubleVector2Data.end(); }
  bool        IsMetadataDoubleVector3Available(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector3, category, name }; return doubleVector3Data.find(key) != doubleVector3Data.end(); }
  bool        IsMetadataDoubleVector4Available(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector4, category, name }; return doubleVector4Data.find(key) != doubleVector4Data.end(); }
  bool        IsMetadataStringAvailable(const char* category, const char* name) const override        { MetadataKey key = { MetadataType::String,        category, name }; return stringData.find(key) != stringData.end(); }
  bool        IsMetadataBLOBAvailable(const char* category, const char* name) const override          { MetadataKey key = { MetadataType::BLOB,          category, name }; return blobData.find(key) != blobData.end(); }

  int         GetMetadataInt(const char* category, const char* name) const override             { MetadataKey key = { MetadataType::Int,           category, name }; auto it = intData.find(key);           return it != intData.end() ? it->second : int(); }
  IntVector2  GetMetadataIntVector2(const char* category, const char* name) const override      { MetadataKey key = { MetadataType::IntVector2,    category, name }; auto it = intVector2Data.find(key);    return it != intVector2Data.end() ? it->second : IntVector2(); }
  IntVector3  GetMetadataIntVector3(const char* category, const char* name) const override      { MetadataKey key = { MetadataType::IntVector3,    category, name }; auto it = intVector3Data.find(key);    return it != intVector3Data.end() ? it->second : IntVector3(); }
  IntVector4  GetMetadataIntVector4(const char* category, const char* name) const override      { MetadataKey key = { MetadataType::IntVector4,    category, name }; auto it = intVector4Data.find(key);    return it != intVector4Data.end() ? it->second : IntVector4(); }
  float        GetMetadataFloat(const char* category, const char* name) const override          { MetadataKey key = { MetadataType::Float,         category, name }; auto it = floatData.find(key);         return it != floatData.end() ? it->second : float(); }
  FloatVector2 GetMetadataFloatVector2(const char* category, const char* name) const override   { MetadataKey key = { MetadataType::FloatVector2,  category, name }; auto it = floatVector2Data.find(key);  return it != floatVector2Data.end() ? it->second : FloatVector2(); }
  FloatVector3 GetMetadataFloatVector3(const char* category, const char* name) const override   { MetadataKey key = { MetadataType::FloatVector3,  category, name }; auto it = floatVector3Data.find(key);  return it != floatVector3Data.end() ? it->second : FloatVector3(); }
  FloatVector4 GetMetadataFloatVector4(const char* category, const char* name) const override   { MetadataKey key = { MetadataType::FloatVector4,  category, name }; auto it = floatVector4Data.find(key);  return it != floatVector4Data.end() ? it->second : FloatVector4(); }
  double        GetMetadataDouble(const char* category, const char* name) const override        { MetadataKey key = { MetadataType::Double,        category, name }; auto it = doubleData.find(key);        return it != doubleData.end() ? it->second : double(); }
  DoubleVector2 GetMetadataDoubleVector2(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector2, category, name }; auto it = doubleVector2Data.find(key); return it != doubleVector2Data.end() ? it->second : DoubleVector2(); }
  DoubleVector3 GetMetadataDoubleVector3(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector3, category, name }; auto it = doubleVector3Data.find(key); return it != doubleVector3Data.end() ? it->second : DoubleVector3(); }
  DoubleVector4 GetMetadataDoubleVector4(const char* category, const char* name) const override { MetadataKey key = { MetadataType::DoubleVector4, category, name }; auto it = doubleVector4Data.find(key); return it != doubleVector4Data.end() ? it->second : DoubleVector4(); }
  const char* GetMetadataString(const char* category, const char* name) const override          { MetadataKey key = { MetadataType::String,        category, name }; auto it = stringData.find(key);        return it != stringData.end() ? it->second.c_str() : ""; }
  void        GetMetadataBLOB(const char* category, const char* name, const void **data, size_t *size)  const override
  {
    MetadataKey key = { MetadataType::BLOB, category, name };
    auto it = blobData.find(key);
    *data = (it != blobData.end()) ? it->second.data() : nullptr;
    *size = (it != blobData.end()) ? it->second.size() : 0;
  }
};

} // end namespace OpenVDS

#endif // OPENVDS_METADATA_H
