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

#include "IO/File.h"
#include <SEGYUtils/SEGYFileInfo.h>

#include <gtest/gtest.h>

using namespace SEGY;

TEST(SEGYScanTest, scan)
{
  std::unique_ptr<OpenVDS::File> file(new OpenVDS::File);

  OpenVDS::Error
    error;

  std::string
    fileName = TEST_DATA_PATH R"(/SEGY/TeapotDome/filt_mig.sgy)";

  if(!OpenVDS::File::Exists(fileName))
  {
    GTEST_SKIP() << "Could not locate test data";
  }

  file->Open(fileName, false, false, false, error);
  EXPECT_EQ(error.code, 0);

  DataProvider dataProvider(file.release());
  SEGYFileInfo
    fileInfo;

  bool success = fileInfo.Scan(dataProvider, HeaderField(189, FieldWidth::FourByte));
  EXPECT_TRUE(success);


  EXPECT_TRUE(fileInfo.m_segmentInfo.front().m_traceStart == 0);

  // check that the segments cover all traces of the file (with no overlap)
  for(int i = 1; i < fileInfo.m_segmentInfo.size(); i++)
  {
    if(fileInfo.m_segmentInfo[i].m_traceStart != fileInfo.m_segmentInfo[i - 1].m_traceStop + 1)
    {
      FAIL() << "Segment " << i << " starting at trace " << fileInfo.m_segmentInfo[i].m_traceStart << " does not directly follow segment " << (i - 1) << " ending at trace " << fileInfo.m_segmentInfo[i - 1].m_traceStop;
    }
  }

  EXPECT_TRUE(fileInfo.m_segmentInfo.back().m_traceStop == fileInfo.m_traceCount - 1);
}
