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

#ifndef KNOWNMETADATA_H
#define KNOWNMETADATA_H

/////////////////////////////////////////////////////////////////////////////
// <copyright>
// Copyright (c) 2016 Hue AS. All rights reserved.
//
// All rights are reserved. Reproduction or transmission in whole or in part, in
// any form or by any means, electronic, mechanical or otherwise, is prohibited
// without the prior written permission of the copyright owner.
// </copyright>
/////////////////////////////////////////////////////////////////////////////


/*! \file KnownMetadata.h
    VDS Metadata:
   <ul>
   <li>Survey coordinate metadata:

   The VDS system does not deal directly with spatial coordinate systems, it only defines an N-dimensional array 
   of voxels. Although the spatial coordinate system is not a property of the VDS object itself, there is some 
   support on the VolumeBox, SeismicLine and Horizon objects for setting up the position from VDSMetadata that 
   follow a specific pattern. In order to position these objects according to the VDSMetadata of the VDS that is 
   connected, the client application must call GetVDSCoordinateSystem() on the VolumeBox/Horizon or 
   GetVDSTraceCoordinates() on the SeismicLine that should be positioned. It's also possible to get an 
   IJKGridDefinition derived from the VDSMetadata from the VDS object by calling GetIJKGridDefinitionFromVDSMetadata().

   The VDS metadata provides the position of the first voxel (origin) in a [3D] world coordinate system. It also 
   provides the step vectors that define in which direction and how far the other voxels are positioned relative to 
   the origin. Each step vector is related to a specific dimension of the VDS and is determined by a naming convention.

   \note The VDS object annotates each dimension which has a number of samples with a name, a unit, starting and ending coordinate. For example, a
   seismic dataset with a certain number of samples in the time domain will annotate the trace dimension (typically dimension 0) with "Time", "ms", start time, end time.

   Two families of VDS metadata for this category are described at \ref KNOWNMETADATA_SURVEYCOORDINATESYSTEM.

   <li>Trace coordinate metadata:

   This category contains VDSMetadataBLOBs that are populated with GetVDSTraceCoordinates() method. More information
   is provided at \ref KNOWNMETADATA_TRACECOORDINATES.
   </ul>
*/

// Survey coordinate metadata ////////////////////////////////////////////////
/*!
   \def KNOWNMETADATA_SURVEYCOORDINATESYSTEM

   SurveyCoordinateSystem:

   This category of VDS metadata contains two families that provide sufficient information to render a VDS in a volume box.
   In the absence of any of these families, a default setting is considered. In the following, these VDS metadata families are explained.
   
   <ul>
   <li>The Inline/Crossline system:

   In this system, the step vector for dimension 0 is always negative Z direction. This will simplify the metadata, and the 
   information we need is a Hue::Util::DoubleVector2 to define the origin (\ref KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_ORIGIN), 
   and two more Hue::Util::DoubleVector2 to define the inline and crossline spacing (\ref KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_INLINESPACING 
   and \ref KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_CROSSLINESPACING respectively). These are applied to transform the VDS dimensions 
   with name Inline/Crossline to XYZ coordinates. Other dimension names that are recognized are Time/Depth/Sample which are all mapped to 
   negative Z (i.e. positive Depth will result in the same negative Z).   
   
   <li>The 3D IJK system:
   
   The Inline/Crossline system has flexibility for only two dimensions. In order to have more freedom, the 3DIJK metadata is defined.
   A Hue::Util::DoubleVector3 is used to represent the origin (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_ORIGIN3D) and three step vectors 
   (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR, \ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR and 
   \ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR) that corresponded to the dimensions named "I", "J" and "K" respectively.
   
   <li>The default system:
   
   Other dimension names that are recognized and transformed to XYZ coordinates are X, Y and Z, which will be mapped directly to the corresponding
   XYZ coordinate.

   Any dimensions which have names that are not recognized or are missing the Spacing metadata will be given a unit vector
   perpendicular to the recognized dimensions. This means you will get a sensible positioning of a Time/Offset/Crossline 
   VolumeBox that is connected to a 4D Time/Offset/Crossline/Inline VDS. The scale of the derived unit vector for the 
   Offset dimension will be something that gives the closest possible approximation to a cubic voxel.
</ul> 
*/

#define KNOWNMETADATA_SURVEYCOORDINATESYSTEM                               "SurveyCoordinateSystem"

// VDS metadata /////////////////////////////////////////////////////////////

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_ORIGIN
/// <ul>
/// <li>Data type   : Hue::Util::DoubleVector2
/// <li>Name        : "Origin"
/// <li>Description : The XY position of the origin of the annotation (Inline/Crossline/Time) coordinate system.
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_ORIGIN              "Origin"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_INLINESPACING
/// <ul>
/// <li>Data type   : Hue::Util::DoubleVector2
/// <li>Name        : "InlineSpacing"
/// <li>Description : The XY spacing between units in the Inline annotation dimension.
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_INLINESPACING       "InlineSpacing"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_CROSSLINESPACING
/// <ul>
/// <li>Data type   : DoubleVector2
/// <li>Name        : "CrosslineSpacing"
/// <li>Description : The XY spacing between units in the Crossline annotation dimension.
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_CROSSLINESPACING    "CrosslineSpacing"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_ORIGIN3D
/// <ul>
/// <li>Data type   : Hue::Util::DoubleVector3
/// <li>Name        : "Origin3D"
/// <li>Description : The XYZ position of the origin of the annotation (I/J/K) coordinate system.
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_ORIGIN3D            "Origin3D"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR
/// <ul>
/// <li>Data type   : Hue::Util::DoubleVector3
/// <li>Name        : "IStepVector"
/// <li>Description : The step vector corresponded to dimension named 'I'
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR         "IStepVector"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR
/// <ul>
/// <li>Data type   : Hue::Util::DoubleVector3
/// <li>Name        : "JStepVector"
/// <li>Description : The step vector corresponded to dimension named 'J'
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR         "JStepVector"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR
/// <ul>
/// <li>Data type   : Hue::Util::DoubleVector3
/// <li>Name        : "KStepVector"
/// <li>Description : The step vector corresponded to dimension named 'K'
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR         "KStepVector"

// Dimension names //////////////////////////////////////////////////////////

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE
/// String representing the name of dimension corresponded to the inline spacing.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE      "Inline"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE
/// String representing the name of dimension corresponded to the crossline spacing.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE   "Crossline"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_TIME
/// String representing the name of dimension corresponded to the negative z direction.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_TIME        "Time"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_DEPTH
/// String representing the name of dimension corresponded to the negative z direction.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_DEPTH       "Depth"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE
/// String representing the name of dimension corresponded to the negative z direction.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE      "Sample"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_I
/// String representing the name of dimension corresponded to the direction represented 
/// by step vector in I direction (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR)
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_I              "I"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_J
/// String representing the name of dimension corresponded to the direction represented 
/// by step vector in J direction (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR)
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_J              "J"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_K
/// String representing the name of dimension corresponded to the direction represented 
/// by step vector in K direction (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR)
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_K              "K"



// Trace coordinate metadata ////////////////////////////////////////////////
/*! \def KNOWNMETADATA_TRACECOORDINATES
   The SeismicLine GetVDSTraceCoordinates() method will populate the PositionProperty, VerticalOffsetProperty, EnergySourcePointNumberProperty and EnsembleNumberProperty from VDSMetadataBLOBs found in the "TraceCoordinates" category.
   The PositionProperty and VerticalOffsetProperty define the position of the SeismicLine, while the EnergySourcePointNumberProperty and EnsembleNumberProperty can be displayed by the SeismicLineViewContext
   
   The known metadata in the category "TraceCoordinates" are:
   
   If the SeismicLine is connected to a VDS with metadata in the "SurveyCoordinateSystem" category, it will use that
   metadata to compute the trace positions for the line, so it's easy to set up a SeismicLine to display a particular
   Inline/Crossline of a 3D dataset.
   
   The SeismicLine GetVDSTraceCoordinates() method will populate the PositionProperty, VerticalOffsetProperty,
   EnergySourcePointNumberProperty and EnsembleNumberProperty from VDSMetadataBLOBs found in the "TraceCoordinates" category.
   The PositionProperty and VerticalOffsetProperty define the position of the SeismicLine, while the EnergySourcePointNumberProperty and EnsembleNumberProperty can be displayed by the SeismicLineViewContext
*/
#define KNOWNMETADATA_TRACECOORDINATES "TraceCoordinates"

/// \def KNOWNMETADATA_TRACEPOSITIONS
/// <ul>
/// <li>Data type   : BLOB
/// <li>Name        : "TracePositions"
/// <li>Description : An array of DoubleVector2 defining the position for each trace, where (0, 0) is treated as an undefined position.
/// </ul>
#define KNOWNMETADATA_TRACEPOSITIONS "TracePositions"

/// \def KNOWNMETADATA_TRACEVERTICALOFFSETS
/// <ul>
/// <li>Data type   : BLOB
/// <li>Name        : "TraceVerticalOffsets"
/// <li>Description : An array of doubles defining the offset for each trace from the vertical start position in the Time/Depth/Sample dimension of the VDS
/// </ul>
#define KNOWNMETADATA_TRACEVERTICALOFFSETS "TraceVerticalOffsets"

/// \def KNOWNMETADATA_ENERGYSOURCEPOINTNUMBERS
/// <ul>
/// <li>Data type   : BLOB
/// <li>Name        : "EnergySourcePointNumbers"
/// <li>Description : An array of scalar int32 values defining the energy source point number for each trace.
/// </ul>
#define KNOWNMETADATA_ENERGYSOURCEPOINTNUMBERS "EnergySourcePointNumbers"

/// \def KNOWNMETADATA_ENSEMBLENUMBERS
/// <ul>
/// <li>Data type   : BLOB
/// <li>Name        : "EnsembleNumbers"
/// <li>Description : An array of scalar int32 values defining the ensemble number for each trace.
/// </ul>
#define KNOWNMETADATA_ENSEMBLENUMBERS "EnsembleNumbers"

// Written region metadata //////////////////////////////////////////////////
/*! \def KNOWNMETADATA_CATEGORY_WRITTENREGION
   This metadata can be specified to indicate which area is written in a sparsely populated VDS.
   The guarantee is that values outside the written area will be NoValue if UseNoValue is true, and ValueRange.Min if UseNoValue is false.
*/
#define KNOWNMETADATA_CATEGORY_WRITTENREGION "WrittenRegion"

/// \def KNOWNMETADATA_WRITTENREGION
/// <ul>
/// <li>Data type   : BLOB
/// <li>Name        : "WrittenRegion"
/// <li>Description : An array of scalar int32 values defining the 6D min and max (NDBox) of the written region
/// </ul>
#define KNOWNMETADATA_WRITTENREGION "WrittenRegion"

// Import information metadata //////////////////////////////////////////////
/*!
\def KNOWNMETADATA_CATEGORY_IMPORTINFORMATION

ImportInformation:

This category of VDS metadata contains information about the initial import to VDS. That is,
information about the original file (file name, last modification time etc.) and when/how it
was imported. The intended use is e.g. to give a default file name for an export operation or
to inform the user about whether the VDS was imported from some particular file.

*/

#define KNOWNMETADATA_CATEGORY_IMPORTINFORMATION "ImportInformation"

/// \def KNOWNMETADATA_IMPORTINFORMATION_DISPLAYNAME
/// <ul>
/// <li>Data type   : String
/// <li>Name        : "DisplayName"
/// <li>Description : An informative name that can be displayed to a user but is not necessarily a valid file name.
/// </ul>

#define KNOWNMETADATA_IMPORTINFORMATION_DISPLAYNAME "DisplayName"

/// \def KNOWNMETADATA_IMPORTINFORMATION_INPUTFILENAME
/// <ul>
/// <li>Data type   : String
/// <li>Name        : "InputFileName"
/// <li>Description : The original input file name. In cases where the input is not a simple file this should still be a valid file name that can be used as the default for a subsequent export operation.
/// </ul>

#define KNOWNMETADATA_IMPORTINFORMATION_INPUTFILENAME "InputFileName"

/// \def KNOWNMETADATA_IMPORTINFORMATION_INPUTFILESIZE
/// <ul>
/// <li>Data type   : Double
/// <li>Name        : "InputFileSize"
/// <li>Description : The total size (in bytes) of the input file(s), which is an integer stored as a double because there is no 64-bit integer metadata type.
/// </ul>

#define KNOWNMETADATA_IMPORTINFORMATION_INPUTFILESIZE "InputFileSize"

/// \def KNOWNMETADATA_IMPORTINFORMATION_INPUTTIMESTAMP
/// <ul>
/// <li>Data type   : String
/// <li>Name        : "InputTimeStamp"
/// <li>Description : The last modified time of the input in ISO8601 format.
/// </ul>

#define KNOWNMETADATA_IMPORTINFORMATION_INPUTTIMESTAMP "InputTimeStamp"

/// \def KNOWNMETADATA_IMPORTINFORMATION_IMPORTTIMESTAMP
/// <ul>
/// <li>Data type   : String
/// <li>Name        : "ImportTimeStamp"
/// <li>Description : The time in ISO8601 format when the data was imported to VDS.
/// </ul>

#define KNOWNMETADATA_IMPORTINFORMATION_IMPORTTIMESTAMP "ImportTimeStamp"

#endif // !KNOWNMETADATA_H
