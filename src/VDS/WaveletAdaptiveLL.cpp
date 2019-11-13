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

#include "WaveletAdaptiveLL.h"

#include "MetadataManager.h"
#include <assert.h>

#if defined(_OPENMP)
#include <omp.h>
#endif

#include <algorithm>
#include <math.h>
#include <string.h>

#include "FSE/fse.h"

namespace OpenVDS
{

#if !defined(_OPENMP)
static int32_t omp_get_max_threads()
{
  return 1;
}

static int32_t omp_get_thread_num()
{
  return 1;
}
#endif


static uint8_t* AssignPtrAndIncrementOffset(int nSize, uint8_t*& workBuffer)
{
  // round off buffer to 256 bytes offset
  uintptr_t round = (ADAPTIVEWAVELET_ALIGNBUFFERSIZE - (uintptr_t)workBuffer) % ADAPTIVEWAVELET_ALIGNBUFFERSIZE;

  workBuffer += round;

  uint8_t* returnBuffer = workBuffer;

  workBuffer += nSize;

  return returnBuffer;
}

int32_t AddSubBand(uint8_t* out, Wavelet_TransformData* transformData, int32_t transformIndex, int32_t iSector, int32_t subBandOffset[][8], bool* isAllNormal)
{
  Wavelet_Compiled_SubBandInfo* band = (Wavelet_Compiled_SubBandInfo*)out;

  bool isNormal = false;

  if (!transformData[transformIndex].isNormal)
  {
    *isAllNormal = false;
  }
  else
  {
    isNormal = true;
  }

  // check if this band is actually normal!
  if (!isNormal && transformData[transformIndex].subBandInfo[iSector].childSubBand == 1)
  {
    // any extra childs?
    if (transformData[transformIndex].subBandInfo[iSector].extraChildEdge[0][0] == -1 &&
      transformData[transformIndex].subBandInfo[iSector].extraChildEdge[0][1] == -1 &&
      transformData[transformIndex].subBandInfo[iSector].extraChildEdge[0][2] == -1)
    {
      // all dividable by two?
      if (!(transformData[transformIndex].subBandInfo[iSector].legalChildEdge[0][0] & 1) &&
        !(transformData[transformIndex].subBandInfo[iSector].legalChildEdge[0][1] & 1) &&
        (!(transformData[transformIndex].subBandInfo[iSector].legalChildEdge[0][2] & 1) || transformData[transformIndex].child == 4))
      {
        isNormal = true;
      }
    }
  }

  band->isNormal = isNormal;

  if (isNormal)
  {
    // normal all the way down?
    if (transformIndex <= 1 ||
      !subBandOffset[transformIndex - 1][iSector])
    {
      return 0;
    }

    band->normalChildSubBand = subBandOffset[transformIndex - 1][iSector];
    return 4;
  }

  band->normalChildSubBand = -1;
  band->childX = transformData[transformIndex].childCount[0];
  band->childY = transformData[transformIndex].childCount[1];
  band->childZ = transformData[transformIndex].childCount[2];

  band->childSubBand = transformData[transformIndex].subBandInfo[iSector].childSubBand;
  band->posX = transformData[transformIndex].subBandInfo[iSector].pos[0];
  band->posY = transformData[transformIndex].subBandInfo[iSector].pos[1];
  band->posZ = transformData[transformIndex].subBandInfo[iSector].pos[2];

  int size = sizeof(Wavelet_Compiled_SubBandInfo) + sizeof(Wavelet_Compiled_SubBand) * (band->childSubBand - 1);

  Wavelet_Compiled_SubBand* subBand = &band->firstSubBand;

  for (int i = 0; i < band->childSubBand; i++)
  {
    subBand[i].childPosX = transformData[transformIndex].subBandInfo[iSector].childPos[i][0];
    subBand[i].childPosY = transformData[transformIndex].subBandInfo[iSector].childPos[i][1];
    subBand[i].childPosZ = transformData[transformIndex].subBandInfo[iSector].childPos[i][2];
    subBand[i].extraChildEdgeX = transformData[transformIndex].subBandInfo[iSector].extraChildEdge[i][0];
    subBand[i].extraChildEdgeY = transformData[transformIndex].subBandInfo[iSector].extraChildEdge[i][1];
    subBand[i].extraChildEdgeZ = transformData[transformIndex].subBandInfo[iSector].extraChildEdge[i][2];
    subBand[i].legalChildEdgeX = transformData[transformIndex].subBandInfo[iSector].legalChildEdge[i][0];
    subBand[i].legalChildEdgeY = transformData[transformIndex].subBandInfo[iSector].legalChildEdge[i][1];
    subBand[i].legalChildEdgeZ = transformData[transformIndex].subBandInfo[iSector].legalChildEdge[i][2];

    int iChildSector = transformData[transformIndex].subBandInfo[iSector].childSector[i];

    if (transformIndex > 1)
    {
      subBand[i].childSubBand = subBandOffset[transformIndex - 1][iChildSector];
    }
    else
    {
      subBand[i].childSubBand = 0;
    }
  }

  return size;
}

static int32_t CompileTransformData(uint8_t* compiledTransformData, int32_t* firstSubBand, const Wavelet_PixelSetChildren* pixelSetChildren, int32_t pixelSetChildrenCount, Wavelet_TransformData* transformData, int32_t transformDataCount, int32_t* transformMask, bool* isAllNormal)
{
  int subBand[32][8];

  // must offset by ! 0 so that 0 means no band!
  int writePos = 4;

  for (int iTransform = 1; iTransform < transformDataCount; iTransform++)
  {
    // go through sectors
    for (int iSector = 1; iSector < 8; iSector++)
    {
      if ((iSector & transformMask[iTransform]) == iSector)
      {
        int size = AddSubBand(compiledTransformData + writePos, transformData, iTransform, iSector, subBand, isAllNormal);

        if (size == 0)
        {
          subBand[iTransform][iSector] = 0;
        }
        else
        {
          subBand[iTransform][iSector] = writePos;
          writePos += size;
        }
      }
    }
  }

  int iLastSector = 0;

  for (int i = 0; i < pixelSetChildrenCount; i++)
  {
    if (pixelSetChildren[i].subBand != iLastSector)
    {
      iLastSector = pixelSetChildren[i].subBand;

      int iteration = pixelSetChildren[i].transformIteration;

      assert(iteration == transformDataCount - 1);

      firstSubBand[iLastSector] = subBand[iteration][iLastSector];
    }
  }


  // all normal?
  if (writePos == 4)
  {
    assert(*isAllNormal);
    writePos = 0;
  }

  return writePos;
}

WaveletAdaptiveLL_DecodeIterator WaveletAdaptiveLL_CreateDecodeIterator(uint8_t* stream, float* picture, int sizeX, int sizeY, int sizeZ,
  const float threshold, const float startThreshold, int* transformMask, Wavelet_TransformData* transformData, int transformDataCount,
  Wavelet_PixelSetChildren* pixelSetChildren, int pixelSetChildrenCount, Wavelet_PixelSetPixel* pixelSetPixelInSignificant, int pixelSetPixelInsignificantCount,
  int maxSizeX, int maxSizeXY, uint8_t* tempBufferCPU, int maxChildren, int maxPixels, int decompressLevel)
{
  int sizeXY = sizeX * sizeY;

  WaveletAdaptiveLL_DecodeIterator decodeIterator;

  decodeIterator.sizeX = sizeX;
  decodeIterator.sizeY = sizeY;
  decodeIterator.sizeZ = sizeZ;
  decodeIterator.sizeXY = sizeXY;
  decodeIterator.maxSizeX = maxSizeX;
  decodeIterator.maxSizeXY = maxSizeXY;
  decodeIterator.maxChildren = maxChildren;
  decodeIterator.maxPixel = maxPixels;

  // Allocate CPU buffers
  decodeIterator.pixelSetPixelInSignificant = (Wavelet_PixelSetPixel*)AssignPtrAndIncrementOffset(WAVELET_MAX_PIXELSETPIXEL_SIZE, tempBufferCPU);
  decodeIterator.pixelSetChildren = (Wavelet_PixelSetChildren*)AssignPtrAndIncrementOffset(WAVELET_MAX_PIXELSETCHILDREN_SIZE, tempBufferCPU);

  decodeIterator.valueEncodingMultiple = (int32_t*)AssignPtrAndIncrementOffset(DECODEITERATOR_MAXDECODEBITS * sizeof(int32_t), tempBufferCPU);
  decodeIterator.valuesAtLevelMultiple = (int32_t*)AssignPtrAndIncrementOffset(DECODEITERATOR_MAXDECODEBITS * sizeof(int32_t), tempBufferCPU);
  decodeIterator.valueEncodingSingle = (int32_t*)AssignPtrAndIncrementOffset(DECODEITERATOR_MAXDECODEBITS * sizeof(int32_t), tempBufferCPU);
  decodeIterator.valuesAtLevelSingle = (int32_t*)AssignPtrAndIncrementOffset(DECODEITERATOR_MAXDECODEBITS * sizeof(int32_t), tempBufferCPU);

  decodeIterator.insig = (Wavelet_FastDecodeInsig*)AssignPtrAndIncrementOffset(maxChildren * sizeof(Wavelet_FastDecodeInsig), tempBufferCPU);
  decodeIterator.sig = (Wavelet_FastDecodeInsig*)AssignPtrAndIncrementOffset(maxChildren * sizeof(Wavelet_FastDecodeInsig), tempBufferCPU);
  decodeIterator.pos = (int32_t*)AssignPtrAndIncrementOffset((maxPixels + maxChildren) * sizeof(int32_t), tempBufferCPU);
  decodeIterator.compiledTransformData = (uint8_t*)AssignPtrAndIncrementOffset(32768, tempBufferCPU);

  decodeIterator.stream = stream;
  decodeIterator.picture = picture;

  // Init precalc data for tree traversal
  for (int iTransform = 0; iTransform < transformDataCount; iTransform++)
  {
    decodeIterator.transformMask[iTransform] = (uint8_t)transformMask[iTransform];
  }

  for (int iTransform = 0; iTransform < transformDataCount; iTransform++)
  {
    int child = 1;

    if (transformMask[iTransform] & 1) child *= 2;
    if (transformMask[iTransform] & 2) child *= 2;
    if (transformMask[iTransform] & 4) child *= 2;

    decodeIterator.children[iTransform] = child;
  }

  int secondTransformMask = decodeIterator.transformMask[1];

  int displacement = 0;


  int andMask = 0;

  if (secondTransformMask & 1) andMask |= WAVELET_ADAPTIVELL_XYZ_AND_MASK << WAVELET_ADAPTIVELL_X_SHIFT;
  if (secondTransformMask & 2) andMask |= WAVELET_ADAPTIVELL_XYZ_AND_MASK << WAVELET_ADAPTIVELL_Y_SHIFT;
  if (secondTransformMask & 4) andMask |= WAVELET_ADAPTIVELL_XYZ_AND_MASK << WAVELET_ADAPTIVELL_Z_SHIFT;

  decodeIterator.allNormalAndMask = andMask;

  for (int iZ = 0; iZ < 2; iZ++)
  {
    for (int iY = 0; iY < 2; iY++)
    {
      for (int iX = 0; iX < 2; iX++)
      {
        int count = iX + iY * 2 + iZ * 4;

        decodeIterator.screenDisplacement[count] = iX + iY * decodeIterator.sizeX + iZ * decodeIterator.sizeXY;

        if ((count & secondTransformMask) == count)
        {
          decodeIterator.screenDisplacementAllNormal[displacement] = iX + iY * decodeIterator.sizeX + iZ * decodeIterator.sizeXY;
          decodeIterator.childDisplacementAllNormal[displacement] = (iX << WAVELET_ADAPTIVELL_X_SHIFT) +
            (iY << WAVELET_ADAPTIVELL_Y_SHIFT) +
            (iZ << WAVELET_ADAPTIVELL_Z_SHIFT);

          displacement++;
        }
      }
    }
  }

  // copy data into potentitally host allocated buffers, so can be used in stream later
  memcpy(decodeIterator.pixelSetChildren, pixelSetChildren, sizeof(Wavelet_PixelSetChildren) * pixelSetChildrenCount);
  memcpy(decodeIterator.pixelSetPixelInSignificant, pixelSetPixelInSignificant, sizeof(Wavelet_PixelSetPixel) * pixelSetPixelInsignificantCount);
  decodeIterator.pixelSetChildrenCount = pixelSetChildrenCount;
  decodeIterator.pixelSetPixelInsignificantCount = pixelSetPixelInsignificantCount;

  // Find if traversal off bit tree is "AllNormal" -> no special rules, just doubling of positions,
  // And compile tree traversal data based on wavelet tranfsform for this item
  bool isAllNormal = true;

  int compiledTransformData = CompileTransformData((uint8_t*)decodeIterator.compiledTransformData, decodeIterator.firstSubBand, pixelSetChildren, pixelSetChildrenCount, transformData, transformDataCount, transformMask, &isAllNormal);


  // Write where we find initial uncompressed values (Lowest Band)
  decodeIterator.streamFirstValues = (float*)(decodeIterator.stream + WAVELET_ADAPTIVE_LEVELS * sizeof(float));

  // if !isAllNormal, we have more ADAPTIVE Level results we need to store, so where we find first uncompressed values is displaced
  if (!isAllNormal)
  {
    decodeIterator.streamFirstValues += WAVELET_ADAPTIVE_LEVELS;
  }

  // find number of bits we are to decode
  int decodeBits = 0;

  float rT = startThreshold;

  while (rT >= threshold)
  {
    rT *= 0.5f;
    decodeBits++;
  }

  // make 0 based
  decodeBits--;

  assert(decodeBits >= 0);
  assert(decodeBits < DECODEITERATOR_MAXDECODEBITS);

  int dimensions = 3;

  if (sizeZ == 1)
  {
    dimensions = 2;

    if (sizeY == 1)
    {
      dimensions = 1;
    }
  }

  assert(decompressLevel < WAVELET_ADAPTIVE_LEVELS);

  // Write these results into encode iterator
  decodeIterator.decodeBits = decodeBits;
  decodeIterator.decompressLevel = decompressLevel;
  decodeIterator.dimensions = dimensions;
  decodeIterator.isAllNormal = isAllNormal;
  decodeIterator.threshold = threshold;
  decodeIterator.startThreshold = startThreshold;
  decodeIterator.multiple = isAllNormal ? decodeIterator.children[1] : 1 << dimensions;

  return decodeIterator;
}

template<bool isAllNormal>
static inline void InitializeInsignificantsKernel(int iElement, const WaveletAdaptiveLL_DecodeIterator& decodeIterator, const Wavelet_PixelSetChildren* pixelSetChildren)
{
  if (isAllNormal)
  {
    Wavelet_FastDecodeInsigAllNormal inSigChild(pixelSetChildren[iElement].x, pixelSetChildren[iElement].y, pixelSetChildren[iElement].z, pixelSetChildren[iElement].transformIteration);

    ((Wavelet_FastDecodeInsigAllNormal*)decodeIterator.insig)[iElement] = inSigChild;
  }
  else
  {
    Wavelet_FastDecodeInsig inSigChild(pixelSetChildren[iElement].x, pixelSetChildren[iElement].y, pixelSetChildren[iElement].z, pixelSetChildren[iElement].transformIteration, decodeIterator.firstSubBand[pixelSetChildren[iElement].subBand]);
    decodeIterator.insig[iElement] = inSigChild;
  }
}

template<bool isAllNormal>
static void InitializeInsignificants(const WaveletAdaptiveLL_DecodeIterator& decodeIterator, const Wavelet_PixelSetChildren* pixelSetChildren, int pixelSetChildrenCount)
{
  for (int iElement = 0; iElement < pixelSetChildrenCount; iElement++)
  {
    InitializeInsignificantsKernel<isAllNormal>(iElement, decodeIterator, pixelSetChildren);
  }
}

template <bool isInsigPass>
static void EvalAndSplitAllNormal(const WaveletAdaptiveLL_DecodeIterator& decodeIterator, uint8_t* puBitStream, int32_t& iCurrentElement, int32_t& nElement, const float rCurrentThreshold, int32_t& nValuesOut, int32_t& nChildrenOut)
{
  assert(iCurrentElement < nElement);

  Wavelet_FastDecodeInsigAllNormal* outputItems = isInsigPass ? (Wavelet_FastDecodeInsigAllNormal*)decodeIterator.sig : (Wavelet_FastDecodeInsigAllNormal*)decodeIterator.insig;

  const Wavelet_FastDecodeInsigAllNormal* inputItems = isInsigPass ? (Wavelet_FastDecodeInsigAllNormal*)decodeIterator.insig : (Wavelet_FastDecodeInsigAllNormal*)decodeIterator.sig;

  const int32_t child = decodeIterator.children[1];

  int32_t *outputPos = decodeIterator.pos;

  int32_t elementsThisRun = nElement - iCurrentElement;

  int32_t writeOut = iCurrentElement;

  // Originally 32. 4 is not ideal for all data sets or all hardware configurations,
  // but it is on average more performant than 32.
#define TEST_THREADS 4

  Wavelet_FastDecodeInsigAllNormal * tempOutputItem[TEST_THREADS];
  Wavelet_FastDecodeInsigAllNormal* tempInputItem[TEST_THREADS];

  int32_t* tempOutputPos[TEST_THREADS];

  int32_t tempOutputItemIndices[TEST_THREADS];
  int32_t tempInputItemIndices[TEST_THREADS];
  int32_t tempOutputPosIndices[TEST_THREADS];

  int32_t threads = elementsThisRun / 1024 + 1;
  int32_t maxThreads = omp_get_max_threads();


  if (threads > TEST_THREADS) threads = TEST_THREADS;
  if (threads > maxThreads) threads = maxThreads;

#pragma omp parallel num_threads(threads)
  {
    int32_t thread = omp_get_thread_num();

    Wavelet_FastDecodeInsigAllNormal* tempOutputItems;
    Wavelet_FastDecodeInsigAllNormal* tempInputItems;

    int32_t* tempCurrentOutputPos;

    int32_t itemStart = (thread * elementsThisRun) / threads;
    int32_t itemEnd = ((thread + 1) * elementsThisRun) / threads;

    tempOutputItems = outputItems + nChildrenOut + (isInsigPass ? itemStart : itemStart * child);
    tempInputItems = ((Wavelet_FastDecodeInsigAllNormal*)inputItems) + iCurrentElement + itemStart;
    tempCurrentOutputPos = outputPos + nValuesOut + itemStart;

    tempOutputPos[thread] = tempCurrentOutputPos;
    tempOutputItem[thread] = tempOutputItems;
    tempInputItem[thread] = tempInputItems;

    for (int iItem = itemStart; iItem < itemEnd; iItem++)
    {
      Wavelet_FastDecodeInsigAllNormal item = inputItems[iItem + iCurrentElement];

      if (puBitStream[iItem >> 3] & (1 << (iItem & 7)))
      {
        // output positions!
        if (isInsigPass)
        {
          // add me to significant list if larger than iteration one (only if insig pass)
          if (item.iterXYZ >= (2 << WAVELET_ADAPTIVELL_ITER_SHIFT)) // same as if (cItem.GetIteration() > 1)
          {
            *tempOutputItems++ = item; // nChildrenOut
          }

          *tempCurrentOutputPos++ = item.iterXYZ; // nValuesOut
        }
        else
        {
          // output insignificants
          // double position and subtract iteration
          item.iterXYZ += item.iterXYZ & decodeIterator.allNormalAndMask;
          item.iterXYZ -= 1 << WAVELET_ADAPTIVELL_ITER_SHIFT;

          for (int iChild = 0; iChild < child; iChild++)
          {
            tempOutputItems[iChild].iterXYZ = item.iterXYZ + decodeIterator.childDisplacementAllNormal[iChild]; //nChildrenOut
          }
          tempOutputItems += child;
        }
      }
      else
      {
        *tempInputItems++ = item;
      }
    }

    tempOutputPosIndices[thread] = (int)(tempCurrentOutputPos - tempOutputPos[thread]);
    tempOutputItemIndices[thread] = (int)(tempOutputItems - tempOutputItem[thread]);
    tempInputItemIndices[thread] = (int)(tempInputItems - tempInputItem[thread]);
  }

  // accumulate
  nChildrenOut += tempOutputItemIndices[0];
  writeOut += tempInputItemIndices[0];
  nValuesOut += tempOutputPosIndices[0];

  // can parallelize this aswell, make accumvalues OPTIMIZEME!
  for (int thread = 1; thread < threads; thread++)
  {
    if (tempOutputItemIndices[thread])
    {
      memmove(outputItems + nChildrenOut, tempOutputItem[thread], tempOutputItemIndices[thread] * sizeof(int));
      nChildrenOut += tempOutputItemIndices[thread];
    }

    if (tempOutputPosIndices[thread])
    {
      memmove(outputPos + nValuesOut, tempOutputPos[thread], tempOutputPosIndices[thread] * sizeof(int));
      nValuesOut += tempOutputPosIndices[thread];
    }

    if (tempInputItemIndices[thread])
    {
      memmove(((Wavelet_FastDecodeInsigAllNormal*)inputItems) + writeOut, tempInputItem[thread], tempInputItemIndices[thread] * sizeof(int));
      writeOut += tempInputItemIndices[thread];
    }
  }

  iCurrentElement = writeOut;
  nElement = writeOut;
}

template <bool isInsigPass, int dimensions>
static void EvalAndSplit(const WaveletAdaptiveLL_DecodeIterator& decodeIterator, uint8_t* puBitStream, int32_t& iCurrentElement, int32_t& nElement, const float rCurrentThreshold, int32_t& nValuesOutMultiple, int32_t& nValuesOutSingle, int32_t& nChildrenOut)
{
  assert(iCurrentElement < nElement);

  Wavelet_FastDecodeInsig* outputItems = isInsigPass ? (Wavelet_FastDecodeInsig*)decodeIterator.sig : (Wavelet_FastDecodeInsig*)decodeIterator.insig;

  Wavelet_FastDecodeInsig* inputItems = isInsigPass ? (Wavelet_FastDecodeInsig*)decodeIterator.insig : (Wavelet_FastDecodeInsig*)decodeIterator.sig;

  int32_t* outputPosMultiple = decodeIterator.pos;
  int32_t* outputPosSingle = decodeIterator.pos + decodeIterator.maxChildren;

  int32_t elementsThisRun = nElement - iCurrentElement;

  int32_t writeOut = iCurrentElement;

  int32_t idealChild = 1 << dimensions;

  for (int iItem = 0; iItem < elementsThisRun; iItem++)
  {
    int iActualElement = iItem + iCurrentElement;

    bool isExpand = false;

    if (puBitStream[iItem >> 3] & (1 << (iItem & 7)))
    {
      isExpand = true;
    }

    if (isExpand)
    {
      // add me to significant list if larger than iteration one (only if insig pass)
      if (isInsigPass)
      {
        if (inputItems[iActualElement].iteration > 1)
        {
          outputItems[nChildrenOut++] = inputItems[iActualElement];
        }
      }

      // go through and count children!
      Wavelet_FastDecodeInsig child = inputItems[iActualElement];

      // fast path?
      if (!child.subBandPos || ((Wavelet_Compiled_SubBandInfo*)(decodeIterator.compiledTransformData + child.subBandPos))->isNormal)
      {
        if (idealChild == decodeIterator.children[child.iteration])
        {
          // double position 
          child.xyz += child.xyz;

          if (isInsigPass)
          {
            // write out pos with correct position for first child
            outputPosMultiple[nValuesOutMultiple++] = child.xyz; // nValuesOut
          }
          else
          {
            //subtract iteration
            child.iteration--;

            int32_t childSubBand = 0;

            if (child.subBandPos)
            {
              Wavelet_Compiled_SubBandInfo* subBandInfo = (Wavelet_Compiled_SubBandInfo*)(decodeIterator.compiledTransformData + child.subBandPos);

              childSubBand = subBandInfo->normalChildSubBand;
            }

            for (int iChild = 0; iChild < idealChild; iChild++)
            {
              outputItems[nChildrenOut].xyz = child.xyz + decodeIterator.childDisplacementAllNormal[iChild];
              outputItems[nChildrenOut].iteration = child.iteration;
              outputItems[nChildrenOut].subBandPos = childSubBand;
              nChildrenOut++;
            }
          }
        }
        else
        {
          int32_t nX = child.GetX();
          int32_t nY = child.GetY();
          int32_t nZ = child.GetZ();

          int32_t transformMask = decodeIterator.transformMask[child.iteration];

          if (transformMask & 1) nX <<= 1;
          if (transformMask & 2) nY <<= 1;
          if (transformMask & 4) nZ <<= 1;

          int32_t relativePos = (nX + nY * decodeIterator.sizeX + nZ * decodeIterator.sizeXY);

          int32_t childSubBand = 0;

          if (child.subBandPos)
          {
            Wavelet_Compiled_SubBandInfo* subBandInfo = (Wavelet_Compiled_SubBandInfo*)(decodeIterator.compiledTransformData + child.subBandPos);

            childSubBand = subBandInfo->normalChildSubBand;
          }

          for (int iDecode = 0; iDecode <= transformMask; iDecode++)
          {
            if ((iDecode & transformMask) == iDecode)
            {
              if (isInsigPass)
              {
                int32_t position = relativePos + decodeIterator.screenDisplacement[iDecode];
                outputPosSingle[nValuesOutSingle++] = position;
              }
              else
              {
                int32_t iX = iDecode & 1;
                int32_t iY = (iDecode & 2) >> 1;
                int32_t iZ = (iDecode & 4) >> 2;

                Wavelet_FastDecodeInsig subChild(nX + iX, nY + iY, nZ + iZ, child.iteration - 1, childSubBand);

                outputItems[nChildrenOut++] = subChild;
              }
            }
          }
        }
      }
      else
      {
        int32_t nX = child.GetX();
        int32_t nY = child.GetY();
        int32_t nZ = child.GetZ();

        Wavelet_Compiled_SubBandInfo* subBandInfo = (Wavelet_Compiled_SubBandInfo*)(decodeIterator.compiledTransformData + child.subBandPos);

        int32_t childCountX = subBandInfo->childX;
        int32_t childCountY = subBandInfo->childY;
        int32_t childCountZ = subBandInfo->childZ;

        int32_t relativePosX = (nX - subBandInfo->posX) * childCountX;
        int32_t relativePosY = (nY - subBandInfo->posY) * childCountY;
        int32_t relativePosZ = (nZ - subBandInfo->posZ) * childCountZ;

        Wavelet_Compiled_SubBand* subBand = &subBandInfo->firstSubBand;

        for (int iSubBand = 0; iSubBand < subBandInfo->childSubBand; iSubBand++)
        {
          int32_t childPosX = subBand[iSubBand].childPosX + relativePosX;
          int32_t childPosY = subBand[iSubBand].childPosY + relativePosY;
          int32_t childPosZ = subBand[iSubBand].childPosZ + relativePosZ;

          int32_t legalChildPosX = subBand[iSubBand].legalChildEdgeX;
          int32_t legalChildPosY = subBand[iSubBand].legalChildEdgeY;
          int32_t legalChildPosZ = subBand[iSubBand].legalChildEdgeZ;

          int32_t childSubBand = subBand[iSubBand].childSubBand;

          int32_t actualChildCountX = childCountX;
          int32_t actualChildCountY = childCountY;
          int32_t actualChildCountZ = childCountZ;

          if (nX == subBand[iSubBand].extraChildEdgeX) actualChildCountX += 1;
          if (nY == subBand[iSubBand].extraChildEdgeY) actualChildCountY += 1;
          if (nZ == subBand[iSubBand].extraChildEdgeZ) actualChildCountZ += 1;

          if (isInsigPass)
          {
            int actualChild = actualChildCountX * actualChildCountY * actualChildCountZ;

            // valid child?
            if ((actualChild == idealChild) &&
              ((childPosX + actualChildCountX) <= legalChildPosX) &&
              ((childPosY + actualChildCountY) <= legalChildPosY) &&
              ((childPosZ + actualChildCountZ) <= legalChildPosZ))
            {
              Wavelet_FastDecodeInsigAllNormal fastChild;

              fastChild.SetXYZIter(childPosX, childPosY, childPosZ, 0);
              outputPosMultiple[nValuesOutMultiple++] = fastChild.iterXYZ;
              continue;
            }
          }

          for (int iCountZ = 0; iCountZ < actualChildCountZ; iCountZ++)
            for (int iCountY = 0; iCountY < actualChildCountY; iCountY++)
              for (int iCountX = 0; iCountX < actualChildCountX; iCountX++)
              {
                int32_t posX = iCountX + childPosX;
                int32_t posY = iCountY + childPosY;
                int32_t posZ = iCountZ + childPosZ;

                if (posX < legalChildPosX &&
                  posY < legalChildPosY &&
                  posZ < legalChildPosZ)
                {
                  if (isInsigPass)
                  {
                    int position = posX + posY * decodeIterator.sizeX + posZ * decodeIterator.sizeXY;

                    outputPosSingle[nValuesOutSingle++] = position;
                  }
                  else
                  {
                    Wavelet_FastDecodeInsig subChild(posX, posY, posZ, child.iteration - 1, childSubBand);
                    outputItems[nChildrenOut++] = subChild;
                  }
                }
              }
        }
      }
    }
    else
    {
      inputItems[writeOut++] = inputItems[iActualElement];
    }
  }

  iCurrentElement = writeOut;
  nElement = writeOut;
}

template<bool isAllNormal, int dimensions>
static int32_t DecodeTreeStructure(const WaveletAdaptiveLL_DecodeIterator& decodeIterator)
{
  const Wavelet_PixelSetChildren* pixelSetChildren = decodeIterator.pixelSetChildren;
  int32_t pixelSetChildrenCount = decodeIterator.pixelSetChildrenCount;

  int32_t pixelSetPixelInsignificantCount = decodeIterator.pixelSetPixelInsignificantCount;

  const int32_t startDecodeBits = decodeIterator.decodeBits;
  const int32_t maxDecodeLevel = decodeIterator.decompressLevel;
  const float startThreshold = decodeIterator.startThreshold;

  int32_t* valueEncodingMultiple = decodeIterator.valueEncodingMultiple;
  int32_t* valuesAtLevelMultiple = decodeIterator.valuesAtLevelMultiple;

  int32_t* valueEncodingSingle = decodeIterator.valueEncodingSingle;
  int32_t* valuesAtLevelSingle = decodeIterator.valuesAtLevelSingle;

  int32_t valuesMultiple = 0;
  int32_t valuesSingle = 0;

  int32_t iStreamPos = WAVELET_ADAPTIVE_LEVELS * sizeof(int);

  int32_t* numberOfValuesPerLevelMultiple = (int32_t*)decodeIterator.stream;
  int32_t* numberOfValuesPerLevelSingle = nullptr;


  if (!isAllNormal)
  {
    numberOfValuesPerLevelSingle = (int32_t*)(decodeIterator.stream + iStreamPos);

    // reserve space for number of valuessingle output
    iStreamPos += WAVELET_ADAPTIVE_LEVELS * sizeof(int);
  }

  // intital values(lowest bands) that we are going to copy into picture later
  iStreamPos += (pixelSetChildrenCount + pixelSetPixelInsignificantCount) * int32_t(sizeof(float));

  // number of sign values encoded/decoded,Insignifacnts and Significants and values in each pass
  int32_t signValueSetMultiple = 0;
  int32_t signValueSetSingle = 0;
  int32_t insig = 0;
  int32_t sig = 0;


  // Create first list of all Inisignificants 
  InitializeInsignificants<isAllNormal>(decodeIterator, pixelSetChildren, pixelSetChildrenCount);

  // Set number of current insignificants
  insig = pixelSetChildrenCount;

  // for each "level" we record where to encode/decode data

  float decodeBitsThreshold = startThreshold;

  int decodeBits = startDecodeBits;

  int multiple = isAllNormal ? decodeIterator.children[1] : 1 << dimensions;

  assert(maxDecodeLevel >= 0);

  // for each adaptive level
  while (decodeBits >= maxDecodeLevel)
  {
    int32_t insigCurrent = 0;
    int32_t sigCurrent = 0;

    // Anything more to do in this pass?
    while (insigCurrent < insig ||
      sigCurrent < sig)
    {
      ///////////////////////////////////////////////////////////////////////////////////////////
      // do Insig list
      if (insigCurrent < insig)
      {
        uint8_t* bitField = decodeIterator.stream + iStreamPos;

        int insigThisPass = insig - insigCurrent;

        iStreamPos += (insigThisPass + 7) / 8;

        // create count out for how many children each item should make (also mark, lowest bit, which insig should go to significant
        if (isAllNormal)
        {
          EvalAndSplitAllNormal<true>(decodeIterator, bitField, insigCurrent, insig, decodeBitsThreshold, valuesMultiple, sig);
        }
        else
        {
          EvalAndSplit<true, dimensions>(decodeIterator, bitField, insigCurrent, insig, decodeBitsThreshold, valuesMultiple, valuesSingle, sig);
        }
      }


      ///////////////////////////////////////////////////////////////////////////////////////////
      // do sig list
      ///////////////////////////////////////////////////////////////////////////////////////////
      if (sigCurrent < sig)
      {
        //printf ("Sig Threads=%d", nSigThisPass);

        uint8_t* bitField = decodeIterator.stream + iStreamPos;

        int sigThisPass = sig - sigCurrent;

        iStreamPos += (sigThisPass + 7) / 8;

        // create count out for how many children each item should make (also mark, lowest bit, which insig should go to significant
        int32_t dummy = 0;
        int32_t dummy2 = 0;

        if (isAllNormal)
        {
          EvalAndSplitAllNormal<false>(decodeIterator, bitField, sigCurrent, sig, decodeBitsThreshold, dummy, insig);
        }
        else
        {
          EvalAndSplit<false, dimensions>(decodeIterator, bitField, sigCurrent, sig, decodeBitsThreshold, dummy, dummy2, insig);
        }
      }
    }


    // values at each decode step
    valuesAtLevelMultiple[decodeBits] = valuesMultiple * multiple;
    valueEncodingMultiple[decodeBits] = iStreamPos;

    // increment stream pos for reserving bit space for later
    iStreamPos += (valuesMultiple * multiple + 7) / 8; // this is where sign bit encoding starts for all new items on this level
    iStreamPos += ((valuesMultiple - signValueSetMultiple) * multiple + 7) / 8;


    if (!isAllNormal)
    {
      // values at each decode step
      valuesAtLevelSingle[decodeBits] = valuesSingle;
      valueEncodingSingle[decodeBits] = iStreamPos;

      // increment stream pos for reserving bit space for later
      iStreamPos += (valuesSingle + 7) / 8; // this is where sign bit encoding starts for all new items on this level
      iStreamPos += ((valuesSingle - signValueSetSingle) + 7) / 8;
    }

    if (decodeBits < WAVELET_ADAPTIVE_LEVELS)
    {
      if (numberOfValuesPerLevelMultiple[decodeBits] != valuesMultiple)
      {
        ;
      }

      assert(numberOfValuesPerLevelMultiple[decodeBits] == valuesMultiple);

      if (!isAllNormal)
      {
        if (numberOfValuesPerLevelSingle[decodeBits] != valuesSingle)
        {
          ;
        }
        assert(numberOfValuesPerLevelSingle[decodeBits] == valuesSingle);
      }
    }

    // Set that we have outputted sign bits all the way up to here!
    signValueSetMultiple = (int32_t)valuesMultiple;
    signValueSetSingle = (int32_t)valuesSingle;

    // next threshold level
    decodeBits--;
    decodeBitsThreshold *= 0.5f;
  }

  return iStreamPos;
}

static inline void ReadWriteStartValueKernel(const int value, float* picture, const int sizeX, const int sizeXY, const Wavelet_PixelSetPixel* pixelSetPixelInSignificant, int pixelSetPixelInsignificantCount, const Wavelet_PixelSetChildren* pixelSetChildren, int pixelSetChildrenCount, float* rw)
{
  int pos;

  if (value >= pixelSetPixelInsignificantCount)
  {
    int read = value - pixelSetPixelInsignificantCount;
    pos = pixelSetChildren[read].x + pixelSetChildren[read].y * sizeX + pixelSetChildren[read].z * sizeXY;
  }
  else
  {
    pos = pixelSetPixelInSignificant[value].x + pixelSetPixelInSignificant[value].y * sizeX + pixelSetPixelInSignificant[value].z * sizeXY;
  }

  picture[pos] = rw[value];
}

static inline void ReadWriteStartValues(const WaveletAdaptiveLL_DecodeIterator& decodeIterator, const Wavelet_PixelSetPixel* pixelSetPixelInSignificant, int pixelSetPixelInsignificantCount, const Wavelet_PixelSetChildren* pixelSetChildren, int pixelSetChildrenCount)
{
  int totalValue = pixelSetPixelInsignificantCount + pixelSetChildrenCount;

  for (int value = 0; value < totalValue; value++)
  {
    ReadWriteStartValueKernel(value, decodeIterator.picture, decodeIterator.sizeX, decodeIterator.sizeXY, pixelSetPixelInSignificant, pixelSetPixelInsignificantCount, pixelSetChildren, pixelSetChildrenCount, decodeIterator.streamFirstValues);
  }
}

template<bool isMultiple, bool isAllNormal, int multiple>
static void DecodeAllBits(const WaveletAdaptiveLL_DecodeIterator& decodeIterator, float threshold, const int* valueEncoding, const int* valuesAtLevel, const int values, const int startDecodeBits, const int maxDecodeLevel)
{
  const float minLevelThreshold = threshold * powf(2.0f, (float)maxDecodeLevel);

  const unsigned char* stream = decodeIterator.stream;

  const int* positions = decodeIterator.pos;

  if (!isMultiple)
  {
    positions += decodeIterator.maxChildren;
  }

  // The limit value 8 seems to provide good performance across
  // different hardware configurations. This OMP block originally
  // had no thread limit, which resulted in poor performance on
  // some hardware.
  // Alternatively, we want to use fewer than 8 threads if OMP
  // max threads is less than that.
  int threads = std::min(8, omp_get_max_threads());

#pragma omp parallel for schedule(dynamic, 256) num_threads(threads)
  for (int32_t parentValue = 0; parentValue < values; parentValue++)
  {
    for (int32_t child = 0; child < multiple; child++)
    {
      int32_t value = parentValue * multiple + child;

      int32_t bit = value & 7;
      int32_t bytePos = value >> 3;

      float rvalue = threshold * 0.5f;

      float currentThreshold = minLevelThreshold;

      for (int32_t decodeBit = maxDecodeLevel; decodeBit <= startDecodeBits; decodeBit++)
      {
        int32_t decodeLevelOffset = valueEncoding[decodeBit];

        // we can read out bit!
        if (stream[decodeLevelOffset + bytePos] & (1 << bit))
        {
          rvalue += currentThreshold;
        }

        currentThreshold *= 2.0f;

        // is this level last one (check next level)?
        int valuesNextLevel = 0;

        if (decodeBit < startDecodeBits)
        {
          valuesNextLevel = valuesAtLevel[decodeBit + 1];
        }

        if (value >= valuesNextLevel)
        {
          int signValue = value - valuesNextLevel;

          bit = signValue & 7;
          bytePos = signValue >> 3;

          const uint8_t* signEncoding = stream + decodeLevelOffset + ((valuesAtLevel[decodeBit] + 7) >> 3);

          if (signEncoding[bytePos] & (1 << bit))
          {
            rvalue *= -1.0f;
          }
          break;
        }
      }

      if (isMultiple)
      {
        Wavelet_FastDecodeInsigAllNormal item;

        item.iterXYZ = positions[parentValue];

        if (isAllNormal)
        {
          item.iterXYZ += item.iterXYZ & decodeIterator.allNormalAndMask;
        }

        int currentPos = item.GetX() + item.GetY() * decodeIterator.sizeX + item.GetZ() * decodeIterator.sizeXY;

        if (isAllNormal)
        {
          decodeIterator.picture[currentPos + decodeIterator.screenDisplacementAllNormal[child]] = rvalue;
        }
        else
        {
          decodeIterator.picture[currentPos + decodeIterator.screenDisplacement[child]] = rvalue;
        }
      }
      else
      {
        decodeIterator.picture[positions[parentValue]] = rvalue;
      }
    }
  }
}

int32_t WaveletAdaptiveLL_DecompressAdaptive(WaveletAdaptiveLL_DecodeIterator decodeIterator)
{
  int streamSize = 0;

  if (decodeIterator.isAllNormal)
  {
    if (decodeIterator.dimensions == 1)
    {
      streamSize = DecodeTreeStructure<true, 1>(decodeIterator);
    }
    else if (decodeIterator.dimensions == 2)
    {
      streamSize = DecodeTreeStructure<true, 2>(decodeIterator);
    }
    else //if (decodeIterator.m_dimensions == 3)
    {
      streamSize = DecodeTreeStructure<true, 3>(decodeIterator);
    }
  }
  else
  {
    if (decodeIterator.dimensions == 1)
    {
      streamSize = DecodeTreeStructure<false, 1>(decodeIterator);
    }
    else if (decodeIterator.dimensions == 2)
    {
      streamSize = DecodeTreeStructure<false, 2>(decodeIterator);
    }
    else //if (decodeIterator.m_dimensions == 3)
    {
      streamSize = DecodeTreeStructure<false, 3>(decodeIterator);
    }
  }


  if (decodeIterator.decompressLevel > decodeIterator.decodeBits)
  {
    ;
  }

  memset(decodeIterator.picture, 0, decodeIterator.sizeX * decodeIterator.sizeY * decodeIterator.sizeZ * sizeof(float));

  ReadWriteStartValues(decodeIterator, decodeIterator.pixelSetPixelInSignificant, decodeIterator.pixelSetPixelInsignificantCount, decodeIterator.pixelSetChildren, decodeIterator.pixelSetChildrenCount);

  if (decodeIterator.decompressLevel <= decodeIterator.decodeBits)
  {
    int multiple = decodeIterator.isAllNormal ? decodeIterator.children[1] : 1 << decodeIterator.dimensions;

    int valuesMultiple = ((int*)decodeIterator.stream)[decodeIterator.decompressLevel];

    if (valuesMultiple)
    {
      if (decodeIterator.isAllNormal)
      {
        if (multiple == 1)      DecodeAllBits<true, true, 1>(decodeIterator, decodeIterator.threshold, decodeIterator.valueEncodingMultiple, decodeIterator.valuesAtLevelMultiple, valuesMultiple, decodeIterator.decodeBits, decodeIterator.decompressLevel);
        else if (multiple == 2) DecodeAllBits<true, true, 2>(decodeIterator, decodeIterator.threshold, decodeIterator.valueEncodingMultiple, decodeIterator.valuesAtLevelMultiple, valuesMultiple, decodeIterator.decodeBits, decodeIterator.decompressLevel);
        else if (multiple == 4) DecodeAllBits<true, true, 4>(decodeIterator, decodeIterator.threshold, decodeIterator.valueEncodingMultiple, decodeIterator.valuesAtLevelMultiple, valuesMultiple, decodeIterator.decodeBits, decodeIterator.decompressLevel);
        else if (multiple == 8) DecodeAllBits<true, true, 8>(decodeIterator, decodeIterator.threshold, decodeIterator.valueEncodingMultiple, decodeIterator.valuesAtLevelMultiple, valuesMultiple, decodeIterator.decodeBits, decodeIterator.decompressLevel);
      }
      else
      {
        // multiple can't be one if using 1 << nDimensions
        if (multiple == 2)      DecodeAllBits<true, false, 2>(decodeIterator, decodeIterator.threshold, decodeIterator.valueEncodingMultiple, decodeIterator.valuesAtLevelMultiple, valuesMultiple, decodeIterator.decodeBits, decodeIterator.decompressLevel);
        else if (multiple == 4) DecodeAllBits<true, false, 4>(decodeIterator, decodeIterator.threshold, decodeIterator.valueEncodingMultiple, decodeIterator.valuesAtLevelMultiple, valuesMultiple, decodeIterator.decodeBits, decodeIterator.decompressLevel);
        else if (multiple == 8) DecodeAllBits<true, false, 8>(decodeIterator, decodeIterator.threshold, decodeIterator.valueEncodingMultiple, decodeIterator.valuesAtLevelMultiple, valuesMultiple, decodeIterator.decodeBits, decodeIterator.decompressLevel);
      }
    }

    if (!decodeIterator.isAllNormal)
    {
      int valuesSingle = ((int32_t*)decodeIterator.stream)[decodeIterator.decompressLevel + WAVELET_ADAPTIVE_LEVELS];

      if (valuesSingle)
      {
        DecodeAllBits<false, false, 1>(decodeIterator, decodeIterator.threshold, decodeIterator.valueEncodingSingle, decodeIterator.valuesAtLevelSingle, valuesSingle, decodeIterator.decodeBits, decodeIterator.decompressLevel);
      }
    }
  }
  return streamSize;
}

#define ADAPTIVEWAVELET_LOSSLESS_CHANNEL_ZERO            0
#define ADAPTIVEWAVELET_LOSSLESS_CHANNEL_UNCOMPRESSED    -1
int32_t WaveletAdaptiveLL_DecompressLossless(uint8_t *in, float *pic, int32_t sizeX, int32_t sizeY, int32_t sizeZ, int32_t allocatedSizeX, int32_t allocatedSizeXY)
{
  unsigned char *start = in;

  unsigned char *count[4];

  int
    nPixels = sizeX * sizeY * sizeZ;

  count[0] = (unsigned char *)malloc(sizeX * sizeY * sizeZ * sizeof(char));
  count[1] = (unsigned char *)malloc(sizeX * sizeY * sizeZ * sizeof(char));
  count[2] = (unsigned char *)malloc(sizeX * sizeY * sizeZ * sizeof(char));
  count[3] = (unsigned char *)malloc(sizeX * sizeY * sizeZ * sizeof(char));

  uint8_t *compressedData[4];


  in += sizeof(int);

  int *readSize = (int *)in;

  in += sizeof(int) * 4;

  int totalSize = 0;

  for (int i = 0; i < 4; i++)
  {
    compressedData[i] = in + totalSize;
    int size = readSize[i];

    if (size == ADAPTIVEWAVELET_LOSSLESS_CHANNEL_UNCOMPRESSED)
    {
      size = nPixels;
    }

    totalSize += size;
  }

#pragma omp parallel for num_threads(4)
  for (int i = 0; i < 4; i++)
  {
    if (readSize[i] == ADAPTIVEWAVELET_LOSSLESS_CHANNEL_UNCOMPRESSED)
    {
      memcpy(count[i], compressedData[i], nPixels);
    }
    else if (readSize[i] > 1)
    {
      FSE_decompress(count[i], nPixels, compressedData[i], readSize[i]);
    }
    else
    {
      assert(readSize[i] == ADAPTIVEWAVELET_LOSSLESS_CHANNEL_ZERO);
      memset(count[i], 0, nPixels);
    }
  }
  
  int *intPic = (int *)pic;

  
  for (int iZ = 0; iZ < sizeZ; iZ++)
  {
#pragma omp parallel for schedule(static)
    for (int iY = 0; iY < sizeY; iY++)
    {
      int
        iWrite = iY * sizeX + iZ * sizeX * sizeY;

      for (int iX = 0; iX < sizeX; iX++)
      {
        int
          iPixel = iX + iY * allocatedSizeX + iZ * allocatedSizeXY;

        int
          uValue = count[0][iWrite] |
                  (count[1][iWrite] << 8) |
                  (count[2][iWrite] << 16) |
                  (count[3][iWrite] << 24);

        iWrite++;

        intPic[iPixel] ^= uValue;
      }
    }
  }
  
  free(count[0]);
  free(count[1]);
  free(count[2]);
  free(count[3]);

  return (int)(in + totalSize - start);
}

}
