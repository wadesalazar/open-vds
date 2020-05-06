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
  py::class_<VDS, std::unique_ptr<VDS, py::nodelete>>
    VDS_(m, "VDS");
  VDS_.def("__int__", [](VDS* self) { return (int64_t)self; });
  
  py::class_<IOManager, std::unique_ptr<IOManager, py::nodelete>>(m, "IOManager");

//AUTOGEN-BEGIN
  // OpenOptions
  py::class_<OpenOptions, std::unique_ptr<OpenOptions>> 
    OpenOptions_(m,"OpenOptions", OPENVDS_DOCSTRING(OpenOptions));

  OpenOptions_.def_readwrite("connectionType"              , &OpenOptions::connectionType   , OPENVDS_DOCSTRING(OpenOptions_connectionType));

  py::enum_<OpenOptions::ConnectionType> 
    OpenOptions_ConnectionType_(OpenOptions_,"ConnectionType", OPENVDS_DOCSTRING(OpenOptions_ConnectionType));

  OpenOptions_ConnectionType_.value("AWS"                         , OpenOptions::ConnectionType::AWS        , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_AWS));
  OpenOptions_ConnectionType_.value("Azure"                       , OpenOptions::ConnectionType::Azure      , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_Azure));
  OpenOptions_ConnectionType_.value("File"                        , OpenOptions::ConnectionType::File       , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_File));
  OpenOptions_ConnectionType_.value("InMemory"                    , OpenOptions::ConnectionType::InMemory   , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_InMemory));

  // AWSOpenOptions
  py::class_<AWSOpenOptions, OpenOptions, std::unique_ptr<AWSOpenOptions>> 
    AWSOpenOptions_(m,"AWSOpenOptions", OPENVDS_DOCSTRING(AWSOpenOptions));

  AWSOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions));
  AWSOpenOptions_.def(py::init<const std::string &, const std::string &, const std::string &>(), py::arg("bucket"), py::arg("key"), py::arg("region"), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions_2));
  AWSOpenOptions_.def_readwrite("bucket"                      , &AWSOpenOptions::bucket        , OPENVDS_DOCSTRING(AWSOpenOptions_bucket));
  AWSOpenOptions_.def_readwrite("key"                         , &AWSOpenOptions::key           , OPENVDS_DOCSTRING(AWSOpenOptions_key));
  AWSOpenOptions_.def_readwrite("region"                      , &AWSOpenOptions::region        , OPENVDS_DOCSTRING(AWSOpenOptions_region));

  // AzureOpenOptions
  py::class_<AzureOpenOptions, OpenOptions, std::unique_ptr<AzureOpenOptions>> 
    AzureOpenOptions_(m,"AzureOpenOptions", OPENVDS_DOCSTRING(AzureOpenOptions));

  AzureOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(AzureOpenOptions_AzureOpenOptions));
  AzureOpenOptions_.def(py::init<const std::string &, const std::string &, const std::string &>(), py::arg("connectionString"), py::arg("container"), py::arg("blob"), OPENVDS_DOCSTRING(AzureOpenOptions_AzureOpenOptions_2));
  AzureOpenOptions_.def(py::init<const std::string &, const std::string &, const std::string &, int &, int &>(), py::arg("connectionString"), py::arg("container"), py::arg("blob"), py::arg("parallelism_factor"), py::arg("max_execution_time"), OPENVDS_DOCSTRING(AzureOpenOptions_AzureOpenOptions_3));
  AzureOpenOptions_.def_readwrite("connectionString"            , &AzureOpenOptions::connectionString, OPENVDS_DOCSTRING(AzureOpenOptions_connectionString));
  AzureOpenOptions_.def_readwrite("container"                   , &AzureOpenOptions::container   , OPENVDS_DOCSTRING(AzureOpenOptions_container));
  AzureOpenOptions_.def_readwrite("parallelism_factor"          , &AzureOpenOptions::parallelism_factor, OPENVDS_DOCSTRING(AzureOpenOptions_parallelism_factor));
  AzureOpenOptions_.def_readwrite("max_execution_time"          , &AzureOpenOptions::max_execution_time, OPENVDS_DOCSTRING(AzureOpenOptions_max_execution_time));

  // InMemoryOpenOptions
  py::class_<InMemoryOpenOptions, OpenOptions, std::unique_ptr<InMemoryOpenOptions>> 
    InMemoryOpenOptions_(m,"InMemoryOpenOptions", OPENVDS_DOCSTRING(InMemoryOpenOptions));

  InMemoryOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(InMemoryOpenOptions_InMemoryOpenOptions));

  // Error
  py::class_<Error, std::unique_ptr<Error>> 
    Error_(m,"Error", OPENVDS_DOCSTRING(Error));

  Error_.def_readwrite("code"                        , &Error::code                   , OPENVDS_DOCSTRING(Error_code));
  Error_.def_readwrite("string"                      , &Error::string                 , OPENVDS_DOCSTRING(Error_string));

  m.def("open"                        , static_cast<native::VDSHandle(*)(const native::OpenOptions &, native::Error &)>(&Open), py::arg("options"), py::arg("error"), OPENVDS_DOCSTRING(Open));
  m.def("open"                        , static_cast<native::VDSHandle(*)(native::IOManager *, native::Error &)>(&Open), py::arg("ioManager"), py::arg("error"), OPENVDS_DOCSTRING(Open_2));
  m.def("create"                      , static_cast<native::VDSHandle(*)(const native::OpenOptions &, const native::VolumeDataLayoutDescriptor &, VectorWrapper<native::VolumeDataAxisDescriptor>, VectorWrapper<native::VolumeDataChannelDescriptor>, const native::MetadataReadAccess &, native::Error &)>(&Create), py::arg("options"), py::arg("layoutDescriptor"), py::arg("axisDescriptors"), py::arg("channelDescriptors"), py::arg("metadata"), py::arg("error"), OPENVDS_DOCSTRING(Create));
  m.def("create"                      , static_cast<native::VDSHandle(*)(native::IOManager *, const native::VolumeDataLayoutDescriptor &, VectorWrapper<native::VolumeDataAxisDescriptor>, VectorWrapper<native::VolumeDataChannelDescriptor>, const native::MetadataReadAccess &, native::Error &)>(&Create), py::arg("ioManager"), py::arg("layoutDescriptor"), py::arg("axisDescriptors"), py::arg("channelDescriptors"), py::arg("metadata"), py::arg("error"), OPENVDS_DOCSTRING(Create_2));
  m.def("getLayout"                   , static_cast<native::VolumeDataLayout *(*)(native::VDSHandle)>(&GetLayout), py::arg("handle"), OPENVDS_DOCSTRING(GetLayout));
  m.def("getAccessManager"            , static_cast<native::VolumeDataAccessManager *(*)(native::VDSHandle)>(&GetAccessManager), py::arg("handle"), OPENVDS_DOCSTRING(GetAccessManager));
  m.def("close"                       , static_cast<void(*)(native::VDSHandle)>(&Close), py::arg("handle"), OPENVDS_DOCSTRING(Close));
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
      case OpenOptions::ConnectionType::AzurePresigned   : conn = std::string("AzurePresigned"   ); break;
      case OpenOptions::ConnectionType::File    : conn = std::string("File"    ); break;
      case OpenOptions::ConnectionType::InMemory: conn = std::string("InMemory"); break;
      }
      return std::string("OpenOptions(connectionType='" + conn + "')");
    });

  // Overloads for "open" and "create" that raise an exception when an error occurs
  m.def("open"                        , [](const native::OpenOptions &opt)
    {
      native::Error err;
      auto handle = native::Open(opt, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("options"), OPENVDS_DOCSTRING(Open));
  m.def("open"                        , [](native::IOManager *mgr)
    {
      native::Error err;
      auto handle = native::Open(mgr, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("ioManager"), OPENVDS_DOCSTRING(Open_2));
  m.def("create"                      , [](const native::OpenOptions &opt, const native::VolumeDataLayoutDescriptor &layout, VectorWrapper<native::VolumeDataAxisDescriptor> axisdesc, VectorWrapper<native::VolumeDataChannelDescriptor> channeldesc, const native::MetadataReadAccess &metadata)
    {
      native::Error err;
      auto handle = native::Create(opt, layout , axisdesc, channeldesc, metadata, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("options"), py::arg("layoutDescriptor"), py::arg("axisDescriptors"), py::arg("channelDescriptors"), py::arg("metadata"), OPENVDS_DOCSTRING(Create));
  m.def("create"                      , [](native::IOManager *mgr, const native::VolumeDataLayoutDescriptor &layout, VectorWrapper<native::VolumeDataAxisDescriptor> axisdesc, VectorWrapper<native::VolumeDataChannelDescriptor> channeldesc, const native::MetadataReadAccess &metadata)
    {
      native::Error err;
      auto handle = native::Create(mgr, layout , axisdesc, channeldesc, metadata, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("ioManager"), py::arg("layoutDescriptor"), py::arg("axisDescriptors"), py::arg("channelDescriptors"), py::arg("metadata"), OPENVDS_DOCSTRING(Create_2));
}

