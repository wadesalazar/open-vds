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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <random>

#include "File.h"

#include "ThreadPool.h"

#ifdef _WIN32
#undef WIN32_LEAN_AND_MEAN // avoid warnings if defined on command line
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX 1
#include <Windows.h>
#endif

template<typename T>
size_t memsize_of_vec(const std::vector<T>& vec)
{
  return vec.size() * sizeof(*vec.data());
}

template<typename T, size_t N>
constexpr size_t array_size(const T (&)[N])
{
  return N;
}

template <typename T>
constexpr auto array_size(const T& t) -> decltype(t.size())
{
      return t.size();
}

struct Offset
{
  uint32_t offset;
  uint32_t size;
};

#define DATA_SIZE (1<<20)


static int test_file_view(OpenVDS::File &file, const std::vector<uint32_t> &rand_data, Offset (&offsets)[128])
{
  OpenVDS::IOError error;
  int result = 0;
  OpenVDS::FileView *fileview = file.createFileView(0, DATA_SIZE * sizeof(*rand_data.data()), true, error);
  if (!fileview)
  {
    fprintf(stderr, "Error %s\n.", error.string.c_str());
    return error.code;
  }

  const void* data = fileview->pointer();
  for (const auto& offset : offsets)
  {
    if (memcmp(static_cast<const uint32_t*>(data) + offset.offset, &rand_data[offset.offset], offset.size * sizeof(*rand_data.data())))
    {
      fprintf(stderr, "Bad compare in fileview %d %d\n", offset.offset, offset.size);
      return -1;
    }
  }
  return 0;
}

static int test_file_view_wrapper(OpenVDS::File &file, const std::vector<uint32_t> &rand_data, Offset (&offsets)[128])
{
  FILEVIEW_TRY
  {
    return test_file_view(file, rand_data, offsets);
  } FILEVIEW_CATCH { fprintf(stderr, "FILEVIEW EXCEPTION\n"); return -2; } FILEVIEW_FINALLY;
}

int main(int argc, char** argv)
{
#ifdef _WIN32
  char dir[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, dir);
  fprintf(stderr, "Current dir: %s\n", dir);
#endif

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<uint32_t>::max());

  std::vector<uint32_t> rand_data;
  rand_data.resize(DATA_SIZE);
  for (auto& i : rand_data)
    i = dis(gen);

  Offset offsets[128];

  int n = 0;
  for (auto& offset : offsets)
  {
    offset.offset = rand_data[n] % DATA_SIZE;
    uint32_t next_rand = ++n == array_size(rand_data) ? rand_data[0] : rand_data[n];
    offset.size = std::min(uint32_t(array_size(rand_data)) - offset.offset, uint32_t(next_rand % 4096));
  }

  OpenVDS::IOError error;
  std::string filename("test.txt");
  {
    OpenVDS::File file;
    OpenVDS::File::CloseGuard closer(file);
    if (!file.open(filename, true, true, true, error))
    {
      fprintf(stderr, "Could not open file for write %s\n", error.string.c_str());
      return -1;
    }
    if (!file.write(rand_data.data(), 0, memsize_of_vec(rand_data), error))
    {
      fprintf(stderr, "Could not write file %s\n", error.string.c_str());
      return -1;
    }
  }
  {
    OpenVDS::File file;
    OpenVDS::File::CloseGuard closer(file);
    if (!file.open(filename, false, false, false, error))
    {
      fprintf(stderr, "Could not open file read %s\n", error.string.c_str());
      return -1;
    }

    std::vector<uint8_t> data;
    data.resize(memsize_of_vec(rand_data));
    if (!file.read(&data[0], 0, data.size(), error))
    {
      fprintf(stderr, "Could not read file %s\n", error.string.c_str());
      return -1;
    }
    if (memcmp(rand_data.data(), data.data(), data.size()))
    {
      fprintf(stderr, "Data from file is not what was suppose to be writtern\n");
      abort();
    }
  }

  ThreadPool thread_pool(32);
  {
    OpenVDS::File file;
    OpenVDS::IOError error;
    if (!file.open("test_multi.txt", true, true, true, error))
    {
      fprintf(stderr, "Could not open file for multi %s\n", error.string.c_str());
      return -1;
    }

    {

      void* empty = calloc(1, memsize_of_vec(rand_data));
      if (!file.write(empty, 0, memsize_of_vec(rand_data), error))
      {
        fprintf(stderr, "Failed to resize to null the multi file: %s\n", error.string.c_str());
        return -1;
      }
      file.flush();
      free(empty);
    }

    std::vector<std::future<OpenVDS::IOError>> results;
    results.reserve(array_size(offsets));

    for (const auto& offset : offsets)
    {
      results.push_back(thread_pool.enqueue([&file, &rand_data](const Offset & offset)
        {
          OpenVDS::IOError error;
          file.write(&rand_data[offset.offset], offset.offset * sizeof(*rand_data.data()), offset.size * sizeof(*rand_data.data()), error);
          return error;
        }, offset));
    }

    for (auto& result_future : results)
    {
      auto result = result_future.get();
      if (result.code != 0)
      {
        fprintf(stderr, "Write reported error %d: %s\n", result.code, result.string.c_str());
        return result.code;
      }
    }
    file.flush();
    results.clear();

    for (const auto& offset : offsets)
    {
      results.push_back(thread_pool.enqueue([&file, &rand_data](const Offset & offset)
        {
          OpenVDS::IOError error;
          std::vector<uint8_t> vec;
          vec.resize(offset.size * sizeof(*rand_data.data()));
          if (!file.read(vec.data(), offset.offset * sizeof(*rand_data.data()), offset.size * sizeof(*rand_data.data()), error))
          {
            return error;
          }

          if (memcmp(vec.data(), &rand_data[offset.offset], offset.size * sizeof(*rand_data.data())))
          {
            error.code = -255;
            error.string = "Compared data is not the same.";
          }
          return error;
        }, offset));
    }

    for (auto& result_future : results)
    {
      auto result = result_future.get();
      if (result.code != 0)
      {
        fprintf(stderr, "Read reported error %d: %s\n", result.code, result.string.c_str());
        return result.code;
      }
    }
 
    int file_view_result = test_file_view_wrapper(file, rand_data, offsets);
    if (file_view_result)
      return file_view_result;

  }

  return 0;
}
