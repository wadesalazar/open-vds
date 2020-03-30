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

#ifndef HUE_GLOBALMETADATACORE_H_INCLUDED
#define HUE_GLOBALMETADATACORE_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// <copyright>
// Copyright (c) 2019 Hue AS. All rights reserved.
//
// All rights are reserved. Reproduction or transmission in whole or in part, in
// any form or by any means, electronic, mechanical or otherwise, is prohibited
// without the prior written permission of the copyright owner.
// </copyright>
////////////////////////////////////////////////////////////////////////////////

#include "KnownMetadata.h"

// Standard names for common dimensions
#define VDS_DIMENSION_INLINE_NAME                   KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE
#define VDS_DIMENSION_CROSSLINE_NAME                KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE
#define VDS_DIMENSION_CDP_NAME                      "CDP"
#define VDS_DIMENSION_GATHER_NAME                   "Gather"
#define VDS_DIMENSION_BARE_TRACE_NAME               "Trace"     // use VDS_DIMENSION_TRACE_NAME instead
#define VDS_DIMENSION_TRACE_NAME(sortOrder)         VDS_DIMENSION_BARE_TRACE_NAME " (" sortOrder ")"
#define VDS_DIMENSION_SAMPLE_NAME                   KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE
#define VDS_DIMENSION_CABLE_NAME                    "Cable"
#define VDS_DIMENSION_SHOT_NAME                     "Shot"
#define VDS_DIMENSION_RECEIVER_NAME                 "Receiver"
#define VDS_DIMENSION_FREQUENCY_NAME                "Frequency"
#define VDS_DIMENSION_X_NAME                        "X"
#define VDS_DIMENSION_Y_NAME                        "Y"
#define VDS_DIMENSION_Z_NAME                        "Z"
#define VDS_DIMENSION_TIME_NAME                     KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_TIME
#define VDS_DIMENSION_DEPTH_NAME                    KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_DEPTH
#define VDS_DIMENSION_VELOCITY_NAME                 "Velocity"
#define VDS_DIMENSION_EASTING_NAME                  "Easting"
#define VDS_DIMENSION_NORTHING_NAME                 "Northing"
#define VDS_DIMENSION_RMO_NAME                      "RMO Parameter"

// Attributes' names
#define DEFAULT_ATTRIBUTE_NAME                      "Attribute"
#define Z_ATTRIBUTE_NAME                            VDS_DIMENSION_Z_NAME
#define TIME_ATTRIBUTE_NAME                         VDS_DIMENSION_TIME_NAME
#define DEPTH_ATTRIBUTE_NAME                        VDS_DIMENSION_DEPTH_NAME
#define AMPLITUDE_ATTRIBUTE_NAME                    "Amplitude"
#define INTECEPT_ATTRIBUTE_NAME                     "Intercept"
#define GRADIENT_ATTRIBUTE_NAME                     "Gradient"
#define ANGLE_ATTRIBUTE_NAME                        "Angle"
#define AZIMUTH_ATTRIBUTE_NAME                      "Azimuth"
#define RMS_VELOCITY_ATTRIBUTE_NAME                 "Vrms"
#define INTERVAL_VELOCITY_ATTRIBUTE_NAME            "Vint"
#define AVERAGE_VELOCITY_ATTRIBUTE_NAME             "Vavg"
#define AVERAGE_VELOCITY_BELOW_DATUM_ATTRIBUTE_NAME "Vavg (below datum)"
#define OFFSET_ATTRIBUTE_NAME                       "Offset"
#define ETA_ATTRIBUTE_NAME                          "Eta"
#define PROBABILITY_ATTRIBUTE_NAME                  "Probability"

// Sort orders for the trace dimension; pass as the parameter to VDS_DIMENSION_TRACE_NAME (case sensitive)
#define VDS_DIMENSION_TRACE_SORT_OFFSET             "offset"
#define VDS_DIMENSION_TRACE_SORT_OFFSET_BY_CABLE    "offset by cable"
#define VDS_DIMENSION_TRACE_SORT_ANGLE              "angle"
#define VDS_DIMENSION_TRACE_SORT_ANGLE_BY_CABLE     "angle by cable"
#define VDS_DIMENSION_TRACE_SORT_AZIMUTH            "azimuth"
#define VDS_DIMENSION_TRACE_SORT_AZIMUTH_BY_CABLE   "azimuth by cable"

//
// Global metadata section
//

// SurveyCoordinateSystem category of global metadata that contains information for locating the seismic in XY space.
#define LATTICE_CATEGORY                            KNOWNMETADATA_SURVEYCOORDINATESYSTEM
// Display lattice information for use in GetVDSCoordinateSystem
#define LATTICE_ORIGIN                              KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_ORIGIN
#define LATTICE_INLINE_SPACING                      KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_INLINESPACING
#define LATTICE_CROSSLINE_SPACING                   KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_CROSSLINESPACING
#define LATTICE_UNIT                                "Unit"
// Original / source lattice
#define LATTICE_ORIGIN_ORIGINAL                     "Original" LATTICE_ORIGIN
#define LATTICE_INLINE_SPACING_ORIGINAL             "Original" LATTICE_INLINE_SPACING
#define LATTICE_CROSSLINE_SPACING_ORIGINAL          "Original" LATTICE_CROSSLINE_SPACING
// Source CRS 4 strings
#define CRS_WKT                                     "CRSWkt"     //The well-known text in this coordinate system

// 2D trace positions
#define TRACE_COORDINATES_2D_CATEGORY               KNOWNMETADATA_TRACECOORDINATES
#define TRACE_POSITIONS_2D                          KNOWNMETADATA_TRACEPOSITIONS
#define TRACE_POSITIONS_2D_ORIGINAL                 "TracePositionsOriginal"
#define TRACE_START_TIMES_2D                        KNOWNMETADATA_TRACEVERTICALOFFSETS

#endif // HUE_GLOBALMETADATACORE_H_INCLUDED
