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
#include "../utils/FacadeIOManager.h"

#include <OpenVDS/IO/IOManager.h>
#include <OpenVDS/IO/IOManagerInMemory.h>
#include <OpenVDS/VDS/ThreadPool.h>

TEST(Multithreading, requests)
{
  int datasetSize = 128;
  int threadCount = 16;
  int requestsPerThread = 16;
  int requestDelayMs = 5;

  OpenVDS::InMemoryOpenOptions options;
  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::IOManager> inMemory(OpenVDS::IOManagerInMemory::CreateIOManager(options, OpenVDS::IOManager::AccessPattern::ReadWrite, error));

  {
    IOManagerFacadeLight *iomanager = new IOManagerFacadeLight(inMemory.get());
    std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(generateSimpleInMemory3DVDS(datasetSize, datasetSize, datasetSize, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, iomanager), OpenVDS::Close);

    fill3DVDSWithBitNoise(handle.get());
  }
  
  SlowIOManager* slowIOManager = new SlowIOManager(requestDelayMs, inMemory.get());
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(slowIOManager, error), OpenVDS::Close);
  OpenVDS::VolumeDataAccessManager accessManager = OpenVDS::GetAccessManager(handle.get());

  ThreadPool threadPool(threadCount);

  struct ThreadRequestData
  {
    std::shared_ptr<OpenVDS::VolumeDataRequest> request;
    int32_t minPos[OpenVDS::Dimensionality_Max];
    int32_t maxPos[OpenVDS::Dimensionality_Max];
    int32_t voxelCount;
    std::vector<float> bufferFloat;
  };
  std::vector<std::vector<ThreadRequestData>> m_dataRequests;
  m_dataRequests.resize(threadCount);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> width_dist(datasetSize/4, datasetSize/2);
  std::uniform_int_distribution<> offset_dist(0, datasetSize/2);

  std::vector<std::future<bool>> results;
  results.reserve(threadCount);
  for (int loop = 0; loop < 10; loop++)
  {
    results.clear();
    for (int thread = 0; thread < threadCount; thread++)
    {
      auto& threadRequestData = m_dataRequests[thread];
      threadRequestData.resize(requestsPerThread);
      for (int i = 0; i < requestsPerThread; i++)
      {
        auto& requestData = threadRequestData[i];

        requestData.minPos[0] = offset_dist(gen);
        requestData.minPos[1] = offset_dist(gen);
        requestData.minPos[2] = offset_dist(gen);
        requestData.maxPos[0] = requestData.minPos[0] + width_dist(gen);
        requestData.maxPos[1] = requestData.minPos[1] + width_dist(gen);
        requestData.maxPos[2] = requestData.minPos[2] + width_dist(gen);
        requestData.voxelCount = (requestData.maxPos[0] - requestData.minPos[0]) * (requestData.maxPos[1] - requestData.minPos[1]) * (requestData.maxPos[2] - requestData.minPos[2]);
      }
      auto threadedRequest = [&threadRequestData, accessManager] () mutable
      {
        std::vector<bool> ret;
        ret.reserve(threadRequestData.size());
        for (auto& requestData : threadRequestData)
        {
          requestData.bufferFloat.resize(requestData.voxelCount);
          requestData.request = accessManager.RequestVolumeSubset<float>(requestData.bufferFloat.data(), requestData.bufferFloat.size() * sizeof(float), OpenVDS::Dimensions_012, 0, 0, requestData.minPos, requestData.maxPos);
        }
        bool success = true;
        for (auto& requestData : threadRequestData)
        {
          if (!requestData.request->WaitForCompletion())
            success = false;
        }
        return success;
      };
      results.push_back(threadPool.Enqueue(threadedRequest));
    }

    for (auto& result : results)
    {
      ASSERT_TRUE(result.get());
    }
  }
}
