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

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeData.h>

#include <cstdlib>

#include <array>

#include <gtest/gtest.h>

#include "../utils/GenerateVDS.h"

GTEST_TEST(OpenVDS_integration, DataAccessorClone)
{
  int dimensionSize = 60;
  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> VDS(generateSimpleInMemory3DVDS(dimensionSize,dimensionSize,dimensionSize, OpenVDS::VolumeDataChannelDescriptor::Format_R32), OpenVDS::Close);
  fill3DVDSWithNoise(VDS.get());

  OpenVDS::IVolumeDataAccessManager *accessManagerInterface = OpenVDS::GetAccessManagerInterface(VDS.get());

  auto pageAccessor = accessManagerInterface->CreateVolumeDataPageAccessor(OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);

  OpenVDS::IVolumeDataReadAccessor<OpenVDS::FloatVector3, float > *accessor1 = accessManagerInterface->Create3DInterpolatingVolumeDataAccessorR32(pageAccessor, 345.f, OpenVDS::InterpolationMethod::Linear);

  auto accessor2 = static_cast<OpenVDS::IVolumeDataReadAccessor<OpenVDS::FloatVector3, float > *>(accessManagerInterface->CloneVolumeDataAccessor(*accessor1));

  for (float z = 0; z < dimensionSize; z+=0.5f)
  {
    for (float y = 0; y < dimensionSize; y+=0.5f)
    {
      for (float x = 0; x < dimensionSize; x+=0.5f)
      {
        OpenVDS::FloatVector3 pos(x,y,z);
        float v1 = accessor1->GetValue(pos);
        float v2 = accessor2->GetValue(pos);
        ASSERT_EQ(v1, v2);
      }
    }
  }
  accessManagerInterface->DestroyVolumeDataAccessor(accessor1);
  accessManagerInterface->DestroyVolumeDataAccessor(accessor2);
}
