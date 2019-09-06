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

#pragma once

#include <stddef.h>

namespace OpenVDS
{
template<typename T, size_t N>
struct Vector
{
  T d[N];
  inline T & operator[] (size_t n) { return d[n]; }
  inline const T &operator[] (size_t n) const { return d[n]; }
};

template<typename T, size_t N>
static inline Vector<T, N> Add(const Vector<T, N> &a, const Vector<T, N> &b)
{
  Vector<T,N> ret;
  for (int i = 0; i < N; i++)
    ret[i] = a[i] + b[i];
  return ret;
}

template<typename T, size_t N>
static inline Vector<T, N> Subtract(const Vector<T, N> &a, const Vector<T, N> &b)
{
  Vector<T,N> ret;
  for (int i = 0; i < N; i++)
    ret[i] = a[i] - b[i];
  return ret;
}

template<typename T, size_t N>
static inline Vector<T, N> Multiply(const Vector<T, N> &a, const Vector<T, N> &b)
{
  Vector<T,N> ret;
  for (int i = 0; i < N; i++)
    ret[i] = a[i] * b[i];
  return ret;
}

template<typename T, size_t N>
static inline bool operator==(const Vector<T, N> &a, const Vector<T, N> &b)
{
  for (int i = 0; i < N; i++)
    if (a[i] != b[i])
      return false;

  return true;
}

template<typename T, size_t N>
bool operator!=(const Vector<T, N> &a, const Vector<T, N> &b)
{
  for (int i = 0; i < N; i++)
    if (a[i] == b[i])
      return false;

  return true;
}

template<typename T>
using Vector2 = Vector<T, 2>;
template<typename T>
using Vector3 = Vector<T, 3>;
template<typename T>
using Vector4 = Vector<T, 4>;

using IntVector2 = Vector2<int>;
using IntVector3 = Vector3<int>;
using IntVector4 = Vector4<int>;

using FloatVector2 = Vector2<float>;
using FloatVector3 = Vector3<float>;
using FloatVector4 = Vector4<float>;

using DoubleVector2 = Vector2<double>;
using DoubleVector3 = Vector3<double>;
using DoubleVector4 = Vector4<double>;

}

