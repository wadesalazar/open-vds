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

#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H

namespace OpenVDS
{

/// Interpolation method used for sampling
enum class InterpolationMethod
{
  Nearest,    ///< Snap to the closest sample
  Linear,     ///< Linear interpolation
  Cubic,      ///< Cubic interpolation
  Angular,    ///< Angular interpolation (same as linear but wraps around the value range)
  Triangular, ///< Triangular interpolation used to interpolate heightmap data
};

/// 2D or 3D dimension group\n
enum class DimensionsND
{
  Group012,
  Group013,
  Group014,
  Group015,
  Group023,
  Group024,
  Group025,
  Group034,
  Group035,
  Group045,
  Group123,
  Group124,
  Group125,
  Group134,
  Group135,
  Group145,
  Group234,
  Group235,
  Group245,
  Group345,
  Group01,
  Group02,
  Group03,
  Group04,
  Group05,
  Group12,
  Group13,
  Group14,
  Group15,
  Group23,
  Group24,
  Group25,
  Group34,
  Group35,
  Group45
};

/// Mapping volume data channels
enum class VolumeDataMapping
{
  Direct,   ///< Each voxel in the volume has a value 
  PerTrace ///< Each trace in the volume has a specified number of values. A trace is the entire length of dimension 0
};

enum class CompressionMethod
{
  None,
  Wavelet,
  Rle,
  Zip,
  WaveletNormalizeBlock,
  WaveletLossless,
  WaveletNormalizeBlockLossless,
  Max
};
} /* namespace OpenVDS */

#endif //VOLUMEDATA_H_INCLUDED
