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

#include <VDS/Hash.h>

#include "IO/File.h"
#include <SEGYUtils/SEGYFileInfo.h>
#include <SEGYUtils/DataProvider.h>

#include <iostream>
#include <algorithm>
#include <cassert>
#include <chrono>

using namespace OpenVDS;
using namespace SEGY;

bool
SEGYFileInfo::Is4D() const
{
  return m_segyType == SEGY::SEGYType::Prestack;
}

bool
SEGYFileInfo::IsUnbinned() const
{
  return IsSEGYTypeUnbinned(m_segyType);
}

bool
SEGYFileInfo::HasGatherOffset() const
{
  // TODO what other SEGY types have offsets?
  return m_segyType == SEGY::SEGYType::Prestack;
}

SEGYBinInfo
SEGYFileInfo::readBinInfoFromHeader(const char *header, SEGYBinInfoHeaderFields const &headerFields, Endianness endianness, int segmentTraceIndex) const
{
  int inlineNumber    = ReadFieldFromHeader(header, headerFields.m_inlineNumberHeaderField,    endianness);
  int crosslineNumber = IsUnbinned() ? segmentTraceIndex : ReadFieldFromHeader(header, headerFields.m_crosslineNumberHeaderField, endianness);

  int ensembleXCoordinate = ReadFieldFromHeader(header, headerFields.m_ensembleXCoordinateHeaderField, endianness);
  int ensembleYCoordinate = ReadFieldFromHeader(header, headerFields.m_ensembleYCoordinateHeaderField, endianness);

  double scaleFactor = 1.0f;

  if(headerFields.m_scaleOverride == 0.0)
  {
    int scale = ReadFieldFromHeader(header, TraceHeader::CoordinateScaleHeaderField, endianness);
    if(scale < 0)
    {
      scaleFactor = 1.0 / float(std::abs(scale));
    }
    else if(scale > 0)
    {
      scaleFactor = 1.0 * float(std::abs(scale));
    }
  }
  else
  {
    scaleFactor = headerFields.m_scaleOverride;
  }

  return SEGYBinInfo(inlineNumber, crosslineNumber, double(ensembleXCoordinate) * scaleFactor, double(ensembleYCoordinate) * scaleFactor);
}

int
SEGYFileInfo::TraceByteSize() const
{
  return TraceHeaderSize + m_sampleCount * SEGY::FormatSize(m_dataSampleFormatCode);
}

uint64_t
SEGYFileInfo::StaticGetUniqueID()
{
  // Make a globally unique ID for the result of this scan operation
  return OpenVDS::HashCombiner(std::chrono::system_clock::now().time_since_epoch().count()).Add(std::chrono::high_resolution_clock::now().time_since_epoch().count()).GetCombinedHash();
}

bool
SEGYFileInfo::Scan(const std::vector<DataProvider>& dataProviders, HeaderField const &primaryKeyHeaderField, HeaderField const &secondaryKeyHeaderField, SEGY::HeaderField const &startTimeHeaderField, SEGYBinInfoHeaderFields const &binInfoHeaderFields)
{
  char textualFileHeader[TextualFileHeaderSize];
  char binaryFileHeader[BinaryFileHeaderSize];
  char traceHeader[TraceHeaderSize];

  m_primaryKey = primaryKeyHeaderField;
  m_secondaryKey = secondaryKeyHeaderField;

  OpenVDS::Error error;

  // we assume that the text/binary headers are the same for all files; read them from the first file
  dataProviders[0].Read(textualFileHeader,                          0, TextualFileHeaderSize, error) &&
  dataProviders[0].Read(binaryFileHeader, TextualFileHeaderSize, BinaryFileHeaderSize,  error);

  if(error.code != 0)
  {
    return false;
  }

  m_dataSampleFormatCode = BinaryHeader::DataSampleFormatCode(ReadFieldFromHeader(binaryFileHeader, BinaryHeader::DataSampleFormatCodeHeaderField, m_headerEndianness));

  int64_t fileSize = dataProviders[0].Size(error);

  if(error.code != 0)
  {
    return false;
  }

  if(fileSize == TextualFileHeaderSize + BinaryFileHeaderSize)
  {
    // TODO for multi-file we may want to search and see if we can find any file that has trace data, instead of only looking at the first file here
    return true;
  }

  // Read first trace header of first file
  dataProviders[0].Read(traceHeader, TextualFileHeaderSize + BinaryFileHeaderSize, TraceHeaderSize, error);

  if(error.code != 0)
  {
    return false;
  }

  m_sampleCount = ReadFieldFromHeader(binaryFileHeader, BinaryHeader::NumSamplesHeaderField, m_headerEndianness);

  m_startTimeMilliseconds = ReadFieldFromHeader(traceHeader, startTimeHeaderField, m_headerEndianness);

  m_sampleIntervalMilliseconds = ReadFieldFromHeader(binaryFileHeader, BinaryHeader::SampleIntervalHeaderField, m_headerEndianness) / 1000.0;

  for (const auto& dataProvider : dataProviders)
  {
    m_segmentInfoLists.emplace_back();
    m_traceCounts.emplace_back();

    auto&
      segmentInfos = m_segmentInfoLists.back();
    auto&
      traceCount = m_traceCounts.back();

    int64_t fileSize = dataProvider.Size(error);

    if (error.code != 0)
    {
      return false;
    }

    if (fileSize == TextualFileHeaderSize + BinaryFileHeaderSize)
    {
      continue;
    }

    // Read first trace header
    dataProvider.Read(traceHeader, TextualFileHeaderSize + BinaryFileHeaderSize, TraceHeaderSize, error);

    if (error.code != 0)
    {
      return false;
    }

    // If the sample count is not set in the binary header we try to find it from the first trace header
    if (m_sampleCount == 0)
    {
      m_sampleCount = ReadFieldFromHeader(traceHeader, TraceHeader::NumSamplesHeaderField, m_headerEndianness);
    }

    // If the sample interval is not set in the binary header we try to find it from the first trace header
    if (m_sampleIntervalMilliseconds == 0.0)
    {
      m_sampleIntervalMilliseconds = ReadFieldFromHeader(traceHeader, TraceHeader::SampleIntervalHeaderField, m_headerEndianness) / 1000.0;
    }

    int64_t traceDataSize = (fileSize - TextualFileHeaderSize - BinaryFileHeaderSize);

    traceCount = traceDataSize / TraceByteSize();

    if (traceDataSize % TraceByteSize() != 0)
    {
      std::cerr << "Warning: File size is inconsistent with trace size";
    }

    if (traceCount == 0)
    {
      continue;
    }

    const int64_t lastTrace = (traceCount - 1);

    // The outside trace is the first known trace outside the current segment (used when binary searching)
    int64_t outsideTrace = 0, jump = 1;

    SEGYBinInfo outsideBinInfo;

    int primaryKey = ReadFieldFromHeader(traceHeader, primaryKeyHeaderField, m_headerEndianness), nextPrimaryKey = 0;

    SEGYSegmentInfo segmentInfo(primaryKey, 0, readBinInfoFromHeader(traceHeader, binInfoHeaderFields, m_headerEndianness, 1));

    int64_t trace = 1;

    int readCount = 1;

    while (segmentInfo.m_traceStop != lastTrace)
    {
      dataProvider.Read(traceHeader, TextualFileHeaderSize + BinaryFileHeaderSize + trace * TraceByteSize(), TraceHeaderSize, error);

      if (error.code != 0)
      {
        return false;
      }
      readCount++;

      int primaryKey = ReadFieldFromHeader(traceHeader, primaryKeyHeaderField, m_headerEndianness);

      if (primaryKey == segmentInfo.m_primaryKey) // expand current segment if the primary key matches
      {
        assert(trace > segmentInfo.m_traceStop);
        segmentInfo.m_traceStop = trace;
        segmentInfo.m_binInfoStop = readBinInfoFromHeader(traceHeader, binInfoHeaderFields, m_headerEndianness, static_cast<int>(trace - segmentInfo.m_traceStart + 1));
      }
      else
      {
        assert(outsideTrace == 0 || trace < outsideTrace);
        outsideTrace = trace;
        outsideBinInfo = readBinInfoFromHeader(traceHeader, binInfoHeaderFields, m_headerEndianness, 1);
        nextPrimaryKey = primaryKey;
      }

      if (outsideTrace == segmentInfo.m_traceStop + 1) // current segment is finished
      {
        segmentInfos.push_back(segmentInfo);
        int64_t segmentLength = segmentInfo.m_traceStop - segmentInfo.m_traceStart + 1;

        // start a new segment
        segmentInfo = SEGYSegmentInfo(nextPrimaryKey, outsideTrace, outsideBinInfo);
        trace = std::min(lastTrace, outsideTrace + segmentLength);
        outsideTrace = 0, jump = 1;
      }
      else if (outsideTrace == 0) // looking for a trace outside the current segment
      {
        trace = std::min(lastTrace, trace + jump);
        jump *= 2;
      }
      else if (trace - jump > segmentInfo.m_traceStop) // looking for a trace inside the current segment
      {
        trace = trace - jump;
        jump *= 2;
      }
      else // search for end of segment which must lie somewhere between the last known trace inside the current segment and the first known trace outside the current segment
      {
        trace = (segmentInfo.m_traceStop + outsideTrace + 1) / 2;
      }
    }

    // final segment in this file is finished
    segmentInfos.push_back(segmentInfo);
  }

  return true;
}
