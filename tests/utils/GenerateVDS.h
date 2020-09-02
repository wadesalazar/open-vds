#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/KnownMetadata.h>
#include <OpenVDS/GlobalMetadataCommon.h>
#include <OpenVDS/MetadataContainer.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/SimplexNoiseKernel.h>

#include <random>

static void getScaleOffsetForFormat(float min, float max, bool novalue, OpenVDS::VolumeDataChannelDescriptor::Format format, float &scale, float &offset)
{
  switch (format)
  {
  case OpenVDS::VolumeDataChannelDescriptor::Format_U8:
    scale = 1.f / (255.f - novalue) * (max - min);
    offset = min;
    break;
  case OpenVDS::VolumeDataChannelDescriptor::Format_U16:
    scale = 1.f/(65535.f - novalue) * (max - min);
    offset = min;
    break;
  case OpenVDS::VolumeDataChannelDescriptor::Format_R32:
  case OpenVDS::VolumeDataChannelDescriptor::Format_U32:
  case OpenVDS::VolumeDataChannelDescriptor::Format_R64:
  case OpenVDS::VolumeDataChannelDescriptor::Format_U64:
  case OpenVDS::VolumeDataChannelDescriptor::Format_1Bit:
  case OpenVDS::VolumeDataChannelDescriptor::Format_Any:
    scale = 1.0f;
    offset = 0.0f;
  }
}

static OpenVDS::VDS *generateSimpleInMemory3DVDS(int32_t samplesX = 100, int32_t samplesY = 100, int32_t samplesZ = 100, OpenVDS::VolumeDataChannelDescriptor::Format format = OpenVDS::VolumeDataChannelDescriptor::Format_R32, OpenVDS::VolumeDataLayoutDescriptor::BrickSize brickSize = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32, OpenVDS::IOManager *ioManager = nullptr)
{
  int negativeMargin = 4;
  int positiveMargin = 4;
  int brickSize2DMultiplier = 4;
  auto lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;
  auto layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
  OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(brickSize, negativeMargin, positiveMargin, brickSize2DMultiplier, lodLevels, layoutOptions);

  std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
  axisDescriptors.emplace_back(samplesX, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, 4.f);
  axisDescriptors.emplace_back(samplesY, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", 1932.f, 2536.f);
  axisDescriptors.emplace_back(samplesZ, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", 9985.f, 10369.f);

  std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
  float rangeMin = -0.1234f;
  float rangeMax = 0.1234f;
  float intScale = 1.0f;
  float intOffset = 0.0f;
  getScaleOffsetForFormat(rangeMin, rangeMax, true, format, intScale, intOffset);
  channelDescriptors.emplace_back(format, OpenVDS::VolumeDataChannelDescriptor::Components_1, AMPLITUDE_ATTRIBUTE_NAME, "", rangeMin, rangeMax, OpenVDS::VolumeDataMapping::Direct, 1, OpenVDS::VolumeDataChannelDescriptor::Default, 0.f, intScale, intOffset);

  OpenVDS::MetadataContainer metadataContainer;
  OpenVDS::Error error;
  if (ioManager)
  {
    return OpenVDS::Create(ioManager, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error);
  }
  OpenVDS::InMemoryOpenOptions options;
  return OpenVDS::Create(options, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error);
}

inline void fill3DVDSWithNoise(OpenVDS::VDS *vds, int32_t channel = 0, const OpenVDS::FloatVector3 &frequency = OpenVDS::FloatVector3(0.6f, 2.f, 4.f))
{
  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(vds);
  //ASSERT_TRUE(layout);
  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetAccessManager(vds);
  //ASSERT_TRUE(accessManager);

  OpenVDS::VolumeDataPageAccessor *pageAccessor = accessManager->CreateVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, channel, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  //ASSERT_TRUE(pageAccessor);

  int32_t chunkCount = int32_t(pageAccessor->GetChunkCount());

  OpenVDS::VolumeDataChannelDescriptor::Format format = layout->GetChannelFormat(channel);

  for (int i = 0; i < chunkCount; i++)
  {
    OpenVDS::VolumeDataPage *page =  pageAccessor->CreatePage(i);
    OpenVDS::VolumeIndexer3D outputIndexer(page, 0, 0, OpenVDS::Dimensions_012, layout);

    int pitch[OpenVDS::Dimensionality_Max];
    void *buffer = page->GetWritableBuffer(pitch);
    OpenVDS::CalculateNoise3D(buffer, format, &outputIndexer, frequency, 0.021f, 0.f, true, 345);
    page->Release();
  }
  pageAccessor->Commit();
  pageAccessor->SetMaxPages(0);
  accessManager->FlushUploadQueue();
  accessManager->DestroyVolumeDataPageAccessor(pageAccessor);

}

inline void fill3DVDSWithBitNoise(OpenVDS::VDS *vds, int32_t channel = 0)
{
  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(vds);
  //ASSERT_TRUE(layout);
  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetAccessManager(vds);
  //ASSERT_TRUE(accessManager);

  OpenVDS::VolumeDataPageAccessor *pageAccessor = accessManager->CreateVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, channel, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_Create);
  //ASSERT_TRUE(pageAccessor);

  int32_t chunkCount = int32_t(pageAccessor->GetChunkCount());

  std::mt19937 gen(123);
  std::bernoulli_distribution dist(0.8);

  for (int i = 0; i < chunkCount; i++)
  {
    OpenVDS::VolumeDataPage *page =  pageAccessor->CreatePage(i);
    OpenVDS::VolumeIndexer3D outputIndexer(page, 0, 0, OpenVDS::Dimensions_012, layout);

    int pitch[OpenVDS::Dimensionality_Max];
    uint8_t *buffer = static_cast<uint8_t *>(page->GetWritableBuffer(pitch));

    int32_t min[OpenVDS::Dimensionality_Max];
    int32_t max[OpenVDS::Dimensionality_Max];
    page->GetMinMax(min, max);
    int32_t size[OpenVDS::Dimensionality_Max];
    for (int i = 0; i < OpenVDS::Dimensionality_Max; i++)
      size[i] = max[i] - min[i];

    for (int z = 0; z < size[2]; z++)
    {
      for (int y = 0; y < size[1]; y++)
      {
        for(int x = 0; x < size[0]; x++)
        {
          int32_t byteIndex = z * pitch[2] + y * pitch[1] + (x / 8);
          int32_t bitIndex = z * size[2] * size[1] * size[0] + y * size[1] * size[0] + x;
          uint8_t byte = buffer[byteIndex];
          bool value = dist(gen);
          if (value)
            byte |= uint8_t(1) << (bitIndex % 8);
          buffer[byteIndex] = byte;
        }
      }
    }

    page->Release();
  }
  pageAccessor->Commit();
  pageAccessor->SetMaxPages(0);
  accessManager->FlushUploadQueue();
  accessManager->DestroyVolumeDataPageAccessor(pageAccessor);

}
