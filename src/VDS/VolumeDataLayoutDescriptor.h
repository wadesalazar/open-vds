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

#ifndef VOLUMEDATALAYOUTDESCRIPTOR_H
#define VOLUMEDATALAYOUTDESCRIPTOR_H

namespace OpenVDS
{
class VolumeDataLayoutDescriptor
{
public:
  enum BrickSize
  {
    BrickSize_32   =  5,
    BrickSize_64   =  6,
    BrickSize_128  =  7,
    BrickSize_256  =  8,
    BrickSize_512  =  9,
    BrickSize_1024 = 10,
    BrickSize_2048 = 11,
    BrickSize_4096 = 12
  };

  enum LODLevels
  {
    LODLevelNone =  0, 
    LODLevel1    =  1, 
    LODLevel2    =  2, 
    LODLevel3    =  3, 
    LODLevel4    =  4, 
    LODLevel5    =  5, 
    LODLevel6    =  6, 
    LODLevel7    =  7, 
    LODLevel8    =  8, 
    LODLevel9    =  9, 
    LODLevel10   = 10, 
    LODLevel11   = 11, 
    LODLevel12   = 12
  };

  enum Options
  {
    Options_None                         = 0,
    Options_Create2DLODs                 = (1 << 0),
    Options_ForceFullResolutionDimension = (1 << 1)
  };

private:
  BrickSize           m_brickSize;

  int                 m_negativeMargin,
                      m_positiveMargin;

  LODLevels           m_lodLevels;

  Options             m_options;

  int                 m_fullResolutionDimension;

public:
  VolumeDataLayoutDescriptor() : m_brickSize(), m_negativeMargin(), m_positiveMargin(), m_lodLevels(), m_options(), m_fullResolutionDimension() {}
  VolumeDataLayoutDescriptor(BrickSize brickSize, int negativeMargin, int positiveMargin, LODLevels lodLevels, Options options, int fullResolutionDimension = 0) : m_brickSize(brickSize), m_negativeMargin(negativeMargin), m_positiveMargin(positiveMargin), m_lodLevels(lodLevels), m_options(options), m_fullResolutionDimension(fullResolutionDimension) {}

  bool                isValid()        const { return m_brickSize != 0; }

  BrickSize           brickSize()      const { return m_brickSize; }
  int                 negativeMargin() const { return m_negativeMargin; }
  int                 positiveMargin() const { return m_positiveMargin; }
  LODLevels           lODLevels()      const { return m_lodLevels; }

  bool                isCreate2DLODs()                 const { return (m_options & Options_Create2DLODs) != 0; }
  bool                isForceFullResolutionDimension() const { return (m_options & Options_ForceFullResolutionDimension) != 0; }

  int                 fullResolutionDimension() const { return m_fullResolutionDimension; }
};
}
#endif //VOLUMEDATALAYOUTDESCRIPTOR_H