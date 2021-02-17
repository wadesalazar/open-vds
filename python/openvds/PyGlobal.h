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
#ifndef PYGLOBAL_H_INCLUDED
#define PYGLOBAL_H_INCLUDED

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeData.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/KnownMetadata.h>
#include <OpenVDS/MetadataAccess.h>
#include <OpenVDS/MetadataContainer.h>
#include <OpenVDS/GlobalState.h>

#include "generated_docstrings.h"

#include <stdexcept>

#define OPENVDS_DOCSTRING(name) __doc_OpenVDS_ ## name

namespace OpenVDS {

class VolumeDataAccessorBase
{
};

#ifdef OPENVDS_DEVELOPMENT_BUILD

struct VDS;
class IOManager;

#else

struct VDS
{
private:
  VDS();
  ~VDS();
};

class IOManager
{
private:
  IOManager();
  ~IOManager();
};

#endif

}

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/attr.h>
#include <pybind11/operators.h>

namespace py = pybind11;
namespace native = OpenVDS;

namespace pybind11 { namespace detail {

template <typename T>
struct type_caster<native::optional<T>> : optional_caster<native::optional<T>> 
{
};

}}

// Helpers for methods that take buffer pointers
template<typename T, bool WRITABLE>
T*
PyGetBufferPtr(py::buffer& buffer)
{
  py::buffer_info info;
  {
    py::gil_scoped_acquire
      acquire;

    info = buffer.request(WRITABLE);
    if (info.ptr == nullptr) 
    {
      throw std::runtime_error("Unable to obtain buffer.");
    }
  }
  return (T*)info.ptr;
}

template<typename T, bool WRITABLE>
T
PyGetBufferSize(py::buffer& buffer)
{
  py::buffer_info info;
  {
    py::gil_scoped_acquire
      acquire;

    info = buffer.request(WRITABLE);
    if (info.ptr == nullptr) 
    {
      throw std::runtime_error("Unable to obtain buffer.");
    }
    return (T)(info.size * info.itemsize);
  }
}

// Adapter class to check fixed-size numpy arrays
template<typename T, int LEN, bool MUTABLE>
struct PyArrayAdapter;

template<typename T, int LEN>
struct PyArrayAdapter<T, LEN, true>
{
  static T*
  getArrayBufferChecked(py::array_t<T>& arr, int* arrayCount = nullptr)
  {
    py::gil_scoped_acquire
      acquire;

    py::ssize_t n = 0;
    int count = 0;
    if (arr.ndim() == 1)
    {
      count = 1;
      n = arr.shape(0);
    }
    else if (arr.ndim() == 2)
    {
      count = (int)arr.shape(0);
      n = arr.shape(1);
    }
    if (arrayCount)
    {
      *arrayCount = count;
    }
    if (n < LEN)
    {
      throw std::invalid_argument("Array has the wrong size/dimensions.");
    }
    else
    {
      return arr.mutable_unchecked().mutable_data(0);
    }
  }

  static T (&getArrayChecked(py::array_t<T> & arr, int* arrayCount = nullptr))[LEN]
  {
    T* tmp = getArrayBufferChecked(arr, arrayCount);
    return *reinterpret_cast<T (*)[LEN]>(tmp);
  }

};

template<typename T, int LEN>
struct PyArrayAdapter<T, LEN, false>
{
  static const T*
  getArrayBufferChecked(py::array_t<T> const& arr, int* arrayCount = nullptr)
  {
    py::gil_scoped_acquire
      acquire;

    py::ssize_t n = 0;
    int count = 0;
    if (arr.ndim() == 1)
    {
      count = 1;
      n = arr.shape(0);
    }
    else if (arr.ndim() == 2)
    {
      count = (int)arr.shape(0);
      n = arr.shape(1);
    }
    if (arrayCount)
    {
      *arrayCount = count;
    }
    if (n < LEN)
    {
      throw std::invalid_argument("Array has the wrong size/dimensions.");
    }
    else
    {
      return arr.unchecked().data(0);
    }
  }

  static const T (&getArrayChecked(py::array_t<T> const& arr, int* arrayCount = nullptr))[LEN]
  {
    const T* tmp = getArrayBufferChecked(arr, arrayCount);
    return *reinterpret_cast<const T (*)[LEN]>(tmp);
  }

  static const T (*getArrayPtrChecked(py::array_t<T> const& arr, int* arrayCount = nullptr))[LEN]
  {
    const T* tmp = getArrayBufferChecked(arr, arrayCount);
    return reinterpret_cast<const T (*)[LEN]>(tmp);
  }
};

struct BLOB
{
  uint8_t*  m_Data;
  size_t    m_Size;

  BLOB() : m_Data(nullptr), m_Size(0)
  {
  }

  BLOB(uint8_t* data, size_t size) : m_Data(data), m_Size(size)
  {
  }
};

template<typename T, size_t N>
struct VectorAdapter
{

};

template<typename T>
struct VectorAdapter<T, 2>
{
  typedef native::Vector<T, 2> VectorType;
  typedef std::tuple<T, T>     AdaptedType;

  static VectorType   fromTuple(AdaptedType const& val) { return val; }
  static AdaptedType  asTuple(VectorType const& val)    { return val; }
};

template<typename T>
struct VectorAdapter<T, 3>
{
  typedef native::Vector<T, 3> VectorType;
  typedef std::tuple<T, T, T>  AdaptedType;

  static VectorType   fromTuple(AdaptedType const& val) { return val; }
  static AdaptedType  asTuple(VectorType const& val)    { return val; }
};

template<typename T>
struct VectorAdapter<T, 4>
{
  typedef native::Vector<T, 4>    VectorType;
  typedef std::tuple<T, T, T, T>  AdaptedType;

  static VectorType   fromTuple(AdaptedType const& val) { return val; }
  static AdaptedType  asTuple(VectorType const& val)    { return val; }
};

template<typename T>
struct VectorAdapter<T, 6>
{
  typedef native::Vector<T, 6>          VectorType;
  typedef std::tuple<T, T, T, T, T, T>  AdaptedType;

  static VectorType   fromTuple(AdaptedType const& val) { return val; }
  static AdaptedType  asTuple(VectorType const& val)    { return val; }
};

template<typename T>
using Vector2Adapter = VectorAdapter<T, 2>;
template<typename T>
using Vector3Adapter = VectorAdapter<T, 3>;
template<typename T>
using Vector4Adapter = VectorAdapter<T, 4>;
template<typename T>
using Vector6Adapter = VectorAdapter<T, 6>;

typedef Vector2Adapter<int>     IntVector2Adapter;
typedef Vector3Adapter<int>     IntVector3Adapter;
typedef Vector4Adapter<int>     IntVector4Adapter;
typedef Vector6Adapter<int>     IntVector6Adapter;

typedef Vector2Adapter<float>   FloatVector2Adapter;
typedef Vector3Adapter<float>   FloatVector3Adapter;
typedef Vector4Adapter<float>   FloatVector4Adapter;
typedef Vector6Adapter<float>   FloatVector6Adapter;

typedef Vector2Adapter<double>  DoubleVector2Adapter;
typedef Vector3Adapter<double>  DoubleVector3Adapter;
typedef Vector4Adapter<double>  DoubleVector4Adapter;
typedef Vector6Adapter<double>  DoubleVector6Adapter;

class PyGlobal
{
public:
  static void initModule(py::module& m);
};

#endif
