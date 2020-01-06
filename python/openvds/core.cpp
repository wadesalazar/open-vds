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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "PyGlobal.h"
#include "PyGlobalMetadataCommon.h"
#include "PyKnownMetadata.h"
#include "PyMetadata.h"
#include "PyRange.h"
#include "PyVector.h"
#include "PyVolumeData.h"
#include "PyVolumeDataAccess.h"
#include "PyVolumeDataAxisDescriptor.h"
#include "PyVolumeDataChannelDescriptor.h"
#include "PyVolumeDataLayout.h"
#include "PyVolumeDataLayoutDescriptor.h"
#include "PyVolumeSampler.h"

#ifndef TEST_ARRAY_WRAP 
#define TEST_ARRAY_WRAP 1
#endif

#if TEST_ARRAY_WRAP

#include <array>

static int sumArray(const int (&data)[6])
{
  int sum = 0;
  for (int i = 0; i < 6; ++i) 
  {
    sum += data[i];
  }
  return sum;
}

static void populateArray(int(&data)[6])
{
  for (int i = 0; i < 6; ++i) 
  {
    data[i] = 0;
  }
}

struct TestArray
{
  int m_Data[6];

  TestArray()
  {
    for (int i = 0; i < 6; ++i) 
    {
      m_Data[i] = 0;
    }
  }

  void setValues(const int (&data)[6])
  {
    for (int i = 0; i < 6; ++i) 
    {
      m_Data[i] = data[i];
    }
  }

  void getValues(int (&data)[6])
  {
    for (int i = 0; i < 6; ++i) 
    {
      data[i] = m_Data[i];
    }
  }

};

#endif

PYBIND11_MODULE(core, m) {
#if TEST_ARRAY_WRAP
  m.def("sumArray", [](std::array<int, 6> const& data) 
    {
      return sumArray((const int(&)[6])data);
    });
  m.def("populateArray", [](std::array<int, 6>& data) 
    {
      return populateArray((int(&)[6])data);
    });
  py::class_<TestArray>(m, "TestArray")
    .def(py::init<>())
    .def("setValues", [](TestArray* self, py::array_t<int> const& data) 
      { 
        return self->setValues((const int(&)[6])*PyArrayAdapter<int, 6, false>::getArrayBufferChecked(data)); 
      },
      py::arg().noconvert()
    )
    .def("getValues", [](TestArray* self, py::array_t<int> data) 
      {
        return self->getValues((int(&)[6])*PyArrayAdapter<int, 6, true>::getArrayBufferChecked(data));
      }, 
      py::arg().noconvert()
    )
  ;
  m.def("testArrayArrays", [](py::array_t<int>& data, int arrayCount)
    {
      auto& checkedData = PyArrayAdapter<int, 3, false>::getArrayChecked(data, arrayCount);
      return true;
    }
  );
#endif

  PyGlobal::initModule(m);
  PyGlobalMetadataCommon::initModule(m);
  PyKnownMetadata::initModule(m);
  PyMetadata::initModule(m);
  PyRange::initModule(m);
  PyVector::initModule(m);
  PyVolumeData::initModule(m);
  PyVolumeDataAccess::initModule(m);
  PyVolumeDataAxisDescriptor::initModule(m);
  PyVolumeDataChannelDescriptor::initModule(m);
  PyVolumeDataLayout::initModule(m);
  PyVolumeDataLayoutDescriptor::initModule(m);
  PyVolumeSampler::initModule(m);
}
