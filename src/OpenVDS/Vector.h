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
template<typename T>
struct Vector2
{
  typedef T element_type;
  enum { element_count = 2 };

  union
  {
    struct
    {
      T X, Y;
    };
    T data[2];
  };

  Vector2() : X(), Y() {}
  Vector2(T X, T Y) : X(X), Y(Y) {}

  inline       T &operator[] (size_t n)        { return data[n]; }
  inline const T &operator[] (size_t n) const  { return data[n]; }
};

template<typename T>
struct Vector3
{
  typedef T element_type;
  enum { element_count = 3 };

  union
  {
    struct
    {
      T X, Y, Z;
    };
    T data[3];
  };

  Vector3() : X(), Y(), Z() {}
  Vector3(T X, T Y, T Z) : X(X), Y(Y), Z(Z) {}

  inline       T &operator[] (size_t n)        { return data[n]; }
  inline const T &operator[] (size_t n) const  { return data[n]; }
};

template<typename TYPE>
struct Vector4
{
  typedef TYPE element_type;
  enum { element_count = 4 };

  union
  {
    struct
    {
      TYPE X, Y, Z, T;
    };
    TYPE data[4];
  };

  Vector4() : X(), Y(), Z(), T() {}
  Vector4(TYPE X, TYPE Y, TYPE Z, TYPE T) : X(X), Y(Y), Z(Z), T(T) {}

  inline       TYPE &operator[] (size_t n)        { return data[n]; }
  inline const TYPE &operator[] (size_t n) const  { return data[n]; }
};

/*
template<typename T, size_t N>
struct Vector
{
  T d[N];
  inline T & operator[] (size_t n) { return d[n]; }
  inline const T &operator[] (size_t n) const { return d[n]; }
};
*/
/*
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
*/
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

