#include <OpenVDS/VolumeIndexer.h>

#include <gtest/gtest.h>

GTEST_TEST(OpenVDS_volumeIndexer, checkSymbolsInVolumeIndexer)
{

  int voxelMin[6] = { 1,2,3,4,5,6 };
  int voxelMax[6] = { 6,5,4,3,2,1 };

  OpenVDS::VolumeIndexer3D vi = OpenVDS::VolumeIndexerBase<3>::CreateTempBufferIndexer(voxelMin, voxelMax, 1);
  OpenVDS::VolumeIndexer3D vi2 = OpenVDS::VolumeIndexerBase<3>::CreateTempBufferIndexer(vi);
  OpenVDS::VolumeIndexer3D vi3 = OpenVDS::VolumeIndexerBase<3>::CreateTempBufferIndexer(vi, voxelMax, voxelMin);

  ASSERT_TRUE(true);
}
