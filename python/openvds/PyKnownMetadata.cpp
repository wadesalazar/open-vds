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
  // KnownAxisNames
  py::class_<KnownAxisNames, std::unique_ptr<KnownAxisNames>> 
    KnownAxisNames_(m,"KnownAxisNames", OPENVDS_DOCSTRING(KnownAxisNames));

  KnownAxisNames_.def_static("inline"                      , static_cast<const char *(*)()>(&KnownAxisNames::Inline), OPENVDS_DOCSTRING(KnownAxisNames_Inline));
  KnownAxisNames_.def_static("crossline"                   , static_cast<const char *(*)()>(&KnownAxisNames::Crossline), OPENVDS_DOCSTRING(KnownAxisNames_Crossline));
  KnownAxisNames_.def_static("time"                        , static_cast<const char *(*)()>(&KnownAxisNames::Time), OPENVDS_DOCSTRING(KnownAxisNames_Time));
  KnownAxisNames_.def_static("depth"                       , static_cast<const char *(*)()>(&KnownAxisNames::Depth), OPENVDS_DOCSTRING(KnownAxisNames_Depth));
  KnownAxisNames_.def_static("sample"                      , static_cast<const char *(*)()>(&KnownAxisNames::Sample), OPENVDS_DOCSTRING(KnownAxisNames_Sample));
  KnownAxisNames_.def_static("i"                           , static_cast<const char *(*)()>(&KnownAxisNames::I), OPENVDS_DOCSTRING(KnownAxisNames_I));
  KnownAxisNames_.def_static("j"                           , static_cast<const char *(*)()>(&KnownAxisNames::J), OPENVDS_DOCSTRING(KnownAxisNames_J));
  KnownAxisNames_.def_static("k"                           , static_cast<const char *(*)()>(&KnownAxisNames::K), OPENVDS_DOCSTRING(KnownAxisNames_K));
  KnownAxisNames_.def_static("x"                           , static_cast<const char *(*)()>(&KnownAxisNames::X), OPENVDS_DOCSTRING(KnownAxisNames_X));
  KnownAxisNames_.def_static("y"                           , static_cast<const char *(*)()>(&KnownAxisNames::Y), OPENVDS_DOCSTRING(KnownAxisNames_Y));
  KnownAxisNames_.def_static("z"                           , static_cast<const char *(*)()>(&KnownAxisNames::Z), OPENVDS_DOCSTRING(KnownAxisNames_Z));

  // KnownUnitNames
  py::class_<KnownUnitNames, std::unique_ptr<KnownUnitNames>> 
    KnownUnitNames_(m,"KnownUnitNames", OPENVDS_DOCSTRING(KnownUnitNames));

  KnownUnitNames_.def_static("meter"                       , static_cast<const char *(*)()>(&KnownUnitNames::Meter), OPENVDS_DOCSTRING(KnownUnitNames_Meter));
  KnownUnitNames_.def_static("millisecond"                 , static_cast<const char *(*)()>(&KnownUnitNames::Millisecond), OPENVDS_DOCSTRING(KnownUnitNames_Millisecond));
  KnownUnitNames_.def_static("foot"                        , static_cast<const char *(*)()>(&KnownUnitNames::Foot), OPENVDS_DOCSTRING(KnownUnitNames_Foot));
  KnownUnitNames_.def_static("USSurveyFoot"                , static_cast<const char *(*)()>(&KnownUnitNames::USSurveyFoot), OPENVDS_DOCSTRING(KnownUnitNames_USSurveyFoot));
  KnownUnitNames_.def_static("second"                      , static_cast<const char *(*)()>(&KnownUnitNames::Second), OPENVDS_DOCSTRING(KnownUnitNames_Second));
  KnownUnitNames_.def_static("metersPerSecond"             , static_cast<const char *(*)()>(&KnownUnitNames::MetersPerSecond), OPENVDS_DOCSTRING(KnownUnitNames_MetersPerSecond));
  KnownUnitNames_.def_static("feetPerSecond"               , static_cast<const char *(*)()>(&KnownUnitNames::FeetPerSecond), OPENVDS_DOCSTRING(KnownUnitNames_FeetPerSecond));
  KnownUnitNames_.def_static("USSurveyFeetPerSecond"       , static_cast<const char *(*)()>(&KnownUnitNames::USSurveyFeetPerSecond), OPENVDS_DOCSTRING(KnownUnitNames_USSurveyFeetPerSecond));

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
  KnownMetadata_.def_static("surveyCoordinateSystemUnit"  , static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemUnit), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemUnit));
  KnownMetadata_.def_static("surveyCoordinateSystemCRSWkt", static_cast<native::MetadataKey(*)()>(&KnownMetadata::SurveyCoordinateSystemCRSWkt), OPENVDS_DOCSTRING(KnownMetadata_SurveyCoordinateSystemCRSWkt));
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

