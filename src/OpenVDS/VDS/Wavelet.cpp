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

#include "Wavelet.h"
#include "VolumeDataStore.h"

#include <assert.h>

#include "WaveletAdaptiveLL.h"

#define ENABLE_SSE_TRANSFORM 1
#include "WaveletSSETransform.h"

#include "FSE/fse.h"

#define WAVELET_SSE_THREADS 4 //Number of streams packed/unpacked for bitencoding/decoding
#define WAVELET_MAX_DIMENSION_SIZE 4096

#define RLE_BYTE_MASK 0
#define RLE_2BYTE_MASK 1
#define RLE_3BYTE_MASK 2
#define RLE_4BYTE_MASK 3

namespace OpenVDS
{

bool Wavelet_Decompress(const void *compressedData, int nCompressedAdaptiveDataSize, VolumeDataChannelDescriptor::Format dataBlockFormat, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, bool isNormalize, int nDecompressLevel, bool isLossless, DataBlock &dataBlock, std::vector<uint8_t> &target, Error &error)
{
  if (isLossless)
  {
    assert(nDecompressLevel == 0);
  }

  int32_t *intHeaderPtr = (int32_t*)(compressedData);
  
  int32_t  dataVersion = intHeaderPtr[0];

  int32_t createSize[DataStoreDimensionality_Max];
  createSize[0] = intHeaderPtr[2];
  createSize[1] = intHeaderPtr[3];
  createSize[2] = intHeaderPtr[4];
  int32_t dimensions = intHeaderPtr[5];

  if (dataVersion != WAVELET_DATA_VERSION_1_4 ||
    dimensions < 1 ||
    dimensions > 3 ||
    createSize[0] < 0 ||
    createSize[0] > 512 ||
    createSize[1] < 0 ||
    createSize[1] > 512 ||
    createSize[2] < 0 ||
    createSize[2] > 512)
  {
    error.code = 100;
    error.string = "Invalid wavelet header";
    return false;
  }

  if (!InitializeDataBlock(VolumeDataChannelDescriptor::Format_R32, VolumeDataChannelDescriptor::Components_1, Dimensionality(dimensions), createSize, dataBlock, error))
    return false;
  target.resize(GetAllocatedByteSize(dataBlock));
  
  Wavelet wavelet(compressedData,
    dataBlock.Size[0],
    dataBlock.Size[1],
    dataBlock.Size[2],
    dataBlock.AllocatedSize[0],
    dataBlock.AllocatedSize[1],
    dataBlock.AllocatedSize[2],
    dimensions,
    dataVersion);

  float threshold;
  float startThreshold;
  float waveletNoValue;

  bool isAnyNoValue;

  if (!wavelet.DeCompress(true, -1, -1, -1, &startThreshold, &threshold, dataBlock.Format, valueRange, integerScale, integerOffset, isUseNoValue, noValue, &isAnyNoValue, &waveletNoValue, isNormalize, nDecompressLevel, isLossless, nCompressedAdaptiveDataSize, dataBlock, target, error))
    return false;

  return true;
}


static int32_t findTransformMethod(IntVector3 (&bandSize)[TRANSFORM_MAX_ITERATIONS + 1], int32_t(&splitMask)[TRANSFORM_MAX_ITERATIONS], int32_t sizeX, int32_t sizeY, int32_t sizeZ, int32_t dataVersion)
{
  int i = 0;

  int32_t bandPosX = 0;
  int32_t bandPosY = 0;
  int32_t bandPosZ = 0;

  while (sizeX >= WAVELET_BAND_MIN_SIZE ||
         sizeY >= WAVELET_BAND_MIN_SIZE ||
         sizeZ >= WAVELET_BAND_MIN_SIZE)
  {
    bandSize[i] = {sizeX, sizeY, sizeZ};

    char mask = 0;

    if  (sizeX >= WAVELET_BAND_MIN_SIZE) mask |= 1;
    if  (sizeY >= WAVELET_BAND_MIN_SIZE) mask |= 2;
    if  (sizeZ >= WAVELET_BAND_MIN_SIZE) mask |= 4;
     
    if (mask & 1) sizeX = (sizeX + 1) >> 1;
    if (mask & 2) sizeY = (sizeY + 1) >> 1;
    if (mask & 4) sizeZ = (sizeZ + 1) >> 1;

    splitMask[i] = mask;

    i++;

    bandPosX = sizeX;
    bandPosY = sizeY;
    bandPosZ = sizeZ;
  }

  bandSize[i] = {sizeX, sizeY, sizeZ};

  if (i==0)
  {
    splitMask[0] = 0;
    bandSize[i+1] = {sizeX, sizeY, sizeZ};
  }

  assert(i <= TRANSFORM_MAX_ITERATIONS);

  return i;
}

static void createTransformData(Wavelet_TransformData (&transformData)[TRANSFORM_MAX_ITERATIONS], const IntVector3 (&bandSize)[TRANSFORM_MAX_ITERATIONS + 1], int32_t *transformMask, int32_t transformIterations)
{
  for (int i=0; i < transformIterations; i++)
  {
    int currentTransformMask = transformMask[i];

    transformData[i].childCount = {0,0,0};
    transformData[i].isNormal = false;

    for (int iSector = 0; iSector < 8; iSector++)
    {
      transformData[i].subBandInfo[iSector].childPos[0] = {-100000,-100000,-100000};
      transformData[i].subBandInfo[iSector].extraChildEdge[0] = {-100000,-100000,-100000};
      transformData[i].subBandInfo[iSector].legalChildEdge[0] = {-100000,-100000,-100000};
      transformData[i].subBandInfo[iSector].childSector[0] = -1;
      transformData[i].subBandInfo[iSector].childSubBand = 666;
    }

    if (i>0)
    {
      transformData[i].childCount = {1,1,1};

      if (currentTransformMask & 1) transformData[i].childCount[0] = 2;
      if (currentTransformMask & 2) transformData[i].childCount[1] = 2;
      if (currentTransformMask & 4) transformData[i].childCount[2] = 2;

      transformData[i].child = transformData[i].childCount[0] * transformData[i].childCount[1] * transformData[i].childCount[2]; 

      int nextTransformMask = transformMask[i - 1];

      int otherTransformMask = currentTransformMask ^ nextTransformMask;

       bool isAddExtras = true;

      bool isNormal = true;

      for (int iSector = 1; iSector < 8; iSector++)
      {
        int32_t childSubBand = 0;

        if ((iSector & currentTransformMask) == iSector)
        {
          // Find subband pos
          IntVector3 subBandPos = {0,0,0};

          if (iSector & 1) subBandPos[0] = bandSize[i+1][0];
          if (iSector & 2) subBandPos[1] = bandSize[i+1][1];
          if (iSector & 4) subBandPos[2] = bandSize[i+1][2];

          IntVector3 childPos = {0,0,0};

          if (iSector & 1) childPos[0] = bandSize[i][0];
          if (iSector & 2) childPos[1] = bandSize[i][1];
          if (iSector & 4) childPos[2] = bandSize[i][2];

          // Add default subband
          transformData[i].subBandInfo[iSector].pos = subBandPos;
          transformData[i].subBandInfo[iSector].childPos[childSubBand] = childPos;
          transformData[i].subBandInfo[iSector].childSector[childSubBand] = iSector;
          childSubBand++;

          if (otherTransformMask)
          {
            isNormal = false;
            // Go through the inverted mask
            for (int iOtherSector = 1; iOtherSector < 8; iOtherSector++)
            {
              if ((otherTransformMask & iOtherSector) == iOtherSector)
              {
                // Add this one aswell
                IntVector3 currentSubPos = childPos;

                if (iOtherSector & 1) currentSubPos[0] = bandSize[i][0];
                if (iOtherSector & 2) currentSubPos[1] = bandSize[i][1];
                if (iOtherSector & 4) currentSubPos[2] = bandSize[i][2];

                transformData[i].subBandInfo[iSector].childPos[childSubBand] = currentSubPos;
                transformData[i].subBandInfo[iSector].childSector[childSubBand] = iSector + iOtherSector;
                childSubBand++;
              }
            }

            // Add extras
            if (isAddExtras)
            {
              isAddExtras = false;
            
              for (int iOtherSector = 1; iOtherSector < 8; iOtherSector++)
              {
                if ((otherTransformMask & iOtherSector) == iOtherSector)
                {
                  // Add this one aswell
                  IntVector3 currentSubPos = subBandPos;

                  if (iOtherSector & 1) currentSubPos[0] = bandSize[i][0];
                  if (iOtherSector & 2) currentSubPos[1] = bandSize[i][1];
                  if (iOtherSector & 4) currentSubPos[2] = bandSize[i][2];

                  if (iSector & 1) currentSubPos[0] = 0;
                  if (iSector & 2) currentSubPos[1] = 0;
                  if (iSector & 4) currentSubPos[2] = 0;

                  transformData[i].subBandInfo[iSector].childPos[childSubBand] = currentSubPos;
                  transformData[i].subBandInfo[iSector].childSector[childSubBand] = iOtherSector;
                  childSubBand++;
                }
              }
            }
          }
        }
        
        transformData[i].subBandInfo[iSector].childSubBand = childSubBand;

        IntVector3 startWrite = {0,0,0};
        IntVector3 endWrite = bandSize[i];

        if (iSector & 1) startWrite[0] = bandSize[i + 1][0];
        else             endWrite[0] = bandSize[i + 1][0];

        if (iSector & 2) startWrite[1] = bandSize[i + 1][1];
        else             endWrite[1] = bandSize[i + 1][1];

        if (iSector & 4) startWrite[2] = bandSize[i + 1][2];
        else             endWrite[2] = bandSize[i + 1][2];


        IntVector3 delta = IntVector3(endWrite.X - startWrite.X,
                                      endWrite.Y - startWrite.Y,
                                      endWrite.Z - startWrite.Z);

        if (currentTransformMask & 1) delta[0] *= 2;
        if (currentTransformMask & 2) delta[1] *= 2;
        if (currentTransformMask & 4) delta[2] *= 2;

        int32_t childX = 1;
        int32_t childY = 1;
        int32_t childZ = 1; 

        if (currentTransformMask & 1) childX = 2;
        if (currentTransformMask & 2) childY = 2;
        if (currentTransformMask & 4) childZ = 2;

        for (int iSubBand = 0; iSubBand < transformData[i].subBandInfo[iSector].childSubBand; iSubBand++)
        {
          int32_t  iChildSector = transformData[i].subBandInfo[iSector].childSector[iSubBand];

          IntVector3 startRead(transformData[i].subBandInfo[iSector].childPos[iSubBand]);
          IntVector3 endRead = bandSize[i];

          if (iChildSector & 1) endRead[0] = bandSize[i-1][0];
          if (iChildSector & 2) endRead[1] = bandSize[i-1][1];
          if (iChildSector & 4) endRead[2] = bandSize[i-1][2];

          transformData[i].subBandInfo[iSector].legalChildEdge[iSubBand] = endRead;

          // Can we easy double to get to the position?
          if ((startWrite[0] * childX) != startRead[0] ||
              (startWrite[1] * childY) != startRead[1] ||
              (startWrite[2] * childZ) != startRead[2])
          {
            isNormal = false;
          }

          // Can a child be created outside legal band?
          if ((startRead[0] + delta[0]) > endRead[0] ||
              (startRead[1] + delta[1]) > endRead[1] ||
              (startRead[2] + delta[2]) > endRead[2])
          {
            isNormal = false;
          }

          // Are there some places we need to create 3 children along one axis?

          endRead = IntVector3(endRead.X - startRead.X,
                               endRead.Y - startRead.Y,
                               endRead.Z - startRead.Z);

          transformData[i].subBandInfo[iSector].extraChildEdge[iSubBand] = {-1, -1, -1};
          
          if (endRead[0] > delta[0])
          {
            isNormal = false;
            transformData[i].subBandInfo[iSector].extraChildEdge[iSubBand][0] = endWrite[0] - 1;
          }
          
          if (endRead[1] > delta[1])
          {
            isNormal = false;
            transformData[i].subBandInfo[iSector].extraChildEdge[iSubBand][1] = endWrite[1] - 1;
          }

          if (endRead[2] > delta[2]) 
          {
            isNormal = false;
            transformData[i].subBandInfo[iSector].extraChildEdge[iSubBand][2] = endWrite[2] - 1;
          }
        }
      }

      transformData[i].isNormal = isNormal;
    }
  }
}

static int CalculateBufferSizeNeeded(int maxPixels, int maxChildren)
{
  int size = 0;

  size += WAVELET_MAX_PIXELSETPIXEL_SIZE + ADAPTIVEWAVELET_ALIGNBUFFERSIZE;
  size += WAVELET_MAX_PIXELSETCHILDREN_SIZE + ADAPTIVEWAVELET_ALIGNBUFFERSIZE;

  size += DECODEITERATOR_MAXDECODEBITS * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // valueEncoding
  size += DECODEITERATOR_MAXDECODEBITS * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // valuesAtLevel
  size += DECODEITERATOR_MAXDECODEBITS * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // valueEncodingSingle
  size += DECODEITERATOR_MAXDECODEBITS * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // valuesAtLevelSingle
  size += maxChildren * sizeof(Wavelet_FastDecodeInsig) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // Insignificants + align to 256
  size += maxChildren * sizeof(Wavelet_FastDecodeInsig) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // Significants + align to 256
  size += (maxPixels + maxChildren) * int32_t(sizeof(int)) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // positions + align to 256
  size += 32768 + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // max sizeof transform hierarchical helper data
  return size;
}

Wavelet::Wavelet(const void *compressedData, int32_t transformSizeX, int32_t transformSizeY, int32_t transformSizeZ, int32_t allocatedSizeX, int32_t allocatedSizeY, int32_t allocatedSizeZ, int32_t dimensions, int32_t dataVersion)
{
  m_readCompressedData = (const uint32_t *)compressedData;
  m_noValueData = nullptr;
  m_dataVersion = dataVersion;
  m_dimensions = dimensions;

  m_dataBlockSizeX = transformSizeX;
  m_dataBlockSizeY = transformSizeY;
  m_dataBlockSizeZ = transformSizeZ;

  m_transformSizeX = transformSizeX;
  m_transformSizeY = transformSizeY;
  m_transformSizeZ = transformSizeZ;

  m_allocatedSizeX = allocatedSizeX;
  m_allocatedSizeY = allocatedSizeY;
  m_allocatedSizeZ = allocatedSizeZ;

  m_allocatedSizeXY = allocatedSizeX * allocatedSizeY;

  m_allocatedHalfSizeX = allocatedSizeX;
  m_allocatedHalfSizeY = allocatedSizeY;
  m_allocatedHalfSizeZ = allocatedSizeZ;

  m_transformIterations = findTransformMethod(m_bandSize, m_transformMask, m_transformSizeX, m_transformSizeY, m_transformSizeZ, m_dataVersion);

  m_allocatedHalfSizeX = m_bandSize[1][0];
  m_allocatedHalfSizeY = m_bandSize[1][1];
  m_allocatedHalfSizeZ = m_bandSize[1][2];

  m_maxTree.reset(new float[m_allocatedHalfSizeX * m_allocatedHalfSizeY * m_allocatedHalfSizeZ]);
  m_maxTreeMaxOnly.reset(new float[m_allocatedHalfSizeX * m_allocatedHalfSizeY * m_allocatedHalfSizeZ]);
  m_tempEncodeDecodeBuffer.reset(new int[(16*16*16*2) + m_allocatedSizeX * m_allocatedSizeY * m_allocatedSizeZ]);

  int allocSize = 256 * 256 * 7;
  m_pixelSetPixelSignificant.reset(new Wavelet_PixelSetPixel[allocSize]);
  m_pixelSetPixelInSignificant.reset(new Wavelet_PixelSetPixel[allocSize]);
  m_pixelSetChildren.reset(new Wavelet_PixelSetChildren[allocSize]);
}

static inline void CreatePixelSetChildren(Wavelet_PixelSetChildren *pixelSet, uint32_t iX, uint32_t iY, uint32_t iZ,  uint32_t uTransformIteration, int32_t iSubBand)
{
  pixelSet->x = iX;
  pixelSet->y = iY;
  pixelSet->z = iZ;
  pixelSet->transformIteration = uTransformIteration;
  pixelSet->subBand = iSubBand;
}

static inline void CreatePixelSetPixel(Wavelet_PixelSetPixel *pixelSet, uint32_t iX, uint32_t iY, uint32_t iZ)
{
  pixelSet->x = iX;
  pixelSet->y = iY;
  pixelSet->z = iZ;
}

void Wavelet::InitCoder()
{
  m_pixelSetPixelInSignificantCount = 0;
  m_pixelSetPixelSignificantCount = 0;
  m_pixelSetChildrenCount = 0;

  for (int i = 0; i < m_transformIterations; i++)
  {
    char transformMask = m_transformMask[i];

    if (i == (m_transformIterations - 1))
    {
      for (int iSector = 1; iSector < 8; iSector++)
      {
        if ((transformMask & iSector) == iSector)
        {
          int32_t startX = 0;
          int32_t startY = 0;
          int32_t startZ = 0;
          int32_t endX = m_bandSize[i + 1][0];
          int32_t endY = m_bandSize[i + 1][1];
          int32_t endZ = m_bandSize[i + 1][2];

          if (iSector & 1)
          {
            startX = m_bandSize[i + 1][0];
            endX = m_bandSize[i][0];
          };

          if (iSector & 2)
          {
            startY = m_bandSize[i + 1][1];
            endY = m_bandSize[i][1];
          }

          if (iSector & 4)
          {
            startZ = m_bandSize[i + 1][2];
            endZ = m_bandSize[i][2];
          }

          for (int32_t iZ = startZ; iZ < endZ; iZ++)
          {
            for (int32_t iY = startY; iY < endY; iY++)
            {
              for (int32_t iX = startX; iX < endX; iX++)
              {
                if (i > 0)
                {
                  CreatePixelSetChildren(&m_pixelSetChildren[m_pixelSetChildrenCount++], iX, iY, iZ, i, iSector);
                }
              }
            }
          }
        }
      }
    }
  }


  int32_t bandSizeX = m_bandSize[m_transformIterations][0];
  int32_t bandSizeY = m_bandSize[m_transformIterations][1];
  int32_t bandSizeZ = m_bandSize[m_transformIterations][2];

  // take also first "children" into account, the are last level
  if (m_transformIterations == 1)
  {
    bandSizeX = m_bandSize[0][0];
    bandSizeY = m_bandSize[0][1];
    bandSizeZ = m_bandSize[0][2];
  }

  for (int32_t iD2 = 0; iD2 < bandSizeZ; iD2++)
  {
    for (int32_t iD1 = 0; iD1 < bandSizeY; iD1++)
    {
      for (int32_t iD0 = 0; iD0 < bandSizeX; iD0++)
      {
          CreatePixelSetPixel(&m_pixelSetPixelInSignificant[m_pixelSetPixelInSignificantCount++], iD0, iD1, iD2);
      }
    }
  }
}

#define NORMAL_BLOCK_SIZE 8
#define NORMAL_BLOCK_SIZE_FLOAT 8.0f

#define READNORMVAL(X,Y,Z) normalizeField[X + Y * nNormalizeX + Z * nNormalizeX * nNormalizeY]

static float GetNormalizedValue(float *normalizeField, int iX, int iY, int iZ, int nNormalizeX, int nNormalizeY,int nNormalizeZ)
{
  float rX = (float)iX;
  float rY = (float)iY;
  float rZ = (float)iZ;

  rX /= NORMAL_BLOCK_SIZE_FLOAT;
  rY /= NORMAL_BLOCK_SIZE_FLOAT;
  rZ /= NORMAL_BLOCK_SIZE_FLOAT;

  rX -= ((NORMAL_BLOCK_SIZE_FLOAT - 1.0f) / 2.0f) / NORMAL_BLOCK_SIZE_FLOAT;
  rY -= ((NORMAL_BLOCK_SIZE_FLOAT - 1.0f) / 2.0f) / NORMAL_BLOCK_SIZE_FLOAT;
  rZ -= ((NORMAL_BLOCK_SIZE_FLOAT - 1.0f) / 2.0f) / NORMAL_BLOCK_SIZE_FLOAT;

  int32_t iNormX = (int32_t)floorf(rX);
  int32_t iNormY = (int32_t)floorf(rY);
  int32_t iNormZ = (int32_t)floorf(rZ);

  // are we at end? if so, move one back so we extrapolate!
  if (iNormX == nNormalizeX - 1) iNormX -= 1;
  if (iNormY == nNormalizeX - 1) iNormY -= 1;
  if (iNormZ == nNormalizeX - 1) iNormZ -= 1;

  int32_t iNormX1 = iNormX+1;
  int32_t iNormY1 = iNormY+1;
  int32_t iNormZ1 = iNormZ+1;

  if (iNormX < 0) iNormX = 0;
  if (iNormY < 0) iNormY = 0;
  if (iNormZ < 0) iNormZ = 0;
  if (iNormX1 < 0) iNormX1 = 0;
  if (iNormY1 < 0) iNormY1 = 0;
  if (iNormZ1 < 0) iNormZ1 = 0;
  if (iNormX1 >= nNormalizeX) iNormX1 = nNormalizeX -1;
  if (iNormY1 >= nNormalizeY) iNormY1 = nNormalizeY -1;
  if (iNormZ1 >= nNormalizeZ) iNormZ1 = nNormalizeZ -1;

  rX -= (float)iNormX;
  rY -= (float)iNormY;
  rZ -= (float)iNormZ;

  if (rX < 0.0f) rX = 0.0f;
  if (rY < 0.0f) rY = 0.0f;
  if (rZ < 0.0f) rZ = 0.0f;

  if (rX > 1.0f) rX = 1.0f;
  if (rY > 1.0f) rY = 1.0f;
  if (rZ > 1.0f) rZ = 1.0f;

  // read and interpolate
  float r0 = READNORMVAL(iNormX, iNormY, iNormZ);
  float r1 = READNORMVAL(iNormX1, iNormY, iNormZ);
  float r2 = READNORMVAL(iNormX, iNormY1, iNormZ);
  float r3 = READNORMVAL(iNormX1, iNormY1, iNormZ);
  float r4 = READNORMVAL(iNormX, iNormY, iNormZ1);
  float r5 = READNORMVAL(iNormX1, iNormY, iNormZ1);
  float r6 = READNORMVAL(iNormX, iNormY1, iNormZ1);
  float r7 = READNORMVAL(iNormX1, iNormY1, iNormZ1);

  float rX0 = (r1 - r0) * rX + r0;
  float rX2 = (r3 - r2) * rX + r2;
  float rX4 = (r5 - r4) * rX + r4;
  float rX6 = (r7 - r6) * rX + r6;

  float rY0 = (rX2 - rX0) * rY + rX0;
  float rY4 = (rX6 - rX4) * rY + rX4;

  float rVal = (rY4 - rY0) * rZ + rY0;

  if (rVal < (1e-22f)) rVal = 1e-22f;

  return rVal;
}

template <class T, bool isHigh>
static void ReplaceZeroFromZeroCount(T *pic, int transformSizeY, int transformSizeZ, int allocatedSizeX, int allocatedSizeY, const uint8_t *countLow, const uint8_t *countHigh, T replaceValue)
{
  for (int iZ=0; iZ<transformSizeZ;iZ++)
  {
  #pragma omp parallel for
    for (int iY=0; iY<transformSizeY;iY++)
    {
      T *read = pic + iY * allocatedSizeX + iZ * allocatedSizeX * allocatedSizeY;

      uint16_t count = countLow[iY + iZ * transformSizeY];
      
      if (isHigh)
      {
        count |= (countHigh[iY + iZ * transformSizeY] << 8);
      }

      for (int i=0;i<count;i++)
      {
        read[i] = replaceValue;
      }
    }
  }
}
// This function decompresses how many zeros along X from X0 - using FSE to compress counts (
static void DecompressZerosAlongX(const uint8_t *in, void *pic, int elementSize, float replaceValue, int transformSizeX, int transformSizeY, int transformSizeZ, int allocatedSizeX, int allocatedSizeY, int allocatedSizeZ, unsigned char *temp)
{
  int totalSize = *((int *)in);
  
  in +=4;

  assert(totalSize >= 4);

  // No zero runs in data, return immediately and do nothing!!
  if (totalSize == 4)
  {
    return;
  }
  
  bool isHigh = transformSizeX >= 256;

  int transformSizeYZ = transformSizeY * transformSizeZ;

  uint8_t *countLow = temp; 
  uint8_t *countHigh = temp + transformSizeYZ;

  int *readSize = (int *)in;

  in +=4;
 
  int size = *readSize++;
  
  if (size == 0)
  {
    int value = *readSize++;
    memset(countLow, value, transformSizeYZ);
    in += 4;
  }
  else if (size < 0)
  {
    memcpy(countLow, in, transformSizeYZ);
    in += transformSizeYZ;
    assert(-size == transformSizeYZ);
  }
  else
  {
    FSE_decompress(countLow, transformSizeYZ, in, size);
    in+=size;
  }

  if (isHigh)
  {
    readSize = (int *)in;
    in +=4;

    size = *readSize++;
  
    if (size == 0)
    {
      int value = *readSize++;
      memset(countHigh, value, transformSizeYZ);
      in += 4;
    }
    else if (size < 0)
    {
      memcpy(countHigh, in, transformSizeYZ);
      in += transformSizeYZ;
      assert(-size == transformSizeYZ);
    }
    else
    {
      FSE_decompress(countHigh, transformSizeYZ, in, size);
      in+=size;
    }
  }

  if (elementSize == 1)
  {
    if (isHigh) ReplaceZeroFromZeroCount<uint8_t, true>((uint8_t*)pic, transformSizeY, transformSizeZ, allocatedSizeX, allocatedSizeY, countLow, countHigh, (unsigned char)replaceValue);
    else        ReplaceZeroFromZeroCount<uint8_t, false>((unsigned char*)pic, transformSizeY, transformSizeZ, allocatedSizeX, allocatedSizeY, countLow, countHigh, (unsigned char)replaceValue);
  }
  else if (elementSize == 2)
  {
    if (isHigh) ReplaceZeroFromZeroCount<uint16_t, true>((unsigned short*)pic, transformSizeY, transformSizeZ, allocatedSizeX, allocatedSizeY, countLow, countHigh, (unsigned short)replaceValue);
    else        ReplaceZeroFromZeroCount<uint16_t, false>((unsigned short*)pic, transformSizeY, transformSizeZ, allocatedSizeX, allocatedSizeY, countLow, countHigh, (unsigned short)replaceValue);
  }
  else
  {
    assert(elementSize == 4);
    if (isHigh) ReplaceZeroFromZeroCount<float, true>((float*)pic, transformSizeY, transformSizeZ, allocatedSizeX, allocatedSizeY, countLow, countHigh, replaceValue);
    else        ReplaceZeroFromZeroCount<float, false>((float*)pic, transformSizeY, transformSizeZ, allocatedSizeX, allocatedSizeY, countLow, countHigh, replaceValue);
  }
}

bool Wavelet::DeCompress(bool isTransform, int32_t decompressInfo, float decompressSlice, int32_t decompressFlip, float* startThreshold, float* threshold, VolumeDataChannelDescriptor::Format dataBlockFormat, const FloatRange& valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, bool* isAnyNoValue, float* waveletNoValue, bool isNormalize, int decompressLevel, bool isLossless, int compressedAdaptiveDataSize, DataBlock& dataBlock, std::vector<uint8_t>& target, Error& error)
{
  assert(m_dataVersion == WAVELET_DATA_VERSION_1_4);
  InitCoder();

  int32_t *startOfCompressedData = (int32_t *)m_readCompressedData;

  int32_t compressedSize = startOfCompressedData[1];

  // no data?
  if (compressedSize < WAVELET_MIN_COMPRESSED_HEADER)
  {
    error.string = "Invalid size of compressed wavlet data";
    error.code = -1;
    *isAnyNoValue = false;
    *waveletNoValue = 0.0f;
    return false;
  }
  else if (compressedSize <= WAVELET_MIN_COMPRESSED_HEADER)
  {
    *isAnyNoValue = false;
    *waveletNoValue = 0.0f;
    return true;
  }

  m_readCompressedData += 6;
  
  int nDeCompressZeroSize = 0;
  
  unsigned char *pnDecompressZeroSize = nullptr;
  
  pnDecompressZeroSize = (unsigned char*)m_readCompressedData;
  nDeCompressZeroSize = *m_readCompressedData;
  m_readCompressedData += (nDeCompressZeroSize + 3) / 4;

  DeCompressNoValuesHeader();

  float *normalField = (float *)(m_readCompressedData);
  
  if (isNormalize)
  {
    for (int iZ = 0; iZ < m_dataBlockSizeZ; iZ+=NORMAL_BLOCK_SIZE)
    {
      for (int iY = 0; iY < m_dataBlockSizeY; iY+=NORMAL_BLOCK_SIZE)
      {
        for (int iX = 0; iX < m_dataBlockSizeX; iX+=NORMAL_BLOCK_SIZE)
        {
          m_readCompressedData++;
        }
      }
    }
  }

  float *floatRead = (float *)m_readCompressedData;

  *threshold = *floatRead++,            // Get Threshold
  *startThreshold = *floatRead++;       // Get StartThreshold

  m_readCompressedData = (uint32_t *)floatRead;

  void *pxData = (void*) target.data();                                                             

  int nAdaptiveSize = *m_readCompressedData++;

  float *floatReadWriteData = (float*)pxData;

  // create transform data
  Wavelet_TransformData transformData[TRANSFORM_MAX_ITERATIONS];

  createTransformData(transformData, m_bandSize, m_transformMask, m_transformIterations);

  int cpuTempDecodeSizeNeeded = CalculateBufferSizeNeeded(m_allocatedSizeX * m_allocatedSizeY * m_allocatedSizeZ, m_allocatedHalfSizeX * m_allocatedHalfSizeY * m_allocatedHalfSizeZ);

  std::vector<uint8_t> cpuTempData;
  cpuTempData.resize(cpuTempDecodeSizeNeeded);

  WaveletAdaptiveLL_DecodeIterator decodeIterator = WaveletAdaptiveLL_CreateDecodeIterator((uint8_t*)m_readCompressedData, floatReadWriteData, m_allocatedSizeX, m_allocatedSizeY, m_allocatedSizeZ, *threshold, *startThreshold, m_transformMask, transformData, m_transformIterations,
      m_pixelSetChildren.get(), m_pixelSetChildrenCount, m_pixelSetPixelInSignificant.get(), m_pixelSetPixelInSignificantCount,
      m_allocatedHalfSizeX, m_allocatedHalfSizeX * m_allocatedHalfSizeY, cpuTempData.data(), m_allocatedHalfSizeX * m_allocatedHalfSizeY * m_allocatedHalfSizeZ, m_allocatedSizeX * m_allocatedSizeY * m_allocatedSizeZ, decompressLevel);

  int size = WaveletAdaptiveLL_DecompressAdaptive(decodeIterator);

  if (isLossless)
  {
    assert(nAdaptiveSize == size);
  }

  cpuTempData = std::vector<uint8_t>();

  InverseTransform(floatReadWriteData);

  // Decompres no values?
  float localWaveletNoValue;

  std::vector<uint32_t> noValueBitBuffer;
  DeCompressNoValues(&localWaveletNoValue, noValueBitBuffer);

  if (noValueBitBuffer.size())
  {
    ApplyNoValues(floatReadWriteData, noValueBitBuffer.data(), localWaveletNoValue);
  }

  if (isAnyNoValue) *isAnyNoValue = noValueBitBuffer.size();
  if (waveletNoValue) *waveletNoValue = localWaveletNoValue;

// DeNormalize?
  if (isNormalize)
  {
    assert(noValueBitBuffer.empty()); // NoValue & Normalization is mutually exclusive (for now).
    int32_t normalizeX = (m_dataBlockSizeX + NORMAL_BLOCK_SIZE - 1) / NORMAL_BLOCK_SIZE;
    int32_t normalizeY = (m_dataBlockSizeY + NORMAL_BLOCK_SIZE - 1) / NORMAL_BLOCK_SIZE;
    int32_t normalizeZ = (m_dataBlockSizeZ + NORMAL_BLOCK_SIZE - 1) / NORMAL_BLOCK_SIZE;

    for (int iZ = 0; iZ < m_dataBlockSizeZ; iZ++)
    {
      for (int iY = 0; iY < m_dataBlockSizeY; iY++)
      {
        for (int iX = 0; iX < m_dataBlockSizeX; iX++)
        {
          floatReadWriteData[iX + iY * m_allocatedSizeX + iZ * m_allocatedSizeXY] *= GetNormalizedValue(normalField, iX, iY, iZ, normalizeX, normalizeY, normalizeZ);
        }
      }
    }
  }

  //  Decompress Zeroes
  if (nDeCompressZeroSize > 4) // if equal to 4, then do nothing, no zero runs. 4 is the minimum size stored
  {
    std::vector<uint8_t> buffer;
    buffer.resize(m_transformSizeY * m_transformSizeZ * int32_t(sizeof(unsigned short)));

    DecompressZerosAlongX(pnDecompressZeroSize, (void*)floatReadWriteData, 4, 0.0f, m_transformSizeX, m_transformSizeY, m_transformSizeZ, m_allocatedSizeX, m_allocatedSizeY, m_allocatedSizeZ, buffer.data());
  }

  // Do lossless diff
  if (isLossless)
  {
    assert(dataBlockFormat == VolumeDataChannelDescriptor::Format_R32);

    m_readCompressedData += (nAdaptiveSize + 3) / 4;

    int32_t size = WaveletAdaptiveLL_DecompressLossless((unsigned char*)m_readCompressedData, floatReadWriteData, m_transformSizeX, m_transformSizeY, m_transformSizeZ, m_allocatedSizeX, m_allocatedSizeXY);

    m_readCompressedData += (size + 3) / 4;
  }

  return true;
}

void Wavelet::DeCompressNoValuesHeader()
{
  int size = *m_readCompressedData;

  if (size == -1)
  {
    m_readCompressedData++;
    m_noValueData = nullptr;
    return; // no NoValues
  }

  m_noValueData = m_readCompressedData;

  m_readCompressedData += 2; // size and no value
  m_readCompressedData += (size + 3) / 4;
}

template<typename T>
static void ResizeVector(std::vector<T> &buffer, int32_t  allocatedSizeX, int32_t allocatedSizeY, int32_t allocatedSizeZ)
{
  int32_t allocatedSize = allocatedSizeX * allocatedSizeY * allocatedSizeZ;

  buffer.resize(allocatedSize);
}

#ifdef ENABLE_SSE_TRANSFORM
void Wavelet::InverseTransform(float *source)
{
  std::vector<float> tempBuffer;
  ResizeVector(tempBuffer, (m_bandSize[0][0] + 3) & ~3, m_bandSize[0][1], m_bandSize[0][2]);

  for (int i = m_transformIterations - 1; i >= 0; i--)
  {
    int32_t bandSize[3];

    char transformMask = m_transformMask[i];

    int32_t bandSizeX = m_bandSize[i][0];
    int32_t bandSizeY = m_bandSize[i][1];
    int32_t bandSizeZ = m_bandSize[i][2];

    bandSize[0] = bandSizeX;
    bandSize[1] = bandSizeY;
    bandSize[2] = bandSizeZ;

    assert(bandSizeX <= m_bandSize[0][0]);
    assert(bandSizeY <= m_bandSize[0][1]);
    assert(bandSizeZ <= m_bandSize[0][2]);

    int32_t bufferPitchXY = (bandSizeX + 3) & ~3;
    int32_t bufferPitchX = bufferPitchXY * bandSizeZ; // Swapping pitches is faster.

    int32_t readPitchX = m_allocatedSizeX;
    int32_t readPitchXY = m_allocatedSizeXY;
    int32_t writePitchX = bufferPitchX;
    int32_t writePitchXY = bufferPitchXY;

    float *read = source;
    float *write = tempBuffer.data();
    const int32_t threadCount = WAVELET_SSE_THREADS;

    if (transformMask == 7)
    {
      // changed so it doez Z first, then Y, then X.
      #pragma omp parallel for num_threads(threadCount) schedule(guided)
      for (int32_t iD1 = 0; iD1 < bandSize[1]; ++iD1)
      {
        Wavelet_InverseTransformSliceInterleave(tempBuffer.data() + iD1 * bufferPitchX, bufferPitchXY, source + iD1 * m_allocatedSizeX, m_allocatedSizeXY, bandSizeX, bandSizeZ);
      }

      #pragma omp parallel for num_threads(threadCount) schedule(guided)
      for (int32_t iD2 = 0; iD2 < bandSize[2]; ++iD2)
      {
        Wavelet_InverseTransformSlice(tempBuffer.data() + iD2 * bufferPitchXY, bufferPitchX, tempBuffer.data() + iD2 * bufferPitchXY, bufferPitchX, bandSizeX, bandSizeY);

        for (int32_t iD1 = 0; iD1 < bandSize[1]; ++iD1)
        {
          int32_t iD1Interleaved = (iD1 & 1 ? (bandSize[1] + 1) >> 1 : 0) + (iD1 >> 1);

          // Wavelet transform x
          float *readLine = tempBuffer.data() + (iD2 * bufferPitchXY + iD1Interleaved * bufferPitchX);

          Wavelet_InverseTransformLine(readLine, bandSizeX);
          Wavelet_InterleaveLine(source + (iD1 * m_allocatedSizeX + iD2 * m_allocatedSizeXY), readLine, readLine + ((bandSizeX + 1) >> 1), bandSizeX);
        }
      }
    }
    else
    {
      if (transformMask & 4)
      {
        #pragma omp parallel for num_threads(threadCount) schedule(guided)
        for (int32_t iD1 = 0; iD1 < bandSize[1]; ++iD1)
        {
          Wavelet_InverseTransformSliceInterleave(write + iD1 * writePitchX, writePitchXY, read + iD1 * readPitchX, readPitchXY, bandSizeX, bandSizeZ);
        }

        read = write;
        readPitchX = writePitchX;
        readPitchXY = writePitchXY;

        write = source;
        writePitchX = m_allocatedSizeX;
        writePitchXY = m_allocatedSizeXY;
      }

      if (transformMask & 2)
      {
        #pragma omp parallel for num_threads(threadCount) schedule(guided)
        for (int32_t iD2 = 0; iD2 < bandSize[2]; ++iD2)
        {
          Wavelet_InverseTransformSliceInterleave(write + iD2 * writePitchXY, writePitchX, read + iD2 * readPitchXY, readPitchX, bandSizeX, bandSizeY);
        }

        read = write;
        readPitchX = writePitchX;
        readPitchXY = writePitchXY;

        write = source;
        writePitchX = m_allocatedSizeX;
        writePitchXY = m_allocatedSizeXY;
      }

      if (transformMask & 1)
      {
        #pragma omp parallel for num_threads(threadCount) schedule(guided)
        for (int32_t iD2 = 0; iD2 < bandSize[2]; ++iD2)
        {
          for (int32_t iD1 = 0; iD1 < bandSize[1]; ++iD1)
          {
            // Wavelet transform x
            float *readDisplaced = read + (iD1 * readPitchX + iD2 * readPitchXY);

            Wavelet_InverseTransformLine(readDisplaced, bandSizeX);
            Wavelet_InterleaveLine(write + (iD1 * writePitchX + iD2 * writePitchXY), readDisplaced, readDisplaced + ((bandSizeX + 1) >> 1), bandSizeX);
          }
        }

        read = write;
        readPitchX = writePitchX;
        readPitchXY = writePitchXY;
      }

      if (read != source)
      {
        #pragma omp parallel for num_threads(threadCount) schedule(static)
        for (int32_t iD2 = 0; iD2 < bandSizeZ; ++iD2)
        {
          Wavelet_CopySlice(source + iD2 * m_allocatedSizeXY, m_allocatedSizeX, read + iD2 * readPitchXY, readPitchX, bandSizeX, bandSizeY);
        }
      }
    }
  }
}

#else

static void GetLine(float *write, float *read, int32_t length, int32_t modulo)
{
  for (int32_t i = 0; i < length; i++)
  {
    write[i] = read[i * modulo];
  }
}

static inline void TransformRotateFloatLeft(float *buffer)
{
  buffer[3] = buffer[2];
  buffer[2] = buffer[1];
  buffer[1] = buffer[0];
}

static void TransformUpdateCoarse(float *write, float *read, int32_t length, float sign, bool isOdd)
{
  float temp[4];

  int32_t extra;

  // Clip 21
  temp[3] = read[1];
  temp[2] = read[0];
  temp[1] = read[0];
  read++;

  if (isOdd)
  {
    // Clip 22
    extra = 2;
  }
  else
  {
    // Clip 21
    extra = 1;
  }

  int32_t dstLengthMiddle = length - extra;

  // Go through two times with different directions
  for (int32_t iPos = 0; iPos < dstLengthMiddle; iPos++)
  {
    // Read in new pixel
    temp[0] = *read++;

    float value = (temp[1] + temp[2]) * 9.0f - (temp[0] + temp[3]);

//    if (fabsf(rValue) > rDebug_Threshold) rValue = 0.0f;

    value *= sign;

    *write += value;
    write++;

    TransformRotateFloatLeft(temp);
  }

  if (!isOdd)
  {
    read--;
  }

  for (int32_t pos = 0; pos < extra; pos++)
  {
    // Read in new pixel
    temp[0] = *--read;

    float value = (temp[1] + temp[2]) * 9.0f - (temp[0] + temp[3]);

//    if (fabsf(rValue) > rDebug_Threshold) rValue = 0.0f;

    value *= sign;
    *write += value;
    write++;

    TransformRotateFloatLeft(temp);
  }
}

static void TransformPredictDetail(float *write, float *read, int32_t length, float sign, bool isOdd)
{
  float buffer[4];

  buffer[3] = read[0]; // New format 1
  buffer[2] = read[0];
  buffer[1] = read[1];
  read += 2;

  int nExtra;

  if (isOdd)
  {
    nExtra = 1;
  }
  else
  {
    nExtra = 2;
  }

  int32_t dstLengthMiddle = (length - nExtra);

  // Go through two times with different directions
  for (int32_t iPos = 0; iPos < dstLengthMiddle; iPos++)
  {
    // Read in new pixel
    buffer[0] = *read++;

    float value = (buffer[1] + buffer[2]) * 9.0f - (buffer[0] + buffer[3]);

//    if (fabsf(value) > rDebug_Threshold) value = 0.0f;

    value *= -sign;

    *write += value;
    write++;

    TransformRotateFloatLeft(buffer);
  }

  if (isOdd) read--;

  for (int32_t iPos = 0; iPos < nExtra; iPos++)
  {
    // Read in new pixel
    buffer[0] = *--read;

    float value = (buffer[1] + buffer[2]) * 9.0f - (buffer[0] + buffer[3]);

//    if (fabsf(value) > rDebug_Threshold) value = 0.0f;

    value *= -sign;

    *write += value;
    write++;

    TransformRotateFloatLeft(buffer);
  }
}

static void InverseTransformLine(float *readWrite, int32_t length)
{
  int32_t lengthLow = (length + 1) >> 1;
  int32_t lengthHigh = length >> 1;

  float val = (float)REAL_INVSQRT2;

  for (int32_t i = 0; i < lengthLow; i++)
  {
    readWrite[i] *= val;
  }

  TransformUpdateCoarse(readWrite, readWrite + lengthLow, lengthLow, -1.0 / 32.0f, length & 1);
  TransformPredictDetail(readWrite + lengthLow, readWrite, lengthHigh , -1.0 / 16.0f, length & 1);
}

static void WriteLineFromLowHighBand(float *write, float *read, int32_t length, int32_t modulo)
{
    int32_t lengthLowBand = (length + 1) >> 1;
    int32_t lengthHighBand = length >> 1;

  float *lowBand = read;
  float *highBand = read + lengthLowBand;

  int32_t i=0;

  for (i = 0; i < lengthHighBand; i++)
  {
    write[i * 2 * modulo] = lowBand[i];
    write[i * 2 * modulo + modulo] = highBand[i];
  }

  if (lengthLowBand != lengthHighBand)
  {
    write[i * 2 * modulo] = lowBand[i];
  }
}

void Wavelet::InverseTransform(float *source)
{
  float line[WAVELET_MAX_DIMENSION_SIZE];

  for (int i = m_transformIterations - 1; i >= 0; i--)
  {
    int32_t bandSize[3];

    char transformMask = m_transformMask[i];

    int32_t   bandSizeX = m_bandSize[i][0];
    int32_t   bandSizeY = m_bandSize[i][1];
    int32_t   bandSizeZ = m_bandSize[i][2];

    bandSize[0] = bandSizeX;
    bandSize[1] = bandSizeY;
    bandSize[2] = bandSizeZ;

    if (transformMask & 4)
    {
      for (int32_t iD1 = 0; iD1 < bandSize[1]; iD1++)
        for (int32_t iD0 = 0; iD0 < bandSize[0]; iD0++)
        {
          int32_t displacement = (iD0 + iD1 * m_allocatedSizeX);

          // Wavelet transform z
          GetLine(line, source + displacement, bandSizeZ, m_allocatedSizeXY);
          InverseTransformLine(line, bandSizeZ);
          WriteLineFromLowHighBand(source + displacement, line, bandSizeZ, m_allocatedSizeXY);
        }
    }

    if (transformMask & 2)
    {
      for (int32_t iD2 = 0; iD2 < bandSize[2]; iD2++)
        for (int32_t iD0 = 0; iD0 < bandSize[0]; iD0++)
        {
          int32_t displacement = (iD0 + iD2 * m_allocatedSizeXY);

          // Wavelet transform y
          GetLine(line, source + displacement, bandSizeY, m_allocatedSizeX);
          InverseTransformLine(line, bandSizeY);
          WriteLineFromLowHighBand(source + displacement, line, bandSizeY, m_allocatedSizeX);
        }
    }

    if (transformMask & 1)
    {
      for (int32_t iD2 = 0; iD2 < bandSize[2]; iD2++)
        for (int32_t iD1 = 0; iD1 < bandSize[1]; iD1++)
        {
          int32_t displacement = (iD1 * m_allocatedSizeX + iD2 * m_allocatedSizeXY);

          // Wavelet transform x
          GetLine(line, source + displacement, bandSizeX, 1);
          InverseTransformLine(line, bandSizeX);
          WriteLineFromLowHighBand(source + displacement, line, bandSizeX, 1);
        }
    }
  }
}

#endif

static inline bool RleDecodeOneRun(uint8_t *&rleByte, uint32_t &setBits)
{
  uint32_t value = *rleByte;
  rleByte++;

  uint8_t mask = (value >> 1) & 0x3;

  setBits = value >> 3;

  if (mask > RLE_BYTE_MASK)
  {
    setBits |= *rleByte << (8 - 3);
    rleByte++;
  }
  if (mask > RLE_2BYTE_MASK)
  {
    setBits |= *rleByte << (16 - 3);
    rleByte++;
  }
  if (mask > RLE_3BYTE_MASK)
  {
    setBits |= *rleByte << (24 - 3);
    rleByte++;
  }

  return value & 1;
}

static void RleDecode(uint8_t *rleBytes, uint32_t *bitBuffer, int32_t intsToDecode)
{
  int bitsToDecode = intsToDecode * 32;

  int writeBit = 0;

  memset(bitBuffer, 0, intsToDecode * 4);

  while (writeBit < bitsToDecode)
  {
    uint32_t setBits = 0;

    if (RleDecodeOneRun(rleBytes, setBits))
    {
      int endBit = writeBit + setBits;

      while (writeBit < endBit)
      {
        bitBuffer[writeBit / 32] |= 1 << (writeBit & 31);
        writeBit++;
      }
    }
    else
    {
      writeBit += setBits;
    }
  }
}

void Wavelet::DeCompressNoValues(float *noValue, std::vector<uint32_t> &buffer)
{
  if (!m_noValueData)
  {
    *noValue = 0.0f;
    buffer = std::vector<uint32_t>();
    return;
  }

  // Create a union for type punning that works with strict aliasing
  union { float fValue; int32_t iValue; } convert;

  convert.iValue = *m_noValueData++; *noValue = convert.fValue;

  ResizeVector(buffer, ((m_transformSizeX + 31) & ~31) / 32, m_transformSizeY, m_transformSizeZ);

  uint32_t *bitBuffer = buffer.data();

  int bitSizeX = (m_transformSizeX + 31) / 32;

  int intsToDecode = bitSizeX *m_transformSizeY *m_transformSizeZ;

  RleDecode((uint8_t *)m_noValueData, bitBuffer, intsToDecode);
}

void Wavelet::ApplyNoValues(float *source, uint32_t *bitBuffer, float noValue)
{
  int32_t u32BitRead = 0;

  for (int iDim2 = 0; iDim2 < m_transformSizeZ; iDim2++)
  {
    for (int iDim1 = 0; iDim1 < m_transformSizeY; iDim1++)
    {
      float *write = source + iDim2 * m_allocatedSizeXY + iDim1 * m_allocatedSizeX;

      for (int iDim0 = 0; iDim0 < m_transformSizeX; iDim0+=32)
      {
        uint32_t values = bitBuffer[u32BitRead++];

        int samples = m_transformSizeX - iDim0;

        if (samples > 32) samples = 32;

        for (int iSample = 0; iSample < samples; iSample++)
        {
          if (values & (1 << iSample))
          {
            write[iSample + iDim0] = noValue;
          }
        }
      }
    }
  }
}
}
