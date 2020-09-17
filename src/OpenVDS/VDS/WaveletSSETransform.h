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

#ifndef WAVELETSSETRANSFORM_H
#define WAVELETSSETRANSFORM_H

#define REAL_SQRT2                        1.4142135623730950488016887242097
#define REAL_INVSQRT2                     0.7071067811865475244008443621048

#ifdef ENABLE_SSE_TRANSFORM
#include <stdint.h>
#include <cmath>
#include <string.h>

#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>


namespace OpenVDS
{

inline void Wavelet_UpdateCoarseSSEInt(float* write, float* readLow, const __m128& mm0, const __m128& mm1, const __m128& mm2, const __m128& mm3, const __m128& mmNine, const __m128& mmSign, const __m128& mmVal)
{
  __m128 mmIntRounded = _mm_mul_ps(_mm_loadu_ps(readLow), mmVal);
  __m128 mmIntCeil = _mm_ceil_ps(mmIntRounded);
  __m128 mmIntFloor = _mm_floor_ps(mmIntRounded);

  // select ceil or floor based on sign bit in floating point in mmIntRounded
  mmIntRounded = _mm_blendv_ps(mmIntCeil, mmIntFloor, mmIntRounded);

  _mm_storeu_ps(
    write,
    _mm_add_ps(
      _mm_round_ps(
        _mm_mul_ps(
          _mm_add_ps(
            _mm_add_ps(mm0, mm3),
            _mm_mul_ps(
              _mm_add_ps(mm1, mm2),
              mmNine
            )
          ),
          mmSign
        ),
        _MM_FROUND_RINT),
      mmIntRounded)
  );
}

inline void Wavelet_PredictDetailSSEInt(float* write, float* prReadHigh, const __m128& mm0, const __m128& mm1, const __m128& mm2, const __m128& mm3, const __m128& mmNine, const __m128& mmSign)
{
  _mm_storeu_ps(
    write,
    _mm_add_ps(
      _mm_round_ps(
        _mm_mul_ps(
          _mm_add_ps(
            _mm_add_ps(mm0, mm3),
            _mm_mul_ps(
              _mm_add_ps(mm1, mm2),
              mmNine
            )
          ),
          mmSign),
        _MM_FROUND_RINT),
      _mm_loadu_ps(prReadHigh)
    )
  );
}

static inline void Wavelet_UpdateCoarseSSE(float* write, float* readLow, const __m128& mm0, const __m128& mm1, const __m128& mm2, const __m128& mm3, const __m128& mmNine, const __m128& mmSign, const __m128& mmVal)
{
  _mm_storeu_ps(
    write,
    _mm_add_ps(
      _mm_mul_ps(
        _mm_add_ps(
          _mm_add_ps(mm0, mm3),
          _mm_mul_ps(
            _mm_add_ps(mm1, mm2),
            mmNine
          )
        ),
        mmSign
      ),
      _mm_mul_ps(
        _mm_loadu_ps(readLow),
        mmVal
      )
    )
  );
}

inline void
Wavelet_PredictDetailSSE(float* write, float* readHigh, const __m128& mm0, const __m128& mm1, const __m128& mm2, const __m128& mm3, const __m128& mmNine, const __m128& mmSign)
{
  _mm_storeu_ps(
    write,
    _mm_add_ps(
      _mm_mul_ps(
        _mm_add_ps(
          _mm_add_ps(mm0, mm3),
          _mm_mul_ps(
            _mm_add_ps(mm1, mm2),
            mmNine
          )
        ),
        mmSign
      ),
      _mm_loadu_ps(readHigh)
    )
  );
}

// readLow and write may be equal.
template<bool isInteger>
inline void
Wavelet_TransformSlice_UpdateCoarse(float* write, int32_t nWritePitch, float* readLow, int32_t nReadLowPitch, float* readHigh, int32_t nReadHighPitch, int32_t nSliceWidth, int32_t nSliceHeight, float rSign, float rVal, bool isOdd, uint32_t integerInfo)
{
  float
    * aprLine[5];

  // Clip 21
  aprLine[0] = readHigh + nReadHighPitch;
  aprLine[1] = readHigh;
  aprLine[2] = readHigh;
  readHigh += nReadHighPitch;

  int32_t
    nExtra = isOdd ? 2 : 1,
    nDstHeightMiddle = nSliceHeight - nExtra;

  if (integerInfo & WAVELET_INTEGERINFO_ISLOSSLESSOPTIMIZED)
  {
    rVal = 1.0f;
  }

  __m128
    mmNine = _mm_set1_ps(-9.0f),
    mmSign = _mm_set1_ps(-rSign),
    mmVal = _mm_set1_ps(rVal);

  int32_t
    iLine = 0;

  for (; iLine < nDstHeightMiddle; ++iLine)
  {
    // Read new line.
    aprLine[3] = readHigh;
    readHigh += nReadHighPitch;

    int32_t
      i = 0;

    if (isInteger)
    {
      for (; i + 4 <= nSliceWidth; i += 4)
      {
        __m128
          mm0 = _mm_loadu_ps(aprLine[0] + i),
          mm1 = _mm_loadu_ps(aprLine[1] + i),
          mm2 = _mm_loadu_ps(aprLine[2] + i),
          mm3 = _mm_loadu_ps(aprLine[3] + i);

        Wavelet_UpdateCoarseSSEInt(write + i, readLow + i, mm0, mm1, mm2, mm3, mmNine, mmSign, mmVal);
      }
      for (; i < nSliceWidth; ++i)
      {
        float rInt = readLow[i] * rVal;
        if (rInt < 0.0f) rInt = floorf(rInt);
        else             rInt = ceilf(rInt);

        write[i] = rInt + rintf(rSign * (9.0f * (aprLine[1][i] + aprLine[2][i]) - (aprLine[0][i] + aprLine[3][i])));
      }
    }
    else
    {
      for (; i + 4 <= nSliceWidth; i += 4)
      {
        __m128
          mm0 = _mm_loadu_ps(aprLine[0] + i),
          mm1 = _mm_loadu_ps(aprLine[1] + i),
          mm2 = _mm_loadu_ps(aprLine[2] + i),
          mm3 = _mm_loadu_ps(aprLine[3] + i);

        Wavelet_UpdateCoarseSSE(write + i, readLow + i, mm0, mm1, mm2, mm3, mmNine, mmSign, mmVal);
      }

      for (; i < nSliceWidth; ++i)
      {
        write[i] = rVal * readLow[i] + rSign * (9.0f * (aprLine[1][i] + aprLine[2][i]) - (aprLine[0][i] + aprLine[3][i]));
      }
    }

    write += nWritePitch;
    readLow += nReadLowPitch;

    aprLine[0] = aprLine[1];
    aprLine[1] = aprLine[2];
    aprLine[2] = aprLine[3];
  }

  if (!isOdd) readHigh -= nReadHighPitch;

  for (int32_t iLine = 0; iLine < nExtra; ++iLine)
  {
    // Read new line.
    readHigh -= nReadHighPitch;
    aprLine[3] = readHigh;

    int32_t
      i = 0;

    if (isInteger)
    {
      for (; i + 4 <= nSliceWidth; i += 4)
      {
        __m128 mm0 = _mm_loadu_ps(aprLine[0] + i);
        __m128 mm1 = _mm_loadu_ps(aprLine[1] + i);
        __m128 mm2 = _mm_loadu_ps(aprLine[2] + i);
        __m128 mm3 = _mm_loadu_ps(aprLine[3] + i);

        Wavelet_UpdateCoarseSSEInt(write + i, readLow + i, mm0, mm1, mm2, mm3, mmNine, mmSign, mmVal);
      }

      for (; i < nSliceWidth; ++i)
      {
        float
          rInt = readLow[i] * rVal;

        if (rInt < 0.0f) rInt = floorf(rInt);
        else             rInt = ceilf(rInt);

        write[i] = rInt + rintf(rSign * (9.0f * (aprLine[1][i] + aprLine[2][i]) - (aprLine[0][i] + aprLine[3][i])));
      }
    }
    else
    {
      for (; i + 4 <= nSliceWidth; i += 4)
      {
        __m128
          mm0 = _mm_loadu_ps(aprLine[0] + i),
          mm1 = _mm_loadu_ps(aprLine[1] + i),
          mm2 = _mm_loadu_ps(aprLine[2] + i),
          mm3 = _mm_loadu_ps(aprLine[3] + i);

        Wavelet_UpdateCoarseSSE(write + i, readLow + i, mm0, mm1, mm2, mm3, mmNine, mmSign, mmVal);
      }

      for (; i < nSliceWidth; ++i)
      {
        write[i] = rVal * readLow[i] + rSign * (9.0f * (aprLine[1][i] + aprLine[2][i]) - (aprLine[0][i] + aprLine[3][i]));
      }
    }

    write += nWritePitch;
    readLow += nReadLowPitch;

    aprLine[0] = aprLine[1];
    aprLine[1] = aprLine[2];
    aprLine[2] = aprLine[3];
  }
}

// readHigh and write may be equal.
template<bool isInteger>
inline void
Wavelet_TransformSlice_PredictDetail(float* write, int32_t nWritePitch, float* readLow, int32_t nReadLowPitch, float* readHigh, int32_t nReadHighPitch, int32_t nSliceWidth, int32_t nSliceHeight, float rSign, bool isOdd, uint32_t integerInfo)
{
  float
    * aprLine[5];

  aprLine[0] = readLow; // New format 1
  aprLine[1] = readLow;
  aprLine[2] = readLow + nReadLowPitch;
  readLow += 2 * nReadLowPitch;

  int32_t
    nExtra = isOdd ? 1 : 2,
    nDstHeightMiddle = nSliceHeight - nExtra;

  __m128
    mmNine = _mm_set1_ps(-9.0f),
    mmSign = _mm_set1_ps(rSign);

  int32_t
    iLine = 0;

  for (; iLine < nDstHeightMiddle; ++iLine)
  {
    // Read in new pixel
    aprLine[3] = readLow;
    readLow += nReadLowPitch;

    int32_t
      i = 0;

    if (isInteger)
    {
      for (; i + 4 <= nSliceWidth; i += 4)
      {
        __m128
          mm0 = _mm_loadu_ps(aprLine[0] + i),
          mm1 = _mm_loadu_ps(aprLine[1] + i),
          mm2 = _mm_loadu_ps(aprLine[2] + i),
          mm3 = _mm_loadu_ps(aprLine[3] + i);
        Wavelet_PredictDetailSSEInt(write + i, readHigh + i, mm0, mm1, mm2, mm3, mmNine, mmSign);
      }

      for (; i < nSliceWidth; ++i)
      {
        write[i] = readHigh[i] - rintf(rSign * (9.0f * (aprLine[1][i] + aprLine[2][i]) - (aprLine[0][i] + aprLine[3][i])));
      }
    }
    else
    {
      for (; i + 4 <= nSliceWidth; i += 4)
      {
        __m128
          mm0 = _mm_loadu_ps(aprLine[0] + i),
          mm1 = _mm_loadu_ps(aprLine[1] + i),
          mm2 = _mm_loadu_ps(aprLine[2] + i),
          mm3 = _mm_loadu_ps(aprLine[3] + i);

        Wavelet_PredictDetailSSE(write + i, readHigh + i, mm0, mm1, mm2, mm3, mmNine, mmSign);
      }

      for (; i < nSliceWidth; ++i)
      {
        write[i] = readHigh[i] - rSign * (9.0f * (aprLine[1][i] + aprLine[2][i]) - (aprLine[0][i] + aprLine[3][i]));
      }
    }
    write += nWritePitch;
    readHigh += nReadHighPitch;

    aprLine[0] = aprLine[1];
    aprLine[1] = aprLine[2];
    aprLine[2] = aprLine[3];
  }

  if (isOdd) readLow -= nReadLowPitch;

  for (int32_t iLine = 0; iLine < nExtra; ++iLine)
  {
    // Read in new pixel
    readLow -= nReadLowPitch;
    aprLine[3] = readLow;

    int32_t
      i = 0;

    if (isInteger)
    {
      for (; i + 4 <= nSliceWidth; i += 4)
      {
        __m128
          mm0 = _mm_loadu_ps(aprLine[0] + i),
          mm1 = _mm_loadu_ps(aprLine[1] + i),
          mm2 = _mm_loadu_ps(aprLine[2] + i),
          mm3 = _mm_loadu_ps(aprLine[3] + i);

        Wavelet_PredictDetailSSEInt(write + i, readHigh + i, mm0, mm1, mm2, mm3, mmNine, mmSign);
      }

      for (; i < nSliceWidth; ++i)
      {
        write[i] = readHigh[i] - rintf(rSign * (9.0f * (aprLine[1][i] + aprLine[2][i]) - (aprLine[0][i] + aprLine[3][i])));
      }
    }
    else
    {
      for (; i + 4 <= nSliceWidth; i += 4)
      {
        __m128
          mm0 = _mm_loadu_ps(aprLine[0] + i),
          mm1 = _mm_loadu_ps(aprLine[1] + i),
          mm2 = _mm_loadu_ps(aprLine[2] + i),
          mm3 = _mm_loadu_ps(aprLine[3] + i);

        Wavelet_PredictDetailSSE(write + i, readHigh + i, mm0, mm1, mm2, mm3, mmNine, mmSign);
      }

      for (; i < nSliceWidth; ++i)
      {
        write[i] = readHigh[i] - rSign * (9.0f * (aprLine[1][i] + aprLine[2][i]) - (aprLine[0][i] + aprLine[3][i]));
      }
    }

    write += nWritePitch;
    readHigh += nReadHighPitch;

    aprLine[0] = aprLine[1];
    aprLine[1] = aprLine[2];
    aprLine[2] = aprLine[3];
  }
}

void
Wavelet_InverseTransformSliceInterleave(float* write, int32_t nWritePitch, float* read, int32_t nReadPitch, int32_t nSliceWidth, int32_t nSliceHeight, uint32_t integerInfo)
{
  int32_t
    nHeightLow = (nSliceHeight + 1) >> 1,
    nHeightHigh = nSliceHeight >> 1;

  if (integerInfo & WAVELET_INTEGERINFO_ISINTEGER)
  {
    Wavelet_TransformSlice_UpdateCoarse<true>(write, 2 * nWritePitch, read, nReadPitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nSliceHeight & 1, integerInfo);
    Wavelet_TransformSlice_PredictDetail<true>(write + nWritePitch, 2 * nWritePitch, write, 2 * nWritePitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightHigh, -1.0f / 16.0f, nSliceHeight & 1, integerInfo);
  }
  else
  {
    Wavelet_TransformSlice_UpdateCoarse<false>(write, 2 * nWritePitch, read, nReadPitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nSliceHeight & 1, integerInfo);
    Wavelet_TransformSlice_PredictDetail<false>(write + nWritePitch, 2 * nWritePitch, write, 2 * nWritePitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightHigh, -1.0f / 16.0f, nSliceHeight & 1, integerInfo);
  }
}

void
Wavelet_InverseTransformSlice(float* write, int32_t nWritePitch, float* read, int32_t nReadPitch, int32_t nSliceWidth, int32_t nSliceHeight, uint32_t integerInfo)
{
  int32_t
    nHeightLow = (nSliceHeight + 1) >> 1,
    nHeightHigh = nSliceHeight >> 1;

  if (integerInfo & WAVELET_INTEGERINFO_ISINTEGER)
  {
    Wavelet_TransformSlice_UpdateCoarse<true>(write, nWritePitch, read, nReadPitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nSliceHeight & 1, integerInfo);
    Wavelet_TransformSlice_PredictDetail<true>(write + nHeightLow * nWritePitch, nWritePitch, write, nWritePitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightHigh, -1.0f / 16.0f, nSliceHeight & 1, integerInfo);
  }
  else
  {
    Wavelet_TransformSlice_UpdateCoarse<false>(write, nWritePitch, read, nReadPitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nSliceHeight & 1, integerInfo);
    Wavelet_TransformSlice_PredictDetail<false>(write + nHeightLow * nWritePitch, nWritePitch, write, nWritePitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightHigh, -1.0f / 16.0f, nSliceHeight & 1, integerInfo);
  }
}

void
Wavelet_CopySlice(float* write, int32_t nWritePitch, float* read, int32_t nReadPitch, int32_t nSliceWidth, int32_t nSliceHeight)
{
  for (int32_t iLine = 0; iLine < nSliceHeight; ++iLine)
  {
    memcpy(write, read, nSliceWidth * sizeof(float));
    read += nReadPitch;
    write += nWritePitch;
  }
}

template<bool isInteger>
inline void
Wavelet_TransformLine_PredictDetail(float* write, float* read, int32_t nLength, float rSign, bool isOdd, uint32_t integerInfo)
{
  int
    nExtra = isOdd ? 1 : 2;

  int32_t
    nDstLengthMiddle = (nLength - nExtra);

  float
    arFloat[4];

  __m128
    mmNine = _mm_set1_ps(-9.0f),
    mmSign = _mm_set1_ps(rSign);

  int32_t
    i = 0;

  if (nDstLengthMiddle >= 7)
  {
    // read:  |   :   :   :   |   :   :   :   |
    //           -1   9   9  -1
    //           \___   _______/
    //                V
    // write: |   :   :   :   |

    // mmReadR2             |   :   :   :   |
    // mmReadR1         |   :   :   :   |
    // mmRead       |   :   :   :   |   :   :   :   | mmReadR4
    // mmReadL1 |   :   :   :   |   :   :   :   | mmReadR3
    // mmWrite      |   :   :   :   |
    // indices        i  i+1 i+2 i+3

    __m128
      mmRead = _mm_loadu_ps(read),
      mmReadL1 = _mm_shuffle_ps(mmRead, mmRead, _MM_SHUFFLE(2, 1, 0, 0));

    for (; i + 6 <= nDstLengthMiddle; i += 4)
    {
      //__m128
      //  mmReadR4 = _mm_loadu_ps(read + i + 4),
      //  mmReadR2 = _mm_shuffle_ps(mmRead, mmReadR4, _MM_SHUFFLE(1, 0, 3, 2)),
      //  mmReadR1 = _mm_shuffle_ps(mmRead, mmReadR2, _MM_SHUFFLE(2, 1, 2, 1)),
      //  mmReadR3 = _mm_shuffle_ps(mmReadR2, mmReadR4, _MM_SHUFFLE(2, 1, 2, 1));

      __m128
        mmReadR2 = _mm_loadu_ps(read + i + 2),
        mmReadR1 = _mm_loadu_ps(read + i + 1); // On my computer, it's faster to load multiple times from memory with offsets than to read once and shuffle around.

      if (isInteger)
      {
        Wavelet_PredictDetailSSEInt(write + i, write + i, mmReadL1, mmRead, mmReadR1, mmReadR2, mmNine, mmSign);
      }
      else
      {
        Wavelet_PredictDetailSSE(write + i, write + i, mmReadL1, mmRead, mmReadR1, mmReadR2, mmNine, mmSign);
      }

      //mmRead = mmReadR4;
      //mmReadL1 = mmReadR3;

      mmRead = _mm_loadu_ps(read + i + 4);
      mmReadL1 = _mm_loadu_ps(read + i + 3);
    }

    _mm_storeu_ps(arFloat, mmReadL1);
  }
  else
  {
    arFloat[0] = read[0]; // New format 1
    arFloat[1] = read[0];
    arFloat[2] = read[1];
  }

  write += i;
  i += 2;

  // Go through two times with different directions
  for (; i <= nDstLengthMiddle + 1; ++i)
  {
    // Read in new pixel
    arFloat[3] = read[i];


    float
      rValue = (arFloat[1] + arFloat[2]) * 9.0f -
      (arFloat[0] + arFloat[3]);

    if (isInteger)
    {
      *write++ += rintf(rValue * -rSign);
    }
    else
    {
      *write++ += (rValue * -rSign);
    }

    arFloat[0] = arFloat[1];
    arFloat[1] = arFloat[2];
    arFloat[2] = arFloat[3];
  }

  if (isOdd) --i;

  for (int32_t iPos = 0; iPos < nExtra; iPos++)
  {
    // Read in new pixel
    arFloat[3] = read[--i];

    float
      rValue = (arFloat[1] + arFloat[2]) * 9.0f -
      (arFloat[0] + arFloat[3]);

    if (isInteger)
    {
      *write++ += rintf(rValue * -rSign);
    }
    else
    {
      *write++ += (rValue * -rSign);
    }

    arFloat[0] = arFloat[1];
    arFloat[1] = arFloat[2];
    arFloat[2] = arFloat[3];
  }
}

template<bool isInteger>
inline void
Wavelet_TransformLine_UpdateCoarse(float* write, float* read, int32_t nLength, float rSign, float rVal, bool isOdd, uint32_t integerInfo)
{
  int32_t
    nExtra = isOdd ? 2 : 1;

  int32_t
    nDstLengthMiddle = nLength - nExtra;

  float
    arFloat[4];

  if (integerInfo & WAVELET_INTEGERINFO_ISLOSSLESSOPTIMIZED)
  {
    rVal = 1.0f;
  }
  __m128
    mmNine = _mm_set1_ps(-9.0f),
    mmSign = _mm_set1_ps(-rSign),
    mmVal = _mm_set1_ps(rVal);

  int32_t
    i = 0;

  if (nDstLengthMiddle >= 7)
  {
    // read:  |   :   :   :   |   :   :   :   |
    //           -1   9   9  -1
    //           \_______   ___/
    //                    V
    // write: |   :   :   :   |

    // mmReadR1             |   :   :   :   |
    // mmRead           |   :   :   :   |   :   :   :   | mmReadR4
    // mmReadL1     |   :   :   :   |
    // mmReadL2 |   :   :   :   |   :   :   :   | mmReadR2
    // mmWrite          |   :   :   :   |
    // indices            i  i+1 i+2 i+3

    __m128
      mmRead = _mm_loadu_ps(read),
      mmReadL2 = _mm_shuffle_ps(mmRead, mmRead, _MM_SHUFFLE(1, 0, 0, 1));

    for (; i + 7 <= nDstLengthMiddle; i += 4)
    {
      //__m128
      //  mmReadR4 = _mm_loadu_ps(read + i + 4),
      //  mmReadR2 = _mm_shuffle_ps(mmRead, mmReadR4, _MM_SHUFFLE(1, 0, 3, 2)),
      //  mmReadL1 = _mm_shuffle_ps(mmReadL2, mmRead, _MM_SHUFFLE(2, 1, 2, 1)),
      //  mmReadR1 = _mm_shuffle_ps(mmRead, mmReadR2, _MM_SHUFFLE(2, 1, 2, 1));

      __m128
        mmReadL1 = _mm_shuffle_ps(mmReadL2, mmRead, _MM_SHUFFLE(2, 1, 2, 1)),
        mmReadR1 = _mm_loadu_ps(read + i + 1); // On my computer, it's faster to load multiple times from memory with offsets than to read once and shuffle around.

      if (isInteger)
      {
        Wavelet_UpdateCoarseSSEInt(write + i, write + i, mmReadL2, mmReadL1, mmRead, mmReadR1, mmNine, mmSign, mmVal);
      }
      else
      {
        Wavelet_UpdateCoarseSSE(write + i, write + i, mmReadL2, mmReadL1, mmRead, mmReadR1, mmNine, mmSign, mmVal);
      }

      //mmRead = mmReadR4;
      //mmReadL2 = mmReadR2;

      mmRead = _mm_loadu_ps(read + i + 4);
      mmReadL2 = _mm_loadu_ps(read + i + 2);
    }

    _mm_storeu_ps(arFloat, mmReadL2);
  }
  else
  {
    // Clip 21
    arFloat[0] = read[1];
    arFloat[1] = read[0];
    arFloat[2] = read[0];
  }

  write += i;
  ++i;

  // Go through two times with different directions
  for (; i <= nDstLengthMiddle; ++i)
  {
    // Read in new pixel
    arFloat[3] = read[i];

    float
      rValue = (arFloat[1] + arFloat[2]) * 9.0f -
      (arFloat[0] + arFloat[3]);

    rValue *= rSign;

    if (isInteger)
    {
      float
        rInt = *write * rVal;

      if (rInt < 0.0f) rInt = floorf(rInt);
      else             rInt = ceilf(rInt);

      *write = rintf(rValue) + rInt;
    }
    else
    {
      *write *= rVal;
      *write += rValue;
    }
    ++write;

    arFloat[0] = arFloat[1];
    arFloat[1] = arFloat[2];
    arFloat[2] = arFloat[3];
  }

  if (!isOdd)
  {
    --i;
  }

  for (int32_t iPos = 0; iPos < nExtra; iPos++)
  {
    // Read in new pixel
    arFloat[3] = read[--i];

    float
      rValue = (arFloat[1] + arFloat[2]) * 9.0f -
      (arFloat[0] + arFloat[3]);

    rValue *= rSign;

    if (isInteger)
    {
      float
        rInt = *write * rVal;

      if (rInt < 0.0f) rInt = floorf(rInt);
      else             rInt = ceilf(rInt);

      *write = rintf(rValue) + rInt;
    }
    else
    {
      *write *= rVal;
      *write += rValue;
    }

    ++write;

    arFloat[0] = arFloat[1];
    arFloat[1] = arFloat[2];
    arFloat[2] = arFloat[3];
  }
}

inline void
Wavelet_InverseTransformLine(float* readWrite, int32_t nLength, uint32_t integerInfo)
{
  int32_t nLengthLow = (nLength + 1) >> 1;
  int32_t nLengthHigh = nLength >> 1;

  if (integerInfo & WAVELET_INTEGERINFO_ISINTEGER)
  {
    Wavelet_TransformLine_UpdateCoarse<true>(readWrite, readWrite + nLengthLow, nLengthLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nLength & 1, integerInfo);
    Wavelet_TransformLine_PredictDetail<true>(readWrite + nLengthLow, readWrite, nLengthHigh, -1.0f / 16.0f, nLength & 1, integerInfo);
  }
  else
  {
    Wavelet_TransformLine_UpdateCoarse<false>(readWrite, readWrite + nLengthLow, nLengthLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nLength & 1, integerInfo);
    Wavelet_TransformLine_PredictDetail<false>(readWrite + nLengthLow, readWrite, nLengthHigh, -1.0f / 16.0f, nLength & 1, integerInfo);
  }
}

inline void
Wavelet_InterleaveLine(float* write, float* readLow, float* readHigh, int32_t nLength)
{
  int32_t i = 0;

  for (; i + 8 <= nLength; i += 8)
  {
    __m128 mmLow = _mm_loadu_ps(readLow + (i >> 1));
    __m128 mmHigh = _mm_loadu_ps(readHigh + (i >> 1));

    _mm_storeu_ps(write + i, _mm_unpacklo_ps(mmLow, mmHigh));
    _mm_storeu_ps(write + i + 4, _mm_unpackhi_ps(mmLow, mmHigh));
  }

  for (; i + 2 <= nLength; i += 2)
  {
    write[i] = readLow[i >> 1];
    write[i + 1] = readHigh[i >> 1];
  }

  if (nLength & 1)
  {
    write[i] = readLow[i >> 1];
  }
}

}

#endif //IFDEF ENABLE_SSE_TRANSFORM
#endif //WAVELETSSETRANSFORM_H
