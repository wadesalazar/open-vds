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

#include "PyKnownMetadata.h"

using namespace native;

void 
PyKnownMetadata::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  // KnownMetadata
  py::class_<KnownMetadata, std::unique_ptr<KnownMetadata>> 
    KnownMetadata_(m,"KnownMetadata", OPENVDS_DOCSTRING(KnownMetadata));

  KnownMetadata_.def_static("categorySurveyCoordinateSystem", static_cast<const char *(*)()>(&KnownMetadata::CategorySurveyCoordinateSystem), OPENVDS_DOCSTRING(KnownMetadata_CategorySurveyCoordinateSystem));
  KnownMetadata_.def_static("surveyCoordinateSystemOrigin", static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemOrigin), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemOrigin));
  KnownMetadata_.def_static("surveyCoordinateSystemInlineSpacing", static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemInlineSpacing), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemInlineSpacing));
  KnownMetadata_.def_static("surveyCoordinateSystemCrosslineSpacing", static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemCrosslineSpacing), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemCrosslineSpacing));
  KnownMetadata_.def_static("surveyCoordinateSystemIJKOrigin", static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemIJKOrigin), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemIJKOrigin));
  KnownMetadata_.def_static("surveyCoordinateSystemIStepVector", static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemIStepVector), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemIStepVector));
  KnownMetadata_.def_static("surveyCoordinateSystemJStepVector", static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemJStepVector), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemJStepVector));
  KnownMetadata_.def_static("surveyCoordinateSystemKStepVector", static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemKStepVector), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemKStepVector));
  KnownMetadata_.def_static("axisNameInline"              , static_cast<const char *(*)()>(&KnownMetadata::AxisNameInline), OPENVDS_DOCSTRING(KnownMetadata_AxisNameInline));
  KnownMetadata_.def_static("axisNameCrossline"           , static_cast<const char *(*)()>(&KnownMetadata::AxisNameCrossline), OPENVDS_DOCSTRING(KnownMetadata_AxisNameCrossline));
  KnownMetadata_.def_static("axisNameTime"                , static_cast<const char *(*)()>(&KnownMetadata::AxisNameTime), OPENVDS_DOCSTRING(KnownMetadata_AxisNameTime));
  KnownMetadata_.def_static("axisNameDepth"               , static_cast<const char *(*)()>(&KnownMetadata::AxisNameDepth), OPENVDS_DOCSTRING(KnownMetadata_AxisNameDepth));
  KnownMetadata_.def_static("axisNameSample"              , static_cast<const char *(*)()>(&KnownMetadata::AxisNameSample), OPENVDS_DOCSTRING(KnownMetadata_AxisNameSample));
  KnownMetadata_.def_static("axisNameI"                   , static_cast<const char *(*)()>(&KnownMetadata::AxisNameI), OPENVDS_DOCSTRING(KnownMetadata_AxisNameI));
  KnownMetadata_.def_static("axisNameJ"                   , static_cast<const char *(*)()>(&KnownMetadata::AxisNameJ), OPENVDS_DOCSTRING(KnownMetadata_AxisNameJ));
  KnownMetadata_.def_static("axisNameK"                   , static_cast<const char *(*)()>(&KnownMetadata::AxisNameK), OPENVDS_DOCSTRING(KnownMetadata_AxisNameK));
  KnownMetadata_.def_static("axisNameX"                   , static_cast<const char *(*)()>(&KnownMetadata::AxisNameX), OPENVDS_DOCSTRING(KnownMetadata_AxisNameX));
  KnownMetadata_.def_static("axisNameY"                   , static_cast<const char *(*)()>(&KnownMetadata::AxisNameY), OPENVDS_DOCSTRING(KnownMetadata_AxisNameY));
  KnownMetadata_.def_static("axisNameZ"                   , static_cast<const char *(*)()>(&KnownMetadata::AxisNameZ), OPENVDS_DOCSTRING(KnownMetadata_AxisNameZ));
  KnownMetadata_.def_static("categoryTraceCoordinates"    , static_cast<const char *(*)()>(&KnownMetadata::CategoryTraceCoordinates), OPENVDS_DOCSTRING(KnownMetadata_CategoryTraceCoordinates));
  KnownMetadata_.def_static("tracePositions"              , static_cast<native::MetadataKey(*)()>(&KnownMetadata::TracePositions), OPENVDS_DOCSTRING(KnownMetadata_TracePositions));
  KnownMetadata_.def_static("traceVerticalOffsets"        , static_cast<native::MetadataKey(*)()>(&KnownMetadata::TraceVerticalOffsets), OPENVDS_DOCSTRING(KnownMetadata_TraceVerticalOffsets));
  KnownMetadata_.def_static("energySourcePointNumbers"    , static_cast<native::MetadataKey(*)()>(&KnownMetadata::EnergySourcePointNumbers), OPENVDS_DOCSTRING(KnownMetadata_EnergySourcePointNumbers));
  KnownMetadata_.def_static("ensembleNumbers"             , static_cast<native::MetadataKey(*)()>(&KnownMetadata::EnsembleNumbers), OPENVDS_DOCSTRING(KnownMetadata_EnsembleNumbers));
  KnownMetadata_.def_static("categoryWrittenRegion"       , static_cast<const char *(*)()>(&KnownMetadata::CategoryWrittenRegion), OPENVDS_DOCSTRING(KnownMetadata_CategoryWrittenRegion));
  KnownMetadata_.def_static("writtenRegion"               , static_cast<native::MetadataKey(*)()>(&KnownMetadata::WrittenRegion), OPENVDS_DOCSTRING(KnownMetadata_WrittenRegion));
  KnownMetadata_.def_static("categoryImportInformation"   , static_cast<const char *(*)()>(&KnownMetadata::CategoryImportInformation), OPENVDS_DOCSTRING(KnownMetadata_CategoryImportInformation));
  KnownMetadata_.def_static("importInformationDisplayName", static_cast<native::MetadataKey(*)()>(&KnownMetadata::ImportInformationDisplayName), OPENVDS_DOCSTRING(KnownMetadata_ImportInformationDisplayName));
  KnownMetadata_.def_static("importInformationInputFileName", static_cast<native::MetadataKey(*)()>(&KnownMetadata::ImportInformationInputFileName), OPENVDS_DOCSTRING(KnownMetadata_ImportInformationInputFileName));
  KnownMetadata_.def_static("importInformationInputFileSize", static_cast<native::MetadataKey(*)()>(&KnownMetadata::ImportInformationInputFileSize), OPENVDS_DOCSTRING(KnownMetadata_ImportInformationInputFileSize));
  KnownMetadata_.def_static("importInformationInputTimeStamp", static_cast<native::MetadataKey(*)()>(&KnownMetadata::ImportInformationInputTimeStamp), OPENVDS_DOCSTRING(KnownMetadata_ImportInformationInputTimeStamp));
  KnownMetadata_.def_static("importInformationImportTimeStamp", static_cast<native::MetadataKey(*)()>(&KnownMetadata::ImportInformationImportTimeStamp), OPENVDS_DOCSTRING(KnownMetadata_ImportInformationImportTimeStamp));
  KnownMetadata_.def_static("categorySEGY"                , static_cast<const char *(*)()>(&KnownMetadata::CategorySEGY), OPENVDS_DOCSTRING(KnownMetadata_CategorySEGY));
  KnownMetadata_.def_static("SEGYTextHeader"              , static_cast<native::MetadataKey(*)()>(&KnownMetadata::SEGYTextHeader), OPENVDS_DOCSTRING(KnownMetadata_SEGYTextHeader));
  KnownMetadata_.def_static("SEGYBinaryHeader"            , static_cast<native::MetadataKey(*)()>(&KnownMetadata::SEGYBinaryHeader), OPENVDS_DOCSTRING(KnownMetadata_SEGYBinaryHeader));
  KnownMetadata_.def_static("SEGYEndianness"              , static_cast<native::MetadataKey(*)()>(&KnownMetadata::SEGYEndianness), OPENVDS_DOCSTRING(KnownMetadata_SEGYEndianness));
  KnownMetadata_.def_static("SEGYDataEndianness"          , static_cast<native::MetadataKey(*)()>(&KnownMetadata::SEGYDataEndianness), OPENVDS_DOCSTRING(KnownMetadata_SEGYDataEndianness));
  KnownMetadata_.def_static("SEGYDataSampleFormatCode"    , static_cast<native::MetadataKey(*)()>(&KnownMetadata::SEGYDataSampleFormatCode), OPENVDS_DOCSTRING(KnownMetadata_SEGYDataSampleFormatCode));
  KnownMetadata_.def_static("SEGYPrimaryKey"              , static_cast<native::MetadataKey(*)()>(&KnownMetadata::SEGYPrimaryKey), OPENVDS_DOCSTRING(KnownMetadata_SEGYPrimaryKey));

//AUTOGEN-END
}

