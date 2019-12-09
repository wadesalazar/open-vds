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

namespace py = pybind11;
namespace native = OpenVDS;


// Adapter class to check fixed-size numpy arrays
template<typename T, int LEN, bool MUTABLE>
struct PyArrayAdapter;

template<typename T, int LEN>
struct PyArrayAdapter<T, LEN, true>
{
  static T*
  getArrayBufferChecked(py::array_t<T>& arr)
  {
    if (arr.ndim() != 1 || arr.size() != LEN)
    {
      throw std::invalid_argument("Array has the wrong size/dimensions.");
    }
    else
    {
      return arr.mutable_unchecked<1>().mutable_data(0);
    }
  }
};

template<typename T, int LEN>
struct PyArrayAdapter<T, LEN, false>
{
  static const T*
  getArrayBufferChecked(py::array_t<T> const& arr)
  {
    if (arr.ndim() != 1 || arr.size() != LEN)
    {
      throw std::invalid_argument("Array has the wrong size/dimensions.");
    }
    else
    {
      return arr.unchecked<1>().data(0);
    }
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

class PyGlobal
{
public:
  static void initModule(py::module& m);
};

#endif
