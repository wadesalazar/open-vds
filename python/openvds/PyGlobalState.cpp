/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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

#include "PyGlobalState.h"

using namespace native;

void
PyGlobalState::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  // GlobalState
  py::class_<GlobalState, std::unique_ptr<GlobalState>> 
    GlobalState_(m,"GlobalState", OPENVDS_DOCSTRING(GlobalState));

  GlobalState_.def("getBytesDownloaded"          , static_cast<uint64_t(GlobalState::*)(OpenOptions::ConnectionType)>(&GlobalState::GetBytesDownloaded), py::arg("connectionType"), OPENVDS_DOCSTRING(GlobalState_GetBytesDownloaded));
  GlobalState_.def("getChunksDownloaded"         , static_cast<uint64_t(GlobalState::*)(OpenOptions::ConnectionType)>(&GlobalState::GetChunksDownloaded), py::arg("connectionType"), OPENVDS_DOCSTRING(GlobalState_GetChunksDownloaded));
  GlobalState_.def("getBytesDecompressed"        , static_cast<uint64_t(GlobalState::*)(OpenOptions::ConnectionType)>(&GlobalState::GetBytesDecompressed), py::arg("connectionType"), OPENVDS_DOCSTRING(GlobalState_GetBytesDecompressed));
  GlobalState_.def("getChunksDecompressed"       , static_cast<uint64_t(GlobalState::*)(OpenOptions::ConnectionType)>(&GlobalState::GetChunksDecompressed), py::arg("connectionType"), OPENVDS_DOCSTRING(GlobalState_GetChunksDecompressed));

//AUTOGEN-END
}
