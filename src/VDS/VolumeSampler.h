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

#ifndef VOLUMESAMPLER_H
#define VOLUMESAMPLER_H

#include <OpenVDS/VolumeData.h>
#include <OpenVDS/Vector.h>

#include "ValueConversion.h"

#include <cmath>

namespace OpenVDS
{
template <typename T>
inline T readElement(const T *ptBuffer, size_t iElement) { return ptBuffer[iElement]; }
template <>
inline bool readElement<bool>(const bool *ptBuffer, size_t iElement) { return (reinterpret_cast<const unsigned char *>(ptBuffer)[iElement / 8] & (1 << (iElement % 8))) != 0; }

template <typename T>
inline int pitchScale() { return 1; }
template <>
inline int pitchScale<bool>() { return 8; }

template <typename T> struct InterpolatedRealType { typedef float type; };
template <> struct InterpolatedRealType<double>   { typedef double type; };
template <> struct InterpolatedRealType<uint64_t> { typedef double type; };
template <> struct InterpolatedRealType<uint32_t> { typedef double type; };

template <typename T, InterpolationMethod INTERPMETHOD, bool ISUSENOVALUE>
class VolumeSampler
{
private:
  int32_t m_sizeX;
  int32_t m_sizeY;
  int32_t m_sizeZ;

  int32_t m_pitchX;
  int32_t m_pitchY;
  int32_t m_pitchZ;

  float m_rangeMin;
  float m_rangeMax;

  T m_noValue;

  float m_replacementNoValue;

  typedef typename InterpolatedRealType<T>::type TREAL;

  QuantizedTypesToFloatConverter<T, ISUSENOVALUE> m_quantizedTypesToFloatConverter;

  inline int clamp(int nIndex, int nSize) const
  {
    return nIndex < 0 ? 0 : (nIndex >= nSize ? nSize - 1: nIndex);
  }

  inline void getCubicInterpolationWeights(float rT, float (&arWeights)[4]) const
  {
    float rT2 = rT * rT;
    float rT3 = rT2 * rT;

    arWeights[0] = -0.5f * rT3 +        rT2 - 0.5f * rT;
    arWeights[1] =  1.5f * rT3 - 2.5f * rT2 + 1.0f;
    arWeights[2] = -1.5f * rT3 + 2.0f * rT2 + 0.5f * rT;
    arWeights[3] =  0.5f * (rT3 - rT2);
  }

  inline TREAL wrapToClosestAngle(TREAL tData, TREAL tFixed) const
  {
    float rangeSize = m_rangeMax - m_rangeMin;

    return tData - (floor((tData - tFixed) / rangeSize + 0.5f)) * rangeSize;
  }

  inline TREAL WrapToRange(TREAL tData) const
  {
    float rangeSize = m_rangeMax - m_rangeMin;

    return tData - floor((tData - m_rangeMin) / rangeSize) * rangeSize;
  }

public:
  VolumeSampler()
    : m_sizeX(0)
    , m_sizeY(0)
    , m_sizeZ(0)
    , m_pitchX(0)
    , m_pitchY(0)
    , m_pitchZ(0)
    , m_rangeMin(0)
    , m_rangeMax(0)
    , m_noValue(0)
    , m_replacementNoValue(0)
    , m_quantizedTypesToFloatConverter()
  {}

  /// \param anSize the 6D size of the buffer to be sampled
  /// \param anPitch the pitch of the buffer to be sampled
  /// \param rangeMin the value range minimum of the data to be sampled
  /// \param rangemax the value range maximum of the data to be sampled
  /// \param noValalue the no value for the data to be sampled
  /// \param replacementNoValue the value to replace any NoValues with
    VolumeSampler(const int(&anSize)[Dimensionality_Max], const int(&anPitch)[Dimensionality_Max], float rangeMin, float rangeMax, float integerScale, float integerOffset, float noValalue, float replacementNoValue)
      : m_sizeX(anSize[0])
      , m_sizeY(anSize[1])
      , m_sizeZ(anSize[2])
      , m_pitchX(anPitch[0])
      , m_pitchY(anPitch[1] * pitchScale<T>())
      , m_pitchZ(anPitch[2] * pitchScale<T>())
      , m_rangeMin(rangeMin)
      , m_rangeMax(rangeMax)
      , m_noValue(convertNoValue<T>(noValalue))
      , m_replacementNoValue(replacementNoValue)
      , m_quantizedTypesToFloatConverter(integerScale, integerOffset, false)
  {}

  /// Sample the given buffer at the the given 3D index\n
  /// The returned TREAL type is double when T is double, int32, or int64 and float for all other types
  /// \param ptBuffer pointer to the buffer to be samples
  /// \param localIndex the local 3D index into the buffer
  /// \return a TREAL sampled using the ::InterpolationType at the given index
  TREAL sample3D(const T *ptBuffer, FloatVector3 localIndex) const
  {
    float
      rU = localIndex[0],
      rV = localIndex[1],
      rW = localIndex[2];

    int
      nNearestU = clamp((int)floorf(rU), m_sizeX),
      nNearestV = clamp((int)floorf(rV), m_sizeY),
      nNearestW = clamp((int)floorf(rW), m_sizeZ);

    int
      nU = (int)floorf(rU - 0.5f),
      nV = (int)floorf(rV - 0.5f),
      nW = (int)floorf(rW - 0.5f);

    if(INTERPMETHOD == InterpolationMethod::Triangular)// || INTERPMETHOD == Triangular_Excluding_Valuerange_Min_And_Less)
    {
      rU = rU - nU - 0.5f;
      rV = rV - nV - 0.5f;

      int
        anU[2] = { clamp(nU, m_sizeX), clamp(nU + 1, m_sizeX) },
        anV[2] = { clamp(nV, m_sizeY), clamp(nV + 1, m_sizeY) };

      T aatData[2][2] = { { readElement(ptBuffer, nNearestW * m_pitchZ + anV[0] * m_pitchY + anU[0]), readElement(ptBuffer, nNearestW * m_pitchZ + anV[0] * m_pitchY + anU[1]) },
                          { readElement(ptBuffer, nNearestW * m_pitchZ + anV[1] * m_pitchY + anU[0]), readElement(ptBuffer, nNearestW * m_pitchZ + anV[1] * m_pitchY + anU[1]) } };

      if(!ISUSENOVALUE || (isHeightValid(aatData[0][1]) && isHeightValid(aatData[1][0])))
      {
        if(rU <= 1.0f - rV && (!ISUSENOVALUE || isHeightValid(aatData[0][0])))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][1]) - rValue,
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][0]) - rValue;

          return rValue + rU * rDeltaU + rV * rDeltaV;
        }
        else if(rU >= 1.0f - rV && (!ISUSENOVALUE || isHeightValid(aatData[1][1])))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][1]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][0]) - rValue,
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][1]) - rValue;

          return rValue + (1.0f - rU) * rDeltaU + (1.0f - rV) * rDeltaV;
        }
      }
      else if(isHeightValid(aatData[0][0]) && isHeightValid(aatData[1][1]))
      {
        if(rU >= rV && isHeightValid(aatData[0][1]))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][1]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]) - rValue,
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][1]) - rValue;

          return rValue + (1.0f - rU) * rDeltaU + rV * rDeltaV;
        }
        else if(rU <= rV && isHeightValid(aatData[1][0]))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][0]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][1]) - rValue,
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]) - rValue;

          return rValue + rU * rDeltaU + (1.0f - rV) * rDeltaV;
        }
      }
      else if(isHeightValid(aatData[0][0]))
      {
        if(rU == 0.0f && rV == 0.0f)
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]);

          return rValue;
        }
        else if(rU == 0.0f && isHeightValid(aatData[1][0]))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]);

          TREAL
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][0]) - rValue;

          return rValue + rV * rDeltaV;
        }
        else if(rV == 0.0f && isHeightValid(aatData[0][1]))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][1]) - rValue;

          return rValue + rU * rDeltaU;
        }
      }

      return m_replacementNoValue;
    }
    else
    {
      T tNearest = readElement(ptBuffer, (nNearestW * m_pitchZ + nNearestV * m_pitchY) + nNearestU);

      if(ISUSENOVALUE && tNearest == m_noValue)
      {
        return m_replacementNoValue;
      }
      else if(INTERPMETHOD == InterpolationMethod::Nearest)
      {
        return (TREAL)m_quantizedTypesToFloatConverter.convertValue(tNearest);
      }
      else if(INTERPMETHOD == InterpolationMethod::Linear || INTERPMETHOD == InterpolationMethod::Angular)
      {
        TREAL rNearest = (TREAL)m_quantizedTypesToFloatConverter.convertValue(tNearest);

        rU = rU - nU - 0.5f;
        rV = rV - nV - 0.5f;
        rW = rW - nW - 0.5f;

        float
          arU[2] = { 1 - rU, rU },
          arV[2] = { 1 - rV, rV },
          arW[2] = { 1 - rW, rW };

        int
          anU[2] = { clamp(nU, m_sizeX), clamp(nU + 1, m_sizeX) },
          anV[2] = { clamp(nV, m_sizeY), clamp(nV + 1, m_sizeY) },
          anW[2] = { clamp(nW, m_sizeZ), clamp(nW + 1, m_sizeZ) };

        TREAL tDataSum = 0.0;

        float rWeightSum = 0.0f;

        for (int k = 0; k < 2; ++k)
        {
          for (int j = 0; j < 2; ++j)
          {
            for (int i = 0; i < 2; ++i)
            {
              T tData = readElement(ptBuffer, anW[k] * m_pitchZ + anV[j] * m_pitchY + anU[i]);

              if (!ISUSENOVALUE || tData != m_noValue)
              {
                float
                  rWeight = arU[i] * arV[j] * arW[k];

                TREAL
                  rData = (TREAL)m_quantizedTypesToFloatConverter.convertValue(tData);

                if (INTERPMETHOD == InterpolationMethod::Angular) rData = wrapToClosestAngle(rData, rNearest);

                tDataSum += (rData - rNearest) * rWeight;
                rWeightSum += rWeight;
              }
            }
          }
        }

        if (ISUSENOVALUE) tDataSum /= rWeightSum;
        tDataSum += rNearest;

        if (INTERPMETHOD == InterpolationMethod::Angular)
        {
          tDataSum = WrapToRange(tDataSum);
        }

        return tDataSum;
      }
      else // CUBIC
      {
        TREAL rNearest = (TREAL)m_quantizedTypesToFloatConverter.convertValue(tNearest);

        rU = rU - nU - 0.5f;
        rV = rV - nV - 0.5f;
        rW = rW - nW - 0.5f;

        float
          arU[4],
          arV[4],
          arW[4];

        getCubicInterpolationWeights(rU, arU);
        getCubicInterpolationWeights(rV, arV);
        getCubicInterpolationWeights(rW, arW);

        int
          anU[4] =
          {
            clamp(nU - 1, m_sizeX),
            clamp(nU + 0, m_sizeX),
            clamp(nU + 1, m_sizeX),
            clamp(nU + 2, m_sizeX)
          },
          anV[4] =
          {
            clamp(nV - 1, m_sizeY),
            clamp(nV + 0, m_sizeY),
            clamp(nV + 1, m_sizeY),
            clamp(nV + 2, m_sizeY)
          },
          anW[4] =
          {
            clamp(nW - 1, m_sizeZ),
            clamp(nW + 0, m_sizeZ),
            clamp(nW + 1, m_sizeZ),
            clamp(nW + 2, m_sizeZ)
          };

        T aaatData[4][4][4];

        for (int k = 0; k < 4; ++k)
        {
          for (int j = 0; j < 4; ++j)
          {
            for (int i = 0; i < 4; ++i)
            {
              aaatData[k][j][i] = readElement(ptBuffer, anW[k] * m_pitchZ + anV[j] * m_pitchY + anU[i]);
            }
          }
        }

        TREAL tSum = 0.0;
        float rWeightSum = 0.0f;

        for (int k = 0; k < 4; ++k)
        {
          int
            centerK = (k <= 1 ? 1 : 2);

          for (int j = 0; j < 4; ++j)
          {
            int
              centerJ = (j <= 1 ? 1 : 2);

            for (int i = 0; i < 4; ++i)
            {
              int
                centerI = (i <= 1 ? 1 : 2);

              if (!ISUSENOVALUE || (aaatData[k][j][i] != m_noValue && aaatData[centerK][centerJ][centerI] != m_noValue))
              {
                float
                  rWeight = arU[i] * arV[j] * arW[k];

                tSum += ((TREAL)m_quantizedTypesToFloatConverter.convertValue(aaatData[k][j][i]) - rNearest) * rWeight;
                rWeightSum += rWeight;
              }
            }
          }
        }

        if (ISUSENOVALUE) tSum /= rWeightSum;

        return tSum + rNearest;
      }
    }
  }

  bool isHeightValid(T tHeight) const
  {
    if(ISUSENOVALUE)
    {
      return tHeight != m_noValue;
    }
//    else if(INTERPMETHOD == Triangular_Excluding_Valuerange_Min_And_Less)
//    {
//      return (TREAL)m_quantizedTypesToFloatConverter.convertValue(tHeight) > m_rangeMin;
//    }
    else
    {
      return true;
    }
  }

  /// Sample the given buffer at the the given 2D index\n
  /// The returned TREAL type is double when T is double, int32, or int64 and float for all other types
  /// \param ptBuffer pointer to the buffer to be samples
  /// \param localIndex the local 2D index into the buffer
  /// \return a TREAL sampled using the ::InterpolationType at the given index
  TREAL sample2D(const T *ptBuffer, FloatVector2 localIndex) const
  {
    float
      rU = localIndex[0],
      rV = localIndex[1];

    int
      nNearestU = clamp((int)floorf(rU), m_sizeX),
      nNearestV = clamp((int)floorf(rV), m_sizeY);

    int
      nU = (int)floorf(rU - 0.5f),
      nV = (int)floorf(rV - 0.5f);

    if(INTERPMETHOD == InterpolationMethod::Triangular) // || INTERPMETHOD == Triangular_Excluding_Valuerange_Min_And_Less)
    {
      rU = rU - nU - 0.5f;
      rV = rV - nV - 0.5f;

      int
        anU[2] = { clamp(nU, m_sizeX), clamp(nU + 1, m_sizeX) },
        anV[2] = { clamp(nV, m_sizeY), clamp(nV + 1, m_sizeY) };

      T aatData[2][2] = { { readElement(ptBuffer, anV[0] * m_pitchY + anU[0]), readElement(ptBuffer, anV[0] * m_pitchY + anU[1]) },
                          { readElement(ptBuffer, anV[1] * m_pitchY + anU[0]), readElement(ptBuffer, anV[1] * m_pitchY + anU[1]) } };

      if(!ISUSENOVALUE || (isHeightValid(aatData[0][1]) && isHeightValid(aatData[1][0])))
      {
        if(rU <= 1.0f - rV && (!ISUSENOVALUE || isHeightValid(aatData[0][0])))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][1]) - rValue,
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][0]) - rValue;

          return rValue + rU * rDeltaU + rV * rDeltaV;
        }
        else if(rU >= 1.0f - rV && (!ISUSENOVALUE || isHeightValid(aatData[1][1])))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][1]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][0]) - rValue,
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][1]) - rValue;

          return rValue + (1.0f - rU) * rDeltaU + (1.0f - rV) * rDeltaV;
        }
      }
      else if(isHeightValid(aatData[0][0]) && isHeightValid(aatData[1][1]))
      {
        if(rU >= rV && isHeightValid(aatData[0][1]))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][1]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]) - rValue,
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][1]) - rValue;

          return rValue + (1.0f - rU) * rDeltaU + rV * rDeltaV;
        }
        else if(rU <= rV && isHeightValid(aatData[1][0]))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][0]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][1]) - rValue,
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]) - rValue;

          return rValue + rU * rDeltaU + (1.0f - rV) * rDeltaV;
        }
      }
      else if(isHeightValid(aatData[0][0]))
      {
        if(rU == 0.0f && rV == 0.0f)
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]);

          return rValue;
        }
        else if(rU == 0.0f && isHeightValid(aatData[1][0]))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]);

          TREAL
            rDeltaV = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[1][0]) - rValue;

          return rValue + rV * rDeltaV;
        }
        else if(rV == 0.0f && isHeightValid(aatData[0][1]))
        {
          TREAL
            rValue = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][0]);

          TREAL
            rDeltaU = (TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[0][1]) - rValue;

          return rValue + rU * rDeltaU;
        }
      }

      return m_replacementNoValue;
    }
    else
    {
      T tNearest = readElement(ptBuffer, nNearestV * m_pitchY + nNearestU);

      if(ISUSENOVALUE && tNearest == m_noValue)
      {
        return m_replacementNoValue;
      }
      else if(INTERPMETHOD == InterpolationMethod::Nearest)
      {
        return (TREAL)m_quantizedTypesToFloatConverter.convertValue(tNearest);
      }
      else if(INTERPMETHOD == InterpolationMethod::Linear|| INTERPMETHOD == InterpolationMethod::Angular)
      {
        TREAL rNearest = (TREAL)m_quantizedTypesToFloatConverter.convertValue(tNearest);

        rU = rU - nU - 0.5f;
        rV = rV - nV - 0.5f;

        float
          arU[2] = { 1 - rU, rU },
          arV[2] = { 1 - rV, rV };

        int
          anU[2] = { clamp(nU, m_sizeX), clamp(nU + 1, m_sizeX) },
          anV[2] = { clamp(nV, m_sizeY), clamp(nV + 1, m_sizeY) };

        TREAL tDataSum = 0.0;

        float rWeightSum = 0.0f;

        for (int j = 0; j < 2; ++j)
        {
          for (int i = 0; i < 2; ++i)
          {
            T
              tData = readElement(ptBuffer, anV[j] * m_pitchY + anU[i]);

            if (!ISUSENOVALUE || tData != m_noValue)
            {
              float
                rWeight = arU[i] * arV[j];

              TREAL
                rData = (TREAL)m_quantizedTypesToFloatConverter.convertValue(tData);

              if (INTERPMETHOD == InterpolationMethod::Angular) rData = wrapToClosestAngle(rData, rNearest);

              tDataSum += (rData - rNearest) * rWeight;
              rWeightSum += rWeight;
            }
          }
        }

        if (ISUSENOVALUE) tDataSum /= rWeightSum;
        tDataSum += rNearest;

        if (INTERPMETHOD == InterpolationMethod::Angular)
        {
          tDataSum = WrapToRange(tDataSum);
        }

        return tDataSum;
      }
      else // CUBIC
      {
        TREAL rNearest = (TREAL)m_quantizedTypesToFloatConverter.convertValue(tNearest);

        rU = rU - nU - 0.5f;
        rV = rV - nV - 0.5f;

        float
          arU[4],
          arV[4];

        getCubicInterpolationWeights(rU, arU);
        getCubicInterpolationWeights(rV, arV);

        int
          anU[4] =
          {
            clamp(nU - 1, m_sizeX),
            clamp(nU + 0, m_sizeX),
            clamp(nU + 1, m_sizeX),
            clamp(nU + 2, m_sizeX)
          },
          anV[4] =
          {
            clamp(nV - 1, m_sizeY),
            clamp(nV + 0, m_sizeY),
            clamp(nV + 1, m_sizeY),
            clamp(nV + 2, m_sizeY)
          };

        T aatData[4][4];

        for (int j = 0; j < 4; ++j)
        {
          for (int i = 0; i < 4; ++i)
          {
            aatData[j][i] = readElement(ptBuffer, anV[j] * m_pitchY + anU[i]);
          }
        }

        TREAL tSum = 0.0;
        float rWeightSum = 0.0f;

        for (int j = 0; j < 4; ++j)
        {
          int
            centerJ = j <= 1 ? 1 : 2;

          for (int i = 0; i < 4; ++i)
          {
            int
              centerI = i <= 1 ? 1 : 2;

            if (!ISUSENOVALUE || (aatData[j][i] != m_noValue && aatData[centerJ][centerI] != m_noValue))
            {
              float
                rWeight = arU[i] * arV[j];

              tSum += ((TREAL)m_quantizedTypesToFloatConverter.convertValue(aatData[j][i]) - rNearest) * rWeight;
              rWeightSum += rWeight;
            }
          }
        }

        if (ISUSENOVALUE) tSum /= rWeightSum;

        return tSum + rNearest;
      }
    }
  }
};

}

#endif
