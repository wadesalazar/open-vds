/*
  This file contains docstrings for the Python bindings.
  Do not edit! These were automatically extracted by mkdoc.py
 */

#define __EXPAND(x)                                      x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...)  COUNT
#define __VA_SIZE(...)                                   __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1))
#define __CAT1(a, b)                                     a ## b
#define __CAT2(a, b)                                     __CAT1(a, b)
#define __DOC1(n1)                                       __doc_##n1
#define __DOC2(n1, n2)                                   __doc_##n1##_##n2
#define __DOC3(n1, n2, n3)                               __doc_##n1##_##n2##_##n3
#define __DOC4(n1, n2, n3, n4)                           __doc_##n1##_##n2##_##n3##_##n4
#define __DOC5(n1, n2, n3, n4, n5)                       __doc_##n1##_##n2##_##n3##_##n4##_##n5
#define __DOC6(n1, n2, n3, n4, n5, n6)                   __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6
#define __DOC7(n1, n2, n3, n4, n5, n6, n7)               __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6##_##n7
#define DOC(...)                                         __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))

#if defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


static const char *__doc_OpenVDS_AWSOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_AWSOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_AWSOpenOptions_2 = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_bucket = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_key = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_region = R"doc()doc";

static const char *__doc_OpenVDS_Close =
R"doc(Close a VDS and free up all associated resources

Parameter ``handle``:
    The handle of the VDS)doc";

static const char *__doc_OpenVDS_CompressionInfo = R"doc()doc";

static const char *__doc_OpenVDS_CompressionInfo_CompressionInfo = R"doc()doc";

static const char *__doc_OpenVDS_CompressionInfo_CompressionInfo_2 = R"doc()doc";

static const char *__doc_OpenVDS_CompressionInfo_GetAdaptiveLevel = R"doc()doc";

static const char *__doc_OpenVDS_CompressionInfo_GetCompressionMethod = R"doc()doc";

static const char *__doc_OpenVDS_CompressionInfo_adaptiveLevel = R"doc()doc";

static const char *__doc_OpenVDS_CompressionInfo_compressionMethod = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod_None = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod_RLE = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod_Wavelet = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod_WaveletLossless = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod_WaveletNormalizeBlock = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod_WaveletNormalizeBlockLossless = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod_Zip = R"doc()doc";

static const char *__doc_OpenVDS_ConvertNoValue = R"doc()doc";

static const char *__doc_OpenVDS_ConvertNoValue_2 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertNoValue_3 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertNoValue_4 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertNoValue_5 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertNoValue_6 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertNoValue_7 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertNoValue_8 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertValue = R"doc()doc";

static const char *__doc_OpenVDS_ConvertValue_2 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertValue_3 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertValue_4 = R"doc()doc";

static const char *__doc_OpenVDS_ConvertValue_5 = R"doc()doc";

static const char *__doc_OpenVDS_Create =
R"doc(Create a new VDS

Parameter ``options``:
    The options for the connection

Parameter ``error``:
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Create_2 =
R"doc(Create a new VDS

Parameter ``ioManager``:
    The IOManager for the connection, it will be deleted automatically
    when the VDS handle is closed

Parameter ``error``:
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_3 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_4 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_5 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_6 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_3 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_4 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_5 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_6 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_7 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_8 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_9 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_10 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_11 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_12 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_13 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_14 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_15 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_16 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_17 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_18 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_19 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_20 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataAccessor_21 = R"doc()doc";

static const char *__doc_OpenVDS_DataBlockDimensionality = R"doc()doc";

static const char *__doc_OpenVDS_DataBlockDimensionality_DataBlockDimensionality_Max = R"doc()doc";

static const char *__doc_OpenVDS_Dimensionality = R"doc()doc";

static const char *__doc_OpenVDS_Dimensionality_Dimensionality_1 = R"doc()doc";

static const char *__doc_OpenVDS_Dimensionality_Dimensionality_2 = R"doc()doc";

static const char *__doc_OpenVDS_Dimensionality_Dimensionality_3 = R"doc()doc";

static const char *__doc_OpenVDS_Dimensionality_Dimensionality_4 = R"doc()doc";

static const char *__doc_OpenVDS_Dimensionality_Dimensionality_5 = R"doc()doc";

static const char *__doc_OpenVDS_Dimensionality_Dimensionality_6 = R"doc()doc";

static const char *__doc_OpenVDS_Dimensionality_Dimensionality_Max = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND = R"doc(2D or 3D dimension group\n)doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_01 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_012 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_013 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_014 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_015 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_02 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_023 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_024 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_025 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_03 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_034 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_035 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_04 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_045 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_05 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_12 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_123 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_124 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_125 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_13 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_134 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_135 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_14 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_145 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_15 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_23 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_234 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_235 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_24 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_245 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_25 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_34 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_345 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_35 = R"doc()doc";

static const char *__doc_OpenVDS_DimensionsND_Dimensions_45 = R"doc()doc";

static const char *__doc_OpenVDS_Error = R"doc()doc";

static const char *__doc_OpenVDS_Error_code = R"doc()doc";

static const char *__doc_OpenVDS_Error_string = R"doc()doc";

static const char *__doc_OpenVDS_GetAccessManager =
R"doc(Get the VolumeDataAccessManager for a VDS

Parameter ``handle``:
    The handle of the VDS)doc";

static const char *__doc_OpenVDS_GetDataBlockDimensionality = R"doc()doc";

static const char *__doc_OpenVDS_GetDataBlockDimensionality_Dimensionality = R"doc()doc";

static const char *__doc_OpenVDS_GetLayout =
R"doc(Get the VolumeDataLayout for a VDS

Parameter ``handle``:
    The handle of the VDS)doc";

static const char *__doc_OpenVDS_IJKGridDefinition = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_IJKGridDefinition = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_IJKGridDefinition_2 = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_iUnitStep = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_jUnitStep = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_kUnitStep = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_origin = R"doc()doc";

static const char *__doc_OpenVDS_IOManager = R"doc()doc";

static const char *__doc_OpenVDS_InMemoryOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_InMemoryOpenOptions_InMemoryOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_IndexRegion = R"doc()doc";

static const char *__doc_OpenVDS_IndexRegion_IndexRegion = R"doc()doc";

static const char *__doc_OpenVDS_IndexRegion_IndexRegion_2 = R"doc()doc";

static const char *__doc_OpenVDS_IndexRegion_Max = R"doc()doc";

static const char *__doc_OpenVDS_IndexRegion_Min = R"doc()doc";

static const char *__doc_OpenVDS_InterpolatedRealType = R"doc()doc";

static const char *__doc_OpenVDS_InterpolatedRealType_2 = R"doc()doc";

static const char *__doc_OpenVDS_InterpolatedRealType_3 = R"doc()doc";

static const char *__doc_OpenVDS_InterpolatedRealType_4 = R"doc()doc";

static const char *__doc_OpenVDS_InterpolationMethod = R"doc(Interpolation method used for sampling)doc";

static const char *__doc_OpenVDS_InterpolationMethod_Angular =
R"doc(< Angular interpolation (same as linear but wraps around the value
range))doc";

static const char *__doc_OpenVDS_InterpolationMethod_Cubic = R"doc(< Cubic interpolation)doc";

static const char *__doc_OpenVDS_InterpolationMethod_Linear = R"doc(< Linear interpolation)doc";

static const char *__doc_OpenVDS_InterpolationMethod_Nearest = R"doc(< Snap to the closest sample)doc";

static const char *__doc_OpenVDS_InterpolationMethod_Triangular = R"doc(< Triangular interpolation used to interpolate heightmap data)doc";

static const char *__doc_OpenVDS_M4 = R"doc()doc";

static const char *__doc_OpenVDS_M4_data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_ClearMetadata = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_ClearMetadata_2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_CopyMetadata = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataBLOB = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataDouble = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataDoubleVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataDoubleVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataDoubleVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataFloat = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataFloatVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataFloatVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataFloatVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataInt = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataIntVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataIntVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataIntVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataString = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_GetOrCreateMetadataKey = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataBLOBAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataDoubleAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataDoubleVector2Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataDoubleVector3Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataDoubleVector4Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataFloatAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataFloatVector2Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataFloatVector3Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataFloatVector4Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataIntAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataIntVector2Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataIntVector3Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataIntVector4Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_IsMetadataStringAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_RemoveMetadataForKey = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataBLOB = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataDouble = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataDoubleVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataDoubleVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataDoubleVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataFloat = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataFloatVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataFloatVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataFloatVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataInt = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataIntVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataIntVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataIntVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_SetMetadataString = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_begin = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_end = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_blobData = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_categories = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_doubleData = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_doubleVector2Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_doubleVector3Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_doubleVector4Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_floatData = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_floatVector2Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_floatVector3Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_floatVector4Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_intData = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_intVector2Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_intVector3Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_intVector4Data = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_keys = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_names = R"doc()doc";

static const char *__doc_OpenVDS_MetadataContainer_m_stringData = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey_category = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey_name = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey_type = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess = R"doc(Interface for read access to Metadata)doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataBLOB = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataBLOB_2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataDouble = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataDoubleVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataDoubleVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataDoubleVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataFloat = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataFloatVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataFloatVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataFloatVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataInt = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataIntVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataIntVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataIntVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataString = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataBLOBAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataDoubleAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataDoubleVector2Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataDoubleVector3Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataDoubleVector4Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataFloatAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataFloatVector2Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataFloatVector3Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataFloatVector4Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataIntAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataIntVector2Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataIntVector3Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataIntVector4Available = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_IsMetadataStringAvailable = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_begin = R"doc()doc";

static const char *__doc_OpenVDS_MetadataReadAccess_end = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_BLOB = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_Double = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_DoubleVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_DoubleVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_DoubleVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_Float = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_FloatVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_FloatVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_FloatVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_Int = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_IntVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_IntVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_IntVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataType_String = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess = R"doc(Interface for write access to Metadata)doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_ClearMetadata = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_ClearMetadata_2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_CopyMetadata =
R"doc(Copy the metadata from another MetadataContainer

Parameter ``category``:
    the metadata category to copy

Parameter ``metadataReadAccess``:
    the MetadataReadAccess interface to copy from)doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataBLOB = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataBLOB_2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataDouble = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataDoubleVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataDoubleVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataDoubleVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataFloat = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataFloatVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataFloatVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataFloatVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataInt = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataIntVector2 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataIntVector3 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataIntVector4 = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataString = R"doc()doc";

static const char *__doc_OpenVDS_MetadataWriteAccess_SetMetadataString_2 = R"doc()doc";

static const char *__doc_OpenVDS_Open =
R"doc(Open an existing VDS

Parameter ``options``:
    The options for the connection

Parameter ``error``:
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Open_2 =
R"doc(Open an existing VDS

Parameter ``ioManager``:
    The IOManager for the connection, it will be deleted automatically
    when the VDS handle is closed

Parameter ``error``:
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_OpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_AWS = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_Azure = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_File = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_InMemory = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_OpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_connectionType = R"doc()doc";

static const char *__doc_OpenVDS_PitchScale = R"doc()doc";

static const char *__doc_OpenVDS_PitchScale_2 = R"doc()doc";

static const char *__doc_OpenVDS_QuantizeValueWithReciprocalScale = R"doc()doc";

static const char *__doc_OpenVDS_QuantizedTypesToFloatConverter = R"doc()doc";

static const char *__doc_OpenVDS_QuantizedTypesToFloatConverter_ConvertValue = R"doc()doc";

static const char *__doc_OpenVDS_QuantizedTypesToFloatConverter_QuantizedTypesToFloatConverter = R"doc()doc";

static const char *__doc_OpenVDS_QuantizedTypesToFloatConverter_QuantizedTypesToFloatConverter_2 = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_ConvertValue = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_QuantizingValueConverterWithNoValue = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_QuantizingValueConverterWithNoValue_2 = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_QuantizingValueConverterWithNoValue_3 = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_QuantizingValueConverterWithNoValue_4 = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_QuantizingValueConverterWithNoValue_5 = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_m_integerOffset = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_m_noValue = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_m_quantizedTypesToFloatConverter = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_m_reciprocalScale = R"doc()doc";

static const char *__doc_OpenVDS_QuantizingValueConverterWithNoValue_m_replacementNoValue = R"doc()doc";

static const char *__doc_OpenVDS_Range = R"doc()doc";

static const char *__doc_OpenVDS_Range_Max = R"doc()doc";

static const char *__doc_OpenVDS_Range_Min = R"doc()doc";

static const char *__doc_OpenVDS_Range_Range = R"doc()doc";

static const char *__doc_OpenVDS_Range_Range_2 = R"doc()doc";

static const char *__doc_OpenVDS_ReadElement = R"doc()doc";

static const char *__doc_OpenVDS_ReadElement_2 = R"doc()doc";

static const char *__doc_OpenVDS_ResultConverter = R"doc()doc";

static const char *__doc_OpenVDS_ResultConverter_ConvertValueT = R"doc()doc";

static const char *__doc_OpenVDS_ResultConverter_ReciprocalScale = R"doc()doc";

static const char *__doc_OpenVDS_VDS = R"doc()doc";

static const char *__doc_OpenVDS_VDS_2 = R"doc()doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase = R"doc()doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_SetGridDefinition =
R"doc(Sets the IJK grid definition and IJK dimension map for use in world
coordinate conversions @see VoxelIndexToWorldCoordinates @see
WorldCoordinatesToVoxelIndex @see WorldCoordinatesToVoxelIndexFloat
@see LocalIndexToWorldCoordinates @see WorldCoordinatesToLocalIndex
@param vdsIjkGridDefinition the IJK grid definition and IJK dimension
map)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_VDSCoordinateTransformerBase =
R"doc(////////////////////////// Constructors /////////////////////////////

Constructor @param cIJKGridAndDImensionMap the IJK grid definition and
IJK dimension map)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_VDSCoordinateTransformerBase_2 = R"doc(Constructor)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_VoxelIndexToWorldCoordinates =
R"doc(Converts a voxel index to world coordinates using the indexer's IJK
grid definition and IJK dimension map @param iVoxelIndex the voxel
index to convert @return the world coordinates)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_VoxelIndexToWorldCoordinates_2 =
R"doc(Converts a float voxel index to world coordinates using the indexer's
IJK grid definition and IJK dimension map @param rVoxelIndex the float
voxel index to convert @return the world coordinates)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_WorldCoordinatesToVoxelIndex =
R"doc(Converts world coordinates to a voxel index, rounding to the nearest
integer @param rWorldCoords the world coordinates to convert @param
anVoxelMin @return the voxel index)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_WorldCoordinatesToVoxelIndexFloat =
R"doc(Converts world coordinates to a float voxel index without rounding
@param rWorldCoords the world coordinates to convert @param anVoxelMin
@return the float voxel index)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_ijkDimensionMap = R"doc(< Map to determine which dimension map to I, J, and K)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_ijkToWorldTransform = R"doc(< Coordinate transfrom matrix)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_ijkToWorldTranslation = R"doc(< Coordinate translation vector)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_worldToIJKTransform = R"doc(< Coordinate transform matrix)doc";

static const char *__doc_OpenVDS_VDSCoordinateTransformerBase_worldToIJKTranslation = R"doc(< Coordinate translation vector)doc";

static const char *__doc_OpenVDS_VDSIJKGridDefinition = R"doc()doc";

static const char *__doc_OpenVDS_VDSIJKGridDefinition_VDSIJKGridDefinition = R"doc()doc";

static const char *__doc_OpenVDS_VDSIJKGridDefinition_VDSIJKGridDefinition_2 = R"doc()doc";

static const char *__doc_OpenVDS_VDSIJKGridDefinition_dimensionMap = R"doc()doc";

static const char *__doc_OpenVDS_Vector = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper_VectorWrapper = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper_data = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper_size = R"doc()doc";

static const char *__doc_OpenVDS_Vector_Vector = R"doc()doc";

static const char *__doc_OpenVDS_Vector_Vector_2 = R"doc()doc";

static const char *__doc_OpenVDS_Vector_data = R"doc()doc";

static const char *__doc_OpenVDS_Vector_operator_array = R"doc()doc";

static const char *__doc_OpenVDS_Vector_operator_array_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_3 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_AccessMode = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_AccessMode_AccessMode_Create = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_AccessMode_AccessMode_ReadOnly = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_AccessMode_AccessMode_ReadWrite = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Cancel =
R"doc(Try to cancel the request. You still have to call
WaitForCompletion/IsCanceled to make sure the buffer is not being
written to and to take the job out of the system. It is possible that
the request has completed concurrently with the call to Cancel in
which case WaitForCompletion will return true.

Parameter ``iRequestID``:
    The RequestID to cancel.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_ClearUploadErrors = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CloneVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DVolumeDataAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DVolumeDataAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DVolumeDataAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DVolumeDataAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create2DVolumeDataAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DVolumeDataAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DVolumeDataAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DVolumeDataAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DVolumeDataAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create3DVolumeDataAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DVolumeDataAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DVolumeDataAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DVolumeDataAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DVolumeDataAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Create4DVolumeDataAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateInterpolatingVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeDataPageAccessor =
R"doc(Create a volume data page accessor object for the VDS associated with
the given VolumeDataLayout object.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the VDS that the
    volume data page accessor will access.

Parameter ``eDimensionsND``:
    The dimensions group that the volume data page accessor will
    access.

Parameter ``lod``:
    The lod level that the volume data page accessor will access.

Parameter ``iChannel``:
    The channel index that the volume data page accessor will access.

Parameter ``nMaxPages``:
    The maximum number of pages that the volume data page accessor
    will cache.

Parameter ``accessMode``:
    This specifies the access mode (ReadOnly/ReadWrite/Create) of the
    volume data page accessor.

Returns:
    A VolumeDataPageAccessor object for the VDS associated with the
    given VolumeDataLayout object.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_DestroyVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_DestroyVolumeDataPageAccessor =
R"doc(Destroy a volume data page accessor object.

Parameter ``pVolumeDataPageAccessor``:
    The VolumeDataPageAccessor object to destroy.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_FlushUploadQueue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_ForceClearAllUploadErrors = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetCompletionFactor =
R"doc(Get the completion factor (between 0 and 1) of the request.

Parameter ``iRequestID``:
    The RequestID to get the completion factor of.

Returns:
    A factor (between 0 and 1) indicating how much of the request has
    been completed.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetCurrentUploadError = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetVolumeDataLayout =
R"doc(Get the VolumeDataLayout object for a VDS.

Returns:
    The VolumeDataLayout object associated with the VDS or NULL if
    there is no valid VolumeDataLayout.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetVolumeSubsetBufferSize =
R"doc(Compute the buffer size for a volume subset request.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``MinVoxelCoordinates``:
    The minimum voxel coordinates to request in each dimension
    (inclusive).

Parameter ``MaxVoxelCoordinates``:
    The maximum voxel coordinates to request in each dimension
    (exclusive).

Parameter ``eFormat``:
    Voxel format the final buffer should be in.

Parameter ``lod``:
    The lod level the requested data is read from.

Returns:
    The buffer size needed)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_IsCanceled =
R"doc(Check if a request was canceled (e.g. the VDS was invalidated before
the request was processed). If the request was canceled, the buffer
does not contain valid data.

Parameter ``iRequestID``:
    The RequestID to check for cancellation.

Returns:
    Either IsCompleted, IsCanceled or WaitForCompletion will return
    true a single time, after that the request is taken out of the
    system.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_IsCompleted =
R"doc(Check if a request completed successfully. If the request completed,
the buffer now contains valid data.

Parameter ``iRequestID``:
    The RequestID to check for completion.

Returns:
    Either IsCompleted, IsCanceled or WaitForCompletion will return
    true a single time, after that the request is taken out of the
    system.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_PrefetchVolumeChunk =
R"doc(Force production of a specific volume data chunk

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested chunk belongs to.

Parameter ``lod``:
    The lod level the requested chunk belongs to.

Parameter ``iChannel``:
    The channel index the requested chunk belongs to.

Parameter ``iChunk``:
    The index of the chunk to prefetch.

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestProjectedVolumeSubset =
R"doc(Request a subset projected from an arbitrary 3D plane through the
subset onto one of the sides of the subset.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``pBuffer``:
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by MinVoxelCoordinates and
    MaxVoxelCoordinates for the projected dimensions.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested data is read from.

Parameter ``lod``:
    The lod level the requested data is read from.

Parameter ``iChannel``:
    The channel index the requested data is read from.

Parameter ``MinVoxelCoordinates``:
    The minimum voxel coordinates to request in each dimension
    (inclusive).

Parameter ``MaxVoxelCoordinates``:
    The maximum voxel coordinates to request in each dimension
    (exclusive).

Parameter ``cVoxelPlane``:
    The plane equation for the projection from the dimension source to
    the projected dimensions (which must be a 2D subset of the source
    dimensions).

Parameter ``eProjectedDimensions``:
    The 2D dimension group that the plane in the source dimensiongroup
    is projected into. It must be a 2D subset of the source
    dimensions.

Parameter ``eInterpolationMethod``:
    Interpolation method to use when sampling the buffer.

Parameter ``eFormat``:
    Voxel format the final buffer should be in.

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestProjectedVolumeSubset_2 =
R"doc(Request a subset projected from an arbitrary 3D plane through the
subset onto one of the sides of the subset.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``pBuffer``:
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by MinVoxelCoordinates and
    MaxVoxelCoordinates for the projected dimensions.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested data is read from.

Parameter ``lod``:
    The lod level the requested data is read from.

Parameter ``iChannel``:
    The channel index the requested data is read from.

Parameter ``MinVoxelCoordinates``:
    The minimum voxel coordinates to request in each dimension
    (inclusive).

Parameter ``MaxVoxelCoordinates``:
    The maximum voxel coordinates to request in each dimension
    (exclusive).

Parameter ``cVoxelPlane``:
    The plane equation for the projection from the dimension source to
    the projected dimensions (which must be a 2D subset of the source
    dimensions).

Parameter ``eProjectedDimensions``:
    The 2D dimension group that the plane in the source dimensiongroup
    is projected into. It must be a 2D subset of the source
    dimensions.

Parameter ``eInterpolationMethod``:
    Interpolation method to use when sampling the buffer.

Parameter ``eFormat``:
    Voxel format the final buffer should be in.

Parameter ``rReplacementNoValue``:
    Value used to replace region of the input VDS that has no data.

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSamples =
R"doc(Request sampling of the input VDS at the specified coordinates.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``pBuffer``:
    Pointer to a preallocated buffer holding at least nSampleCount
    elements.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested data is read from.

Parameter ``lod``:
    The lod level the requested data is read from.

Parameter ``iChannel``:
    The channel index the requested data is read from.

Parameter ``SamplePositions``:
    Pointer to array of Dimensionality_Max-elements indicating the
    positions to sample. May be deleted once requestVolumeSamples
    return, as OpenVDS makes a deep copy of the data.

Parameter ``nSampleCount``:
    Number of samples to request.

Parameter ``eInterpolationMethod``:
    Interpolation method to use when sampling the buffer.

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSamples_2 =
R"doc(Request sampling of the input VDS at the specified coordinates.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``pBuffer``:
    Pointer to a preallocated buffer holding at least nSampleCount
    elements.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested data is read from.

Parameter ``lod``:
    The lod level the requested data is read from.

Parameter ``iChannel``:
    The channel index the requested data is read from.

Parameter ``SamplePositions``:
    Pointer to array of Dimensionality_Max-elements indicating the
    positions to sample. May be deleted once requestVolumeSamples
    return, as OpenVDS makes a deep copy of the data.

Parameter ``nSampleCount``:
    Number of samples to request.

Parameter ``eInterpolationMethod``:
    Interpolation method to use when sampling the buffer.

Parameter ``rReplacementNoValue``:
    Value used to replace region of the input VDS that has no data.

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSubset =
R"doc(Request a subset of the input VDS.

Parameter ``pBuffer``:
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by MinVoxelCoordinates and
    MaxVoxelCoordinates.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested data is read from.

Parameter ``lod``:
    The lod level the requested data is read from.

Parameter ``iChannel``:
    The channel index the requested data is read from.

Parameter ``MinVoxelCoordinates``:
    The minimum voxel coordinates to request in each dimension
    (inclusive).

Parameter ``MaxVoxelCoordinates``:
    The maximum voxel coordinates to request in each dimension
    (exclusive).

Parameter ``eFormat``:
    Voxel format the final buffer should be in.

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSubset_2 =
R"doc(Request a subset of the input VDS.

Parameter ``pBuffer``:
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by MinVoxelCoordinates and
    MaxVoxelCoordinates.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested data is read from.

Parameter ``lod``:
    The lod level the requested data is read from.

Parameter ``iChannel``:
    The channel index the requested data is read from.

Parameter ``MinVoxelCoordinates``:
    The minimum voxel coordinates to request in each dimension
    (inclusive).

Parameter ``MaxVoxelCoordinates``:
    The maximum voxel coordinates to request in each dimension
    (exclusive).

Parameter ``eFormat``:
    Voxel format the final buffer should be in.

Parameter ``rReplacementNoValue``:
    Value used to replace region of the input VDS that has no data.

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeTraces =
R"doc(Request traces from the input VDS.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``pBuffer``:
    Pointer to a preallocated buffer holding at least nTraceCount *
    number of samples in the iTraceDimension.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested data is read from.

Parameter ``lod``:
    The lod level the requested data is read from.

Parameter ``iChannel``:
    The channel index the requested data is read from.

Parameter ``TracePositions``:
    Pointer to array of nTraceCount
    VolumeDataLayout::Dimensionality_Max-elements indicating the trace
    positions.

Parameter ``nTraceCount``:
    Number of traces to request.

Parameter ``eInterpolationMethod``:
    Interpolation method to use when sampling the buffer.

Parameter ``iTraceDimension``:
    The dimension to trace

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeTraces_2 =
R"doc(Request traces from the input VDS.

Parameter ``pVolumeDataLayout``:
    The VolumeDataLayout object associated with the input VDS.

Parameter ``pBuffer``:
    Pointer to a preallocated buffer holding at least nTraceCount *
    number of samples in the iTraceDimension.

Parameter ``eDimensionsND``:
    The dimensiongroup the requested data is read from.

Parameter ``lod``:
    The lod level the requested data is read from.

Parameter ``iChannel``:
    The channel index the requested data is read from.

Parameter ``TracePositions``:
    Pointer to array of nTraceCount
    VolumeDataLayout::Dimensionality_Max-elements indicating the trace
    positions.

Parameter ``nTraceCount``:
    Number of traces to request.

Parameter ``eInterpolationMethod``:
    Interpolation method to use when sampling the buffer.

Parameter ``iTraceDimension``:
    The dimension to trace

Parameter ``rReplacementNoValue``:
    Value used to replace region of the input VDS that has no data.

Returns:
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_UploadErrorCount = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_VolumeDataAccessManager = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_WaitForCompletion =
R"doc(Wait for a request to complete successfully. If the request completed,
the buffer now contains valid data.

Parameter ``iRequestID``:
    The RequestID to wait for completion of.

Parameter ``nMillisecondsBeforeTimeout``:
    The number of milliseconds to wait before timing out (optional). A
    value of 0 indicates there is no timeout and we will wait for
    however long it takes. Note that the request is not automatically
    canceled if the wait times out, you can also use this mechanism to
    e.g. update a progress bar while waiting. If you want to cancel
    the request you have to explicitly call CancelRequest() and then
    wait for the request to stop writing to the buffer.

Returns:
    Either IsCompleted, IsCanceled or WaitForCompletion will return
    true a single time, after that the request is taken out of the
    system. Whenever WaitForCompletion returns false you need to call
    IsCanceled() to know if that was because of a timeout or if the
    request was canceled.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessorWithRegions = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessorWithRegions_CurrentRegion = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessor_GetLayout = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessor_GetManager = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessor_IndexOutOfRangeException = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessor_VolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_2 =
R"doc(Describes the number of samples, name, unit and coordinates
(annotation) of an axis (dimension) of the volume)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_CoordinateToSampleIndex =
R"doc(Convert a coordinate to a sample index (rounding to the closest index)

Parameter ``coordinate``:
    The coordinate to get the sample index of

Returns:
    The sample index of the coordinate)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetCoordinateMax =
R"doc(Get the coordinate of the last sample of this axis

Returns:
    The coordinate of the last sample of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetCoordinateMin =
R"doc(Get the coordinate of the first sample of this axis

Returns:
    The coordinate of the first sample of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetCoordinateStep =
R"doc(Get the coordiante step size per sample

Returns:
    The coordiante step size per sample)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetName =
R"doc(Get the name of this axis

Returns:
    The name of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetNumSamples =
R"doc(Get the number of samples along this axis

Returns:
    The number of samples along this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetUnit =
R"doc(Get the unit of the coordinates of this axis

Returns:
    The unit of the coordinates of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_SampleIndexToCoordinate =
R"doc(Convert a sample index on this axis to a coordinate

Parameter ``sampleIndex``:
    The sample index get the coordinate of

Returns:
    The coordinate of the sample index)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_VolumeDataAxisDescriptor = R"doc(Default constructor)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_VolumeDataAxisDescriptor_2 =
R"doc(Constructor

Parameter ``numSamples``:
    The number of samples along this axis

Parameter ``name``:
    The name of this axis

Parameter ``unit``:
    The unit of the coordinates of this axis

Parameter ``coordinateMin``:
    The coordinate of the first sample of this axis

Parameter ``coordinateMax``:
    The coordinate of the last sample of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_coordinateMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_coordinateMin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_name = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_numSamples = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_unit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_2 = R"doc(\class VolumeDataChannelDescriptor Describes a channel of a VDS)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Components = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Components_Components_1 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Components_Components_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Components_Components_4 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Flags = R"doc(Flags for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Flags_Default = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Flags_DiscreteData = R"doc(< This channel contains discrete data)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Flags_NoLossyCompression = R"doc(< Do not allow lossy compression on this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Flags_NoLossyCompressionUseZip = R"doc(< Use Zip when compressing this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Flags_NotRenderable = R"doc(< This channel is not renderable)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format_Format_1Bit = R"doc(< data is in packed 1-bit format)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format_Format_Any = R"doc(< The DataBlock can be in any format)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format_Format_R32 = R"doc(< data is in 32 bit float)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format_Format_R64 = R"doc(< data is in 64 bit double)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format_Format_U16 = R"doc(< data is in unsigned 16 bit)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format_Format_U32 = R"doc(< data is in unsigned 32 bit)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format_Format_U64 = R"doc(< data is in unsigned 64 bit)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_Format_Format_U8 = R"doc(< data is in unsigned 8 bit)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetComponents = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetFormat = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetIntegerOffset = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetIntegerScale = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetMappedValueCount = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetMapping = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetName = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetNoValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetUnit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetValueRange = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetValueRangeMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_GetValueRangeMin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_IsAllowLossyCompression = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_IsDiscrete = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_IsRenderable = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_IsUseNoValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_IsUseZipForLosslessCompression = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor =
R"doc(Named constructor for a trace mapped channel

Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``mappedValueCount``:
    When using per trace mapping, the number of values to store per
    trace

Parameter ``flags``:
    the flags for this channel

Returns:
    a trace mapped descriptor)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor_2 =
R"doc(Named constructor for a trace mapped channel

Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``mappedValueCount``:
    When using per trace mapping, the number of values to store per
    trace

Parameter ``flags``:
    the flags for this channel

Parameter ``noValue``:
    the No Value for this channel

Returns:
    a trace mapped descriptor)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_2 =
R"doc(The minimum constructor for a VolumeDataChannelDescriptor. This will
use direct mapping, default flags, and no No Value

Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_3 =
R"doc(Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``mapping``:
    the mapping for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_4 =
R"doc(Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``flags``:
    the flags for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_5 =
R"doc(Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``mapping``:
    the mapping for this channel

Parameter ``flags``:
    the flags for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_6 =
R"doc(Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``mapping``:
    the mapping for this channel

Parameter ``mappedValueCount``:
    When using per trace mapping, the number of values to store per
    trace

Parameter ``flags``:
    the flags for this channel

Parameter ``integerScale``:
    the scale to use for integer types

Parameter ``integerOffset``:
    the offset to use for integer types)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_7 =
R"doc(Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``noValue``:
    the No Value for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_8 =
R"doc(Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``noValue``:
    the No Value for this channel

Parameter ``mapping``:
    the mapping for this channel

Parameter ``flags``:
    the flags for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_9 =
R"doc(Parameter ``format``:
    the data format for this channel

Parameter ``components``:
    the vector count for this channel

Parameter ``pName``:
    the name of this channel

Parameter ``pUnit``:
    the unit of this channel

Parameter ``valueRangeMin``:
    the value range minimum of this channel

Parameter ``valueRangeMax``:
    the value range maximum of this channel

Parameter ``mapping``:
    the mapping for this channel

Parameter ``mappedValueCount``:
    When using per trace mapping, the number of values to store per
    trace

Parameter ``flags``:
    the flags for this channel

Parameter ``noValue``:
    the No Value for this channel

Parameter ``integerScale``:
    the scale to use for integer types

Parameter ``integerOffset``:
    the offset to use for integer types)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_components = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_flags = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_format = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_integerOffset = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_integerScale = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_mappedValueCount = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_mapping = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_name = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_noValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_unit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_useNoValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_m_valueRange = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_3 =
R"doc(\class VolumeDataLayout a class that contains axis and channel
information for a VDS A layout is associated with a VDS object and
contains information about axis and channels in the VDS. A
VolumeDataLayout can be invalidated when the VDS is invalidated.)doc";

static const char *__doc_OpenVDS_VolumeDataLayout_4 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize_BrickSize_1024 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize_BrickSize_128 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize_BrickSize_2048 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize_BrickSize_256 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize_BrickSize_32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize_BrickSize_4096 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize_BrickSize_512 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_BrickSize_BrickSize_64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_GetBrickSize = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_GetBrickSizeMultiplier2D = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_GetFullResolutionDimension = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_GetLODLevels = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_GetNegativeMargin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_GetPositiveMargin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_IsCreate2DLODs = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_IsForceFullResolutionDimension = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_IsValid = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_1 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_10 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_11 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_12 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_3 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_4 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_5 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_6 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_7 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_9 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_LODLevels_LODLevels_None = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_Options = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_Options_Options_Create2DLODs = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_Options_Options_ForceFullResolutionDimension = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_Options_Options_None = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_VolumeDataLayoutDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_VolumeDataLayoutDescriptor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_m_brickSize = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_m_brickSize2DMultiplier = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_m_fullResolutionDimension = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_m_lodLevels = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_m_negativeMargin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_m_options = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayoutDescriptor_m_positiveMargin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetAxisDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelComponents = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelCount = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelFormat = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelIndex = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelIntegerOffset = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelIntegerScale = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelMapping = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelName = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelNoValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelUnit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelValueRangeMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetChannelValueRangeMin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetContentsHash = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetDimensionMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetDimensionMin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetDimensionName = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetDimensionNumSamples = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetDimensionUnit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_GetDimensionality = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_IsChannelAllowingLossyCompression = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_IsChannelAvailable = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_IsChannelDiscrete = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_IsChannelRenderable = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_IsChannelUseNoValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_IsChannelUseZipForLosslessCompression = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataLayout_VolumeDataLayout = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataMapping = R"doc(Mapping volume data channels)doc";

static const char *__doc_OpenVDS_VolumeDataMapping_Direct = R"doc(< Each voxel in the volume has a value)doc";

static const char *__doc_OpenVDS_VolumeDataMapping_PerTrace =
R"doc(< Each trace in the volume has a specified number of values. A trace
is the entire length of dimension 0)doc";

static const char *__doc_OpenVDS_VolumeDataPage = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_3 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_AddReference = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_Commit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_CreatePage = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetChannelDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetChannelIndex = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetChunkCount = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetChunkIndex = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetChunkMinMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetChunkMinMaxExcludingMargin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetLOD = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetLayout = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetMaxPages = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetNumSamples = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_ReadPage = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_ReadPageAtPosition = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_RemoveReference = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_SetMaxPages = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_VolumeDataPageAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetBuffer = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetMinMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetMinMaxExcludingMargin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetWritableBuffer = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_Release = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_UpdateWrittenRegion = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_VolumeDataPage = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_GetValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_Cancel = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_Commit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_SetValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRegions = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRegions_Region = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRegions_RegionCount = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRegions_RegionFromIndex = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerBase = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_BitMaskFromBitDataIndex =
R"doc(Gets the bit mask for a bit data index\n Used with the BitDataIndex
functions to read and write 1Bit data\n\n Common usage:\n bool bit =
buffer[iBitDataIndex >> 3] & BitMaskFromBitDataIndex(iBitDataIndex) !=
0; @param iBitDataIndex the local index to compute the mask from
@return the bit mask for the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_BitMaskFromLocalChunkIndex =
R"doc(Gets the bit mask for a local chunk index\n Used with the BitDataIndex
functions to read and write 1Bit data\n\n Common usage:\n bool bit =
buffer[LocalChunkIndexToBitDataIndex(iLocalChunkIndex) / 8] &
BitMaskFromLocalChunkIndex(iLocalChunkIndex) != 0; @see
LocalChunkIndexToBitDataIndex @param iLocalChunkIndex the local index
to compute the mask from @return the bit mask for the local chunk
index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_BitMaskFromLocalIndex =
R"doc(Gets the bit mask for a local index\n Used with the BitDataIndex
functions to read and write 1Bit data\n\n Common usage:\n bool bit =
buffer[LocalIndexToBitDataIndex(iLocalIndex) / 8] &
BitMaskFromLocalIndex(iLocalIndex) != 0; @see LocalIndexToBitDataIndex
@param iLocalIndex the local index to compute the mask from @return
the bit mask for the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_BitMaskFromVoxelIndex =
R"doc(Gets the bit mask for a voxel index\n Used with the BitDataIndex
functions to read and write 1Bit data\n\n Common usage:\n bool bit =
buffer[VoxelIndexToBitDataIndex(iVoxelIndex) / 8] &
BitMaskFromVoxelIndex(iVoxelIndex) != 0; @see VoxelIndexToBitDataIndex
@param iVoxelIndex the voxel index to compute the mask from @return
the bit mask for the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_BitValueFromBitDataIndex =
R"doc(Convenience function for reading the bit value from a 1Bit DataBlock
buffer at a local index @see BitMaskFromBitDataIndex @param data the
1Bit buffer to read @param iBitDataIndex the bit index to read @return
the binary value read)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_BitValueFromLocalChunkIndex =
R"doc(Convenience function for reading the bit value from a 1Bit DataBlock
buffer at a local chunk index @see LocalChunkIndexToBitDataIndex @see
BitMaskFromLocalChunkIndex @param data the 1Bit buffer to read @param
iLocalChunkIndex the local chunk index to read @return the binary
value read)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_BitValueFromLocalIndex =
R"doc(Convenience function for reading the bit value from a 1Bit DataBlock
buffer at a local index @see LocalIndexToBitDataIndex @see
BitMaskFromLocalIndex @param data the 1Bit buffer to read @param
iLocalIndex the local index to read @return the binary value read)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_BitValueFromVoxelIndex =
R"doc(Convenience function for reading the bit value from a 1Bit DataBlock
buffer at a voxel index @see VoxelIndexToBitDataIndex @see
BitMaskFromVoxelIndex @param data the 1Bit buffer to read @param
iVoxelIndex the voxel index to read @return the binary value read)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_CoordinateToLocalIndex =
R"doc(Converts coordinates to a local index, rounding to the nearest integer
@param rCoordinate the coordinates to convert @return the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_CoordinateToLocalIndexFloat =
R"doc(Converts coordinates to a float local index without rounding @param
rCoordinate the coordinates to convert @return the float local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_CoordinateToVolumeSamplerLocalIndex =
R"doc(Converts coordinates to a volume sampler local index. The volume
sampler works like OpenGL textures so the first sample starts at 0.5.

@param rCoordinate the coordinates to convert @return the float volume
sampler local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_CoordinateToVoxelIndex =
R"doc(Converts a coordinate for a specific volume dimension to a voxel
index, rounding to the nearest integer @param rCoordinate the
coordinate to convert @param iDimension the volume dimension @return
the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_CoordinateToVoxelIndexFloat =
R"doc(Converts a coordinate for a specific volume dimension to a float voxel
index without rounding @param rCoordinate the coordinate to convert
@param iDimension the volume dimension @return the float voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_CreateTempBufferIndexer =
R"doc(Creates an indexer for a temp buffer from voxel minimum and
maximums.\n @param anVoxelMin voxel minimum for this indexer @param
anVoxelMax voxel maximum for this indexer @param iLOD the LOD for this
indexer @return the created indexer)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_CreateTempBufferIndexer_2 =
R"doc(Create a temp buffer indexer with the same area and configuration as
another indexer. @param indexer the indexer to copy configuration and
area from @return the created indexer)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_CreateTempBufferIndexer_3 =
R"doc(Create a temp buffer indexer with the same configuration as another
indexer, but a new voxel min and max @param indexer the indexer to
copy configuration from @param anNewVoxelMin the voxel minimum for the
created indexer @param anNewVoxleMax the voxel maximum for the created
indexer @return the created indexer)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_GetCoordinateStep =
R"doc(Gets the coordinate step for the given volume dimension at LOD 0
@param iDimension the volume dimension @return the coordinate step at
LOD 0)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_GetDataBlockNumSamples =
R"doc(Gets the number of samples for a dimension in the DataBlock @param
iDimension the DataBlock dimension @return the number of samples in
the dimension)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_GetLocalChunkNumSamples =
R"doc(Get the number of samples for a dimension in the volume @param
iDimension the volume dimension @return the number of samples in the
dimension)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalChunkIndexInProcessArea =
R"doc(Checks if a local chunk index is within the chunk this indexer was
created with @param iLocalChunkIndex the local chunk index to check
@return true if the index is within this chunk, false otherwise)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalChunkIndexToBitDataIndex =
R"doc(Converts a local chunk index to a bit data index\n Used with the
BitMask functions to read and write 1Bit data @see
BitMaskFromLocalChunkIndex @param iLocalChunkIndex the local chunk
index to convert @return the buffer offset (in number of bits) for the
local chunk index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalChunkIndexToDataIndex =
R"doc(Converts a local chunk index to a data index @param iLocalChunkIndex
the local chunk index to convert @return the buffer offset for the
local chunk index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalChunkIndexToLocalIndex =
R"doc(Converts a local chunk index to a local index @param iLocalChunkIndex
the local chunk index to convert @return the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalChunkIndexToVoxelIndex =
R"doc(Converts a local chunk index to a voxel index @param iLocalChunkIndex
the local chunk index to convert @return the local voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexInProcessArea =
R"doc(Checks if a local index is within the DataBlock this indexer was
created with @param iLocalIndex the local index to check @return true
if the index is within this chunk, false otherwise)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexToBitDataIndex =
R"doc(Converts a local index to a bit data index\n Used with the BitMask
functions to read and write 1Bit data @see BitMaskFromLocalIndex
@param iLocalIndex the local index to convert @return the buffer
offset (in number of bits) for the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexToCoordinate =
R"doc(Converts a local index to coordinates @param iLocalIndex the local
index to convert @return the coordinates)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexToDataIndex =
R"doc(Converts a local index to a data index @param iLocalIndex the local
index to convert @return the buffer offset for the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexToLocalChunkIndex =
R"doc(Converts a local index to a local chunk index @param iLocalIndex the
local index to convert @return the local chunk index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexToRelativeAxisPosition =
R"doc(Converts a local index to a relative position (between 0 and 1) along
the volume axes @param iLocalIndex the local index to convert @return
the relative position along volume axes)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexToVoxelIndex =
R"doc(Converts a local index to a voxel index @param iLocalIndex the local
index to convert @return the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexToVoxelIndexFloat =
R"doc(Converts a float local index to a float voxel index @param iLocalIndex
the float local index to convert @return the float voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_LocalIndexToWorldCoordinates =
R"doc(Converts a local index to world coordinates using the indexer's IJK
grid definition and IJK dimension map @param iLocalIndex the local
index to convert @return the world coordinates)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_RelativeAxisPositionToLocalIndex =
R"doc(Converts a relative position along the volume axes to a local index
@param rPosition the relative postion to convert @return the local
index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_RelativeAxisPositionToVoxelIndex =
R"doc(Converts a relative volume axis position to a voxel index, rounding to
the nears integer @param rPosition the axis position @iDimension the
volume dimension @return the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VolumeIndexerBase = R"doc(////////////////////////// Constructors /////////////////////////////)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexFloatToCoordinate =
R"doc(Converts a float voxel index for a specific volume dimension to a
coordinate value @param rVoxelIndex the voxel index to convert @param
iDimension the volume dimension @return the coordinate)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexInProcessArea =
R"doc(Checks if a voxel index is within the chunk this indexer was created
with @param iVoxelIndex the voxel index to check @return true if the
index is within this chunk, false otherwise)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToBitDataIndex =
R"doc(Converts a voxel index to a bit data index\n Used with the BitMask
functions to read and write 1Bit data @see BitMaskFromVoxelIndex
@param iVoxelIndex the voxel index to convert @return the buffer
offset (in number of bits) for the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToCoordinate =
R"doc(Converts a voxel index for a specific volume dimension to a coordinate
value @param iVoxelIndex the voxel index to convert @param iDimension
the volume dimension @return the coordinate)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToDataIndex =
R"doc(Converts a voxel index to a data index @param iVoxelIndex the voxel
index to convert @return the buffer offset for the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToLocalChunkIndex =
R"doc(Converts a voxel index to a local chunk index @param iVoxelIndex the
voxel index to convert @return the local chunk index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToLocalIndex =
R"doc(Converts a voxel index to a local index @param iVoxelIndex the voxel
index to convert @return the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToLocalIndexFloat =
R"doc(Converts a float voxel index to a float local index @param iVoxelIndex
the float voxel index to convert @return the float local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToRelativeAxisPosition =
R"doc(Converts a voxel index for a specific dimension to a relative position
(between 0 and 1) along a volume axis @param iVoxelIndex the voxel
index to convert @param iDimension the volume dimension @return the
relative position along the iDimension axis)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToVolumeSamplerLocalIndex =
R"doc(Converts a float voxel index to a volume sampler local index. The
volume sampler works like OpenGL textures so the first sample starts
at 0.5.

@param iVoxelIndex the float voxel index to convert @return the float
volume sampler local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToWorldCoordinates =
R"doc(Converts a voxel index to world coordinates using the indexer's IJK
grid definition and IJK dimension map @param iVoxelIndex the voxel
index to convert @return the world coordinates)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToWorldCoordinates_2 =
R"doc(Converts a float voxel index to world coordinates using the indexer's
IJK grid definition and IJK dimension map @param rVoxelIndex the float
voxel index to convert @return the world coordinates)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_WorldCoordinatesToLocalIndex =
R"doc(Converts world coordinates to a local index, rounding to the nearest
integer @param rWorldCoords the world coordinates to convert @return
the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_WorldCoordinatesToVoxelIndex =
R"doc(Converts world coordinates to a voxel index, rounding to the nearest
integer @param rWorldCoords the world coordinates to convert @return
the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_WorldCoordinatesToVoxelIndexFloat =
R"doc(Converts world coordinates to a float voxel index without rounding
@param rWorldCoords the world coordinates to convert @return the float
voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerData = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerData_VolumeIndexerData = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerData_axisNumSamples =
R"doc(< Total number of samples in volume dimensions for whole volume (not
just this chunk))doc";

static const char *__doc_OpenVDS_VolumeIndexerData_bitPitch = R"doc(< Pitch for bits in volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_coordinateMax = R"doc(< Maximum coordinates in volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_coordinateMin = R"doc(< Minimum coordinates in volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_dataBlockAllocatedSize = R"doc(< Allocated size of buffer in DataBlock dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_dataBlockBitPitch = R"doc(< Pitch for bits in DataBlock dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_dataBlockPitch = R"doc(< Pitch for buffer in DataBlock dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_dataBlockSamples = R"doc(< Number of sample in DataBlock dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_dimensionMap = R"doc(< Map from DataBlock dimensions to volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_isDimensionLODDecimated = R"doc(< Indicates if a volume dimension is decimated with the LOD or not)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_localChunkAllocatedSize = R"doc(< Allocated size of buffer in volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_localChunkSamples = R"doc(< Number of samples in volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_lod = R"doc(< Level of Detail for data being indexed)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_pitch = R"doc(< Pitch for buffer in volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_valueRangeMax = R"doc(< The value range maximum for the volume)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_valueRangeMin = R"doc(< The value range minimum for the volume)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_voxelMax = R"doc(< Maximum voxel range in volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeIndexerData_voxelMin = R"doc(< Minimum voxel range in volume dimensions)doc";

static const char *__doc_OpenVDS_VolumeSampler = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_Clamp = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_GetCubicInterpolationWeights = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_IsHeightValid = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_Sample2D =
R"doc(Sample the given buffer at the the given 2D index\n The returned TREAL
type is double when T is double, int32, or int64 and float for all
other types

Parameter ``ptBuffer``:
    pointer to the buffer to be samples

Parameter ``localIndex``:
    the local 2D index into the buffer

Returns:
    a TREAL sampled using the ::InterpolationType at the given index)doc";

static const char *__doc_OpenVDS_VolumeSampler_Sample3D =
R"doc(Sample the given buffer at the the given 3D index\n The returned TREAL
type is double when T is double, int32, or int64 and float for all
other types

Parameter ``ptBuffer``:
    pointer to the buffer to be samples

Parameter ``localIndex``:
    the local 3D index into the buffer

Returns:
    a TREAL sampled using the ::InterpolationType at the given index)doc";

static const char *__doc_OpenVDS_VolumeSampler_VolumeSampler = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_VolumeSampler_2 =
R"doc(Parameter ``anSize``:
    the 6D size of the buffer to be sampled

Parameter ``anPitch``:
    the pitch of the buffer to be sampled

Parameter ``rangeMin``:
    the value range minimum of the data to be sampled

Parameter ``rangemax``:
    the value range maximum of the data to be sampled

Parameter ``noValalue``:
    the no value for the data to be sampled

Parameter ``replacementNoValue``:
    the value to replace any NoValues with)doc";

static const char *__doc_OpenVDS_VolumeSampler_WrapToClosestAngle = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_WrapToRange = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_noValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_pitchX = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_pitchY = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_pitchZ = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_quantizedTypesToFloatConverter = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_rangeMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_rangeMin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_replacementNoValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_sizeX = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_sizeY = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_m_sizeZ = R"doc()doc";

static const char *__doc_OpenVDS_fastInvert = R"doc()doc";

static const char *__doc_OpenVDS_operator_bor = R"doc()doc";

static const char *__doc_OpenVDS_operator_bor_2 = R"doc()doc";

static const char *__doc_OpenVDS_operator_eq = R"doc()doc";

static const char *__doc_OpenVDS_rangeSize = R"doc()doc";

static const char *__doc_std_hash = R"doc()doc";

static const char *__doc_std_hash_operator_call = R"doc()doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

