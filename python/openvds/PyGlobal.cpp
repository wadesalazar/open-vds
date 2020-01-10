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

#include "PyGlobal.h"

using namespace native;

void 
PyGlobal::initModule(py::module& m)
{
  // These are opaque pointers, so they must not be destructed from pybind11 code
  py::class_<VDS, std::unique_ptr<VDS, py::nodelete>>(m, "VDS");
  py::class_<IOManager, std::unique_ptr<IOManager, py::nodelete>>(m, "IOManager");

//AUTOGEN-BEGIN
  // OpenOptions
  py::class_<OpenOptions> 
    OpenOptions_(m,"OpenOptions", OPENVDS_DOCSTRING(OpenOptions));

  OpenOptions_.def_readwrite("connectionType"              , &OpenOptions::connectionType   , OPENVDS_DOCSTRING(OpenOptions_connectionType));

  py::enum_<OpenOptions::ConnectionType> 
    OpenOptions_ConnectionType_(OpenOptions_,"ConnectionType", OPENVDS_DOCSTRING(OpenOptions_ConnectionType));

  OpenOptions_ConnectionType_.value("AWS"                         , OpenOptions::ConnectionType::AWS        , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_AWS));
  OpenOptions_ConnectionType_.value("Azure"                       , OpenOptions::ConnectionType::Azure      , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_Azure));
  OpenOptions_ConnectionType_.value("File"                        , OpenOptions::ConnectionType::File       , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_File));
  OpenOptions_ConnectionType_.value("InMemory"                    , OpenOptions::ConnectionType::InMemory   , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_InMemory));

  // AWSOpenOptions
  py::class_<AWSOpenOptions, OpenOptions> 
    AWSOpenOptions_(m,"AWSOpenOptions", OPENVDS_DOCSTRING(AWSOpenOptions));

  AWSOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions));
  AWSOpenOptions_.def(py::init<const std::string &, const std::string &, const std::string &>(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions_2));
  AWSOpenOptions_.def_readwrite("bucket"                      , &AWSOpenOptions::bucket        , OPENVDS_DOCSTRING(AWSOpenOptions_bucket));
  AWSOpenOptions_.def_readwrite("key"                         , &AWSOpenOptions::key           , OPENVDS_DOCSTRING(AWSOpenOptions_key));
  AWSOpenOptions_.def_readwrite("region"                      , &AWSOpenOptions::region        , OPENVDS_DOCSTRING(AWSOpenOptions_region));

  // InMemoryOpenOptions
  py::class_<InMemoryOpenOptions, OpenOptions> 
    InMemoryOpenOptions_(m,"InMemoryOpenOptions", OPENVDS_DOCSTRING(InMemoryOpenOptions));

  InMemoryOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(InMemoryOpenOptions_InMemoryOpenOptions));

  // Error
  py::class_<Error> 
    Error_(m,"Error", OPENVDS_DOCSTRING(Error));

  Error_.def_readwrite("code"                        , &Error::code                   , OPENVDS_DOCSTRING(Error_code));
  Error_.def_readwrite("string"                      , &Error::string                 , OPENVDS_DOCSTRING(Error_string));

  m.def("open"                        , static_cast<native::VDSHandle(*)(const native::OpenOptions &, native::Error &)>(&Open), OPENVDS_DOCSTRING(Open));
  m.def("open"                        , static_cast<native::VDSHandle(*)(native::IOManager *, native::Error &)>(&Open), OPENVDS_DOCSTRING(Open_2));
  m.def("create"                      , static_cast<native::VDSHandle(*)(const native::OpenOptions &, const native::VolumeDataLayoutDescriptor &, VectorWrapper<native::VolumeDataAxisDescriptor>, VectorWrapper<native::VolumeDataChannelDescriptor>, const native::MetadataReadAccess &, native::Error &)>(&Create), OPENVDS_DOCSTRING(Create));
  m.def("create"                      , static_cast<native::VDSHandle(*)(native::IOManager *, const native::VolumeDataLayoutDescriptor &, VectorWrapper<native::VolumeDataAxisDescriptor>, VectorWrapper<native::VolumeDataChannelDescriptor>, const native::MetadataReadAccess &, native::Error &)>(&Create), OPENVDS_DOCSTRING(Create_2));
  m.def("getLayout"                   , static_cast<native::VolumeDataLayout *(*)(native::VDSHandle)>(&GetLayout), OPENVDS_DOCSTRING(GetLayout));
  m.def("getAccessManager"            , static_cast<native::VolumeDataAccessManager *(*)(native::VDSHandle)>(&GetAccessManager), OPENVDS_DOCSTRING(GetAccessManager));
  m.def("close"                       , static_cast<void(*)(native::VDSHandle)>(&Close), OPENVDS_DOCSTRING(Close));
//AUTOGEN-END
  Error_.def(py::init<>());
  Error_.def("__repr__", [](native::Error const& self){ std::string tmp = std::to_string(self.code); return std::string("Error(code=") + tmp + ", string='" + self.string + "')"; });

  OpenOptions_.def("__repr__", [](OpenOptions const& self)
    {
      std::string conn = "Unknown";
      switch(self.connectionType)
      {
      case OpenOptions::ConnectionType::AWS     : conn = std::string("AWS"     ); break;
      case OpenOptions::ConnectionType::Azure   : conn = std::string("Azure"   ); break;
      case OpenOptions::ConnectionType::File    : conn = std::string("File"    ); break;
      case OpenOptions::ConnectionType::InMemory: conn = std::string("InMemory"); break;
      }
      return std::string("OpenOptions(connectionType='" + conn + "')");
    });
}

