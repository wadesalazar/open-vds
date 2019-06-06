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

#include <cmath>

namespace OpenVDS
{
class VolumeDataAxisDescriptor
{
  int         numSamples;
  std::string pName;
  std::string pUnit;
  float       coordinateMin;
  float       coordinateMax;

public:
  VolumeDataAxisDescriptor() : numSamples(-1), pName(0), pUnit(0), coordinateMin(0.0f), coordinateMax(0.0f) {}

  /// \param numSamples the number of samples along this axis
  /// \param pName the name of this axis
  /// \param pUnit the unit for this axis
  /// \param coordinateMin the minumum coordinate for this axis
  /// \param coordinateMax the maximum coordinate for this axis
  VolumeDataAxisDescriptor(int numSamples, const std::string &pName, const std::string &pUnit, float coordinateMin, float coordinateMax)
    : numSamples(numSamples), pName(pName), pUnit(pUnit), coordinateMin(coordinateMin), coordinateMax(coordinateMax) {}

  int         GetNumSamples()    const { return numSamples; }
  const std::string &GetName()          const { return pName; }
  const std::string &GetUnit()          const { return pUnit; }
  float       GetCoordinateMin() const { return coordinateMin; }
  float       GetCoordinateMax() const { return coordinateMax; }
  float       GetCoordinateStep() const { return (numSamples > 1) ? ((coordinateMax - coordinateMin) / (numSamples - 1)) : 0; }

  /// Convert a sample index on this axis to a coordinate value
  /// \param sampleIndex the sample index to convert
  /// \return the coordinate
  float       SampleIndexToCoordinate(int sampleIndex)  { return coordinateMin + sampleIndex * GetCoordinateStep(); }

  /// Convert a coordinate to a sample index (rounding to the closest index)
  /// \param coordinate the coordinate to convert
  /// \return the sample index
  int         CoordinateToSampleIndex(float coordinate) { return (coordinate == coordinateMin) ? 0 : (int)floorf(((coordinate - coordinateMin) / (coordinateMax - coordinateMin)) * (numSamples - 1) + 0.5f); }
};
}
#endif //VOLUMEDATAAXISDESCRIPTOR_H

