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
#include "PyMetadataKey.h"
#include "PyKnownMetadata.h"
#include "PyMetadataAccess.h"
#include "PyMetadataContainer.h"
#include "PyVolumeData.h"
#include "PyVolumeDataAccess.h"
#include "PyVolumeDataAccessManager.h"
#include "PyVolumeDataAxisDescriptor.h"
#include "PyVolumeDataChannelDescriptor.h"
#include "PyVolumeDataLayout.h"
#include "PyVolumeDataLayoutDescriptor.h"
#include "PyVolumeSampler.h"
#include "PyGlobalState.h"


PYBIND11_MODULE(core, m) {
  PyGlobal::initModule(m);
  PyMetadataKey::initModule(m);
  PyKnownMetadata::initModule(m);
  PyMetadataAccess::initModule(m);
  PyMetadataContainer::initModule(m);
  PyVolumeData::initModule(m);
  PyVolumeDataAccess::initModule(m);
  PyVolumeDataAccessManager::initModule(m);
  PyVolumeDataAxisDescriptor::initModule(m);
  PyVolumeDataChannelDescriptor::initModule(m);
  PyVolumeDataLayout::initModule(m);
  PyVolumeDataLayoutDescriptor::initModule(m);
  PyVolumeSampler::initModule(m);
  PyGlobalState::initModule(m);
}
