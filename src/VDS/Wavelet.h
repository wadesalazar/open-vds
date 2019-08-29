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
#include <Math/Vector.h>
#include "DataBlock.h"

#include <memory>

#define WAVELET_MIN_COMPRESSION_TOLERANCE 0.01f

#define WAVELET_OLD_DATA_VERSION (666)
#define WAVELET_DATA_VERSION_1_0 (667)
#define WAVELET_DATA_VERSION_1_1 (668)
#define WAVELET_DATA_VERSION_1_2 (669) // added 1 bit mask support for novalue
#define WAVELET_DATA_VERSION_1_3 (670) // added handling of 0 values
#define WAVELET_DATA_VERSION_1_4 (671) // progressive wavelet transform

#define WAVELET_MIN_COMPRESSED_HEADER (6 * 4)


namespace OpenVDS
{

enum
{
  WAVELET_NORMAL,
  WAVELET_FLIP_XZ,
  WAVELET_FLIP_YZ,
  WAVELET_SLICE_X,
  WAVELET_SLICE_Y,
  WAVELET_SLICE_Z
};

bool Wavelet_Decompress(void *compressedData, int nCompressedAdaptiveDataSize, VolumeDataChannelDescriptor::Format dataBlockFormat, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, bool isNormalize, int nDecompressLevel, bool isLossless, DataBlock &dataBlock, std::vector<uint8_t> &target, Error &error);

struct Wavelet_PixelSetPixel
{
  int32_t _nX;
  int32_t _nY;
  int32_t _nZ;
};

struct Wavelet_PixelSetChildren
{
  uint32_t _uTransformIteration;

  int32_t _nX;
  int32_t _nY;
  int32_t _nZ;

  int32_t _iSubBand;
};

struct Wavelet_SubBandInfo
{
  IntVector3 pos;

  int32_t childSubBand;

  IntVector3 childPos[7];

  IntVector3 extraChildEdge[7];

  IntVector3 legalChildEdge[7];

  int32_t childSector[7];
};

struct Wavelet_TransformData
{
  int child;

  IntVector3 childCount;

  int isNormal;

  Wavelet_SubBandInfo subBandInfo[8];
};

#define WAVELET_ADAPTIVELL_X_SHIFT 0
#define WAVELET_ADAPTIVELL_Y_SHIFT 9
#define WAVELET_ADAPTIVELL_Z_SHIFT 18
#define WAVELET_ADAPTIVELL_ITER_SHIFT 27
#define WAVELET_ADAPTIVELL_XYZ_AND_MASK ((1 << 9) - 1)

struct Wavelet_FastEncodeInsig
{
  int m_XYZ;

  uint16_t m_subBandPos;

  char m_iteration;
  char m_isDeleteMe;
   
  int getX() const {return (m_XYZ >> WAVELET_ADAPTIVELL_X_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }
  int getY() const {return (m_XYZ >> WAVELET_ADAPTIVELL_Y_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }
  int getZ() const {return (m_XYZ >> WAVELET_ADAPTIVELL_Z_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }

  void    SetXYZ(int uX, int uY, int uZ) {m_XYZ = uX | (uY << WAVELET_ADAPTIVELL_Y_SHIFT) | (uZ << WAVELET_ADAPTIVELL_Z_SHIFT); }
 
  Wavelet_FastEncodeInsig() {}
  Wavelet_FastEncodeInsig(int nX, int nY, int nZ, int nIteration, uint16_t iSubBandPos)
  {
    SetXYZ(nX, nY, nZ);
    m_iteration = nIteration;
    m_subBandPos = iSubBandPos;
    m_isDeleteMe = 0;
  }
};

enum
{
  TRANSFORM_MAX_ITERATIONS = 10
};

class Wavelet
{
  uint8_t *m_compressedData;
  uint32_t *m_wordCompressedData;

  uint32_t *m_noValueData;

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
  Wavelet(void *compressedData, int32_t transformSizeX, int32_t transformSizeY, int32_t transformSizeZ, int32_t allocatedSizeX, int32_t allocatedSizeY, int32_t allocatedSizeZ, int32_t dimensions, int32_t dataVersion);
  void initCoder();

  bool deCompress(bool isTransform, int32_t decompressInfo, float decompressSlice, int32_t decompressFlip, float *startThreshold, float *threshold, VolumeDataChannelDescriptor::Format dataBlockFormat, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, bool *isAnyNoValue, float *waveletNoValue, bool isNormalize, int decompressLevel, bool isLossless, int compressedAdaptiveDataSize, DataBlock &dataBlock, std::vector<uint8_t> &target, Error &error);
  void          deCompressNoValuesHeader();
};
}

#endif