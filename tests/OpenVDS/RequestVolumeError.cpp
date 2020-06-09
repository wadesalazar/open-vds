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
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeData.h>

#include <IO/IOManagerInMemory.h>

#include <cstdlib>

#include <random>
#include <array>

#include <fmt/printf.h>

#include <gtest/gtest.h>

#include "../utils/GenerateVDS.h"
#include "../utils/FacadeIOManager.h"
#include "../utils/SlowIOManager.h"

template<typename T, size_t N> inline T (&PODArrayReference(std::array<T,N> &a))[N] { return *reinterpret_cast<T (*)[N]>(a.data()); }

static bool strContains(const std::string& searchIn, const std::string& searchFor)
{
  return searchIn.find(searchFor) != std::string::npos;
}

class SyncTransfer : public OpenVDS::TransferDownloadHandler
{
public:
  void HandleObjectSize(int64_t size) override { }
  void HandleObjectLastWriteTime(const std::string& lastWriteTimeISO8601) override {}
  void HandleMetadata(const std::string& key, const std::string& header) override { headers.emplace_back(key, header); }
  void HandleData(std::vector<uint8_t>&& data) override { this->data = std::move(data); }
  void Completed(const OpenVDS::Request& request, const OpenVDS::Error& error) override {}

  std::vector<uint8_t> data;
  std::vector<std::pair<std::string, std::string>> headers;
};

GTEST_TEST(OpenVDS_integration, SimpleRequestVolumeError)
{
  OpenVDS::Error error;

  std::unique_ptr<OpenVDS::IOManager> iomanager(OpenVDS::IOManagerInMemory::CreateIOManagerInMemory(OpenVDS::InMemoryOpenOptions(), error));
  std::unique_ptr<IOManagerFacade> facadeIOManager(new IOManagerFacade(iomanager.get()));
  SlowIOManager* slowIOManager = new SlowIOManager(5, facadeIOManager.get());

  int dim[3] = { 100, 100, 100 };
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(generateSimpleInMemory3DVDS(dim[0], dim[1], dim[2], OpenVDS::VolumeDataChannelDescriptor::Format_1Bit, OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, slowIOManager), OpenVDS::Close);
  ASSERT_TRUE(handle);
  fill3DVDSWithBitNoise(handle.get());

  for (int i = 0; i < 100; i++)
  {
    std::string objectName = fmt::format("Dimensions_012LOD0/{}", i);
    auto syncTransfer = std::make_shared<SyncTransfer>();
    auto req = iomanager->ReadObject(objectName, syncTransfer);
    req->WaitForFinish();
    if (req->IsSuccess(error))
    {
      auto& object = facadeIOManager->m_data[objectName];
      object.data = std::move(syncTransfer->data);
      object.metaHeader = std::move(syncTransfer->headers);
      for (auto& metaHeader : object.metaHeader)
      {
        if (strContains(metaHeader.first, "vdschunkmetadata"))
        {
          if (metaHeader.second[4] == 'a')
            metaHeader.second[4] = 'b';
          else
            metaHeader.second[4] = 'a';
        }
      }
    }
  }
  error = OpenVDS::Error();


  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());
  ASSERT_TRUE(layout);

  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetAccessManager(handle.get());
  ASSERT_TRUE(accessManager);

  int samples[3];
  for (int i = 0; i < 3; i++)
  {
    samples[i] = layout->GetDimensionNumSamples(i);
  }

  struct BB {
    std::array<int, OpenVDS::Dimensionality_Max> voxelMin;
    std::array<int, OpenVDS::Dimensionality_Max> voxelMax;
  };

  std::array<BB, 10> boundingBoxes;

  int steps[3];
  int size[3];
  steps[0] = samples[0] / int(boundingBoxes.size() * 2);
  size[0] = samples[0] / int(boundingBoxes.size());
  steps[1] = 0;
  size[1] = 10;
  steps[2] = 0;
  size[2] = 1;

  int currentPos[3] = { 0,0,0 };
  for (auto& bb : boundingBoxes)
  {
    bb.voxelMin = {currentPos[0], currentPos[1], currentPos[2], 0, 0, 0};
    bb.voxelMax = { currentPos[0] + size[0], currentPos[1] + size[1], currentPos[2] + size[2], 1, 1, 1 };
    currentPos[0] += steps[0];
    currentPos[1] += steps[1];
    currentPos[2] += steps[2];
  }

  std::array<std::vector<float>, boundingBoxes.size()> buffers;
  for (int i = 0; i < int(boundingBoxes.size()); i++)
  {
    auto size = accessManager->GetVolumeSubsetBufferSize(layout, PODArrayReference(boundingBoxes[i].voxelMin), PODArrayReference(boundingBoxes[i].voxelMax), OpenVDS::VolumeDataChannelDescriptor::Format_R32, 0, 0);
    buffers[i].resize(size);
  }

  std::array<int64_t, boundingBoxes.size()> requestIds;
  for (int i = 0; i < int(boundingBoxes.size()); i++)
  {
    requestIds[i] = accessManager->RequestVolumeSubset(buffers[i].data(), layout, OpenVDS::Dimensions_012, 0, 0, PODArrayReference(boundingBoxes[i].voxelMin), PODArrayReference(boundingBoxes[i].voxelMax), OpenVDS::VolumeDataChannelDescriptor::Format_R32);
  }
  for (int i = 0; i < int(boundingBoxes.size()); i++)
  {
    bool returned = accessManager->WaitForCompletion(requestIds[i]);
    (void)returned;
  }
  //we don't need to assert anything as we are just loking for that the system does not segfault or deadlock.
}

