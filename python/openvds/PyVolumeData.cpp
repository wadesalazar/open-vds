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

#include "PyVolumeData.h"

using namespace native;

void 
PyVolumeData::initModule(py::module& m)
{
//AUTOGEN-BEGIN
  py::enum_<InterpolationMethod> 
    InterpolationMethod_(m,"InterpolationMethod", OPENVDS_DOCSTRING(InterpolationMethod));

  InterpolationMethod_.value("Nearest"                     , InterpolationMethod::Nearest            , OPENVDS_DOCSTRING(InterpolationMethod_Nearest));
  InterpolationMethod_.value("Linear"                      , InterpolationMethod::Linear             , OPENVDS_DOCSTRING(InterpolationMethod_Linear));
  InterpolationMethod_.value("Cubic"                       , InterpolationMethod::Cubic              , OPENVDS_DOCSTRING(InterpolationMethod_Cubic));
  InterpolationMethod_.value("Angular"                     , InterpolationMethod::Angular            , OPENVDS_DOCSTRING(InterpolationMethod_Angular));
  InterpolationMethod_.value("Triangular"                  , InterpolationMethod::Triangular         , OPENVDS_DOCSTRING(InterpolationMethod_Triangular));

  py::enum_<DimensionsND> 
    DimensionsND_(m,"DimensionsND", OPENVDS_DOCSTRING(DimensionsND));

  DimensionsND_.value("Dimensions_012"              , DimensionsND::Dimensions_012            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_012));
  DimensionsND_.value("Dimensions_013"              , DimensionsND::Dimensions_013            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_013));
  DimensionsND_.value("Dimensions_014"              , DimensionsND::Dimensions_014            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_014));
  DimensionsND_.value("Dimensions_015"              , DimensionsND::Dimensions_015            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_015));
  DimensionsND_.value("Dimensions_023"              , DimensionsND::Dimensions_023            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_023));
  DimensionsND_.value("Dimensions_024"              , DimensionsND::Dimensions_024            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_024));
  DimensionsND_.value("Dimensions_025"              , DimensionsND::Dimensions_025            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_025));
  DimensionsND_.value("Dimensions_034"              , DimensionsND::Dimensions_034            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_034));
  DimensionsND_.value("Dimensions_035"              , DimensionsND::Dimensions_035            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_035));
  DimensionsND_.value("Dimensions_045"              , DimensionsND::Dimensions_045            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_045));
  DimensionsND_.value("Dimensions_123"              , DimensionsND::Dimensions_123            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_123));
  DimensionsND_.value("Dimensions_124"              , DimensionsND::Dimensions_124            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_124));
  DimensionsND_.value("Dimensions_125"              , DimensionsND::Dimensions_125            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_125));
  DimensionsND_.value("Dimensions_134"              , DimensionsND::Dimensions_134            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_134));
  DimensionsND_.value("Dimensions_135"              , DimensionsND::Dimensions_135            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_135));
  DimensionsND_.value("Dimensions_145"              , DimensionsND::Dimensions_145            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_145));
  DimensionsND_.value("Dimensions_234"              , DimensionsND::Dimensions_234            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_234));
  DimensionsND_.value("Dimensions_235"              , DimensionsND::Dimensions_235            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_235));
  DimensionsND_.value("Dimensions_245"              , DimensionsND::Dimensions_245            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_245));
  DimensionsND_.value("Dimensions_345"              , DimensionsND::Dimensions_345            , OPENVDS_DOCSTRING(DimensionsND_Dimensions_345));
  DimensionsND_.value("Dimensions_01"               , DimensionsND::Dimensions_01             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_01));
  DimensionsND_.value("Dimensions_02"               , DimensionsND::Dimensions_02             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_02));
  DimensionsND_.value("Dimensions_03"               , DimensionsND::Dimensions_03             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_03));
  DimensionsND_.value("Dimensions_04"               , DimensionsND::Dimensions_04             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_04));
  DimensionsND_.value("Dimensions_05"               , DimensionsND::Dimensions_05             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_05));
  DimensionsND_.value("Dimensions_12"               , DimensionsND::Dimensions_12             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_12));
  DimensionsND_.value("Dimensions_13"               , DimensionsND::Dimensions_13             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_13));
  DimensionsND_.value("Dimensions_14"               , DimensionsND::Dimensions_14             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_14));
  DimensionsND_.value("Dimensions_15"               , DimensionsND::Dimensions_15             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_15));
  DimensionsND_.value("Dimensions_23"               , DimensionsND::Dimensions_23             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_23));
  DimensionsND_.value("Dimensions_24"               , DimensionsND::Dimensions_24             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_24));
  DimensionsND_.value("Dimensions_25"               , DimensionsND::Dimensions_25             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_25));
  DimensionsND_.value("Dimensions_34"               , DimensionsND::Dimensions_34             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_34));
  DimensionsND_.value("Dimensions_35"               , DimensionsND::Dimensions_35             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_35));
  DimensionsND_.value("Dimensions_45"               , DimensionsND::Dimensions_45             , OPENVDS_DOCSTRING(DimensionsND_Dimensions_45));

  py::enum_<VolumeDataMapping> 
    VolumeDataMapping_(m,"VolumeDataMapping", OPENVDS_DOCSTRING(VolumeDataMapping));

  VolumeDataMapping_.value("Direct"                      , VolumeDataMapping::Direct               , OPENVDS_DOCSTRING(VolumeDataMapping_Direct));
  VolumeDataMapping_.value("PerTrace"                    , VolumeDataMapping::PerTrace             , OPENVDS_DOCSTRING(VolumeDataMapping_PerTrace));

  py::enum_<CompressionMethod> 
    CompressionMethod_(m,"CompressionMethod", OPENVDS_DOCSTRING(CompressionMethod));

  CompressionMethod_.value("None"                        , CompressionMethod::None                 , OPENVDS_DOCSTRING(CompressionMethod_None));
  CompressionMethod_.value("Wavelet"                     , CompressionMethod::Wavelet              , OPENVDS_DOCSTRING(CompressionMethod_Wavelet));
  CompressionMethod_.value("RLE"                         , CompressionMethod::RLE                  , OPENVDS_DOCSTRING(CompressionMethod_RLE));
  CompressionMethod_.value("Zip"                         , CompressionMethod::Zip                  , OPENVDS_DOCSTRING(CompressionMethod_Zip));
  CompressionMethod_.value("WaveletNormalizeBlock"       , CompressionMethod::WaveletNormalizeBlock, OPENVDS_DOCSTRING(CompressionMethod_WaveletNormalizeBlock));
  CompressionMethod_.value("WaveletLossless"             , CompressionMethod::WaveletLossless      , OPENVDS_DOCSTRING(CompressionMethod_WaveletLossless));
  CompressionMethod_.value("WaveletNormalizeBlockLossless", CompressionMethod::WaveletNormalizeBlockLossless, OPENVDS_DOCSTRING(CompressionMethod_WaveletNormalizeBlockLossless));

  // CompressionInfo
  py::class_<CompressionInfo> 
    CompressionInfo_(m,"CompressionInfo", OPENVDS_DOCSTRING(CompressionInfo));

  CompressionInfo_.def(py::init<                              >(), OPENVDS_DOCSTRING(CompressionInfo_CompressionInfo));
  CompressionInfo_.def(py::init<native::CompressionMethod, int>(), OPENVDS_DOCSTRING(CompressionInfo_CompressionInfo_2));
  CompressionInfo_.def("getCompressionMethod"        , static_cast<native::CompressionMethod(CompressionInfo::*)() const>(&CompressionInfo::GetCompressionMethod), OPENVDS_DOCSTRING(CompressionInfo_GetCompressionMethod));
  CompressionInfo_.def("getAdaptiveLevel"            , static_cast<int(CompressionInfo::*)() const>(&CompressionInfo::GetAdaptiveLevel), OPENVDS_DOCSTRING(CompressionInfo_GetAdaptiveLevel));

  py::enum_<Dimensionality> 
    Dimensionality_(m,"Dimensionality", OPENVDS_DOCSTRING(Dimensionality));

  Dimensionality_.value("Dimensionality_1"            , Dimensionality::Dimensionality_1        , OPENVDS_DOCSTRING(Dimensionality_Dimensionality_1));
  Dimensionality_.value("Dimensionality_2"            , Dimensionality::Dimensionality_2        , OPENVDS_DOCSTRING(Dimensionality_Dimensionality_2));
  Dimensionality_.value("Dimensionality_3"            , Dimensionality::Dimensionality_3        , OPENVDS_DOCSTRING(Dimensionality_Dimensionality_3));
  Dimensionality_.value("Dimensionality_4"            , Dimensionality::Dimensionality_4        , OPENVDS_DOCSTRING(Dimensionality_Dimensionality_4));
  Dimensionality_.value("Dimensionality_5"            , Dimensionality::Dimensionality_5        , OPENVDS_DOCSTRING(Dimensionality_Dimensionality_5));
  Dimensionality_.value("Dimensionality_6"            , Dimensionality::Dimensionality_6        , OPENVDS_DOCSTRING(Dimensionality_Dimensionality_6));
  Dimensionality_.value("Dimensionality_Max"          , Dimensionality::Dimensionality_Max      , OPENVDS_DOCSTRING(Dimensionality_Dimensionality_Max));

  m.def("getLODSize"                  , static_cast<int(*)(int, int, int, bool)>(&GetLODSize), OPENVDS_DOCSTRING(GetLODSize));
//AUTOGEN-END
}

