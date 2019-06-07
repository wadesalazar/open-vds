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
#include "SEGYFileInfo.h"

#include <iostream>
#include <algorithm>
#include <cassert>

using namespace OpenVDS;

static int
readFieldFromHeader(const char *header, HeaderField const &headerField, Endianness endianness)
{
  if(!headerField.defined())
  {
    return 0;
  }

  // NOTE: SEG-Y byte locations start at 1
  int index = headerField.m_byteLocation - 1;

  auto signed_header   = reinterpret_cast<const signed   char *>(header);
  auto unsigned_header = reinterpret_cast<const unsigned char *>(header);

  if(headerField.m_fieldWidth == FieldWidth::FourByte)
  {
    if(endianness == Endianness::BigEndian)
    {
      return (int32_t)(signed_header[index + 0] << 24 | unsigned_header[index + 1] << 16 | unsigned_header[index + 2] << 8 | unsigned_header[index + 3]);
    }
    else
    {
      assert(endianness == Endianness::LittleEndian);
      return (int32_t)(signed_header[index + 3] << 24 | unsigned_header[index + 2] << 16 | unsigned_header[index + 1] << 8 | unsigned_header[index + 0]);
    }
  }
  else
  {
    assert(headerField.m_fieldWidth == FieldWidth::TwoByte);
    if(endianness == Endianness::BigEndian)
    {
      return (int16_t)(signed_header[index + 0] << 8 | unsigned_header[index + 1]);
    }
    else
    {
      assert(endianness == Endianness::LittleEndian);
      return (int16_t)(signed_header[index + 1] << 8 | unsigned_header[index + 0]);
    }
  }
}

int
SEGYFileInfo::traceByteSize()
{
  int formatSize;

  switch(m_dataSampleFormatCode)
  {
  default:
    return false;
  case SEGY::BinaryHeader::DataSampleFormatCode::Int8:
  case SEGY::BinaryHeader::DataSampleFormatCode::UInt8:
    formatSize = 1; break;
  case SEGY::BinaryHeader::DataSampleFormatCode::Int16:
  case SEGY::BinaryHeader::DataSampleFormatCode::UInt16:
    formatSize = 2; break;
  case SEGY::BinaryHeader::DataSampleFormatCode::Int24:
  case SEGY::BinaryHeader::DataSampleFormatCode::UInt24:
    formatSize = 3; break;
  case SEGY::BinaryHeader::DataSampleFormatCode::IBMFloat:
  case SEGY::BinaryHeader::DataSampleFormatCode::Int32:
  case SEGY::BinaryHeader::DataSampleFormatCode::FixedPoint:
  case SEGY::BinaryHeader::DataSampleFormatCode::IEEEFloat:
  case SEGY::BinaryHeader::DataSampleFormatCode::UInt32:
    formatSize = 4; break;
  case SEGY::BinaryHeader::DataSampleFormatCode::IEEEDouble:
  case SEGY::BinaryHeader::DataSampleFormatCode::Int64:
  case SEGY::BinaryHeader::DataSampleFormatCode::UInt64:
    formatSize = 8; break;
  }

  return SEGY::TraceHeaderSize + m_sampleCount * formatSize;
}

bool
SEGYFileInfo::readTraceHeader(OpenVDS::File const &file, int64_t trace, char (&header)[SEGY::TraceHeaderSize], OpenVDS::IOError &error)
{
  int64_t
    offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize + trace * traceByteSize();

  return file.read(header, offset, SEGY::TraceHeaderSize, error);
}

bool
SEGYFileInfo::scan(OpenVDS::File const &file, HeaderField const &primaryKeyHeaderField)
{
  char textualFileHeader[SEGY::TextualFileHeaderSize];
  char binaryFileHeader[SEGY::BinaryFileHeaderSize];
  char traceHeader[SEGY::TraceHeaderSize];

  OpenVDS::IOError error;

  file.read(textualFileHeader,                          0, SEGY::TextualFileHeaderSize, error) &&
  file.read(binaryFileHeader, SEGY::TextualFileHeaderSize, SEGY::BinaryFileHeaderSize,  error);

  if(error.code != 0)
  {
    return false;
  }

  m_dataSampleFormatCode = SEGY::BinaryHeader::DataSampleFormatCode(readFieldFromHeader(binaryFileHeader, SEGY::BinaryHeader::DataSampleFormatCodeHeaderField, m_headerEndianness));

  int64_t fileSize = file.size(error);

  if(error.code != 0)
  {
    return false;
  }

  m_sampleCount = readFieldFromHeader(binaryFileHeader, SEGY::BinaryHeader::NumSamplesHeaderField, m_headerEndianness);
  
  // If the sample count is not set in the binary header we try to read the first trace header and find the sample count there
  if(m_sampleCount == 0)
  {
    readTraceHeader(file, 0, traceHeader, error);
    m_sampleCount = readFieldFromHeader(traceHeader, SEGY::TraceHeader::NumSamplesHeaderField, m_headerEndianness);
  }

  if(m_sampleCount == 0)
  {
    return false;
  }

  int64_t traceDataSize = (fileSize - SEGY::TextualFileHeaderSize - SEGY::BinaryFileHeaderSize);

  m_traceCount = traceDataSize / traceByteSize();

  if(traceDataSize % traceByteSize() != 0)
  {
    std::cerr << "Warning: File size is inconsistent with trace size";
  }

  const int64_t lastTrace = (m_traceCount - 1);

  // The inside/outside trace is the last known trace in the current segment and the first known trace outside the current segment (used when binary searching)
  int64_t insideTrace = 0, outsideTrace = 0, jump = 1;

  SEGYSegmentInfo
    segmentInfo;

  int64_t trace = 0;

  int nextPrimaryKey = 0;

  int readCount = 0;

  while(insideTrace != lastTrace)
  {
    readTraceHeader(file, trace, traceHeader, error);
    if(error.code != 0)
    {
      return false;
    }
    readCount++;

    int primaryKey = readFieldFromHeader(traceHeader, primaryKeyHeaderField, m_headerEndianness);

    if(trace == 0) // start the first segment of the file
    {
      segmentInfo = SEGYSegmentInfo(primaryKey, trace);
    }
    else if(primaryKey == segmentInfo.m_primaryKey) // expand current segment if the primary key matches
    {
      assert(insideTrace == 0 || trace > insideTrace);
      insideTrace = trace;
      assert(trace > segmentInfo.m_traceStop);
      segmentInfo.m_traceStop = trace;
    }
    else
    {
      assert(outsideTrace == 0 || trace < outsideTrace);
      outsideTrace = trace;
      nextPrimaryKey = primaryKey;
    }

    if(outsideTrace == insideTrace + 1) // current segment is finished
    {
      m_segmentInfo.push_back(segmentInfo);
      int64_t segmentLength = segmentInfo.m_traceStop - segmentInfo.m_traceStart + 1;

      // start a new segment
      segmentInfo = SEGYSegmentInfo(nextPrimaryKey, outsideTrace);
      trace = std::min(lastTrace, outsideTrace + segmentLength);
      insideTrace = 0, outsideTrace = 0, jump = 1;
    }
    else if(outsideTrace == 0) // looking for a trace outside the current segment
    {
      trace = std::min(lastTrace, trace + jump);
      jump *= 2;
    }
    else if(insideTrace == 0) // looking for a trace inside the current segment
    {
      trace = std::max(segmentInfo.m_traceStart, trace - jump);
      jump *= 2;
    }
    else // search for end of segment which must lie somewhere between the last known trace in the current segment and the first known trace outside the current segment
    {
      trace = (insideTrace + outsideTrace + 1) / 2;
    }
  }

  // final segment is finished
  m_segmentInfo.push_back(segmentInfo);
  return true;
}
