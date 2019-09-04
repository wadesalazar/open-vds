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

#ifndef WAVELETADAPTIVELL_H
#define WAVELETADAPTIVELL_H

#include "WaveletTypes.h"

#include <inttypes.h>

namespace OpenVDS
{

struct WaveletAdaptiveLL_DecodeIterator
{
  Wavelet_FastDecodeInsig *insig;
  Wavelet_FastDecodeInsig *sig;

  int32_t *pos;
  const uint8_t *compiledTransformData;
  
  float* streamFirstValues;
 
  int32_t *valueEncodingMultiple;
  int32_t *valuesAtLevelMultiple;
  int32_t *valueEncodingSingle;
  int32_t *valuesAtLevelSingle;
  
  float *picture;

  uint8_t *stream;

  int32_t sizeX;
  int32_t sizeY;
  int32_t sizeZ;
  int32_t sizeXY;
  int32_t maxSizeX;
  int32_t maxSizeXY;
  int32_t maxChildren;
  int32_t maxPixel;

  int32_t firstSubBand[8];

  int32_t screenDisplacementAllNormal[8];
  int32_t childDisplacementAllNormal[8];
  int32_t screenDisplacement[8];

  int32_t allNormalAndMask;

  int32_t decodeBits;
  int32_t decompressLevel;
  int32_t dimensions;
  int32_t multiple;

  float threshold;
  float startThreshold;

  bool isAllNormal;

  uint8_t transformMask[12];
  uint8_t children[8];

  Wavelet_PixelSetChildren *pixelSetChildren;
  int32_t pixelSetChildrenCount;

  Wavelet_PixelSetPixel *pixelSetPixelInSignificant;
  int32_t pixelSetPixelInsignificantCount;
};

WaveletAdaptiveLL_DecodeIterator waveletAdaptiveLL_CreateDecodeIterator(uint8_t *streamCPU, float *pictureCPU, int sizeX, int sizeY, int sizeZ,
                                                                        const float threshold, const float startThreshold, int *transformMask, Wavelet_TransformData *transformData, int transformDataCount,
                                                                        Wavelet_PixelSetChildren *pixelSetChildren, int pixelSetChildrenCount, Wavelet_PixelSetPixel *pixelSetPixelInSignificant, int pixelSetPixelInsignificantCount,
                                                                        int maxSizeX, int maxSizeXY, uint8_t *tempBufferCPU, int maxChildren, int maxPixels, int decompressLevel);

int32_t waveletAdaptiveLL_DecompressAdaptive(WaveletAdaptiveLL_DecodeIterator decodeIterator);
int32_t waveletAdaptiveLL_DecompressLossless(uint8_t *in, float *pic, int32_t sizeX, int32_t sizeY, int32_t sizeZ, int32_t allocatedSizeX, int32_t allocatedSizeXY);
}

#endif