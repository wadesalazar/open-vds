/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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

#ifndef TRACE_DATA_MANAGER_H
#define TRACE_DATA_MANAGER_H

#include "SEGYFileInfo.h"

// Convenience layer that works with a DataViewManager to access trace data
//
class TraceDataManager
{
public:
  TraceDataManager(std::shared_ptr<DataViewManager> dvm, int64_t tracesPerPage, int64_t traceByteSize, int64_t numTraces) :
    m_dataViewManager(dvm),
    m_tracesPerPage(tracesPerPage),
    m_traceByteSize(traceByteSize),
    m_numTraces(numTraces),
    m_currentPageTrace(-1)
  {
  }

  void
  addDataRequests(int requestSecondaryKeyStart, int requestSecondaryKeyEnd, const std::vector<SEGYSegmentInfo>::iterator& lower, const std::vector<SEGYSegmentInfo>::iterator& upper)
  {
    std::vector<DataRequestInfo> requests;

    for (auto segment = lower; segment != upper; ++segment)
    {
      const bool
        isSecondaryAscending = segment->m_binInfoStart.m_crosslineNumber <= segment->m_binInfoStop.m_crosslineNumber;

      // does this segment have traces within the request secondary key range?
      if (requestSecondaryKeyEnd < (isSecondaryAscending ? segment->m_binInfoStart.m_crosslineNumber : segment->m_binInfoStop.m_crosslineNumber)
          || requestSecondaryKeyStart > (isSecondaryAscending ? segment->m_binInfoStop.m_crosslineNumber : segment->m_binInfoStart.m_crosslineNumber))
        continue;

      if (segment->m_traceStart == segment->m_traceStop)
      {
        // The segment is only one trace long, and that trace is in the request range, so add a request
        addTraceRequests(requests, segment->m_traceStart, segment->m_traceStart);
      }
      else
      {
        // axis factors for guessing trace numbers
        const auto
          secondaryKeySpanTotal = static_cast<double>(std::abs(segment->m_binInfoStop.m_crosslineNumber - segment->m_binInfoStart.m_crosslineNumber));
        const auto
          secondaryStartSpan = isSecondaryAscending ? requestSecondaryKeyStart - segment->m_binInfoStart.m_crosslineNumber : segment->m_binInfoStart.m_crosslineNumber - requestSecondaryKeyEnd,
          secondaryStopSpan = isSecondaryAscending ? segment->m_binInfoStop.m_crosslineNumber - requestSecondaryKeyEnd : requestSecondaryKeyStart - segment->m_binInfoStop.m_crosslineNumber;
        const double
          traceStartFactor = secondaryStartSpan <= 0 ? 0.0 : secondaryStartSpan / secondaryKeySpanTotal,
          traceStopFactor = secondaryStopSpan <= 0 ? 1.0 : 1.0 - secondaryStopSpan / secondaryKeySpanTotal;

        // calculate approximate start and end traces
        const auto
          traceDistance = segment->m_traceStop - segment->m_traceStart;
        const auto
          startTrace = static_cast<int64_t>(segment->m_traceStart + traceDistance * traceStartFactor),
          stopTrace = static_cast<int64_t>(segment->m_traceStart + traceDistance * traceStopFactor);

        assert(startTrace >= segment->m_traceStart && startTrace <= segment->m_traceStop);
        assert(stopTrace >= segment->m_traceStart && stopTrace <= segment->m_traceStop);

        addTraceRequests(requests, startTrace, stopTrace);
      }
    }

    m_dataViewManager->addDataRequests(requests);
  }

  const char *
  getTraceData(int64_t traceNumber, OpenVDS::Error & error) const
  {
    // figure out which "page" is associated with the trace
    // request the "page" from the data view manager
    // return trace data pointer within the page

    std::shared_ptr<DataView>
      pageView;

    error = {};

    const auto
      pageTrace = GetPageStartForTrace(traceNumber);
    if (pageTrace == m_currentPageTrace)
    {
      pageView = m_currentPageView;
    }
    else
    {
      auto
        pageRequestInfo = createPageRequestInfo(pageTrace);

      pageView = m_dataViewManager->acquireDataView(pageRequestInfo, true, error);

      if (error.code)
      {
        return nullptr;
      }

      if (!pageView)
      {
        error.code = 1;
        error.string = "Failed to acquire DataView";
        return nullptr;
      }
    }

    const char
      * basePtr = static_cast<const char *>(pageView->Pointer(error));
    return basePtr + (traceNumber - pageTrace) * m_traceByteSize;
  }

  void
  retirePagesBefore(int64_t traceNumber) const
  {
    const auto
      pageTrace = GetPageStartForTrace(traceNumber);
    auto
      pageRequestInfo = createPageRequestInfo(pageTrace);
    m_dataViewManager->retireDataViewsBefore(pageRequestInfo);
  }

  void retireAllPages() const
  {
    m_dataViewManager->retireAllDataViews();
  }

private:
  std::shared_ptr<DataViewManager>
    m_dataViewManager;
  const int64_t
    m_tracesPerPage,
    m_traceByteSize,
    m_numTraces;
  int64_t
    m_currentPageTrace;
  std::shared_ptr<DataView>
    m_currentPageView;

  void
  addTraceRequests(std::vector<DataRequestInfo>& requests, int64_t startTrace, int64_t stopTrace)
  {
    const int64_t
      pageStartStart = GetPageStartForTrace(startTrace),
      pageStartStop = GetPageStartForTrace(stopTrace);

    for (auto pageTrace = pageStartStart; pageTrace <= pageStartStop; pageTrace += m_tracesPerPage)
    {
      requests.emplace_back(createPageRequestInfo(pageTrace));
    }
  }

  DataRequestInfo
  createPageRequestInfo(int64_t pageTrace) const
  {
    const auto
      traceCount = std::min(m_tracesPerPage, m_numTraces - pageTrace);

    assert(traceCount > 0);

    DataRequestInfo
      dataRequestInfo;
    dataRequestInfo.offset = SEGY::TextualFileHeaderSize + SEGY::BinaryFileHeaderSize + pageTrace * m_traceByteSize;
    dataRequestInfo.size = traceCount * m_traceByteSize;

    return dataRequestInfo;
  }

  int64_t
  GetPageStartForTrace(int64_t trace) const
  {
    return trace - (trace % m_tracesPerPage);
  }

};

#endif  // TRACE_DATA_MANAGER_H
