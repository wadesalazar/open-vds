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

#include <Math/Range.h>

#include <cmath>
#include <string>

namespace OpenVDS
{
class VolumeDataAxisDescriptor
{
  int         m_numSamples;
  const char *m_name;
  const char *m_unit;
  FloatRange m_coordinateRange;

public:
  VolumeDataAxisDescriptor() : m_numSamples(-1), m_name(nullptr), m_unit(nullptr), m_coordinateRange({0.0f, 0.0f}) {}

  /// \param numSamples the number of samples along this axis
  /// \param pName the name of this axis
  /// \param pUnit the unit for this axis
  /// \param coordinateMin the minumum coordinate for this axis
  /// \param coordinateMax the maximum coordinate for this axis
  VolumeDataAxisDescriptor(int numSamples, const char *name, const char *unit, float coordinateMin, float coordinateMax)
    : m_numSamples(numSamples), m_name(name), m_unit(unit), m_coordinateRange({coordinateMin, coordinateMax}) {}

  VolumeDataAxisDescriptor(int numSamples, const char *name, const char *unit, const FloatRange &coordinateRange)
    : m_numSamples(numSamples), m_name(name), m_unit(unit), m_coordinateRange(coordinateRange) {}
  int getNumSamples()    const { return m_numSamples; }
  const char *getName()          const { return m_name; }
  const char *getUnit()          const { return m_unit; }
  float getCoordinateMin() const { return m_coordinateRange.min; }
  float getCoordinateMax() const { return m_coordinateRange.max; }
  const FloatRange &getCoordinateRange() const { return m_coordinateRange; }
  float getCoordinateStep() const { return (m_numSamples > 1) ? ((m_coordinateRange.max - m_coordinateRange.min) / (m_numSamples - 1)) : 0; }

  /// Convert a sample index on this axis to a coordinate value
  /// \param sampleIndex the sample index to convert
  /// \return the coordinate
  float sampleIndexToCoordinate(int sampleIndex)  { return m_coordinateRange.min + sampleIndex * getCoordinateStep(); }

  /// Convert a coordinate to a sample index (rounding to the closest index)
  /// \param coordinate the coordinate to convert
  /// \return the sample index
  int coordinateToSampleIndex(float coordinate) { return (coordinate == m_coordinateRange.min) ? 0 : (int)floorf(((coordinate - m_coordinateRange.min) / (m_coordinateRange.max - m_coordinateRange.min)) * (m_numSamples - 1) + 0.5f); }
};
}
#endif //VOLUMEDATAAXISDESCRIPTOR_H

