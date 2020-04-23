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

#ifndef VOLUMEDATAAXISDESCRIPTOR_H
#define VOLUMEDATAAXISDESCRIPTOR_H

#include <OpenVDS/Range.h>

#include <cmath>
#include <string>

namespace OpenVDS
{
/// <summary>
/// Describes the number of samples, name, unit and coordinates (annotation) of an axis (dimension) of the volume
/// </summary>
class VolumeDataAxisDescriptor
{
  int         m_numSamples;
  const char *m_name;
  const char *m_unit;
  float       m_coordinateMin;
  float       m_coordinateMax;

public:
  /// <summary>
  /// Default constructor
  /// </summary>
  VolumeDataAxisDescriptor() : m_numSamples(), m_name(), m_unit(), m_coordinateMin(), m_coordinateMax() {}

  /// <summary>
  /// Constructor
  /// </summary>
  /// <param name="numSamples">
  /// The number of samples along this axis
  /// </param>
  /// <param name="name">
  /// The name of this axis
  /// </param>
  /// <param name="unit">
  /// The unit of the coordinates of this axis
  /// </param>
  /// <param name="coordinateMin">
  /// The coordinate of the first sample of this axis
  /// </param>
  /// <param name="coordinateMax">
  /// The coordinate of the last sample of this axis
  /// </param>
  VolumeDataAxisDescriptor(int numSamples, const char *name, const char *unit, float coordinateMin, float coordinateMax)
    : m_numSamples(numSamples), m_name(name), m_unit(unit), m_coordinateMin(coordinateMin), m_coordinateMax(coordinateMax) {}

  /// <summary>
  /// Get the number of samples along this axis
  /// </summary>
  /// <returns>
  /// The number of samples along this axis
  /// </returns>
  int GetNumSamples()    const { return m_numSamples; }

  /// <summary>
  /// Get the name of this axis
  /// </summary>
  /// <returns>
  /// The name of this axis
  /// </returns>
  const char *GetName()          const { return m_name; }

  /// <summary>
  /// Get the unit of the coordinates of this axis
  /// </summary>
  /// <returns>
  /// The unit of the coordinates of this axis
  /// </returns>
  const char *GetUnit()          const { return m_unit; }

  /// <summary>
  /// Get the coordinate of the first sample of this axis
  /// </summary>
  /// <returns>
  /// The coordinate of the first sample of this axis
  /// </returns>
  float GetCoordinateMin() const { return m_coordinateMin; }

  /// <summary>
  /// Get the coordinate of the last sample of this axis
  /// </summary>
  /// <returns>
  /// The coordinate of the last sample of this axis
  /// </returns>
  float GetCoordinateMax() const { return m_coordinateMax; }

  /// <summary>
  /// Get the coordiante step size per sample
  /// </summary>
  /// <returns>
  /// The coordiante step size per sample
  /// </returns>
  float GetCoordinateStep() const { return (m_numSamples > 1) ? ((m_coordinateMax - m_coordinateMin) / (m_numSamples - 1)) : 0; }

  /// <summary>
  /// Convert a sample index on this axis to a coordinate 
  /// </summary>
  /// <param name="sampleIndex">
  /// The sample index get the coordinate of
  /// </param>
  /// <returns>
  /// The coordinate of the sample index
  /// </returns>
  float SampleIndexToCoordinate(int sampleIndex)  { return m_coordinateMin + sampleIndex * GetCoordinateStep(); }

  /// <summary>
  /// Convert a coordinate to a sample index (rounding to the closest index)
  /// </summary>
  /// <param name="coordinate">
  /// The coordinate to get the sample index of
  /// </param>
  /// <returns>
  /// The sample index of the coordinate
  /// </returns>
  int CoordinateToSampleIndex(float coordinate) { return (coordinate == m_coordinateMin) ? 0 : (int)floorf(((coordinate - m_coordinateMin) / (m_coordinateMax - m_coordinateMin)) * (m_numSamples - 1) + 0.5f); }

  /// <summary>
  /// Convert a coordinate to a sample position (to use with RequestVolumeSamples or an InterpolatingVolumeDataAccessor)
  /// </summary>
  /// <param name="coordinate">
  /// The coordinate to get the sample position of
  /// </param>
  /// <returns>
  /// The sample position of the coordinate
  /// </returns>
  float CoordinateToSamplePosition(float coordinate) { return (coordinate == m_coordinateMin) ? 0.5f : ((coordinate - m_coordinateMin) / (m_coordinateMax - m_coordinateMin)) * (m_numSamples - 1) + 0.5f; }
};
}
#endif //VOLUMEDATAAXISDESCRIPTOR_H

