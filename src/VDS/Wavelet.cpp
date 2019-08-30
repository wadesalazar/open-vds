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

namespace OpenVDS
{

#define WAVELET_BAND_MIN_SIZE  8
#define NORMAL_BLOCK_SIZE 8
#define NORMAL_BLOCK_SIZE_FLOAT 8.0f

#define WAVELET_MAX_PIXELSETPIXEL_SIZE (8 * 8 * 8 * (sizeof(Wavelet_PixelSetPixel)))
#define WAVELET_MAX_PIXELSETCHILDREN_SIZE (7 * 7 * 7 * 7 * (sizeof(Wavelet_PixelSetChildren)))

#define ADAPTIVEWAVELET_ALIGNBUFFERSIZE 256
#define ENCODEITERATOR_MAXDECODEBITS       256

bool Wavelet_Decompress(void *compressedData, int nCompressedAdaptiveDataSize, VolumeDataChannelDescriptor::Format dataBlockFormat, const FloatRange &valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, bool isNormalize, int nDecompressLevel, bool isLossless, DataBlock &dataBlock, std::vector<uint8_t> &target, Error &error)
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

  if (!initializeDataBlock(VolumeDataChannelDescriptor::Format_R32, VolumeDataChannelDescriptor::Components_1, Dimensionality(dimensions), createSize, dataBlock, error))
    return false;
  target.resize(getAllocatedByteSize(dataBlock));
  
  Wavelet wavelet(compressedData,
    dataBlock.size[0],
    dataBlock.size[1],
    dataBlock.size[2],
    dataBlock.allocatedSize[0],
    dataBlock.allocatedSize[1],
    dataBlock.allocatedSize[2],
    dimensions,
    dataVersion);

  float threshold;
  float startThreshold;
  float waveletNoValue;

  bool isAnyNoValue;

  if (wavelet.deCompress(true, -1, -1, -1, &startThreshold, &threshold, dataBlock.format, valueRange, integerScale, integerOffset, isUseNoValue, noValue, &isAnyNoValue, &waveletNoValue, isNormalize, nDecompressLevel, isLossless, nCompressedAdaptiveDataSize, dataBlock, target, error))
    return false;
//  pcDataBlock->UnPinBuffer();
//
//  // Correct format?
//  if (pcDataBlock->GetFormat() != eDataBlockFormat)
//  {
//    bool
//      isExtendedToEdges = pcDataBlock->GetIsExtendedToEdges();
//
//    pcDataBlock->SetIsExtendedToEdges(true);
//
//    // Change format 
//    DataBlock finalDataBlock = DataBlock_c(*pcDataBlock, eDataBlockFormat, cValueRange, rIntegerScale, rIntegerOffset, isUseNoValue, rNoValue, rNoValue);
//
//    pcFinalDataBlock->SetIsExtendedToEdges(isExtendedToEdges);
//
//    delete pcDataBlock;
//
//    pcDataBlock = pcFinalDataBlock;
//  }
//

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
    char and = 0;

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

static void Wavelet_CreateTransformData(Wavelet_TransformData (&transformData)[TRANSFORM_MAX_ITERATIONS], const IntVector3 (&bandSize)[TRANSFORM_MAX_ITERATIONS + 1], int32_t *transformMask, int32_t transformIterations)
{
  int32_t previousBandSizeX = -1;
  int32_t previousBandSizeY = -1;
  int32_t previousBandSizeZ = -1;

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


        IntVector3 delta = Subtract(endWrite, startWrite);

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

          endRead = Subtract(endRead, startRead);

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

int waveletAdaptiveLL_CalculateBufferSizeNeeded(int maxPixels, int maxChildren)
{
  int size = 0;

  size += WAVELET_MAX_PIXELSETPIXEL_SIZE + ADAPTIVEWAVELET_ALIGNBUFFERSIZE;
  size += WAVELET_MAX_PIXELSETCHILDREN_SIZE + ADAPTIVEWAVELET_ALIGNBUFFERSIZE;

  size += ENCODEITERATOR_MAXDECODEBITS * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // valueEncoding
  size += ENCODEITERATOR_MAXDECODEBITS * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // valuesAtLevel
  size += ENCODEITERATOR_MAXDECODEBITS * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // valueEncodingSingle
  size += ENCODEITERATOR_MAXDECODEBITS * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // valuesAtLevelSingle
  size += maxChildren * sizeof(Wavelet_FastEncodeInsig) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // Insignificants + align to 256
  size += maxChildren * sizeof(Wavelet_FastEncodeInsig) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // Significants + align to 256
  size += (maxPixels + maxChildren) * sizeof(int) + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // positions + align to 256
  size += 32768 + ADAPTIVEWAVELET_ALIGNBUFFERSIZE; // max sizeof transform hierarchical helper data
  return size;
}

Wavelet::Wavelet(void *compressedData, int32_t transformSizeX, int32_t transformSizeY, int32_t transformSizeZ, int32_t allocatedSizeX, int32_t allocatedSizeY, int32_t allocatedSizeZ, int32_t dimensions, int32_t dataVersion)
{
  m_compressedData = (uint8_t *)compressedData;
  m_wordCompressedData = (uint32_t *)compressedData;
  m_noValueData = NULL;
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

  char transformMask = m_transformMask[0];

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

static inline void createPixelSetChildren(Wavelet_PixelSetChildren *pixelSet, uint32_t iX, uint32_t iY, uint32_t iZ,  uint32_t uTransformIteration, int32_t iSubBand)
{
  pixelSet->_nX = iX;
  pixelSet->_nY = iY;
  pixelSet->_nZ = iZ;
  pixelSet->_uTransformIteration = uTransformIteration;
  pixelSet->_iSubBand = iSubBand;
}

static inline void createPixelSetPixel(Wavelet_PixelSetPixel *pixelSet, uint32_t iX, uint32_t iY, uint32_t iZ)
{
  pixelSet->_nX = iX;
  pixelSet->_nY = iY;
  pixelSet->_nZ = iZ;
}

void Wavelet::initCoder()
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
                  createPixelSetChildren(&m_pixelSetChildren[m_pixelSetChildrenCount++], iX, iY, iZ, i, iSector);
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
          createPixelSetPixel(&m_pixelSetPixelInSignificant[m_pixelSetPixelInSignificantCount++], iD0, iD1, iD2);
      }
    }
  }
}

bool Wavelet::deCompress(bool isTransform, int32_t decompressInfo, float decompressSlice, int32_t decompressFlip, float* startThreshold, float* threshold, VolumeDataChannelDescriptor::Format dataBlockFormat, const FloatRange& valueRange, float integerScale, float integerOffset, bool isUseNoValue, float noValue, bool* isAnyNoValue, float* waveletNoValue, bool isNormalize, int decompressLevel, bool isLossless, int compressedAdaptiveDataSize, DataBlock& dataBlock, std::vector<uint8_t>& target, Error& error)
{
  assert(m_dataVersion == WAVELET_DATA_VERSION_1_4);
  initCoder();

  int *startOfCompressedData = (int *)m_compressedData;

  int32_t compressedSize = m_wordCompressedData[1];

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

  m_wordCompressedData += 6;
  
  int nDeCompressZeroSize = 0;
  
  unsigned char *pnDecompressZeroSize = NULL;
  
  pnDecompressZeroSize = (unsigned char*)m_wordCompressedData;
  nDeCompressZeroSize = *m_wordCompressedData;
  m_wordCompressedData += (nDeCompressZeroSize + 3) / 4;

  deCompressNoValuesHeader();

  float *normalField = (float *)(m_wordCompressedData);
  
  if (isNormalize)
  {
    for (int iZ = 0; iZ < m_dataBlockSizeZ; iZ+=NORMAL_BLOCK_SIZE)
    {
      for (int iY = 0; iY < m_dataBlockSizeY; iY+=NORMAL_BLOCK_SIZE)
      {
        for (int iX = 0; iX < m_dataBlockSizeX; iX+=NORMAL_BLOCK_SIZE)
        {
          m_wordCompressedData++;
        }
      }
    }
  }

  float *floatRead = (float *)m_wordCompressedData;

  *threshold = *floatRead++,            // Get Threshold
  *startThreshold = *floatRead++;       // Get StartThreshold

  m_wordCompressedData = (uint32_t *)floatRead;

  void *pxData = (void*) target.data();                                                             

  int nAdaptiveSize = *m_wordCompressedData++;

  float *_prFloatReadWriteData = (float*)pxData;

  // create transform data
  Wavelet_TransformData transformData[TRANSFORM_MAX_ITERATIONS];

  Wavelet_CreateTransformData(transformData, m_bandSize, m_transformMask, m_transformIterations);

  int cpuTempDecodeSizeNeeded = waveletAdaptiveLL_CalculateBufferSizeNeeded(m_allocatedSizeX * m_allocatedSizeY * m_allocatedSizeZ, m_allocatedHalfSizeX * m_allocatedHalfSizeY * m_allocatedHalfSizeZ);

  std::vector<uint8_t> cpuTempData;
  cpuTempData.resize(cpuTempDecodeSizeNeeded);

  abort();//TODO: REMOVE!!!!!
//  WaveletAdaptiveLL_EncodeIterator_c
//    cEncodeIterator = WaveletAdaptiveLL_CreateEncodeIterator(false, (unsigned char*)_puReadWriteCompressedData, _prFloatReadWriteData, _nAllocatedSizeX, _nAllocatedSizeY, _nAllocatedSizeZ, rThreshold, rStartThreshold, _abTransformMask, acTransformData, _nTransformIterations,
//      _pacPixelSetChildren, _nPixelSetChildren, _ppacPixelSetPixelInSignificant[0], _anPixelSetPixelInSignificant[0],
//      NULL, NULL, _nAllocatedHalfSizeX, _nAllocatedHalfSizeX * _nAllocatedHalfSizeY, (unsigned char*)cCPUTempData.GetPointer(), _nAllocatedHalfSizeX * _nAllocatedHalfSizeY * _nAllocatedHalfSizeZ, _nAllocatedSizeX * _nAllocatedSizeY * _nAllocatedSizeZ, nDecompressLevel);

//  int
//    nSize = WaveletAdaptiveLL_DecompressAdaptive(cEncodeIterator, -1, -1, -1);
//
//  if (isLossless)
//  {
//    assert(nAdaptiveSize == nSize);
//  }
//
//  Processing_c::Instance()->ReturnCPUBuffer(cCPUTempData);
//
//  InverseTransform();
//
//  // Decompres no values?
//  R32
//    rWaveletNoValue;
//
//  U32_pu
//    puNoValueBitBuffer = DeCompressNoValues(&rWaveletNoValue);
//
//  if (puNoValueBitBuffer)
//  {
//    ApplyNoValues(puNoValueBitBuffer, rWaveletNoValue);
//  }
//
//  if (pisAnyNoValue)* pisAnyNoValue = (puNoValueBitBuffer != NULL);
//  if (prWaveletNoValue)* prWaveletNoValue = rWaveletNoValue;
//
//  // DeNormalize?
//  if (isNormalize)
//  {
//    DEBUG_ASSERT(!puNoValueBitBuffer); // NoValue & Normalization is mutually exclusive (for now).
//    int
//      nNormalizeX = (_nDataBlockSizeX + NORMAL_BLOCK_SIZE - 1) / NORMAL_BLOCK_SIZE,
//      nNormalizeY = (_nDataBlockSizeY + NORMAL_BLOCK_SIZE - 1) / NORMAL_BLOCK_SIZE,
//      nNormalizeZ = (_nDataBlockSizeZ + NORMAL_BLOCK_SIZE - 1) / NORMAL_BLOCK_SIZE;
//
//    for (int iZ = 0; iZ < _nDataBlockSizeZ; iZ++)
//    {
//      for (int iY = 0; iY < _nDataBlockSizeY; iY++)
//      {
//        for (int iX = 0; iX < _nDataBlockSizeX; iX++)
//        {
//          _prFloatReadWriteData[iX + iY * _nAllocatedSizeX + iZ * _nAllocatedSizeXY] *= GetNormalizedValue(prNormalField, iX, iY, iZ, nNormalizeX, nNormalizeY, nNormalizeZ);
//        }
//      }
//    }
//  }
//
//  //  Decompress Zeroes
//  if (nDeCompressZeroSize > 4) // if equal to 4, then do nothing, no zero runs. 4 is the minimum size stored
//  {
//    DEBUG_ASSERT(_nDataVersion >= WAVELET_DATA_VERSION_1_3);
//
//    CPUBuffer_c
//      cCPUBufferZeroRun = Processing_c::Instance()->GetCPUBuffer(_nTransformSizeY * _nTransformSizeZ * sizeof(unsigned short), Processing_c::MemoryAllocationHint_TempBuffer | Processing_c::MemoryAllocationHint_PageLocked);
//
//    WaveletAdaptiveLL_DecompressZerosAlongX(pnDecompressZeroSize, (void*)_prFloatReadWriteData, 4, 0.0f, _nTransformSizeX, _nTransformSizeY, _nTransformSizeZ, _nAllocatedSizeX, _nAllocatedSizeY, _nAllocatedSizeZ, cCPUBufferZeroRun.GetPointer<unsigned char>(), 0, 0, 0);
//
//    Processing_c::Instance()->ReturnCPUBuffer(cCPUBufferZeroRun);
//  }
//
//  // Do lossless diff
//  if (isLossless)
//  {
//    DEBUG_ASSERT(eDataBlockFormat == DataBlock_c::FORMAT_R32);
//
//    _puReadWriteCompressedData += (nAdaptiveSize + 3) / 4;
//
//    int
//      nSize = WaveletAdaptiveLL_DecompressLossless((unsigned char*)_puReadWriteCompressedData, _prFloatReadWriteData, _nTransformSizeX, _nTransformSizeY, _nTransformSizeZ, _nAllocatedSizeX, _nAllocatedSizeXY);
//
//    _puReadWriteCompressedData += (nSize + 3) / 4;
//  }

  return true;
}

void Wavelet::deCompressNoValuesHeader()
{
  int size = *m_wordCompressedData;

  if (size == -1)
  {
    m_wordCompressedData++;
    m_noValueData = NULL;
    return; // no NoValues
  }

  m_noValueData = m_wordCompressedData;

  m_wordCompressedData += 2; // size and no value
  m_wordCompressedData += (size + 3) / 4;
}

}
