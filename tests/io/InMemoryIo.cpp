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
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/KnownMetadata.h>
#include <OpenVDS/GlobalMetadataCommon.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>

#include <VDS/SimplexNoiceKernel.h>

#include <cstdlib>
#include <fmt/format.h>
#include <gtest/gtest.h>

#include <array>

inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

TEST(IOTests, InMemory)
{
  OpenVDS::Error error;
  OpenVDS::InMemoryOpenOptions options;

  auto brickSize = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_64;
  int negativeMargin = 0;
  int positiveMargin = 0;
  int brickSize2DMultiplier = 4;
  auto lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;
  auto layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(brickSize, negativeMargin, positiveMargin, brickSize2DMultiplier, lodLevels, layoutOptions);

  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
  axisDescriptors.push_back(OpenVDS::VolumeDataAxisDescriptor(1126, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, 4.f));
  axisDescriptors.emplace_back(605, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", 1932.f, 2536.f);
  axisDescriptors.emplace_back(385, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", 9985.f, 10369.f);

  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
  channelDescriptors.emplace_back(OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", -0.10919982194900513, 0.1099749207496643);

  OpenVDS::MetadataContainer metadataContainer;
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Create(options, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error), &OpenVDS::Close);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());
  ASSERT_TRUE(layout);
  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetDataAccessManager(handle.get());
  ASSERT_TRUE(accessManager);
  OpenVDS::VolumeDataPageAccessor *pageAccessor = accessManager->CreateVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  ASSERT_TRUE(pageAccessor);

  int32_t chunkCount = int32_t(pageAccessor->GetChunkCount());

  //float value_min

  for (int i = 0; i < chunkCount; i++)
  {
    OpenVDS::VolumeDataPage *page =  pageAccessor->CreatePage(i);
    OpenVDS::VolumeIndexer3D outputIndexer(page, 0, 0, OpenVDS::Dimensions_012, layout);
    OpenVDS::FloatVector3 frequency(0.6f, 2.f, 4.f);

    int pitch[OpenVDS::Dimensionality_Max];
    void *buffer = page->GetWritableBuffer(pitch);
    OpenVDS::CalculateNoise3D(buffer, OpenVDS::VolumeDataChannelDescriptor::Format_R32, &outputIndexer, frequency, 0.001f, 0.f, false, 345);
    page->Release();
  }
  pageAccessor->Commit();
  pageAccessor->SetMaxPages(0);
  accessManager->FlushUploadQueue();
///////////////////////////////////////////////////////
  int32_t output_width = 2000;
  int32_t output_height = 2000;


  std::string file_name = "output.bmp";
  if (!ends_with(file_name, ".bmp"))
    file_name = file_name + ".bmp";

  int32_t axis_mapper[3] = { 2, 1, 0};

  std::unique_ptr<FILE, decltype(&fclose)> file(fopen(file_name.c_str(), "wb"), &fclose);
  if (!file)
  {
    fmt::print(stderr, "Failed to open file: {}\n", file_name);
    ASSERT_TRUE(false);
  }

  int sampleCount[3];
  sampleCount[0] = layout->GetDimensionNumSamples(axis_mapper[0]);
  sampleCount[1] = layout->GetDimensionNumSamples(axis_mapper[1]);
  sampleCount[2] = layout->GetDimensionNumSamples(axis_mapper[2]);

  fmt::print(stdout, "Found data set with sample count [{}, {}, {}]\n", sampleCount[0], sampleCount[1], sampleCount[2]);

  float x_sample_shift = float(sampleCount[1]) / output_width;
  float y_sample_shift = float(sampleCount[2]) / output_height;

  std::vector<std::array<float, OpenVDS::Dimensionality_Max>> samples;
  samples.resize(size_t(output_width) * size_t(output_height));

  int32_t axis_position = 300;
  axis_position = std::max(0, axis_position);
  axis_position = std::min(sampleCount[0], axis_position);

  for (int y = 0; y < output_height; y++)
  {
    float y_pos = y * y_sample_shift;
    for (int x = 0; x < output_width; x++)
    {
      float x_pos = x * x_sample_shift;
      auto &pos = samples[size_t(y) * size_t(output_width) + size_t(x)];
      pos[size_t(axis_mapper[0])] = axis_position;
      pos[size_t(axis_mapper[1])] = x_pos;
      pos[size_t(axis_mapper[2])] = y_pos;
    }
  }

  std::vector<float> data;
  data.resize(size_t(output_width) * size_t(output_height));

  int64_t request = accessManager->RequestVolumeSamples(data.data(), layout, OpenVDS::Dimensions_012, 0, 0, reinterpret_cast<const float (*)[OpenVDS::Dimensionality_Max]>(samples.data()), samples.size(), OpenVDS::InterpolationMethod::Linear);
  bool finished = accessManager->WaitForCompletion(request);
  if (!finished)
  {
    fmt::print(stderr, "Failed to download reuqest. Failing\n");
    ASSERT_TRUE(false);
  }

  float minValue = layout->GetChannelValueRangeMin(0);
  float maxValue = layout->GetChannelValueRangeMax(0);
  float intScale = layout->GetChannelIntegerScale(0);
  float intLayout = layout->GetChannelIntegerOffset(0);
  OpenVDS::QuantizingValueConverterWithNoValue<uint8_t, float, false> converter(minValue, maxValue, intScale, intLayout, 0.f, 0.f);

  std::vector<std::array<uint8_t, 3>> fileData;
  fileData.resize(size_t(output_width) * size_t(output_height));
  for (int y = 0; y < output_height; y++)
  {
    for (int x = 0; x < output_width; x++)
    {

      uint8_t value =  converter.ConvertValue(data[size_t(y * output_width + x)]);
      auto &color = fileData[size_t(y * output_width + x)];
      color[0] =  value;
      color[1] =  value;
      color[2] =  value;
    }
  }

  uint32_t filesize = 54 + (sizeof(*fileData.data()) * uint32_t(fileData.size()));
  uint8_t bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
  uint8_t bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
  uint8_t bmppad[3] = {0,0,0};

  bmpfileheader[ 2] = (uint8_t)(filesize    );
  bmpfileheader[ 3] = (uint8_t)(filesize>> 8);
  bmpfileheader[ 4] = (uint8_t)(filesize>>16);
  bmpfileheader[ 5] = (uint8_t)(filesize>>24);

  bmpinfoheader[ 4] = (uint8_t)(output_width    );
  bmpinfoheader[ 5] = (uint8_t)(output_width>> 8);
  bmpinfoheader[ 6] = (uint8_t)(output_width>>16);
  bmpinfoheader[ 7] = (uint8_t)(output_width>>24);
  bmpinfoheader[ 8] = (uint8_t)(output_height    );
  bmpinfoheader[ 9] = (uint8_t)(output_height>> 8);
  bmpinfoheader[10] = (uint8_t)(output_height>>16);
  bmpinfoheader[11] = (uint8_t)(output_height>>24);
  fwrite(bmpfileheader,1,14,file.get());
  fwrite(bmpinfoheader,1,40,file.get());

  for (int32_t i = 0; i < output_height; i++)
  {
    fwrite(reinterpret_cast<const uint8_t *>(fileData.data()) +(output_width*(output_height-i-1)*3),3,size_t(output_width),file.get());
    fwrite(bmppad,1,(-3 * output_width) & 3, file.get());
  }
  file.reset();
  fmt::print(stdout, "File written to: {}\n", file_name);

}
