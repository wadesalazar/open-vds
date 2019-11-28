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

#include <OpenVDS/Vector.h>
#include <OpenVDS/ValueConversion.h>
#include <OpenVDS/VolumeIndexer.h>

#define NOISE_OCTAVES (5)
#define NOISE_SCALE (0.06f * 0.1f)

namespace OpenVDS
{
template<int dimensionality, int octaves>
float SimplexNoise(float *position, unsigned int randomSeed)
{
  int integerPosition[4];
  int index[4];
  float distance[4];
  float sortedDistance[4];
  int dimension;

  float skew = (sqrtf((float)dimensionality + 1) - 1.f) / (float)dimensionality;
  float unskew = (dimensionality + 1 - sqrtf((float)dimensionality + 1)) / ((float)dimensionality * (dimensionality + 1));

  float noise = 0.f;
  float scale  = 1.f;
  float amplitude = 1.f;
  float amplitudeSum  = 0.f;

  for(int octave = 0; octave < octaves; octave++)
  {
    float positionSum = position[0] * scale;
    for(dimension = 1; dimension < dimensionality; dimension++)
    {
      positionSum += position[dimension] * scale;
    }

    integerPosition[0] = (int)floorf(position[0] * scale + positionSum * skew);
    int integerPositionSum = integerPosition[0];
    for(dimension = 1; dimension < dimensionality; dimension++)
    {
      integerPosition[dimension] = (int)floorf(position[dimension] * scale + positionSum * skew);
      integerPositionSum += integerPosition[dimension];
    }

    for(dimension = 0; dimension < dimensionality; dimension++)
    {
      distance[dimension] = position[dimension] * scale - (integerPosition[dimension] - integerPositionSum * unskew);
      sortedDistance[dimension] = distance[dimension];
      index[dimension] = dimension;
    }

    for(dimension = 0; dimension < dimensionality - 1; dimension++)
    {
      if(sortedDistance[dimension] < sortedDistance[dimension + 1])
      {
        float rTemp = sortedDistance[dimension];
        int iTemp = index[dimension];
        sortedDistance[dimension] = sortedDistance[dimension + 1];
        index[dimension] = index[dimension + 1];
        sortedDistance[dimension + 1] = rTemp;
        index[dimension + 1] = iTemp;
        if(dimension > 0) dimension-=2;
      }
    }

    for(int vertex = 0; true; vertex++)
    {
      unsigned int
        random = randomSeed,
        u0 = 79555561 + octave * 322147;

      for(dimension = 0; dimension < dimensionality; dimension++)
      {
        random += integerPosition[dimension] * random;
        u0 += random;
        random ^= u0 + 8953453 * random;
      }

      float weight = 0.6f - distance[0] * distance[0]; // This vertex contributes relative to the distance of the point from this vertex
      for(dimension = 1; dimension < dimensionality; dimension++)
      {
        weight -= distance[dimension] * distance[dimension];
      }

      if(weight > 0.f)
      {
        weight *= weight;
        weight *= weight;
        // We use a gradient vector which is the midpoint of some edge of an N-dimensional cube centered on origo
        // To get this, we chose a vertex of the N-dimensional cube, and then chose one of it's coordinates to be 0
        // This gives us two ways of making every edge, but that is completely unproblematic
        int iNull = random % dimensionality;
        random /= dimensionality;
        // We loop through each component to form the dot product between the gradient vector and the distance vector
        float dot = 0.f;
        for(dimension = 0; dimension < dimensionality; dimension++)
        {
          if(dimension != iNull)
          {
            dot += distance[dimension] * ((int)(random & 2) - 1);
          }
          random >>= 1;
        }
        noise += dot * weight * amplitude;
      }

      if(vertex >= dimensionality) break;
      for(dimension = 0; dimension < dimensionality; dimension++)
      {
        distance[dimension] += unskew;
      }
      distance[index[vertex]] -= 1.f;
      integerPosition[index[vertex]]++;
    }
    amplitude *= 0.35f;
    amplitudeSum += amplitude;
    scale *= 3.5f;
  }
  return 0.5f + noise * 5.0f / amplitudeSum;
}

template <typename T, bool useNoValue>
void Noise2DKernel(T* output, VolumeIndexer2D const &outputIndexer2D, FloatVector2 const &frequency, float threshold, float noValue, unsigned int random)
{
  IntVector<2> numSamples;
  IntVector<2> localOutIndex;

  for (int i=0; i<2; i++)
  {
    numSamples[i] = outputIndexer2D.GetDataBlockNumSamples(i);
  }

  float
    valueRangeScale = outputIndexer2D.valueRangeMax - outputIndexer2D.valueRangeMin;

  QuantizingValueConverterWithNoValue<T, float, useNoValue> converter(outputIndexer2D.valueRangeMin, outputIndexer2D.valueRangeMax, 1.0f, 0.0f, noValue, noValue);

  #pragma omp parallel for firstprivate(converter)
  for (int iDim1 = 0; iDim1 < numSamples[1]; iDim1++)
  for (int iDim0 = 0; iDim0 < numSamples[0]; iDim0++)
  {
    IntVector<2>
      localOutIndex(iDim0, iDim1);

    IntVector<2>
      voxelIndex = outputIndexer2D.LocalIndexToVoxelIndex(localOutIndex);

    float
      pos[2] = { voxelIndex[0] * frequency[0] * NOISE_SCALE,
                 voxelIndex[1] * frequency[1] * NOISE_SCALE };

    float
      value = SimplexNoise<2, NOISE_OCTAVES>(pos, random);

    output[outputIndexer2D.LocalIndexToDataIndex(localOutIndex)] = converter.ConvertValue(value < threshold ? noValue : (value * valueRangeScale + outputIndexer2D.valueRangeMin));
  }
}

void
CalculateNoise2D(void* output, VolumeDataChannelDescriptor::Format eFormat, VolumeIndexer2D *pOutputIndexer, FloatVector2 frequency, float threshold, float noValue, bool useNoValue, unsigned int random)
{
  if (useNoValue)
  {
    switch(eFormat)
    {
    case VolumeDataChannelDescriptor::Format_U8:  Noise2DKernel<uint8_t, true>((uint8_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U16: Noise2DKernel<uint16_t, true>((uint16_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R32: Noise2DKernel<float, true>((float *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U32: Noise2DKernel<uint32_t, true>((uint32_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R64: Noise2DKernel<double, true>((double *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U64: Noise2DKernel<uint64_t, true>((uint64_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    }
  }
  else
  {
    switch(eFormat)
    {
    case VolumeDataChannelDescriptor::Format_U8:  Noise2DKernel<uint8_t, false>((uint8_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U16: Noise2DKernel<uint16_t, false>((uint16_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R32: Noise2DKernel<float, false>((float *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U32: Noise2DKernel<uint32_t, false>((uint32_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R64: Noise2DKernel<double, false>((double *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U64: Noise2DKernel<uint64_t, false>((uint64_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    }
  }
}

template <typename T, bool useNoValue>
void Noise3DKernel(T* output, VolumeIndexer3D const &outputIndexer3D, FloatVector3 const &frequency, float threshold, float noValue, unsigned int random)
{
  IntVector<3> numSamples;
  IntVector<3> localOutIndex;

  for (int i=0; i<3; i++)
  {
    numSamples[i] = outputIndexer3D.GetDataBlockNumSamples(i);
  }

  float
    valueRangeScale = outputIndexer3D.valueRangeMax - outputIndexer3D.valueRangeMin;

  QuantizingValueConverterWithNoValue<T, float, useNoValue> converter(outputIndexer3D.valueRangeMin, outputIndexer3D.valueRangeMax, 1.0f, 0.0f, noValue, noValue);

  #pragma omp parallel for if(numSamples[2] > 1) firstprivate(converter)
  for (int iDim2 = 0; iDim2 < numSamples[2]; iDim2++)
  #pragma omp parallel for if(numSamples[2] == 1) firstprivate(converter)
  for (int iDim1 = 0; iDim1 < numSamples[1]; iDim1++)
  for (int iDim0 = 0; iDim0 < numSamples[0]; iDim0++)
  {
    IntVector<3>
      localOutIndex(iDim0, iDim1, iDim2);

    IntVector<3>
      voxelIndex = outputIndexer3D.LocalIndexToVoxelIndex(localOutIndex);

    float
      pos[3] = { voxelIndex[0] * frequency[0] * NOISE_SCALE,
                 voxelIndex[1] * frequency[1] * NOISE_SCALE,
                 voxelIndex[2] * frequency[2] * NOISE_SCALE };

    float
      value = SimplexNoise<3, NOISE_OCTAVES>(pos, random);

    output[outputIndexer3D.LocalIndexToDataIndex(localOutIndex)] = converter.ConvertValue(value < threshold ? noValue : (value * valueRangeScale + outputIndexer3D.valueRangeMin));
  }
}

void CalculateNoise3D(void* output, VolumeDataChannelDescriptor::Format eFormat, VolumeIndexer3D *pOutputIndexer, FloatVector3 frequency, float threshold, float noValue, bool useNoValue, unsigned int random)
{
  if (useNoValue)
  {
    switch(eFormat)
    {
    case VolumeDataChannelDescriptor::Format_U8:  Noise3DKernel<uint8_t, true>((uint8_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U16: Noise3DKernel<uint16_t, true>((uint16_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R32: Noise3DKernel<float, true>((float *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U32: Noise3DKernel<uint32_t, true>((uint32_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R64: Noise3DKernel<double, true>((double *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U64: Noise3DKernel<uint64_t, true>((uint64_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    }
  }
  else
  {
    switch(eFormat)
    {
    case VolumeDataChannelDescriptor::Format_U8:  Noise3DKernel<uint8_t, false>((uint8_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U16: Noise3DKernel<uint16_t, false>((uint16_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R32: Noise3DKernel<float, false>((float *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U32: Noise3DKernel<uint32_t, false>((uint32_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R64: Noise3DKernel<double, false>((double *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U64: Noise3DKernel<uint64_t, false>((uint64_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    }
  }
}

template <typename T, bool useNoValue>
void Noise4DKernel(T* output, VolumeIndexer4D const &outputIndexer4D, FloatVector4 const &frequency, float threshold, float noValue, unsigned int random)
{
  IntVector<4> numSamples;

  for (int i=0; i<4; i++)
  {
    numSamples[i] = outputIndexer4D.GetDataBlockNumSamples(i);
  }

  float
    valueRangeScale = outputIndexer4D.valueRangeMax - outputIndexer4D.valueRangeMin;

  QuantizingValueConverterWithNoValue<T, float, useNoValue> converter(outputIndexer4D.valueRangeMin, outputIndexer4D.valueRangeMax, 1.0f, 0.0f, noValue, noValue);

  #pragma omp parallel for if(numSamples[2] > 1) firstprivate(converter)
  for (int iDim2 = 0; iDim2 < numSamples[2]; iDim2++)
  #pragma omp parallel for if(numSamples[2] == 1) firstprivate(converter)
  for (int iDim1 = 0; iDim1 < numSamples[1]; iDim1++)
  for (int iDim0 = 0; iDim0 < numSamples[0]; iDim0++)
  {
    IntVector<4>
      localOutIndex(iDim0, iDim1, iDim2, 0);

    IntVector<4>
      voxelIndex = outputIndexer4D.LocalIndexToVoxelIndex(localOutIndex);

    float
      pos[4] = { voxelIndex[0] * frequency[0] * NOISE_SCALE,
                 voxelIndex[1] * frequency[1] * NOISE_SCALE,
                 voxelIndex[2] * frequency[2] * NOISE_SCALE,
                 voxelIndex[3] * frequency[3] * NOISE_SCALE };

    float
      value = SimplexNoise<4, NOISE_OCTAVES>(pos, random);

    output[outputIndexer4D.LocalIndexToDataIndex(localOutIndex)] = converter.ConvertValue(value < threshold ? noValue : (value * valueRangeScale + outputIndexer4D.valueRangeMin));
  }
}

void CalculateNoise4D(void* output, VolumeDataChannelDescriptor::Format eFormat, VolumeIndexer4D *pOutputIndexer, FloatVector4 frequency, float threshold, float noValue, bool useNoValue, unsigned int random)
{
  if (useNoValue)
  {
    switch(eFormat)
    {
    case VolumeDataChannelDescriptor::Format_U8:  Noise4DKernel<uint8_t, true>((uint8_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U16: Noise4DKernel<uint16_t, true>((uint16_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R32: Noise4DKernel<float, true>((float *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U32: Noise4DKernel<uint32_t, true>((uint32_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R64: Noise4DKernel<double, true>((double *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U64: Noise4DKernel<uint64_t, true>((uint64_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    }
  }
  else
  {
    switch(eFormat)
    {
    case VolumeDataChannelDescriptor::Format_U8:  Noise4DKernel<uint8_t, false>((uint8_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U16: Noise4DKernel<uint16_t, false>((uint16_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R32: Noise4DKernel<float, false>((float *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U32: Noise4DKernel<uint32_t, false>((uint32_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_R64: Noise4DKernel<double, false>((double *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    case VolumeDataChannelDescriptor::Format_U64: Noise4DKernel<uint64_t, false>((uint64_t *)output, *pOutputIndexer, frequency, threshold, noValue, random); break;
    }
  }
}
}
