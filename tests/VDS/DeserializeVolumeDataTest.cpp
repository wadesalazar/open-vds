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
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <VDS/VolumeDataStore.h>
#include <VDS/DataBlock.h>
#include <IO/File.h>

#include <cstdlib>
#include <cmath>

#include <gtest/gtest.h>

namespace OpenVDS
{
  bool DeserializeVolumeData(const std::vector<uint8_t>& serializedData, VolumeDataChannelDescriptor::Format format, CompressionMethod compressionMethod, const FloatRange& valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, int32_t adaptiveLevel, DataBlock& dataBlock, std::vector<uint8_t>& destination, Error& error);
}

static void Stats(const OpenVDS::DataBlock &aDataBlock, const std::vector<uint8_t> &aData, const OpenVDS::DataBlock &bDataBlock, const std::vector<uint8_t> &bData, double &diff, double &maxError, double &deviation, double &samples)
{
  diff = 0.0;
  samples = 0.0;

  assert(aDataBlock.Size[0] == bDataBlock.Size[0] &&
         aDataBlock.Size[1] == bDataBlock.Size[1] &&
         aDataBlock.Size[2] == bDataBlock.Size[2]);

  assert(aDataBlock.Format == bDataBlock.Format);

  OpenVDS::VolumeDataChannelDescriptor::Format format = aDataBlock.Format;

  const void *a = bData.data();
  
  const void *b = aData.data();

  double localAverage = 0.0;
  maxError = 0.0;


  for (int i2 = 0; i2 < aDataBlock.Size[2]; i2++)
  {
    for (int i1 = 0; i1 < aDataBlock.Size[1]; i1++)
    {
      for (int i0 = 0; i0 < aDataBlock.Size[0]; i0++)
      {
        int32_t offset = i0 + i1 * aDataBlock.Pitch[1] + i2 * aDataBlock.Pitch[2];

        double errorDelta;

        if (format == OpenVDS::VolumeDataChannelDescriptor::Format_R32)
        {
          errorDelta = *(((float *)b) + offset) - *(((float *)a) + offset);
        }
        else if (format == OpenVDS::VolumeDataChannelDescriptor::Format_U16)
        {
          errorDelta = (float)*(((uint16_t *)b) + offset) - (float)*(((uint16_t*)a) + offset);
          errorDelta /= 65535.0f;
        }
        else if (format == OpenVDS::VolumeDataChannelDescriptor::Format_U8)
        {
          errorDelta = (float)*(((uint8_t *)b) + offset) - (float)*(((uint8_t *)a) + offset);
          errorDelta /= 255.0f;
        }
        else
        {
          fprintf(stderr, "Illegal datablock format!");
          abort();
        }

        if (fabs(errorDelta) > maxError) maxError = fabs(errorDelta);

        diff += fabs(errorDelta);
        localAverage += fabs(errorDelta);
        samples+=1.0;
      }
    }
  }

  double average = localAverage / samples;
  deviation = 0.0;

  for (int i2 = 0; i2 < aDataBlock.Size[2]; i2++)
  {
    for (int i1 = 0; i1 < aDataBlock.Size[1]; i1++)
    {
      for (int i0 = 0; i0 < aDataBlock.Size[0]; i0++)
      {
        int32_t offset = i0 + i1 * aDataBlock.Pitch[1] + i2 * aDataBlock.Pitch[2];

        double errorDelta;

        if (format == OpenVDS::VolumeDataChannelDescriptor::Format_R32)
        {
          errorDelta = *(((float *)b) + offset) - *(((float *)a) + offset);
        }
        else if (format == OpenVDS::VolumeDataChannelDescriptor::Format_U16)
        {
          errorDelta = (float)*(((uint16_t *)b) + offset) - (float)*(((uint16_t *)a) + offset);
          errorDelta /= 65535.0f;
        }
        else if (format == OpenVDS::VolumeDataChannelDescriptor::Format_U8)
        {
          errorDelta = (float)*(((uint8_t *)b) + offset) - (float)*(((uint8_t *)a) + offset);
          errorDelta /= 255.0f;
        }
        else
        {
          fprintf(stderr, "Illegal datablock format!");
          abort();
        }
        deviation += (average - fabs(errorDelta)) * (average - fabs(errorDelta));
      }
    }
  }
}

static std::vector<uint8_t> LoadTestFile(const std::string &file)
{
    OpenVDS::File chunkFile;
    OpenVDS::IOError error;
    chunkFile.Open(TEST_DATA_PATH + file, false, false, false, error);
    EXPECT_EQ(error.code, 0);

    int64_t fileSize = chunkFile.Size(error);
    EXPECT_EQ(error.code, 0);

    std::vector<uint8_t> serializedData;
    serializedData.resize(fileSize);
    chunkFile.Read(&serializedData[0], 0, (int32_t)fileSize, error);
    EXPECT_EQ(error.code, 0);
    return serializedData;
}

GTEST_TEST(VDS_integration, DeSerializeVolumeData)
{
  OpenVDS::Error error;

  std::vector<uint8_t> serializedNone = LoadTestFile("/chunk.CompressionMethod_None");
  std::vector<uint8_t> dataNone;
  OpenVDS::DataBlock dataBlockNone;
  OpenVDS::DeserializeVolumeData(serializedNone, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::CompressionMethod::None, OpenVDS::FloatRange(-0.07883811742067337f, 0.07883811742067337f), 1.0f, 0.0f, false, 0.0f, 0, dataBlockNone, dataNone, error);
  EXPECT_EQ(error.code, 0);
  
  std::vector<uint8_t> serializedWavelet = LoadTestFile("/chunk.CompressionMethod_Wavelet");
  std::vector<uint8_t> dataWavelet;
  OpenVDS::DataBlock dataBlockWavelet;
  OpenVDS::DeserializeVolumeData(serializedWavelet, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::CompressionMethod::Wavelet, OpenVDS::FloatRange(-0.07883811742067337f, 0.07883811742067337f), 1.0f, 0.0f, false, 0.0f, 0, dataBlockWavelet, dataWavelet, error);
  EXPECT_EQ(error.code, 0);

  EXPECT_EQ(dataNone.size(), dataWavelet.size());

  double diff;
  double maxError;
  double deviation;
  double samples = 0;
  Stats(dataBlockNone, dataNone, dataBlockWavelet, dataWavelet, diff, maxError, deviation, samples);

  double variance = deviation / samples;
  double std_dev = sqrt(variance);
  double avg_diff = diff / samples;
  double one_procent_range = (0.07883811742067337 + 0.07883811742067337) / 100;
  EXPECT_TRUE(avg_diff < one_procent_range * 2);

  std::vector<uint8_t> serializedWaveletLossless = LoadTestFile("/chunk.CompressionMethod_WaveletLossless");
  std::vector<uint8_t> dataWaveletLossless;
  OpenVDS::DataBlock dataBlockWaveletLossless;
  OpenVDS::DeserializeVolumeData(serializedWaveletLossless, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::CompressionMethod::WaveletLossless, OpenVDS::FloatRange(-0.07883811742067337f, 0.07883811742067337f), 1.0f, 0.0f, false, 0.0f, -1, dataBlockWaveletLossless, dataWaveletLossless, error);
  EXPECT_EQ(error.code, 0);

  EXPECT_EQ(dataNone.size(), dataWaveletLossless.size());
  int comp_lossless = memcmp(dataNone.data(), dataWaveletLossless.data(), dataNone.size());
  EXPECT_TRUE(comp_lossless == 0);

  std::vector<uint8_t> serializedRLE = LoadTestFile("/chunk.CompressionMethod_RLE");
  std::vector<uint8_t> dataRLE;
  OpenVDS::DataBlock dataBlockRLE;
  OpenVDS::DeserializeVolumeData(serializedRLE, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::CompressionMethod::RLE, OpenVDS::FloatRange(-0.07883811742067337f, 0.07883811742067337f), 1.0f, 0.0f, false, 0.0f, 0, dataBlockRLE, dataRLE, error);
  EXPECT_EQ(error.code, 0);

  EXPECT_EQ(dataNone.size(), dataRLE.size());
  int comp_rle = memcmp(dataNone.data(), dataRLE.data(), dataNone.size());
  EXPECT_TRUE(comp_rle == 0);
  
  std::vector<uint8_t> serializedZip = LoadTestFile("/chunk.CompressionMethod_Zip");
  std::vector<uint8_t> dataZip;
  OpenVDS::DataBlock dataBlockZip;
  OpenVDS::DeserializeVolumeData(serializedZip, OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::CompressionMethod::Zip, OpenVDS::FloatRange(-0.07883811742067337f, 0.07883811742067337f), 1.0f, 0.0f, false, 0.0f, 0, dataBlockZip, dataZip, error);
  EXPECT_EQ(error.code, 0);

  EXPECT_EQ(dataNone.size(), dataZip.size());
  int comp_zip = memcmp(dataNone.data(), dataZip.data(), dataNone.size());
  EXPECT_TRUE(comp_zip == 0);
}
