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

#ifndef WAVELET_H
#define WAVELET_H

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/Vector.h>
#include "DataBlock.h"

#include <memory>

#include "WaveletTypes.h"

namespace OpenVDS
{

class Wavelet
{
  const uint32_t *m_readCompressedData;

  const uint32_t *m_noValueData;

  IntVector3 m_bandSize[TRANSFORM_MAX_ITERATIONS + 1];
  int32_t m_transformMask[TRANSFORM_MAX_ITERATIONS];
  int32_t m_transformIterations;

  int32_t m_dataVersion;
  int32_t m_dimensions;
  int32_t m_dataBlockSizeX;
  int32_t m_dataBlockSizeY;
  int32_t m_dataBlockSizeZ;
  int32_t m_transformSizeX;
  int32_t m_transformSizeY;
  int32_t m_transformSizeZ;
  int32_t m_allocatedSizeX;
  int32_t m_allocatedSizeY;
  int32_t m_allocatedSizeZ;
  int32_t m_allocatedSizeXY;
  int32_t m_allocatedHalfSizeX;
  int32_t m_allocatedHalfSizeY;
  int32_t m_allocatedHalfSizeZ;
  int32_t m_pixelSetChildrenCount;

  std::unique_ptr<float[]> m_maxTree;
  std::unique_ptr<float[]> m_maxTreeMaxOnly;

  std::unique_ptr<int32_t[]> m_tempEncodeDecodeBuffer;

  std::unique_ptr<Wavelet_PixelSetPixel[]> m_pixelSetPixelSignificant;
  std::unique_ptr<Wavelet_PixelSetPixel[]> m_pixelSetPixelInSignificant;
  std::unique_ptr<Wavelet_PixelSetChildren[]> m_pixelSetChildren;

  int32_t m_pixelSetPixelSignificantCount;
  int32_t m_pixelSetPixelInSignificantCount;

public:
  Wavelet(const void *compressedData, int32_t transformSizeX, int32_t transformSizeY, int32_t transformSizeZ, int32_t allocatedSizeX, int32_t allocatedSizeY, int32_t allocatedSizeZ, int32_t dimensions, int32_t dataVersion);
  void InitCoder();

  bool DeCompress(bool isTransform, int32_t decompressInfo, float decompressSlice, int32_t decompressFlip, float *startThreshold, float *threshold, VolumeDataChannelDescriptor::Format dataBlockFormat, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, bool *isAnyNoValue, float *waveletNoValue, bool isNormalize, int decompressLevel, bool isLossless, int compressedAdaptiveDataSize, DataBlock &dataBlock, std::vector<uint8_t> &target, Error &error);
  void DeCompressNoValuesHeader();
  void InverseTransform(float *source);
  void DeCompressNoValues(float* noValue, std::vector<uint32_t> &buffer);
  void ApplyNoValues(float *source, uint32_t* bitBuffer, float noValue);
};

}

#endif
