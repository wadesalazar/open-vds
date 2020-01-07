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

#include "generated_docstrings.h"

#include <stdexcept>

#define OPENVDS_DOCSTRING(name) __doc_OpenVDS_ ## name

namespace OpenVDS {

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

namespace py = pybind11;
namespace native = OpenVDS;


// Adapter class to check fixed-size numpy arrays
template<typename T, int LEN, bool MUTABLE>
struct PyArrayAdapter;

template<typename T, int LEN>
struct PyArrayAdapter<T, LEN, true>
{
  static T*
  getArrayBufferChecked(py::array_t<T>& arr, int* arrayCount = nullptr)
  {
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
    if (n != LEN)
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
    if (n != LEN)
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

template<typename T>
struct Vector2Adapter
{
  typedef std::tuple<T, T> AdaptedType;

  static native::Vector<T, 2> fromTuple(AdaptedType const& val) { return val; }
  static AdaptedType asTuple(native::Vector<T, 2> const& val)   { return val; }
};

template<typename T>
struct Vector3Adapter
{
  typedef std::tuple<T, T, T> AdaptedType;

  static native::Vector<T, 3> fromTuple(AdaptedType const& val) { return val; }
  static AdaptedType asTuple(native::Vector<T, 3> const& val)   { return val; }
};

template<typename T>
struct Vector4Adapter
{
  typedef std::tuple<T, T, T, T> AdaptedType;

  static native::Vector<T, 4> fromTuple(AdaptedType const& val) { return val; }
  static AdaptedType asTuple(native::Vector<T, 4> const& val)   { return val; }
};

template<typename T>
struct Vector6Adapter
{
  typedef std::tuple<T, T, T, T, T, T> AdaptedType;

  static native::Vector<T, 6> fromTuple(AdaptedType const& val) { return val; }
  static AdaptedType asTuple(native::Vector<T, 6> const& val)   { return val; }
};

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
