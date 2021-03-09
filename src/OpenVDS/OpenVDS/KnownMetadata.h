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

#include "MetadataKey.h"

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
   information we need is a DoubleVector2 to define the origin (\ref KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_ORIGIN), 
   and two more DoubleVector2 to define the inline and crossline spacing (\ref KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_INLINESPACING 
   and \ref KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_CROSSLINESPACING respectively). These are applied to transform the VDS dimensions 
   with name Inline/Crossline to XYZ coordinates. Other dimension names that are recognized are Time/Depth/Sample which are all mapped to 
   negative Z (i.e. positive Depth will result in the same negative Z).   
   
   <li>The 3D IJK system:
   
   The Inline/Crossline system has flexibility for only two dimensions. In order to have more freedom, the 3DIJK metadata is defined.
   A DoubleVector3 is used to represent the origin (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_ORIGIN3D) and three step vectors 
   (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR, \ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR and 
   \ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR) that corresponding to the dimensions named "I", "J" and "K" respectively.
   
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
/// <li>Data type   : DoubleVector2
/// <li>Name        : "Origin"
/// <li>Description : The XY position of the origin of the annotation (Inline/Crossline/Time) coordinate system.
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_ORIGIN              "Origin"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_INLINESPACING
/// <ul>
/// <li>Data type   : DoubleVector2
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
/// <li>Data type   : DoubleVector3
/// <li>Name        : "Origin3D"
/// <li>Description : The XYZ position of the origin of the annotation (I/J/K) coordinate system.
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_ORIGIN3D            "Origin3D"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR
/// <ul>
/// <li>Data type   : DoubleVector3
/// <li>Name        : "IStepVector"
/// <li>Description : The step vector corresponding to dimension named 'I'
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR         "IStepVector"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR
/// <ul>
/// <li>Data type   : DoubleVector3
/// <li>Name        : "JStepVector"
/// <li>Description : The step vector corresponding to dimension named 'J'
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR         "JStepVector"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR
/// <ul>
/// <li>Data type   : DoubleVector3
/// <li>Name        : "KStepVector"
/// <li>Description : The step vector corresponding to dimension named 'K'
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR         "KStepVector"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_UNIT
/// <ul>
/// <li>Data type   : String
/// <li>Name        : "Unit"
/// <li>Description : The unit of the coordinate system
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_UNIT                       "Unit"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_CRS_WKT
/// <ul>
/// <li>Data type   : String
/// <li>Name        : "CRSWkt"
/// <li>Description : The well-known text representation of the coordinate reference system
/// </ul>
#define KNOWNMETADATA_SURVEYCOORDINATE_CRS_WKT                    "CRSWkt"

// Dimension names //////////////////////////////////////////////////////////

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE
/// String representing the name of the axis corresponding to the inline spacing.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE      "Inline"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE
/// String representing the name of the axis corresponding to the crossline spacing.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE   "Crossline"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_TIME
/// String representing the name of the axis corresponding to the negative z direction.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_TIME        "Time"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_DEPTH
/// String representing the name of the axis corresponding to the negative z direction.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_DEPTH       "Depth"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE
/// String representing the name of the axis corresponding to the negative z direction.
#define KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE      "Sample"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_I
/// String representing the name of the axis corresponding to the direction represented 
/// by step vector in I direction (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR)
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_I              "I"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_J
/// String representing the name of the axis corresponding to the direction represented 
/// by step vector in J direction (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR)
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_J              "J"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_K
/// String representing the name of the axis corresponding to the direction represented 
/// by step vector in K direction (\ref KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR)
#define KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_K              "K"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_X
/// String representing the name of the axis mapped directly to the X coordinate
#define KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_X              "X"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_Y
/// String representing the name of the axis mapped directly to the Y coordinate
#define KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_Y              "Y"

/// \def KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_Z
/// String representing the name of the axis mapped directly to the Z coordinate
#define KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_Z              "Z"

/// \def KNOWNMETADATA_UNIT_METER
/// String representing the SI meter unit
#define KNOWNMETADATA_UNIT_METER                     "m"

/// \def KNOWNMETADATA_UNIT_MILLISECOND
/// String representing the SI milliseconds unit
#define KNOWNMETADATA_UNIT_MILLISECOND               "ms"

/// \def KNOWNMETADATA_UNIT_FOOT
/// String representing the foot unit
#define KNOWNMETADATA_UNIT_FOOT                      "ft"

/// \def KNOWNMETADATA_UNIT_US_SURVEY_FOOT
/// String representing the US survey foot unit
#define KNOWNMETADATA_UNIT_US_SURVEY_FOOT            "ussft"

/// \def KNOWNMETADATA_UNIT_SECOND
/// String representing the SI second unit
#define KNOWNMETADATA_UNIT_SECOND                    "s"

/// \def KNOWNMETADATA_UNIT_METERS_PER_SECOND
/// String representing the meters per second unit
#define KNOWNMETADATA_UNIT_METERS_PER_SECOND          "m/s"

/// \def KNOWNMETADATA_UNIT_FEET_PER_SECOND
/// String representing the feet per second unit
#define KNOWNMETADATA_UNIT_FEET_PER_SECOND           "ft/s"

/// \def KNOWNMETADATA_UNIT_US_SURVEY_FEET_PER_SECOND
/// String representing the US survey feet per second unit
#define KNOWNMETADATA_UNIT_US_SURVEY_FEET_PER_SECOND "ussft/s"

/// \def KNOWNMETADATA_UNIT_UNITLESS
/// String representing the unitless unit
#define KNOWNMETADATA_UNIT_UNITLESS                  "unitless"

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

// SEG-Y metadata ///////////////////////////////////////////////////////////
/*! \def KNOWNMETADATA_CATEGORY_SEGY
This metadata is intended to capture information necessary required to re-create the original SEG-Y file.
*/
#define KNOWNMETADATA_CATEGORY_SEGY "SEGY"

/// \def KNOWNMETADATA_SEGY_TEXTHEADER
/// <ul>
/// <li>Data type   : BLOB
/// <li>Name        : "TextHeader"
/// <li>Description : The original text header of the SEG-Y file
/// </ul>

#define KNOWNMETADATA_SEGY_TEXTHEADER "TextHeader"

/// \def KNOWNMETADATA_SEGY_BINARYHEADER
/// <ul>
/// <li>Data type   : BLOB
/// <li>Name        : "BinaryHeader"
/// <li>Description : The original binary header of the SEG-Y file
/// </ul>

#define KNOWNMETADATA_SEGY_BINARYHEADER "BinaryHeader"

/// \def KNOWNMETADATA_SEGY_ENDIANNESS
/// <ul>
/// <li>Data type   : Int
/// <li>Name        : "Endianness"
/// <li>Description : The endianness of the SEG-Y file (0=BigEndian)(1=LittleEndian)
/// </ul>

#define KNOWNMETADATA_SEGY_ENDIANNESS "Endianness"

/// \def KNOWNMETADATA_SEGY_DATAENDIANNESS
/// <ul>
/// <li>Data type   : Int
/// <li>Name        : "DataEndianness"
/// <li>Description : The endianness of the data samples in the SEG-Y file if different from the header endianness. This is a non-standard feature, but some SEG-Y files use it. (0=BigEndian)(1=LittleEndian)
/// </ul>

#define KNOWNMETADATA_SEGY_DATAENDIANNESS "DataEndianness"

/// \def KNOWNMETADATA_SEGY_DATASAMPLEFORMATCODE
/// <ul>
/// <li>Data type   : Int
/// <li>Name        : "DataSampleFormatCode"
/// <li>Description : The data sample format code of the data samples imported from the SEG-Y file. (0=Unknown)(1=IBMFloat)(2=Int32)(3=Int16)(4=FixedPoint)(5=IEEEFloat)(6=IEEEDouble)(7=Int24)(8=Int8)(9=Int64)(10=UInt32)(11=UInt16)(12=UInt64)(15=UInt24)(16=UInt8)
/// </ul>

#define KNOWNMETADATA_SEGY_DATASAMPLEFORMATCODE "DataSampleFormatCode"

/// \def KNOWNMETADATA_SEGY_PRIMARYKEY
/// <ul>
/// <li>Data type   : String
/// <li>Name        : "PrimaryKey"
/// <li>Description : The primary key (i.e. sort order) of the original SEG-Y file. This is expected to be the same as the name of one of the dimensions of the VDS. VDSs are expected to transpose crossline-oriented seismic to use the standard (Sample, Crossline, Inline) dimensions, so in order to export the file in the original order we need to know which dimension to loop over.
/// </ul>

#define KNOWNMETADATA_SEGY_PRIMARYKEY "PrimaryKey"

namespace OpenVDS
{

class KnownAxisNames
{
public:
  /// <summary>
  /// String representing the name of the axis corresponding to the inline spacing.
  /// </summary>
  static const char *Inline() { return KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE; }
  /// <summary>
  /// String representing the name of the axis corresponding to the crossline spacing.
  /// </summary>
  static const char *Crossline() { return KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE; }
  /// <summary>
  /// String representing the name of the axis corresponding to the negative z direction.
  /// </summary>
  static const char *Time() { return KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_TIME; }
  /// <summary>
  /// String representing the name of the axis corresponding to the negative z direction.
  /// </summary>
  static const char *Depth() { return KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_DEPTH; }
  /// <summary>
  /// String representing the name of the axis corresponding to the negative z direction.
  /// </summary>
  static const char *Sample() { return KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE; }
  /// <summary>
  /// String representing the name of the axis corresponding to the I step vector (\ref SurveyCoordinateSystemIStep)
  /// </summary>
  static const char *I() { return KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_I; }
  /// <summary>
  /// String representing the name of the axis corresponding to the J step vector (\ref SurveyCoordinateSystemIStep)
  /// </summary>
  static const char *J() { return KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_J; }
  /// <summary>
  /// String representing the name of the axis corresponding to the K step vector (\ref SurveyCoordinateSystemKStep)
  /// </summary>
  static const char *K() { return KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_AXISNAME_K; }
  /// <summary>
  /// String representing the name of the axis that maps directly to the X coordinate in the XYZ coordinate system
  /// </summary>
  static const char *X() { return KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_X; }
  /// <summary>
  /// String representing the name of the axis that maps directly to the Y coordinate in the XYZ coordinate system
  /// </summary>
  static const char *Y() { return KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_Y; }
  /// <summary>
  /// String representing the name of the axis that maps directly to the Z coordinate in the XYZ coordinate system
  /// </summary>
  static const char *Z() { return KNOWNMETADATA_SURVEYCOORDINATE_XYZ_AXISNAME_Z; }
};

class KnownUnitNames
{
public:
  /// <summary>
  /// String representing the SI meter unit
  /// </summary>
  static const char *Meter() { return KNOWNMETADATA_UNIT_METER; }

  /// <summary>
  /// String representing the SI milliseconds unit
  /// </summary>
  static const char *Millisecond() { return KNOWNMETADATA_UNIT_MILLISECOND; }

  /// <summary>
  /// String representing the foot unit
  /// </summary>
  static const char *Foot() { return KNOWNMETADATA_UNIT_FOOT; }

  /// <summary>
  /// String representing the US survey foot unit
  /// </summary>
  static const char *USSurveyFoot() { return KNOWNMETADATA_UNIT_US_SURVEY_FOOT; }

  /// <summary>
  /// String representing the SI second unit
  /// </summary>
  static const char *Second() { return KNOWNMETADATA_UNIT_SECOND; }

  /// <summary>
  /// String representing the meters per second unit
  /// </summary>
  static const char *MetersPerSecond() { return KNOWNMETADATA_UNIT_METERS_PER_SECOND; }

  /// <summary>
  /// String representing the feet per second unit
  /// </summary>
  static const char *FeetPerSecond() { return KNOWNMETADATA_UNIT_FEET_PER_SECOND; }

  /// <summary>
  /// String representing the US survey feet per second unit
  /// </summary>
  static const char *USSurveyFeetPerSecond() { return KNOWNMETADATA_UNIT_US_SURVEY_FEET_PER_SECOND; }

  /// <summary>
  /// String representing unitless
  /// </summary>
  static const char *Unitless() { return KNOWNMETADATA_UNIT_UNITLESS; }
};

class KnownMetadata
{
public:
  /// <summary>
  /// The metadata in the survey coordinate system category contains either information used to transform from an Inline/Crossline annotation system to XY (Easting, Northing) coordinates
  /// or information used to transform from an I/J/K annotation system to XYZ coordinates.
  /// <para>
  /// In the Inline/Crossline system, the step vector for dimension 0 (Which can be named either Time, Depth or Sample) is always negative Z direction. The origin is the position of (inline, crossline) (0, 0), the inline spacing is the (X, Y) step
  /// when the inline position increments by 1 and the crossline spacing is the (X, Y) step when the crossline position increments by 1.
  /// </para>
  /// <para>
  /// In the I/J/K system, all three dimensions have 3D step vectors allowing for any affine tranform to XYZ coordinates. The IJK origin is the position of (I, J, K) (0, 0, 0), the I step vector is the (X, Y, Z) step when the I position increments by 1,
  /// the J step vector is the (X, Y, Z) step when the J position increments by 1 and the K step vector is the (X, Y, Z) step when the K position increments by 1.
  /// </para>
  /// <para>
  /// It is also possible to define the X, Y, Z coordinates directly by using X/Y/Z as the axis names.
  /// </para>
  /// <para>
  /// Any dimensions which have names that are not recognized or are missing the Spacing metadata will be given a unit vector
  /// perpendicular to the recognized dimensions.
  /// </para>
  /// </summary>
  static const char *CategorySurveyCoordinateSystem() { return KNOWNMETADATA_SURVEYCOORDINATESYSTEM; }
  /// <summary>
  /// The XY position of the origin of the annotation (Inline/Crossline/Time) coordinate system.
  /// </summary>
  static MetadataKey SurveyCoordinateSystemOrigin() { return MetadataKey(MetadataType::DoubleVector2, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_ORIGIN); }
  /// <summary>
  /// The XY spacing between units in the Inline annotation dimension.
  /// </summary>
  static MetadataKey SurveyCoordinateSystemInlineSpacing() { return MetadataKey(MetadataType::DoubleVector2, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_INLINESPACING); }
  /// <summary>
  /// The XY spacing between units in the Crossline annotation dimension.
  /// </summary>
  static MetadataKey SurveyCoordinateSystemCrosslineSpacing() { return MetadataKey(MetadataType::DoubleVector2, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_CROSSLINESPACING); }
  /// <summary>
  /// The XYZ position of the origin of the annotation (I/J/K) coordinate system.
  /// </summary>
  static MetadataKey SurveyCoordinateSystemIJKOrigin() { return MetadataKey(MetadataType::DoubleVector3, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_ORIGIN3D); }
  /// <summary>
  /// The step vector corresponding to dimension named 'I'
  /// </summary>
  static MetadataKey SurveyCoordinateSystemIStepVector() { return MetadataKey(MetadataType::DoubleVector3, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_I_STEPVECTOR); }
  /// <summary>
  /// The step vector corresponding to dimension named 'J'
  /// </summary>
  static MetadataKey SurveyCoordinateSystemJStepVector() { return MetadataKey(MetadataType::DoubleVector3, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_J_STEPVECTOR); }
  /// <summary>
  /// The step vector corresponding to dimension named 'K'
  /// </summary>
  static MetadataKey SurveyCoordinateSystemKStepVector() { return MetadataKey(MetadataType::DoubleVector3, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_3DIJK_K_STEPVECTOR); }
  /// <summary>
  /// The unit of the coordinate system
  /// </summary>
  static MetadataKey SurveyCoordinateSystemUnit() { return MetadataKey(MetadataType::String, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_UNIT); }
  /// <summary>
  /// The well-known text representation of the coordinate reference system
  /// </summary>
  static MetadataKey SurveyCoordinateSystemCRSWkt() { return MetadataKey(MetadataType::String, KNOWNMETADATA_SURVEYCOORDINATESYSTEM, KNOWNMETADATA_SURVEYCOORDINATE_CRS_WKT); }

  /// <summary>
  /// The metadata in the TraceCoordinates category is used for positioning of 2D seismic. It relates to the 'Trace' dimension of the VDS where the annotation coordinate is used to look up in the arrays in this category.
  /// This allows using the same positions with subsetted and interpolated or decimated data.
  /// </summary>
  static const char *CategoryTraceCoordinates() { return KNOWNMETADATA_TRACECOORDINATES; }
  /// <summary>
  /// An array of DoubleVector2 defining the position for each trace, where (0, 0) is treated as an undefined position.
  /// </summary>
  static MetadataKey TracePositions() { return MetadataKey(MetadataType::BLOB, KNOWNMETADATA_TRACECOORDINATES, KNOWNMETADATA_TRACEPOSITIONS); }
  /// <summary>
  /// An array of doubles defining the offset for each trace from the vertical start position in the Time/Depth/Sample dimension of the VDS
  /// </summary>
  static MetadataKey TraceVerticalOffsets() { return MetadataKey(MetadataType::BLOB, KNOWNMETADATA_TRACECOORDINATES, KNOWNMETADATA_TRACEVERTICALOFFSETS); }
  /// <summary>
  /// An array of scalar int32 values defining the energy source point number for each trace.
  /// </summary>
  static MetadataKey EnergySourcePointNumbers() { return MetadataKey(MetadataType::BLOB, KNOWNMETADATA_TRACECOORDINATES, KNOWNMETADATA_ENERGYSOURCEPOINTNUMBERS); }
  /// <summary>
  /// An array of scalar int32 values defining the ensemble number for each trace.
  /// </summary>
  static MetadataKey EnsembleNumbers() { return MetadataKey(MetadataType::BLOB, KNOWNMETADATA_TRACECOORDINATES, KNOWNMETADATA_ENSEMBLENUMBERS); }

  /// <summary>
  /// The metadata in the WrittenRegion category is used to indicate which area is written in a sparsely populated VDS.
  /// The guarantee is that values outside the written area will be NoValue if UseNoValue is true, and ValueRange.Min if UseNoValue is false.
  /// </summary>
  static const char *CategoryWrittenRegion() { return KNOWNMETADATA_CATEGORY_WRITTENREGION; }
  /// <summary>
  /// An array of scalar int32 values defining the 6D min and max (NDBox) of the written region
  /// </summary>
  static MetadataKey WrittenRegion() { return MetadataKey(MetadataType::BLOB, KNOWNMETADATA_CATEGORY_WRITTENREGION, KNOWNMETADATA_WRITTENREGION); }

  /// <summary>
  /// The metadata in the ImportInformation category contains information about the initial import to VDS. That is,
  /// information about the original file (file name, last modification time etc.) and when/how it
  /// was imported. The intended use is e.g. to give a default file name for an export operation or
  /// to inform the user about whether the VDS was imported from some particular file.
  /// </summary>
  static const char *CategoryImportInformation() { return KNOWNMETADATA_CATEGORY_IMPORTINFORMATION; }
  /// <summary>
  /// An informative name that can be displayed to a user but is not necessarily a valid file name.
  /// </summary>
  static MetadataKey ImportInformationDisplayName() { return MetadataKey(MetadataType::String, KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_DISPLAYNAME); }
  /// <summary>
  /// The original input file name. In cases where the input is not a simple file this should still be a valid file name that can be used as the default for a subsequent export operation.
  /// </summary>
  static MetadataKey ImportInformationInputFileName() { return MetadataKey(MetadataType::String, KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_INPUTFILENAME); }
  /// <summary>
  /// The total size (in bytes) of the input file(s), which is an integer stored as a double because there is no 64-bit integer metadata type.
  /// </summary>
  static MetadataKey ImportInformationInputFileSize() { return MetadataKey(MetadataType::Double, KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_INPUTFILESIZE); }
  /// <summary>
  /// The last modified time of the input in ISO8601 format.
  /// </summary>
  static MetadataKey ImportInformationInputTimeStamp() { return MetadataKey(MetadataType::String, KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_INPUTTIMESTAMP); }
  /// <summary>
  /// The time in ISO8601 format when the data was imported to VDS.
  /// </summary>
  static MetadataKey ImportInformationImportTimeStamp() { return MetadataKey(MetadataType::String, KNOWNMETADATA_CATEGORY_IMPORTINFORMATION, KNOWNMETADATA_IMPORTINFORMATION_IMPORTTIMESTAMP); }

  /// <summary>
  /// The metadata in the SEGY category is intended to capture information required to re-create the original SEG-Y file. It is only present if the VDS was imported from a SEG-Y file.
  /// </summary>
  static const char *CategorySEGY() { return KNOWNMETADATA_CATEGORY_SEGY; }
  /// <summary>
  /// The original text header of the SEG-Y file
  /// </summary>
  static MetadataKey SEGYTextHeader() { return MetadataKey(MetadataType::BLOB, KNOWNMETADATA_CATEGORY_SEGY, KNOWNMETADATA_SEGY_TEXTHEADER); }
  /// <summary>
  /// The original binary header of the SEG-Y file
  /// </summary>
  static MetadataKey SEGYBinaryHeader() { return MetadataKey(MetadataType::BLOB, KNOWNMETADATA_CATEGORY_SEGY, KNOWNMETADATA_SEGY_BINARYHEADER); }
  /// <summary>
  /// The endianness of the SEG-Y file (0=BigEndian)(1=LittleEndian)
  /// </summary>
  static MetadataKey SEGYEndianness() { return MetadataKey(MetadataType::Int, KNOWNMETADATA_CATEGORY_SEGY, KNOWNMETADATA_SEGY_ENDIANNESS); }
  /// <summary>
  /// The endianness of the data samples in the SEG-Y file if different from the header endianness. This is a non-standard feature, but some SEG-Y files use it. (0=BigEndian)(1=LittleEndian)
  /// </summary>
  static MetadataKey SEGYDataEndianness() { return MetadataKey(MetadataType::Int, KNOWNMETADATA_CATEGORY_SEGY, KNOWNMETADATA_SEGY_DATAENDIANNESS); }
  /// <summary>
  /// The data sample format code of the data samples imported from the SEG-Y file. (0=Unknown)(1=IBMFloat)(2=Int32)(3=Int16)(4=FixedPoint)(5=IEEEFloat)(6=IEEEDouble)(7=Int24)(8=Int8)(9=Int64)(10=UInt32)(11=UInt16)(12=UInt64)(15=UInt24)(16=UInt8)
  /// </summary>
  static MetadataKey SEGYDataSampleFormatCode() { return MetadataKey(MetadataType::Int, KNOWNMETADATA_CATEGORY_SEGY, KNOWNMETADATA_SEGY_DATASAMPLEFORMATCODE); }
  /// <summary>
  /// The primary key (i.e. sort order) of the original SEG-Y file. VDSs are expected to transpose crossline-oriented seismic to use the standard (Sample, Crossline, Inline) axis triple, so in order to export the file in the original order we need to know which dimension to loop over.
  /// </summary>
  static MetadataKey SEGYPrimaryKey() { return MetadataKey(MetadataType::String, KNOWNMETADATA_CATEGORY_SEGY, KNOWNMETADATA_SEGY_PRIMARYKEY); }
};

} // end namespace OpenVDS


#endif // !KNOWNMETADATA_H
