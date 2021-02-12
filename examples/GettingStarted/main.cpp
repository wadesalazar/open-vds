#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>

#include <iostream>

int main(int argc, char *argv[])
{
  std::string url = TEST_URL;
  std::string connectionString = TEST_CONNECTION;

  OpenVDS::Error error;
  OpenVDS::VDSHandle handle = OpenVDS::Open(url, connectionString, error);

  if(error.code != 0)
  {
    std::cerr << "Could not open VDS: " << error.string << std::endl;
    exit(1);
  }

  OpenVDS::VolumeDataAccessManager accessManager = OpenVDS::GetAccessManager(handle);
  OpenVDS::VolumeDataLayout const *layout = accessManager.GetVolumeDataLayout();

  const int sampleDimension = 0, crosslineDimension = 1, inlineDimension = 2;
  OpenVDS::VolumeDataAxisDescriptor inlineAxisDescriptor = layout->GetAxisDescriptor(inlineDimension);
  int inlineNumber = int((inlineAxisDescriptor.GetCoordinateMin() + inlineAxisDescriptor.GetCoordinateMax()) / 2);
  int inlineIndex = inlineAxisDescriptor.CoordinateToSampleIndex((float)inlineNumber);

  int voxelMin[OpenVDS::Dimensionality_Max] = { 0, 0, 0, 0, 0, 0};
  int voxelMax[OpenVDS::Dimensionality_Max] = { 1, 1, 1, 1, 1, 1};

  voxelMin[sampleDimension] = 0;
  voxelMax[sampleDimension] = layout->GetDimensionNumSamples(sampleDimension);
  voxelMin[crosslineDimension] = 0;
  voxelMax[crosslineDimension] = layout->GetDimensionNumSamples(crosslineDimension);
  voxelMin[inlineDimension] = inlineIndex;
  voxelMax[inlineDimension] = inlineIndex + 1;

  std::vector<float> buffer(layout->GetDimensionNumSamples(sampleDimension) * layout->GetDimensionNumSamples(crosslineDimension));

  auto request = accessManager.RequestVolumeSubset<float>(buffer.data(), buffer.size() * sizeof(float), OpenVDS::Dimensions_012, 0, 0, voxelMin, voxelMax);

  bool success = request->WaitForCompletion();
  (void)success;

  {
    auto request = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, voxelMin, voxelMax);
    std::vector<float> data = std::move(request->Data());
  }
}
