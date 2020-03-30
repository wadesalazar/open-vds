#include <OpenVDS/VolumeIndexer.h>

#include "VDS/VolumeDataLayoutImpl.h"
#include "VDS/VolumeDataPageImpl.h"
#include "VDS/DimensionGroup.h"

namespace OpenVDS
{
VolumeIndexerData::VolumeIndexerData(const VolumeDataPage *page,
                                     int32_t channelIndex,
                                     int32_t lod,
                                     DimensionsND dimensions,
                                     const VolumeDataLayout *layout)
{
  valueRangeMin = layout->GetChannelDescriptor(channelIndex).GetValueRangeMin();
  valueRangeMax = layout->GetChannelDescriptor(channelIndex).GetValueRangeMax();

  this->lod = lod;
  page->GetMinMax(voxelMin, voxelMax);

  for (int iDimension = 0; iDimension < Dimensionality_Max; iDimension++)
  {
    pitch[iDimension] = 0;
    bitPitch[iDimension] = 0;

    axisNumSamples[iDimension] = layout->GetDimensionNumSamples(iDimension);
    coordinateMin[iDimension] = (iDimension < layout->GetDimensionality()) ? layout->GetDimensionMin(iDimension) : 0;
    coordinateMax[iDimension] = (iDimension < layout->GetDimensionality()) ? layout->GetDimensionMax(iDimension) : 0;

    localChunkSamples[iDimension] = 1;
    isDimensionLODDecimated[iDimension] = false;

    localChunkAllocatedSize[iDimension] = 1;
  }

  auto &dataBlock = static_cast<const VolumeDataPageImpl *>(page)->GetDataBlock();
  dataBlockPitch[0] = dataBlock.Pitch[0];
  dataBlockPitch[1] = dataBlock.Pitch[1];
  dataBlockPitch[2] = dataBlock.Pitch[2];
  dataBlockPitch[3] = dataBlock.Pitch[3];

  dataBlockAllocatedSize[0] = dataBlock.AllocatedSize[0];
  dataBlockAllocatedSize[1] = dataBlock.AllocatedSize[1];
  dataBlockAllocatedSize[2] = dataBlock.AllocatedSize[2];
  dataBlockAllocatedSize[3] = dataBlock.AllocatedSize[3];

  dataBlockSamples[0] = dataBlock.Size[0];
  dataBlockSamples[1] = dataBlock.Size[1];
  dataBlockSamples[2] = dataBlock.Size[2];
  dataBlockSamples[3] = dataBlock.Size[3];

  for (int iDataBlockDim = 0; iDataBlockDim < DataStoreDimensionality::DataStoreDimensionality_Max; iDataBlockDim++)
  {
    dataBlockBitPitch[iDataBlockDim] = dataBlockPitch[iDataBlockDim] * (iDataBlockDim == 0 ? 1 : 8);

    int iDimension = DimensionGroupUtil::GetDimension(DimensionGroupUtil::GetDimensionGroupFromDimensionsND(dimensions), iDataBlockDim);
    dimensionMap[iDataBlockDim] = iDimension;
    if (iDimension >= 0 && iDimension < Dimensionality_Max)
    {
      pitch[iDimension] = dataBlockPitch[iDataBlockDim];
      bitPitch[iDimension] = dataBlockBitPitch[iDataBlockDim];
      localChunkAllocatedSize[iDimension] = dataBlockAllocatedSize[iDataBlockDim];

      isDimensionLODDecimated[iDimension] = (dataBlockSamples[iDataBlockDim] < voxelMax[iDimension] - voxelMin[iDimension]);
      localChunkSamples[iDimension] = dataBlockSamples[iDataBlockDim];
    }
  }

}

}
