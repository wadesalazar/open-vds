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


namespace OpenVDS
{
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
inline void
Wavelet_TransformSlice_UpdateCoarse(float* write, int32_t nWritePitch, float* readLow, int32_t nReadLowPitch, float* readHigh, int32_t nReadHighPitch, int32_t nSliceWidth, int32_t nSliceHeight, float rSign, float rVal, bool isOdd)
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

    write += nWritePitch;
    readLow += nReadLowPitch;

    aprLine[0] = aprLine[1];
    aprLine[1] = aprLine[2];
    aprLine[2] = aprLine[3];
  }
}

// readHigh and write may be equal.
inline void
Wavelet_TransformSlice_PredictDetail(float* write, int32_t nWritePitch, float* readLow, int32_t nReadLowPitch, float* readHigh, int32_t nReadHighPitch, int32_t nSliceWidth, int32_t nSliceHeight, float rSign, bool isOdd)
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

    write += nWritePitch;
    readHigh += nReadHighPitch;

    aprLine[0] = aprLine[1];
    aprLine[1] = aprLine[2];
    aprLine[2] = aprLine[3];
  }
}

void
Wavelet_InverseTransformSliceInterleave(float* write, int32_t nWritePitch, float* read, int32_t nReadPitch, int32_t nSliceWidth, int32_t nSliceHeight)
{
  int32_t
    nHeightLow = (nSliceHeight + 1) >> 1,
    nHeightHigh = nSliceHeight >> 1;

  Wavelet_TransformSlice_UpdateCoarse(write, 2 * nWritePitch, read, nReadPitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nSliceHeight & 1);
  Wavelet_TransformSlice_PredictDetail(write + nWritePitch, 2 * nWritePitch, write, 2 * nWritePitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightHigh, -1.0f / 16.0f, nSliceHeight & 1);
}

void
Wavelet_InverseTransformSlice(float* write, int32_t nWritePitch, float* read, int32_t nReadPitch, int32_t nSliceWidth, int32_t nSliceHeight)
{
  int32_t
    nHeightLow = (nSliceHeight + 1) >> 1,
    nHeightHigh = nSliceHeight >> 1;

  Wavelet_TransformSlice_UpdateCoarse(write, nWritePitch, read, nReadPitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nSliceHeight & 1);
  Wavelet_TransformSlice_PredictDetail(write + nHeightLow * nWritePitch, nWritePitch, write, nWritePitch, read + nHeightLow * nReadPitch, nReadPitch, nSliceWidth, nHeightHigh, -1.0f / 16.0f, nSliceHeight & 1);
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

inline void
Wavelet_TransformLine_PredictDetail(float* write, float* read, int32_t nLength, float rSign, bool isOdd)
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

      Wavelet_PredictDetailSSE(write + i, write + i, mmReadL1, mmRead, mmReadR1, mmReadR2, mmNine, mmSign);

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

    *write++ += (rValue * -rSign);

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

    *write++ += (rValue * -rSign);

    arFloat[0] = arFloat[1];
    arFloat[1] = arFloat[2];
    arFloat[2] = arFloat[3];
  }
}

inline void
Wavelet_TransformLine_UpdateCoarse(float* write, float* read, int32_t nLength, float rSign, float rVal, bool isOdd)
{
  int32_t
    nExtra = isOdd ? 2 : 1;

  int32_t
    nDstLengthMiddle = nLength - nExtra;

  float
    arFloat[4];

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

      Wavelet_UpdateCoarseSSE(write + i, write + i, mmReadL2, mmReadL1, mmRead, mmReadR1, mmNine, mmSign, mmVal);

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

    *write *= rVal;
    *write += rValue;
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

    *write *= rVal;
    *write += rValue;
    ++write;

    arFloat[0] = arFloat[1];
    arFloat[1] = arFloat[2];
    arFloat[2] = arFloat[3];
  }
}

inline void
Wavelet_InverseTransformLine(float* readWrite, int32_t nLength)
{
  int32_t nLengthLow = (nLength + 1) >> 1;
  int32_t nLengthHigh = nLength >> 1;

  Wavelet_TransformLine_UpdateCoarse(readWrite, readWrite + nLengthLow, nLengthLow, -1.0f / 32.0f, (float)REAL_INVSQRT2, nLength & 1);
  Wavelet_TransformLine_PredictDetail(readWrite + nLengthLow, readWrite, nLengthHigh, -1.0f / 16.0f, nLength & 1);
}

inline void
Wavelet_InterleaveLine(float* write, float* readLow, float* readHigh, int32_t nLength)
{
  int32_t nLengthLow = (nLength + 1) >> 1;
  int32_t nLengthHigh = nLength >> 1;

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