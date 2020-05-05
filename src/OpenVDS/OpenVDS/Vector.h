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
#include <tuple>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#endif

namespace OpenVDS
{

template<typename T, size_t N>
struct Vector
{
  T data[N];

  Vector()
    : data{}
  {}
  template<typename ...Args>
  Vector(Args... args)
    : data{args...}
  {}
  inline       T &operator[] (size_t n)        { return data[n]; }
  inline const T &operator[] (size_t n) const  { return data[n]; }
};

template<typename TYPE>
struct Vector<TYPE,2>
{
  typedef TYPE element_type;
  enum { element_count = 2 };

  union
  {
    struct
    {
      TYPE X, Y;
    };
    TYPE data[2];
  };

  Vector() : X(), Y() {}
  Vector(TYPE X, TYPE Y) : X(X), Y(Y) {}
  Vector(std::tuple<TYPE, TYPE> const& t) : X(std::get<0>(t)), Y(std::get<1>(t)) {}

  inline       TYPE &operator[] (size_t n)        { return data[n]; }
  inline const TYPE &operator[] (size_t n) const  { return data[n]; }
  inline operator std::tuple<TYPE, TYPE>() const  { return std::make_tuple(data[0], data[1]); }
};

template<typename TYPE>
struct Vector<TYPE,3>
{
  typedef TYPE element_type;
  enum { element_count = 3 };

  union
  {
    struct
    {
      TYPE X, Y, Z;
    };
    TYPE data[3];
  };

  Vector() : X(), Y(), Z() {}
  Vector(TYPE X, TYPE Y, TYPE Z) : X(X), Y(Y), Z(Z) {}
  Vector(std::tuple<TYPE, TYPE, TYPE> const& t) : X(std::get<0>(t)), Y(std::get<1>(t)), Z(std::get<2>(t)) {}

  inline       TYPE &operator[] (size_t n)              { return data[n]; }
  inline const TYPE &operator[] (size_t n) const        { return data[n]; }
  inline operator std::tuple<TYPE, TYPE, TYPE>() const  { return std::make_tuple(data[0], data[1], data[2]); }
};

template<typename TYPE>
struct Vector<TYPE, 4>
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

  Vector() : X(), Y(), Z(), T() {}
  Vector(TYPE X, TYPE Y, TYPE Z, TYPE T) : X(X), Y(Y), Z(Z), T(T) {}
  Vector(std::tuple<TYPE, TYPE, TYPE, TYPE> const& t) : X(std::get<0>(t)), Y(std::get<1>(t)), Z(std::get<2>(t)), T(std::get<3>(t)) {}

  inline       TYPE &operator[] (size_t n)                    { return data[n]; }
  inline const TYPE &operator[] (size_t n) const              { return data[n]; }
  inline operator std::tuple<TYPE, TYPE, TYPE, TYPE>() const  { return std::make_tuple(data[0], data[1], data[2], data[3]); }
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
using IntVector2 = Vector<int,2>;
using IntVector3 = Vector<int,3>;
using IntVector4 = Vector<int,4>;

using FloatVector2 = Vector<float, 2>;
using FloatVector3 = Vector<float, 3>;
using FloatVector4 = Vector<float, 4>;

using DoubleVector2 = Vector<double, 2>;
using DoubleVector3 = Vector<double, 3>;
using DoubleVector4 = Vector<double, 4>;

template<size_t N>
using IntVector = Vector<int, N>;
template<size_t N>
using FloatVector = Vector<float, N>;
template<size_t N>
using DoubleVector = Vector<double, N>;
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

