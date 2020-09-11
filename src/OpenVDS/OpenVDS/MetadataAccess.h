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

#ifndef OPENVDS_METADATAACCESS_H
#define OPENVDS_METADATAACCESS_H

#include <OpenVDS/MetadataKey.h>
#include <OpenVDS/Vector.h>

#include <string>
#include <vector>

class PyMetadataAccess;

namespace OpenVDS
{

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

  friend PyMetadataAccess;
};

/// \brief Interface for write access to Metadata
class MetadataWriteAccess
{
protected:
  virtual ~MetadataWriteAccess() {}

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

  virtual void        SetMetadataString(const char* category, const char* name, const char* value) = 0;   ///< Sets a metadata string with the given category and name to the given value
  inline  void        SetMetadataString(const char* category, const char* name, std::string const &value) ///< Sets a metadata string with the given category and name to the given value
                      {
                        SetMetadataString(category, name, value.c_str());
                      }

  virtual void        SetMetadataBLOB(const char* category, const char* name, const void *data, size_t size) = 0; ///< Sets a metadata BLOB with the given category and name to the given value
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

#endif // OPENVDS_METADATAACCESS_H
