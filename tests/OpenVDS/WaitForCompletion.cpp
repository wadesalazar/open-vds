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

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/ValueConversion.h>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "../utils/GenerateVDS.h"
#include "../utils/SlowIOManager.h"

#include <OpenVDS/IO/IOManager.h>
#include <OpenVDS/IO/IOManagerInMemory.h>

TEST(WaitForCompletion, waitTimeout)
{
  OpenVDS::InMemoryOpenOptions options;
  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::IOManager> inMemory(OpenVDS::IOManagerInMemory::CreateIOManager(options, OpenVDS::IOManager::AccessPattern::ReadWrite, error));
  SlowIOManager* slowIOManager = new SlowIOManager(50, inMemory.get());
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(generateSimpleInMemory3DVDS(60,60,60, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, slowIOManager), OpenVDS::Close);
  fill3DVDSWithBitNoise(handle.get());
  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());
  OpenVDS::VolumeDataAccessManager accessManager = OpenVDS::GetAccessManager(handle.get());

  int32_t minPos[OpenVDS::Dimensionality_Max];
  int32_t maxPos[OpenVDS::Dimensionality_Max];
  int32_t voxelCount;
  minPos[0] = 0; minPos[1] = 10; minPos[2] = 10;
  maxPos[0] = 50; maxPos[1] = 60; maxPos[2] = 50;
  voxelCount = (maxPos[0] - minPos[0]) * (maxPos[1] - minPos[1]) * (maxPos[2] - minPos[2]);

  auto requestFloat = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, minPos, maxPos);
  ASSERT_FALSE(requestFloat->WaitForCompletion(1));
  ASSERT_TRUE(requestFloat->WaitForCompletion(0));
}
