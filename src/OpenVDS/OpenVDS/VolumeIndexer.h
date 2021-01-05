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

#ifndef VOLUMEINDEXER_H
#define VOLUMEINDEXER_H

#include <OpenVDS/openvds_export.h>
#include <OpenVDS/Vector.h>
#include <OpenVDS/VolumeData.h>
#include <OpenVDS/CoordinateTransformer.h>

#include <assert.h>

namespace OpenVDS
{

class VolumeDataPage;
class VolumeDataLayout;

struct OPENVDS_EXPORT VolumeIndexerData
{
  static constexpr int DataBlockDimensionality_Max = 4;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  template <int D>
  struct GetDataBlockDimensionality { enum { Dimensionality = (D < DataBlockDimensionality_Max) ? D : DataBlockDimensionality_Max }; };
#endif

  VolumeIndexerData()
    : valueRangeMin(0)
    , valueRangeMax(0)
  {}

  VolumeIndexerData(const VolumeDataPage *page,
                    int32_t channelIndex,
                    int32_t lod,
                    DimensionsND dimensions,
                    const VolumeDataLayout *layout);

  int32_t voxelMin[Dimensionality_Max]; ///< Minimum voxel range in volume dimensions
  int32_t voxelMax[Dimensionality_Max]; ///< Maximum voxel range in volume dimensions
  int32_t localChunkSamples[Dimensionality_Max]; ///< Number of samples in volume dimensions
  int32_t localChunkAllocatedSize[Dimensionality_Max]; ///< Allocated size of buffer in volume dimensions
  int32_t pitch[Dimensionality_Max]; ///< Pitch for buffer in volume dimensions
  int32_t bitPitch[Dimensionality_Max]; ///< Pitch for bits in volume dimensions

  int32_t dataBlockSamples[DataBlockDimensionality_Max]; ///< Number of sample in DataBlock dimensions
  int32_t dataBlockAllocatedSize[DataBlockDimensionality_Max]; ///< Allocated size of buffer in DataBlock dimensions
  int32_t dataBlockPitch[DataBlockDimensionality_Max]; ///< Pitch for buffer in DataBlock dimensions
  int32_t dataBlockBitPitch[DataBlockDimensionality_Max]; ///< Pitch for bits in DataBlock dimensions
  int32_t dimensionMap[DataBlockDimensionality_Max]; ///< Map from DataBlock dimensions to volume dimensions
  int32_t lod; ///< Level of Detail for data being indexed

  bool isDimensionLODDecimated[Dimensionality_Max]; ///< Indicates if a volume dimension is decimated with the LOD or not

  float coordinateMin[Dimensionality_Max]; ///< Minimum coordinates in volume dimensions
  float coordinateMax[Dimensionality_Max]; ///< Maximum coordinates in volume dimensions

  int axisNumSamples[Dimensionality_Max]; ///< Total number of samples in volume dimensions for whole volume (not just this chunk)


  float valueRangeMin; ///< The value range minimum for the volume
  float valueRangeMax; ///< The value range maximum for the volume

};

template<int N>
struct VolumeIndexerBase : public VolumeIndexerData
{
  ///
  /// Gets the number of samples for a dimension in the DataBlock
  /// @param iDimension the DataBlock dimension
  /// @return the number of samples in the dimension
  ///
  int GetDataBlockNumSamples(int iDimension) const
  {
    return dataBlockSamples[iDimension];
  }
  
  ///
  /// Get the number of samples for a dimension in the volume
  /// @param iDimension the volume dimension
  /// @return the number of samples in the dimension
  ///
  int GetLocalChunkNumSamples(int iDimension) const
  {
    return localChunkSamples[iDimension];
  }

  ///
  /// Converts a local index to a voxel index
  /// @param iLocalIndex the local index to convert
  /// @return the voxel index
  ///
  IntVector<N> LocalIndexToVoxelIndex(const IntVector<N> &iLocalIndex) const
  {
    IntVector<N> iVoxelIndex;

    for (int i = 0; i < N; i++)
      iVoxelIndex[i] = voxelMin[i];

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
      if (dimensionMap[i] >= 0)
        iVoxelIndex[dimensionMap[i]] += iLocalIndex[i] << (isDimensionLODDecimated[dimensionMap[i]] ? lod : 0);

    return iVoxelIndex;
  }

  ///
  /// Converts a float local index to a float voxel index
  /// @param iLocalIndex the float local index to convert
  /// @return the float voxel index
  ///
  FloatVector<N> LocalIndexToVoxelIndexFloat(const FloatVector<N> &iLocalIndex) const
  {
    FloatVector<N> iVoxelIndex;

    for (int i = 0; i < N; i++)
      iVoxelIndex[i] = (float)voxelMin[i];

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
    {
      if (dimensionMap[i] >= 0)
      {
        iVoxelIndex[dimensionMap[i]] += iLocalIndex[i] * (1 << (isDimensionLODDecimated[dimensionMap[i]] ? lod : 0));

        if (lod && isDimensionLODDecimated[dimensionMap[i]])
        {
          float max = iVoxelIndex[dimensionMap[i]] + ((1 << lod) - 1);
          iVoxelIndex[dimensionMap[i]] = (iVoxelIndex[dimensionMap[i]] + max) / 2.0f;
        }
      }
    }

    return iVoxelIndex;
  }

  ///
  /// Converts a local index to a local chunk index
  /// @param iLocalIndex the local index to convert
  /// @return the local chunk index
  ///
  IntVector<N> LocalIndexToLocalChunkIndex(const IntVector<N> &iLocalIndex) const
  {
    IntVector<N> iLocalChunkIndex;

    for (int i = 0; i < N; i++)
      iLocalChunkIndex[i] = 0;

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
      if (dimensionMap[i] >= 0)
        iLocalChunkIndex[dimensionMap[i]] += iLocalIndex[i];

    return iLocalChunkIndex;
  }

  ///
  /// Converts a voxel index to a local index
  /// @param iVoxelIndex the voxel index to convert
  /// @return the local index
  ///
  IntVector<N> VoxelIndexToLocalIndex(const IntVector<N> &iVoxelIndex) const
  {
    IntVector<N> iLocalIndex;

    for (int i = 0; i < N; i++)
      iLocalIndex[i] = 0;

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
    {
      if (dimensionMap[i] >= 0)
      {
        iLocalIndex[i] = iVoxelIndex[dimensionMap[i]] - voxelMin[dimensionMap[i]];
        iLocalIndex[i] >>= (isDimensionLODDecimated[dimensionMap[i]] ? lod : 0);
      }
    }

    return iLocalIndex;
  }

  ///
  /// Converts a float voxel index to a float local index
  /// @param iVoxelIndex the float voxel index to convert
  /// @return the float local index
  ///
  FloatVector<N> VoxelIndexToLocalIndexFloat(const FloatVector<N> &iVoxelIndex) const
  {
    FloatVector<N> iLocalIndex;

    for (int i = 0; i < N; i++)
      iLocalIndex[i] = 0.0f;

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
    {
      if (dimensionMap[i] >= 0)
      {
        iLocalIndex[i] = iVoxelIndex[dimensionMap[i]] - voxelMin[dimensionMap[i]];
        iLocalIndex[i] /= (1 << (isDimensionLODDecimated[dimensionMap[i]] ? lod : 0));
      }
    }

    return iLocalIndex;
  }

  ///
  /// Converts a float voxel index to a volume sampler
  /// local index. The volume sampler works like
  /// OpenGL textures so the first sample starts at 0.5.
  /// 
  /// @param iVoxelIndex the float voxel index to convert
  /// @return the float volume sampler local index
  ///
  FloatVector<N> VoxelIndexToVolumeSamplerLocalIndex(const FloatVector<N> &iVoxelIndex) const
  {
    FloatVector<N> iLocalIndex = VoxelIndexToLocalIndexFloat(iVoxelIndex);

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
    {
      iLocalIndex[i] += 0.5f;
    }

    return iLocalIndex;
  }

  ///
  /// Converts a voxel index to a local chunk index
  /// @param iVoxelIndex the voxel index to convert
  /// @return the local chunk index
  ///
  IntVector<N> VoxelIndexToLocalChunkIndex(const IntVector<N> &iVoxelIndex) const
  {
    IntVector<N> iLocalChunkIndex;

    for (int i = 0; i < N; i++)
    {
      iLocalChunkIndex[i] = iVoxelIndex[i] - voxelMin[i];
      iLocalChunkIndex[i] >>= (isDimensionLODDecimated[i] ? lod : 0);
    }

    return iLocalChunkIndex;
  }

  ///
  /// Converts a local chunk index to a local index
  /// @param iLocalChunkIndex the local chunk index to convert
  /// @return the local index
  ///
  IntVector<N> LocalChunkIndexToLocalIndex(const IntVector<N> &iLocalChunkIndex) const
  {
    IntVector<N> iLocalIndex;

    for (int i = 0; i < N; i++)
      iLocalIndex[i] = 0;

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
    {
      if (dimensionMap[i] >= 0)
      {
        iLocalIndex[i] = iLocalChunkIndex[dimensionMap[i]];
      }
    }

    return iLocalIndex;
  }

  ///
  /// Converts a local chunk index to a voxel index
  /// @param iLocalChunkIndex the local chunk index to convert
  /// @return the local voxel index
  ///
  IntVector<N> LocalChunkIndexToVoxelIndex(const IntVector<N> &iLocalChunkIndex) const
  {
    IntVector<N> iVoxelIndex;

    for (int i = 0; i < N; i++)
      iVoxelIndex[i] = voxelMin[i];

    for (int i = 0; i < N; i++)
      iVoxelIndex[i] += iLocalChunkIndex[i] << (isDimensionLODDecimated[i] ? lod : 0);

    return iVoxelIndex;
  }

  ///
  /// Converts a voxel index to a data index
  /// @param iVoxelIndex the voxel index to convert
  /// @return the buffer offset for the voxel index
  ///
  int VoxelIndexToDataIndex(const IntVector<N> &iVoxelIndex) const
  {
    return LocalIndexToDataIndex(VoxelIndexToLocalIndex(iVoxelIndex));
  }

  ///
  /// Converts a local index to a data index
  /// @param iLocalIndex the local index to convert
  /// @return the buffer offset for the local index
  ///
  int LocalIndexToDataIndex(const IntVector<N> &iLocalIndex) const
  {
    int index = 0;

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
    {
      index += dataBlockPitch[i] * iLocalIndex[i];
    }

    return index;
  }

  ///
  /// Converts a local chunk index to a data index
  /// @param iLocalChunkIndex the local chunk index to convert
  /// @return the buffer offset for the local chunk index
  ///
  int LocalChunkIndexToDataIndex(const IntVector<N> &iLocalChunkIndex) const
  {
    int dataIndex = 0;

    for (int i = 0; i < N; i++)
    {
      dataIndex += iLocalChunkIndex[i] *pitch[i];
    }

    return dataIndex;
  }

  ///
  /// Converts a voxel index to a bit data index\n
  /// Used with the BitMask functions to read and write 1Bit data
  /// @see BitMaskFromVoxelIndex
  /// @param iVoxelIndex the voxel index to convert
  /// @return the buffer offset (in number of bits) for the voxel index
  ///
  int VoxelIndexToBitDataIndex(const IntVector<N> &iVoxelIndex) const
  {
    return LocalChunkIndexToBitDataIndex(VoxelIndexToLocalChunkIndex(iVoxelIndex));
  }

  ///
  /// Converts a local index to a bit data index\n
  /// Used with the BitMask functions to read and write 1Bit data
  /// @see BitMaskFromLocalIndex
  /// @param iLocalIndex the local index to convert
  /// @return the buffer offset (in number of bits) for the local index
  ///
  int LocalIndexToBitDataIndex(const IntVector<N> &iLocalIndex) const
  {
    int index = 0;
    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
    {
      index += dataBlockBitPitch[i] * iLocalIndex[i];
    }
    return index;
  }

  ///
  /// Converts a local chunk index to a bit data index\n
  /// Used with the BitMask functions to read and write 1Bit data
  /// @see BitMaskFromLocalChunkIndex
  /// @param iLocalChunkIndex the local chunk index to convert
  /// @return the buffer offset (in number of bits) for the local chunk index
  ///
  int LocalChunkIndexToBitDataIndex(const IntVector<N> &iLocalChunkIndex) const
  {
    int index = 0;

    for (int i = 0; i < N; i++)
    {
      index += iLocalChunkIndex[i] *bitPitch[i];
    }

    return index;
  }

  ///
  /// Gets the bit mask for a bit data index\n
  /// Used with the BitDataIndex functions to read and write 1Bit data\n\n
  /// Common usage:\n
  /// bool bit = buffer[iBitDataIndex >> 3] & BitMaskFromBitDataIndex(iBitDataIndex) != 0;
  /// @param iBitDataIndex the local index to compute the mask from
  /// @return the bit mask for the local index
  /// 
  static unsigned char BitMaskFromBitDataIndex(int iBitDataIndex)
  {
    return (1 << (iBitDataIndex & 0x7));
  }

  ///
  /// Gets the bit mask for a local index\n
  /// Used with the BitDataIndex functions to read and write 1Bit data\n\n
  /// Common usage:\n
  /// bool bit = buffer[LocalIndexToBitDataIndex(iLocalIndex) / 8] & BitMaskFromLocalIndex(iLocalIndex) != 0;
  /// @see LocalIndexToBitDataIndex
  /// @param iLocalIndex the local index to compute the mask from
  /// @return the bit mask for the local index
  /// 
  unsigned char BitMaskFromLocalIndex(const IntVector<N> &iLocalIndex) const
  {
    return BitMaskFromBitDataIndex(LocalIndexToBitDataIndex(iLocalIndex));
  }

  ///
  /// Gets the bit mask for a voxel index\n
  /// Used with the BitDataIndex functions to read and write 1Bit data\n\n
  /// Common usage:\n
  /// bool bit = buffer[VoxelIndexToBitDataIndex(iVoxelIndex) / 8] & BitMaskFromVoxelIndex(iVoxelIndex) != 0;
  /// @see VoxelIndexToBitDataIndex
  /// @param iVoxelIndex the voxel index to compute the mask from
  /// @return the bit mask for the voxel index
  /// 
  unsigned char BitMaskFromVoxelIndex(const IntVector<N> &iVoxelIndex) const
  {
    return BitMaskFromBitDataIndex(VoxelIndexToBitDataIndex(iVoxelIndex));
  }

  ///
  /// Gets the bit mask for a local chunk index\n
  /// Used with the BitDataIndex functions to read and write 1Bit data\n\n
  /// Common usage:\n
  /// bool bit = buffer[LocalChunkIndexToBitDataIndex(iLocalChunkIndex) / 8] & BitMaskFromLocalChunkIndex(iLocalChunkIndex) != 0;
  /// @see LocalChunkIndexToBitDataIndex
  /// @param iLocalChunkIndex the local index to compute the mask from
  /// @return the bit mask for the local chunk index
  /// 
  unsigned char BitMaskFromLocalChunkIndex(const IntVector<N> &iLocalChunkIndex) const
  {
    return BitMaskFromBitDataIndex(LocalChunkIndexToBitDataIndex(iLocalChunkIndex));
  }

  ///
  /// Convenience function for reading the bit value from a 1Bit DataBlock buffer at a local index
  /// @see BitMaskFromBitDataIndex
  /// @param data the 1Bit buffer to read
  /// @param iBitDataIndex the bit index to read
  /// @return the binary value read
  ///
  static bool BitValueFromBitDataIndex(const unsigned char* data, int iBitDataIndex)
  {
    return (data[iBitDataIndex >> 3] & BitMaskFromBitDataIndex(iBitDataIndex)) != 0;
  }

  ///
  /// Convenience function for reading the bit value from a 1Bit DataBlock buffer at a local index
  /// @see LocalIndexToBitDataIndex
  /// @see BitMaskFromLocalIndex
  /// @param data the 1Bit buffer to read
  /// @param iLocalIndex the local index to read
  /// @return the binary value read
  ///
  bool BitValueFromLocalIndex(const unsigned char* data, const IntVector<N> &iLocalIndex) const
  {
    return BitValueFromBitDataIndex(data, LocalIndexToBitDataIndex(iLocalIndex));
  }

  ///
  /// Convenience function for reading the bit value from a 1Bit DataBlock buffer at a voxel index
  /// @see VoxelIndexToBitDataIndex
  /// @see BitMaskFromVoxelIndex
  /// @param data the 1Bit buffer to read
  /// @param iVoxelIndex the voxel index to read
  /// @return the binary value read
  ///
  bool BitValueFromVoxelIndex(const unsigned char* data, const IntVector<N> &iVoxelIndex) const
  {
    return BitValueFromBitDataIndex(data, VoxelIndexToBitDataIndex(iVoxelIndex));
  }

  ///
  /// Convenience function for reading the bit value from a 1Bit DataBlock buffer at a local chunk index
  /// @see LocalChunkIndexToBitDataIndex
  /// @see BitMaskFromLocalChunkIndex
  /// @param data the 1Bit buffer to read
  /// @param iLocalChunkIndex the local chunk index to read
  /// @return the binary value read
  ///
  bool BitValueFromLocalChunkIndex(const unsigned char* data, const IntVector<N> &iLocalChunkIndex) const
  {
    return BitValueFromBitDataIndex(data, LocalChunkIndexToBitDataIndex(iLocalChunkIndex));
  }

  ///
  /// Checks if a voxel index is within the chunk this indexer was created with
  /// @param iVoxelIndex the voxel index to check
  /// @return true if the index is within this chunk, false otherwise
  ///
  bool VoxelIndexInProcessArea(const IntVector<N> &iVoxelIndex) const
  {
    bool ret = true;

    for (int i = 0; i < N; i++)
    {
      ret = ret && (iVoxelIndex[i] < voxelMax[i]) && (iVoxelIndex[i] >= voxelMin[i]);
    }

    return ret;
  }

  ///
  /// Checks if a local index is within the DataBlock this indexer was created with
  /// @param iLocalIndex the local index to check
  /// @return true if the index is within this chunk, false otherwise
  ///
  bool LocalIndexInProcessArea(const IntVector<N> &iLocalIndex) const
  {
    bool ret = true;

    for (int i = 0; i < GetDataBlockDimensionality<N>::Dimensionality; i++)
    {
      ret = ret && (iLocalIndex[i] < dataBlockSamples[i]) && (iLocalIndex[i] >= 0);
    }

    return ret;
  }

  ///
  /// Checks if a local chunk index is within the chunk this indexer was created with
  /// @param iLocalChunkIndex the local chunk index to check
  /// @return true if the index is within this chunk, false otherwise
  ///
  bool LocalChunkIndexInProcessArea(const IntVector<N> &iLocalChunkIndex) const
  {
    bool ret = true;

    for (int i = 0; i < N; i++)
    {
      ret = ret && (iLocalChunkIndex[i] < localChunkSamples[i]) && (iLocalChunkIndex[i] >= 0);
    }

    return ret;
  }

  ///
  /// Gets the coordinate step for the given volume dimension at LOD 0
  /// @param iDimension the volume dimension
  /// @return the coordinate step at LOD 0
  ///
  float GetCoordinateStep(int iDimension) const
  {
    return (axisNumSamples[iDimension] > 1) ? ((coordinateMax[iDimension] - coordinateMin[iDimension]) / (axisNumSamples[iDimension] - 1)) : 0;
  }

  ///
  /// Converts a local index to coordinates
  /// @param iLocalIndex the local index to convert
  /// @return the coordinates
  ///
  FloatVector<N> LocalIndexToCoordinate(const IntVector<N> &iLocalIndex) const
  {
    FloatVector<N> iLocalIndexFloat;

    for (int i = 0; i < N; i++)
    {
      iLocalIndexFloat[i] = (float)iLocalIndex[i];
    }

    FloatVector<N> iVoxelIndex = LocalIndexToVoxelIndexFloat(iLocalIndexFloat);
    FloatVector<N> rCoord;

    for (int i = 0; i < N; i++)
      rCoord[i] = VoxelIndexFloatToCoordinate(iVoxelIndex[i], i);

    return rCoord;
  }

  /// 
  /// Converts a voxel index for a specific volume dimension to a coordinate value
  /// @param iVoxelIndex the voxel index to convert
  /// @param iDimension the volume dimension
  /// @return the coordinate
  ///
  float VoxelIndexToCoordinate(int iVoxelIndex, int iDimension) const
  {
    float step = GetCoordinateStep(iDimension);
    float coord = coordinateMin[iDimension] + iVoxelIndex * step;

    return coord;
  }

  /// 
  /// Converts a float voxel index for a specific volume dimension to a coordinate value
  /// @param rVoxelIndex the voxel index to convert
  /// @param iDimension the volume dimension
  /// @return the coordinate
  ///
  float VoxelIndexFloatToCoordinate(float rVoxelIndex, int iDimension) const
  {
    float step = GetCoordinateStep(iDimension);
    float coord = coordinateMin[iDimension] + rVoxelIndex * step;

    return coord;
  }

  ///
  /// Converts coordinates to a local index, rounding to the nearest integer
  /// @param rCoordinate the coordinates to convert
  /// @return the local index
  ///
  IntVector<N> CoordinateToLocalIndex(const FloatVector<N> &rCoordinate) const
  {
    IntVector<N> iVoxelIndex;

    for (int i = 0; i < N; i++)
      iVoxelIndex[i] = CoordinateToVoxelIndex(rCoordinate[i], i);

    return VoxelIndexToLocalIndex(iVoxelIndex);
  }

  ///
  /// Converts coordinates to a float local index without rounding
  /// @param rCoordinate the coordinates to convert
  /// @return the float local index
  ///
  FloatVector<N> CoordinateToLocalIndexFloat(const FloatVector<N> &rCoordinate) const
  {
    FloatVector<N> iVoxelIndex;

    for (int i = 0; i < N; i++)
      iVoxelIndex[i] = CoordinateToVoxelIndexFloat(rCoordinate[i], i);

    return VoxelIndexToLocalIndexFloat(iVoxelIndex);
  }

  ///
  /// Converts coordinates to a volume sampler
  /// local index. The volume sampler works like
  /// OpenGL textures so the first sample starts at 0.5.
  /// 
  /// @param rCoordinate the coordinates to convert
  /// @return the float volume sampler local index
  ///
  FloatVector<N> CoordinateToVolumeSamplerLocalIndex(const FloatVector<N> &rCoordinate) const
  {
    FloatVector<N> iVoxelIndex;

    for (int i = 0; i < N; i++)
      iVoxelIndex[i] = CoordinateToVoxelIndexFloat(rCoordinate[i], i);

    return VoxelIndexToVolumeSamplerLocalIndex(iVoxelIndex);
  }

  ///
  /// Converts a coordinate for a specific volume dimension to a voxel index, rounding to the nearest integer
  /// @param rCoordinate the coordinate to convert
  /// @param iDimension the volume dimension
  /// @return the voxel index
  ///
  int CoordinateToVoxelIndex(float rCoordinate, int iDimension) const
  {
    if (rCoordinate == coordinateMin[iDimension]) return 0;

    int iVoxelIndex = (int)floorf(((rCoordinate - coordinateMin[iDimension]) / (coordinateMax[iDimension] - coordinateMin[iDimension])) * (axisNumSamples[iDimension] - 1) + 0.5f);

    return iVoxelIndex;
  }

  ///
  /// Converts a coordinate for a specific volume dimension to a float voxel index without rounding
  /// @param rCoordinate the coordinate to convert
  /// @param iDimension the volume dimension
  /// @return the float voxel index
  ///
  float CoordinateToVoxelIndexFloat(float rCoordinate, int iDimension) const
  {
    if (rCoordinate == coordinateMin[iDimension]) return 0;

    float iVoxelIndex = ((rCoordinate - coordinateMin[iDimension]) / (coordinateMax[iDimension] - coordinateMin[iDimension])) * (axisNumSamples[iDimension] - 1);

    return iVoxelIndex;
  }

  ///
  /// Converts a local index to a relative position (between 0 and 1) along the volume axes
  /// @param iLocalIndex the local index to convert
  /// @return the relative position along volume axes
  ///
  FloatVector<N> LocalIndexToRelativeAxisPosition(const IntVector<N> &iLocalIndex) const
  {
    IntVector<N> iVoxelIndex = LocalIndexToVoxelIndex(iLocalIndex);
    FloatVector<N> rRelativePos;

    for (int i = 0; i < N; i++)
      rRelativePos[i] = VoxelIndexToRelativeAxisPosition(iVoxelIndex[i], i);

    return rRelativePos;
  }

  ///
  /// Converts a relative position along the volume axes to a local index
  /// @param rPosition the relative postion to convert
  /// @return the local index 
  ///
  IntVector<N> RelativeAxisPositionToLocalIndex(const FloatVector<N> &rPosition) const
  {
    IntVector<N> iVoxelIndex;

    for (int i = 0; i < N; i++)
      iVoxelIndex[i] = RelativeAxisPositionToVoxelIndex(rPosition[i], i);

    return VoxelIndexToLocalIndex(iVoxelIndex);
  }

  ///
  /// Converts a voxel index for a specific dimension to a relative position (between 0 and 1) along a volume axis
  /// @param iVoxelIndex the voxel index to convert
  /// @param iDimension the volume dimension
  /// @return the relative position along the iDimension axis
  ///
  float VoxelIndexToRelativeAxisPosition(int iVoxelIndex, int iDimension) const
  {
    return (axisNumSamples[iDimension] > 1) ? (float)iVoxelIndex / (float)(axisNumSamples[iDimension] - 1) : 0;
  }

  ///
  /// Converts a relative volume axis position to a voxel index, rounding to the nears integer
  /// @param rPosition the axis position
  /// @param iDimension the volume dimension
  /// @return the voxel index
  ///
  int RelativeAxisPositionToVoxelIndex(float rPosition, int iDimension) const
  {
    return (axisNumSamples[iDimension] > 1) ? (int)floorf(rPosition * (axisNumSamples[iDimension] - 1) + 0.5f) : 0;
  }

  ///
  /// Converts a voxel index to world coordinates using the coordinate transformer's IJK grid definition and IJK dimension map
  /// @param iVoxelIndex the voxel index to convert
  /// @param cVDSCoordinateTransformer the coordinate transformer to use
  /// @return the world coordinates
  ///
  FloatVector<3> VoxelIndexToWorldCoordinates(const IntVector<N> &iVoxelIndex, const VDSCoordinateTransformerBase<N> &cVDSCoordinateTransformer) const
  {
    return cVDSCoordinateTransformer.VoxelIndexToWorldCoordinates(iVoxelIndex);
  }

  ///
  /// Converts a float voxel index to world coordinates using the coordinate transformer's IJK grid definition and IJK dimension map
  /// @param rVoxelIndex the float voxel index to convert
  /// @param cVDSCoordinateTransformer the coordinate transformer to use
  /// @return the world coordinates
  ///
  FloatVector<3> VoxelIndexToWorldCoordinates(const FloatVector<N> &rVoxelIndex, const VDSCoordinateTransformerBase<N> &cVDSCoordinateTransformer) const
  {
    return cVDSCoordinateTransformer.VoxelIndexToWorldCoordinates(rVoxelIndex);
  }

  ///
  /// Converts world coordinates to a voxel index, rounding to the nearest integer
  /// @param rWorldCoords the world coordinates to convert
  /// @param cVDSCoordinateTransformer the coordinate transformer to use
  /// @return the voxel index
  ///
  IntVector<N> WorldCoordinatesToVoxelIndex(const FloatVector<3> &rWorldCoords, const VDSCoordinateTransformerBase<N> &cVDSCoordinateTransformer) const
  {
    return cVDSCoordinateTransformer.WorldCoordinatesToVoxelIndex(rWorldCoords, voxelMin);
  }

  ///
  /// Converts world coordinates to a float voxel index without rounding
  /// @param rWorldCoords the world coordinates to convert
  /// @param cVDSCoordinateTransformer the coordinate transformer to use
  /// @return the float voxel index
  ///
  FloatVector<N> WorldCoordinatesToVoxelIndexFloat(const FloatVector<3> &rWorldCoords, const VDSCoordinateTransformerBase<N> &cVDSCoordinateTransformer) const
  {
    return cVDSCoordinateTransformer.WorldCoordinatesToVoxelIndexFloat(rWorldCoords, voxelMin);
  }

  ///
  /// Converts a local index to world coordinates using the coordinate transformer's IJK grid definition and IJK dimension map
  /// @param iLocalIndex the local index to convert
  /// @param cVDSCoordinateTransformer the coordinate transformer to use
  /// @return the world coordinates
  ///
  FloatVector<3> LocalIndexToWorldCoordinates(const IntVector<N> &iLocalIndex, const VDSCoordinateTransformerBase<N> &cVDSCoordinateTransformer) const
  {
    FloatVector<N> rLocalIndex;
    for (int i = 0; i < N; i++)
      rLocalIndex[i] = (float)iLocalIndex[i];

    return VoxelIndexToWorldCoordinates(LocalIndexToVoxelIndexFloat(rLocalIndex), cVDSCoordinateTransformer);
  }

  ///
  /// Converts world coordinates to a local index, rounding to the nearest integer
  /// @param rWorldCoords the world coordinates to convert
  /// @param cVDSCoordinateTransformer the coordinate transformer to use
  /// @return the local index
  ///
  IntVector<N> WorldCoordinatesToLocalIndex(const FloatVector<3> &rWorldCoords, const VDSCoordinateTransformerBase<N> &cVDSCoordinateTransformer) const
  {
    return VoxelIndexToLocalIndex(WorldCoordinatesToVoxelIndex(rWorldCoords, cVDSCoordinateTransformer));
  }

///////////////////////////// Constructors /////////////////////////////

  VolumeIndexerBase()
  {}

  VolumeIndexerBase(const VolumeDataPage *page,
                    int32_t channelIndex,
                    int32_t lod,
                    DimensionsND dimensions,
                    const VolumeDataLayout *layout)
    : VolumeIndexerData(page, channelIndex, lod, dimensions, layout)
  {
  }
  
  ///
  /// Creates an indexer for a temp buffer from voxel minimum and maximums.\n
  /// @param anVoxelMin voxel minimum for this indexer
  /// @param anVoxelMax voxel maximum for this indexer
  /// @param iLOD the LOD for this indexer
  /// @return the created indexer
  ///
  static VolumeIndexerBase<N> CreateTempBufferIndexer(int (&anVoxelMin)[6], int (&anVoxelMax)[6], int iLOD = 0)
  {
    VolumeIndexerBase<N> newIndexer;

    //All of these static casts are to work around a bug in gcc
    static_cast<VolumeIndexerData &>(newIndexer).lod = iLOD;
    static_cast<VolumeIndexerData &>(newIndexer).valueRangeMin = 0;
    static_cast<VolumeIndexerData &>(newIndexer).valueRangeMax = 0;

    //initialize datablock info
    for (int iDataBlockDim = 0; iDataBlockDim < DataBlockDimensionality_Max; iDataBlockDim++)
    {
      static_cast<VolumeIndexerData &>(newIndexer).dimensionMap[iDataBlockDim] = -1;
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockSamples[iDataBlockDim] = 1;
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockAllocatedSize[iDataBlockDim] = 1;
    }

    {
      int pitch = 1, bitPitch = 1;

      int iDataBlockDim = 0;
      for (int iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
      {
        static_cast<VolumeIndexerData &>(newIndexer).voxelMin[iDimension] = anVoxelMin[iDimension];
        static_cast<VolumeIndexerData &>(newIndexer).voxelMax[iDimension] = anVoxelMax[iDimension];
        static_cast<VolumeIndexerData &>(newIndexer).axisNumSamples[iDimension] = anVoxelMax[iDimension] - anVoxelMin[iDimension];
        static_cast<VolumeIndexerData &>(newIndexer).pitch[iDimension] = pitch;
        static_cast<VolumeIndexerData &>(newIndexer).bitPitch[iDimension] = bitPitch;
        pitch *= (anVoxelMax[iDimension] - anVoxelMin[iDimension] + (1 << iLOD) - 1) >> iLOD;
        bitPitch = pitch * 8;

        static_cast<VolumeIndexerData &>(newIndexer).coordinateMin[iDimension] = 0;
        static_cast<VolumeIndexerData &>(newIndexer).coordinateMax[iDimension] = 0;

        static_cast<VolumeIndexerData &>(newIndexer).isDimensionLODDecimated[iDimension] = false;

        static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension] = (anVoxelMax[iDimension] - anVoxelMin[iDimension] + (1 << iLOD) - 1) >> iLOD;
        static_cast<VolumeIndexerData &>(newIndexer).localChunkAllocatedSize[iDimension] = static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension];

        if (static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension] > 1)
        {
          //Don't allow temp buffers with more than 4D data blocks
          assert(iDataBlockDim < DataBlockDimensionality_Max);

          // dimension has size, make sure it's mapped
          static_cast<VolumeIndexerData &>(newIndexer).dataBlockSamples[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension];
          static_cast<VolumeIndexerData &>(newIndexer).dataBlockAllocatedSize[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension];
          static_cast<VolumeIndexerData &>(newIndexer).dimensionMap[iDataBlockDim] = iDimension;
          static_cast<VolumeIndexerData &>(newIndexer).isDimensionLODDecimated[iDimension] = true;

          iDataBlockDim++;
        }
      }
    }

    //set pitches
    static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[0] = 1;
    static_cast<VolumeIndexerData &>(newIndexer).dataBlockBitPitch[0] = 1;
    for (int iDataBlockDim = 1; iDataBlockDim < DataBlockDimensionality_Max; iDataBlockDim++)
    {
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim - 1] * static_cast<VolumeIndexerData &>(newIndexer).dataBlockSamples[iDataBlockDim - 1];
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockBitPitch[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim] * 8;
    }


    return newIndexer;
  }

  ///
  /// Create a temp buffer indexer with the same area and configuration as another indexer.
  /// @param indexer the indexer to copy configuration and area from
  /// @return the created indexer
  ///
  static VolumeIndexerBase<N> CreateTempBufferIndexer(const VolumeIndexerBase<N> &indexer)
  {
    VolumeIndexerBase<N> newIndexer;

    //All of these static casts are to work around a bug in gcc
    // reset pitches
    static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[0] = 1;
    static_cast<VolumeIndexerData &>(newIndexer).dataBlockBitPitch[0] = 1;
    for (int iDataBlockDim = 1; iDataBlockDim < DataBlockDimensionality_Max; iDataBlockDim++)
    {
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim - 1] * indexer.dataBlockSamples[iDataBlockDim - 1];
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockBitPitch[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim] * 8;
    }

    for (int iDataBlockDim = 0; iDataBlockDim < DataBlockDimensionality_Max; iDataBlockDim++)
    {
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockAllocatedSize[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockSamples[iDataBlockDim];
      int iDimension = indexer.dimensionMap[iDataBlockDim];
      if (iDimension >= 0 && iDimension < Dimensionality_Max)
      {
        static_cast<VolumeIndexerData &>(newIndexer).pitch[iDimension] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim];
        static_cast<VolumeIndexerData &>(newIndexer).bitPitch[iDimension] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockBitPitch[iDataBlockDim];
        static_cast<VolumeIndexerData &>(newIndexer).localChunkAllocatedSize[iDimension] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockSamples[iDataBlockDim];
      }
    }

    return newIndexer;
  }

  ///
  /// Create a temp buffer indexer with the same configuration as another indexer, but a new voxel min and max
  /// @param indexer the indexer to copy configuration from
  /// @param anNewVoxelMin the voxel minimum for the created indexer
  /// @param anNewVoxleMax the voxel maximum for the created indexer
  /// @return the created indexer
  ///
  static VolumeIndexerBase<N> CreateTempBufferIndexer(const VolumeIndexerBase<N> &indexer, int anNewVoxelMin[6], int anNewVoxelMax[6])
  {
    VolumeIndexerBase<N> newIndexer;

    //All of these static casts are to work around a bug in gcc
    //initialize datablock info
    for (int iDataBlockDim = 0; iDataBlockDim < DataBlockDimensionality_Max; iDataBlockDim++)
    {
      static_cast<VolumeIndexerData &>(newIndexer).dimensionMap[iDataBlockDim] = -1;
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockSamples[iDataBlockDim] = 1;
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockAllocatedSize[iDataBlockDim] = 1;
    }

    {
      int pitch = 1, bitPitch = 1;
      int iDataBlockDim = 0;
      for (int iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
      {
        static_cast<VolumeIndexerData &>(newIndexer).voxelMin[iDimension] = anNewVoxelMin[iDimension];
        static_cast<VolumeIndexerData &>(newIndexer).voxelMax[iDimension] = anNewVoxelMax[iDimension];
        static_cast<VolumeIndexerData &>(newIndexer).pitch[iDimension] = pitch;
        static_cast<VolumeIndexerData &>(newIndexer).bitPitch[iDimension] = bitPitch;
        static_cast<VolumeIndexerData &>(newIndexer).isDimensionLODDecimated[iDimension] = true;

        if (indexer.localChunkSamples[iDimension] == indexer.voxelMax[iDimension] - indexer.voxelMin[iDimension])
        {
          static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension] = anNewVoxelMax[iDimension] - anNewVoxelMin[iDimension];
        }
        else
        {
          static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension] = (anNewVoxelMax[iDimension] - anNewVoxelMin[iDimension] + (1 << static_cast<VolumeIndexerData &>(newIndexer).lod) - 1) >> static_cast<VolumeIndexerData &>(newIndexer).lod;
        }

        static_cast<VolumeIndexerData &>(newIndexer).localChunkAllocatedSize[iDimension] = static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension];

        pitch *= static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension];
        bitPitch = pitch * 8;

        if (static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension] > 1)
        {
          //Don't allow temp buffers with more than 4D data blocks
          assert(iDataBlockDim < DataBlockDimensionality_Max);

          // dimension has size, make sure it's mapped
          static_cast<VolumeIndexerData &>(newIndexer).dimensionMap[iDataBlockDim] = iDimension;

          // check for LOD decimation, and copy that (Dimension Maps may be different, so don't use that)
          if (static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension] == static_cast<VolumeIndexerData &>(newIndexer).voxelMax[iDimension] - static_cast<VolumeIndexerData &>(newIndexer).voxelMin[iDimension])
          {
            static_cast<VolumeIndexerData &>(newIndexer).isDimensionLODDecimated[iDimension] = false;
          }

          static_cast<VolumeIndexerData &>(newIndexer).dataBlockSamples[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension];
          static_cast<VolumeIndexerData &>(newIndexer).dataBlockAllocatedSize[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).localChunkSamples[iDimension];

          iDataBlockDim++;
        }
      }
    }

    //set pitches
    static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[0] = 1;
    static_cast<VolumeIndexerData &>(newIndexer).dataBlockBitPitch[0] = 1;
    for (int iDataBlockDim = 1; iDataBlockDim < DataBlockDimensionality_Max; iDataBlockDim++)
    {
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim - 1] * static_cast<VolumeIndexerData &>(newIndexer).dataBlockSamples[iDataBlockDim - 1];
      static_cast<VolumeIndexerData &>(newIndexer).dataBlockBitPitch[iDataBlockDim] = static_cast<VolumeIndexerData &>(newIndexer).dataBlockPitch[iDataBlockDim] * 8;
    }

    return newIndexer;
  }

private:

};

typedef VolumeIndexerBase<2> VolumeIndexer2D;
typedef VolumeIndexerBase<3> VolumeIndexer3D;
typedef VolumeIndexerBase<4> VolumeIndexer4D;
typedef VolumeIndexerBase<5> VolumeIndexer5D;
typedef VolumeIndexerBase<6> VolumeIndexer6D;

}

#endif //VOLUMEINDEXER_H
