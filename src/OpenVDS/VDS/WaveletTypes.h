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

#ifndef WAVELETTYPES_H
#define WAVELETTYPES_H

#include <OpenVDS/Vector.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include "DataBlock.h"

#define WAVELET_MIN_COMPRESSION_TOLERANCE 0.01f

#define WAVELET_DATA_VERSION_1_4 (671) // progressive wavelet transform
#define WAVELET_DATA_VERSION_1_5 (672) // U8, U16 and U32 native/lossless compression

#define WAVELET_MIN_COMPRESSED_HEADER (6 * 4)

#define WAVELET_BAND_MIN_SIZE  8
#define NORMAL_BLOCK_SIZE 8
#define NORMAL_BLOCK_SIZE_FLOAT 8.0f

#define WAVELET_MAX_PIXELSETPIXEL_SIZE (8 * 8 * 8 * (sizeof(OpenVDS::Wavelet_PixelSetPixel)))
#define WAVELET_MAX_PIXELSETCHILDREN_SIZE (7 * 7 * 7 * 7 * (sizeof(OpenVDS::Wavelet_PixelSetChildren)))

#define ADAPTIVEWAVELET_ALIGNBUFFERSIZE 256
#define DECODEITERATOR_MAXDECODEBITS    256

#define WAVELET_ADAPTIVE_LEVELS 16

namespace OpenVDS
{

inline bool CompressionMethod_IsWavelet(CompressionMethod compressionMethod)
{
  return compressionMethod == CompressionMethod::Wavelet ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlock ||
         compressionMethod == CompressionMethod::WaveletLossless ||
         compressionMethod == CompressionMethod::WaveletNormalizeBlockLossless;
}

enum Wavelet_IntegerInfo
{
  WAVELET_INTEGERINFO_ISINTEGER = (1 << 0),
  WAVELET_INTEGERINFO_ISLOSSLESSOPTIMIZED = (1 << 1),
  WAVELET_INTEGERINFO_ISCOMPRESSEDWITHDIFFPASS = (1 << 2),
  WAVELET_INTEGERINFO_16BIT = (1 << 3)
};

bool Wavelet_Decompress(const void *compressedData, int nCompressedAdaptiveDataSize, VolumeDataChannelDescriptor::Format dataBlockFormat, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, bool isNormalize, int nDecompressLevel, bool isLossless, DataBlock &dataBlock, std::vector<uint8_t> &target, Error &error);

struct Wavelet_PixelSetPixel
{
  int32_t x;
  int32_t y;
  int32_t z;
};

struct Wavelet_PixelSetChildren
{
  uint32_t transformIteration;

  int32_t x;
  int32_t y;
  int32_t z;

  int32_t subBand;
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

enum
{
  TRANSFORM_MAX_ITERATIONS = 10
};

struct Wavelet_Compiled_SubBand
{
  uint16_t childPosX;
  uint16_t childPosY;
  uint16_t childPosZ;

  uint16_t extraChildEdgeX;
  uint16_t extraChildEdgeY;
  uint16_t extraChildEdgeZ;

  uint16_t legalChildEdgeX;
  uint16_t legalChildEdgeY;
  uint16_t legalChildEdgeZ;

  uint16_t childSubBand;
};

struct Wavelet_Compiled_SubBandInfo
{
  uint8_t isNormal;
  uint8_t dummyAlign;

  uint16_t normalChildSubBand;

  uint8_t childX;
  uint8_t childY;
  uint8_t childZ;

  uint16_t posX;
  uint16_t posY;
  uint16_t posZ;

  uint8_t childSubBand;
  uint8_t dummyAlign2;

  Wavelet_Compiled_SubBand firstSubBand; // can me more than one based on nChildSubBand
};

struct Wavelet_FastDecodeInsig
{
  int32_t xyz;

  uint16_t subBandPos;

  uint8_t iteration;
  uint8_t padding;
   
  int     GetX() const {return (xyz >> WAVELET_ADAPTIVELL_X_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }
  int     GetY() const {return (xyz >> WAVELET_ADAPTIVELL_Y_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }
  int     GetZ() const {return (xyz >> WAVELET_ADAPTIVELL_Z_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }

  void    SetXYZ(int uX, int uY, int uZ) {xyz = uX | (uY << WAVELET_ADAPTIVELL_Y_SHIFT) | (uZ << WAVELET_ADAPTIVELL_Z_SHIFT); }
 
  Wavelet_FastDecodeInsig() : xyz(), subBandPos(), iteration(), padding() {}
  Wavelet_FastDecodeInsig(int nX, int nY, int nZ, int nIteration, unsigned short iSubBandPos)
  {
    SetXYZ(nX, nY, nZ);
    iteration = nIteration;
    subBandPos = iSubBandPos;
    padding = 0;
  }

};

struct Wavelet_FastDecodeInsigAllNormal
{
public:
  uint32_t iterXYZ;

  int GetX() const {return (iterXYZ >> WAVELET_ADAPTIVELL_X_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }
  int GetY() const {return (iterXYZ >> WAVELET_ADAPTIVELL_Y_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }
  int GetZ() const {return (iterXYZ >> WAVELET_ADAPTIVELL_Z_SHIFT) & WAVELET_ADAPTIVELL_XYZ_AND_MASK; }
  int GetIteration() const {return iterXYZ >> WAVELET_ADAPTIVELL_ITER_SHIFT; }

  void SetXYZIter(int uX, int uY, int uZ, int uIter) {iterXYZ = uX | (uY << WAVELET_ADAPTIVELL_Y_SHIFT) | (uZ << WAVELET_ADAPTIVELL_Z_SHIFT) | (uIter << WAVELET_ADAPTIVELL_ITER_SHIFT); }

  Wavelet_FastDecodeInsigAllNormal() {}

  Wavelet_FastDecodeInsigAllNormal(int nX, int nY, int nZ, int nIteration)
  {
    SetXYZIter(nX, nY, nZ, nIteration);
  }
};

}
#endif
