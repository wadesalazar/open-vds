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

#include "cxxopts.hpp"

#include <fmt/printf.h>

#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/ValueConversion.h>

#include <array>
#include <limits>

inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool in_axis_mapping_rage(char a)
{
  return a >= '0' && a <='2';
}
bool parse_axis_mapping(const std::string arg, int32_t (&axis)[3])
{
  if (!in_axis_mapping_rage(arg[0]))
    return false;
  axis[0] = arg[0] - '0';
  if (!in_axis_mapping_rage(arg[2]))
    return false;
  axis[1] = arg[2] - '0';
  if (!in_axis_mapping_rage(arg[4]))
    return false;
  axis[2] = arg[4] - '0';
  return true;
}

int main(int argc, char **argv)
{
  cxxopts::Options options("slicedump", "slicedump - A tool to dump a slice to file");
  options.positional_help("<output file>");

  std::string file_name;
  std::string bucket;
  std::string region;
  std::string object;
  std::string axis = "0,1,2";
  int axis_position = std::numeric_limits<int>::min();
  int32_t output_width = 2000;
  int32_t output_height = 2000;

  options.add_option("", "", "bucket",   "Bucket to downlaod from.", cxxopts::value(bucket), "<string>");
  options.add_option("", "", "region",   "Region of bucket to download from.", cxxopts::value(region), "<string>");
  options.add_option("", "", "object",   "ObjectId of the VDS", cxxopts::value(object), "<string>");
  options.add_option("", "", "axis",     "Axis mapping. Comma seperated list. First digite is the axis for the slice. "
                                         "Second is the x axis and third is the y axis", cxxopts::value(axis), "<axis id>");
  options.add_option("", "", "position", "Position on axis for slice", cxxopts::value(axis_position), "<position in axis>");
  options.add_option("", "", "o_width",  "Output image width (default 200)", cxxopts::value(output_width), "<output width>");
  options.add_option("", "", "o_height", "Output image height (default 200)", cxxopts::value(output_height), "<output height>");
  options.add_option("", "", "output", "", cxxopts::value(file_name), "");
  options.parse_positional("output");

  try
  {
    options.parse(argc, argv);
  }
  catch(cxxopts::OptionParseException e)
  {
    fmt::print(stderr, "{}\n", e.what());
    return EXIT_FAILURE;
  }

  std::string missing_argument;
  if (bucket.empty())
    missing_argument = "bucket";
  else if (region.empty())
    missing_argument = "region";
  else if (object.empty())
    missing_argument = "object";
  else if (axis_position == std::numeric_limits<int>::min())
    missing_argument = "axis";
  else if (file_name.empty())
    missing_argument = "output filename";
  if (missing_argument.size())
  {
    fmt::print(stderr, "Missing required argument: {}\n", missing_argument);
    fmt::print(stderr, "{}\n", options.help());
    return EXIT_FAILURE;
  }

  int32_t axis_mapper[3];
  if (!parse_axis_mapping(axis, axis_mapper))
  {
    fmt::print(stderr, "Invalid axis mapping format: {}\n", axis);
    fmt::print(stderr, "Expected to comma seperated list ie. 1,2,0");
    return EXIT_FAILURE;
  }
  fmt::print(stdout, "Using axis mapping [{}, {}, {}]\n", axis_mapper[0], axis_mapper[1], axis_mapper[2]);

  OpenVDS::AWSOpenOptions connectionOptions;
  connectionOptions.key = object;
  connectionOptions.bucket = bucket;
  connectionOptions.region = region;

  OpenVDS::Error error;
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(connectionOptions, error), &OpenVDS::Close);
  if (!handle)
  {
    fmt::print(stderr, "Failed to open VDS: {}\n", error.string.c_str());
    return error.code;
  }

  if (!ends_with(file_name, ".bmp"))
    file_name = file_name + ".bmp";

  std::unique_ptr<FILE, decltype(&fclose)> file(fopen(file_name.c_str(), "wb"), &fclose);
  if (!file)
  {
    fmt::print(stderr, "Failed to open file: {}\n", file_name);
    return -4;
  }

  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());
  OpenVDS::VolumeDataAccessManager *dataAccessManager = OpenVDS::GetDataAccessManager(handle.get());

  int sampleCount[3];
  sampleCount[0] = layout->GetDimensionNumSamples(axis_mapper[0]);
  sampleCount[1] = layout->GetDimensionNumSamples(axis_mapper[1]);
  sampleCount[2] = layout->GetDimensionNumSamples(axis_mapper[2]);

  fmt::print(stdout, "Found data set with sample count [{}, {}, {}]\n", sampleCount[0], sampleCount[1], sampleCount[2]);

  float x_sample_shift = float(sampleCount[1]) / output_width;
  float y_sample_shift = float(sampleCount[2]) / output_height;

  std::vector<std::array<float, OpenVDS::Dimensionality_Max>> samples;
  samples.resize(size_t(output_width) * size_t(output_height));

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

  int64_t request = dataAccessManager->RequestVolumeSamples(data.data(), layout, OpenVDS::Dimensions_012, 0, 0, reinterpret_cast<const float (*)[OpenVDS::Dimensionality_Max]>(samples.data()), samples.size(), OpenVDS::InterpolationMethod::Linear);
  bool finished = dataAccessManager->WaitForCompletion(request);
  if (!finished)
  {
    fmt::print(stderr, "Failed to download reuqest. Failing\n");
    return -2;
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
  return 0;
}
