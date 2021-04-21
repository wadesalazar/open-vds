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


static const char *__doc_OpenVDS_AWSOpenOptions = R"doc(Options for opening a VDS in AWS)doc";

static const char *__doc_OpenVDS_AWSOpenOptions_AWSOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_AWSOpenOptions_2 =
R"doc(AWSOpenOptions constructor

Parameters:
-----------

bucket :
    The bucket of the VDS

key :
    The key prefix of the VDS

region :
    The region of the bucket of the VDS

endpointOverride :
    This parameter allows to override the endpoint url

connectionTimeoutMs :
    This parameter allows to override the time a connection can spend
    on connecting to AWS

requestTimeoutMs :
    This paramter allows to override the time a request can take)doc";

static const char *__doc_OpenVDS_AWSOpenOptions_accessKeyId = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_bucket = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_connectionTimeoutMs = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_endpointOverride = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_expiration = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_key = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_logFilenamePrefix = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_loglevel = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_region = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_requestTimeoutMs = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_secretKey = R"doc()doc";

static const char *__doc_OpenVDS_AWSOpenOptions_sessionToken = R"doc()doc";

static const char *__doc_OpenVDS_AzureOpenOptions = R"doc(Options for opening a VDS in Azure)doc";

static const char *__doc_OpenVDS_AzureOpenOptions_AzureOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_AzureOpenOptions_AzureOpenOptions_2 =
R"doc(AzureOpenOptions constructor

Parameters:
-----------

connectionString :
    The connectionString for the VDS

container :
    The container of the VDS

blob :
    The blob prefix of the VDS)doc";

static const char *__doc_OpenVDS_AzureOpenOptions_AzureOpenOptions_3 =
R"doc(AzureOpenOptions constructor

Parameters:
-----------

connectionString :
    The connectionString for the VDS

container :
    The container of the VDS

blob :
    The blob prefix of the VDS

parallelism_factor :
    The parallelism factor setting for the Azure Blob Storage library

max_execution_time :
    The max execution time setting for the Azure Blob Storage library)doc";

static const char *__doc_OpenVDS_AzureOpenOptions_AzureOpenOptionsBearer =
R"doc(AzureOpenOptions factory function for bearer token based
authentication

Parameters:
-----------

bearerToken :
    The bearer token

container :
    The container of the VDS

blob :
    The blob prefix of the VDS

Returns:
--------
    A valid AzureOpenOptions)doc";

static const char *__doc_OpenVDS_AzureOpenOptions_accountName = R"doc()doc";

static const char *__doc_OpenVDS_AzureOpenOptions_bearerToken = R"doc()doc";

static const char *__doc_OpenVDS_AzureOpenOptions_blob = R"doc()doc";

static const char *__doc_OpenVDS_AzureOpenOptions_connectionString = R"doc()doc";

static const char *__doc_OpenVDS_AzureOpenOptions_container = R"doc()doc";

static const char *__doc_OpenVDS_AzureOpenOptions_max_execution_time = R"doc()doc";

static const char *__doc_OpenVDS_AzureOpenOptions_parallelism_factor = R"doc()doc";

static const char *__doc_OpenVDS_AzurePresignedOpenOptions = R"doc(Options for opening a VDS with presigned Azure url)doc";

static const char *__doc_OpenVDS_AzurePresignedOpenOptions_AzurePresignedOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_AzurePresignedOpenOptions_AzurePresignedOpenOptions_2 =
R"doc(AzurePresignedOpenOptions constructor

Parameters:
-----------

baseUrl :
    The base url for the VDS

urlSuffix :
    The suffix of the presigned url)doc";

static const char *__doc_OpenVDS_AzurePresignedOpenOptions_baseUrl = R"doc()doc";

static const char *__doc_OpenVDS_AzurePresignedOpenOptions_urlSuffix = R"doc()doc";

static const char *__doc_OpenVDS_CalculateNoise2D = R"doc()doc";

static const char *__doc_OpenVDS_CalculateNoise3D = R"doc()doc";

static const char *__doc_OpenVDS_CalculateNoise4D = R"doc()doc";

static const char *__doc_OpenVDS_Close =
R"doc(Close a VDS and free up all associated resources

Parameters:
-----------

handle :
    The handle of the VDS)doc";

static const char *__doc_OpenVDS_CompressionMethod = R"doc()doc";

static const char *__doc_OpenVDS_CompressionMethod_IsWavelet = R"doc()doc";

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
R"doc(Create a new VDS.

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

connectionString :
    The cloud provider specific connection string Specifies additional
    arguments for the cloud provider

compressionMethod :
    The overall compression method to be used for the VDS. The channel
    descriptors can have additional options to control how a channel
    is compressed.

compressionTolerance :
    This property specifies the compression tolerance [1..255] when
    using the wavelet compression method. This value is the maximum
    deviation from the original data value when the data is converted
    to 8-bit using the value range. A value of 1 means the maximum
    allowable loss is the same as quantizing to 8-bit (but the average
    loss will be much much lower than quantizing to 8-bit). It is not
    a good idea to directly relate the tolerance to the quality of the
    compressed data, as the average loss will in general be an order
    of magnitude lower than the allowable loss.

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Create_2 =
R"doc(Create a new VDS.

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

connectionString :
    The cloud provider specific connection string Specifies additional
    arguments for the cloud provider

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Create_3 =
R"doc(Create a new VDS. This is a simple wrapper that uses an empty
connectionString

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

compressionMethod :
    The overall compression method to be used for the VDS. The channel
    descriptors can have additional options to control how a channel
    is compressed.

compressionTolerance :
    This property specifies the compression tolerance [1..255] when
    using the wavelet compression method. This value is the maximum
    deviation from the original data value when the data is converted
    to 8-bit using the value range. A value of 1 means the maximum
    allowable loss is the same as quantizing to 8-bit (but the average
    loss will be much much lower than quantizing to 8-bit). It is not
    a good idea to directly relate the tolerance to the quality of the
    compressed data, as the average loss will in general be an order
    of magnitude lower than the allowable loss.

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Create_4 =
R"doc(Create a new VDS. This is a simple wrapper that uses an empty
connectionString

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Create_5 =
R"doc(Create a new VDS.

Parameters:
-----------

options :
    The options for the connection

compressionMethod :
    The overall compression method to be used for the VDS. The channel
    descriptors can have additional options to control how a channel
    is compressed.

compressionTolerance :
    This property specifies the compression tolerance [1..255] when
    using the wavelet compression method. This value is the maximum
    deviation from the original data value when the data is converted
    to 8-bit using the value range. A value of 1 means the maximum
    allowable loss is the same as quantizing to 8-bit (but the average
    loss will be much much lower than quantizing to 8-bit). It is not
    a good idea to directly relate the tolerance to the quality of the
    compressed data, as the average loss will in general be an order
    of magnitude lower than the allowable loss.

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Create_6 =
R"doc(Create a new VDS.

Parameters:
-----------

options :
    The options for the connection

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Create_7 =
R"doc(Create a new VDS.

Parameters:
-----------

ioManager :
    The IOManager for the connection, it will be deleted automatically
    when the VDS handle is closed

compressionMethod :
    The overall compression method to be used for the VDS. The channel
    descriptors can have additional options to control how a channel
    is compressed.

compressionTolerance :
    This property specifies the compression tolerance [1..255] when
    using the wavelet compression method. This value is the maximum
    deviation from the original data value when the data is converted
    to 8-bit using the value range. A value of 1 means the maximum
    allowable loss is the same as quantizing to 8-bit (but the average
    loss will be much much lower than quantizing to 8-bit). It is not
    a good idea to directly relate the tolerance to the quality of the
    compressed data, as the average loss will in general be an order
    of magnitude lower than the allowable loss.

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Create_8 =
R"doc(Create a new VDS.

Parameters:
-----------

ioManager :
    The IOManager for the connection, it will be deleted automatically
    when the VDS handle is closed

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_3 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_4 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_5 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_6 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_7 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_8 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_9 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_10 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_11 = R"doc()doc";

static const char *__doc_OpenVDS_CreateInterpolatingVolumeDataAccessor_12 = R"doc()doc";

static const char *__doc_OpenVDS_CreateOpenOptions =
R"doc(Create an OpenOptions struct from a url and connection string

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

connectionString :
    The cloud provider specific connection string Specifies additional
    arguments for the cloud provider <param name="error"> If an error
    occured, the error code and message will be written to this output
    parameter

Returns:
--------
    This function news a OpenOptions struct that has to be deleted by
    the caller. This is a helper function to allow applications modify
    the OpenOption before passing it to Open. Use the Open and Create
    functions with url and string instead if this is not needed.)doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_3 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_4 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_5 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_6 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_7 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_8 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_9 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_10 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_11 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_12 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_13 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_14 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_15 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_16 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_17 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_18 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_19 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_20 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_21 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_22 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_23 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_24 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_25 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_26 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_27 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_28 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_29 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_30 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_31 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_32 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_33 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_34 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_35 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_36 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_37 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_38 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_39 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_40 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_41 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadAccessor_42 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_3 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_4 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_5 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_6 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_7 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_8 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_9 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_10 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_11 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_12 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_13 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_14 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_15 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_16 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_17 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_18 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_19 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_20 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_21 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_22 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_23 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_24 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_25 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_26 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_27 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_28 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_29 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_30 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_31 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_32 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_33 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_34 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_35 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_36 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_37 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_38 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_39 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_40 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_41 = R"doc()doc";

static const char *__doc_OpenVDS_CreateVolumeDataReadWriteAccessor_42 = R"doc()doc";

static const char *__doc_OpenVDS_DMSOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_DMSOpenOptions_DMSOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_DMSOpenOptions_DMSOpenOptions_2 = R"doc()doc";

static const char *__doc_OpenVDS_DMSOpenOptions_datasetPath = R"doc()doc";

static const char *__doc_OpenVDS_DMSOpenOptions_logLevel = R"doc()doc";

static const char *__doc_OpenVDS_DMSOpenOptions_sdApiKey = R"doc()doc";

static const char *__doc_OpenVDS_DMSOpenOptions_sdAuthorityUrl = R"doc()doc";

static const char *__doc_OpenVDS_DMSOpenOptions_sdToken = R"doc()doc";

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

static const char *__doc_OpenVDS_Exception = R"doc()doc";

static const char *__doc_OpenVDS_Exception_GetErrorMessage = R"doc()doc";

static const char *__doc_OpenVDS_Exception_what = R"doc()doc";

static const char *__doc_OpenVDS_FatalException = R"doc()doc";

static const char *__doc_OpenVDS_FatalException_FatalException = R"doc()doc";

static const char *__doc_OpenVDS_FatalException_GetErrorMessage = R"doc()doc";

static const char *__doc_OpenVDS_FatalException_m_errorMessage = R"doc()doc";

static const char *__doc_OpenVDS_GenericDispatcher = R"doc()doc";

static const char *__doc_OpenVDS_GenericDispatcher_1 = R"doc()doc";

static const char *__doc_OpenVDS_GetAccessManager =
R"doc(Get the VolumeDataAccessManager for a VDS

Parameters:
-----------

handle :
    The handle of the VDS

Returns:
--------
    The VolumeDataAccessManager of the VDS)doc";

static const char *__doc_OpenVDS_GetAccessManagerInterface =
R"doc(Get the VolumeDataAccessManagerInterface for a VDS

Parameters:
-----------

handle :
    The handle of the VDS

Returns:
--------
    The VolumeDataAccessManagerInterface of the VDS)doc";

static const char *__doc_OpenVDS_GetGlobalState =
R"doc(Get the GlobalState interface

Returns:
--------
    A pointer to the GlobalState interface)doc";

static const char *__doc_OpenVDS_GetLODSize =
R"doc(Get the number of voxels at a particular LOD from a voxel range
(ranges are exclusive).

Parameters:
-----------

voxelMin :
    The minimum voxel index of the range.

voxelMax :
    The maximum voxel index of the range (ranges are exclusive, so the
    range does not include voxelMax).

includePartialUpperVoxel :
    This controls the rounding. Usually you want the size of the range
    to be all voxels in the range, but when we are copying from
    multiple adjacent ranges we only want to round up for the last
    one.

Returns:
--------
    The number of voxels at the given LOD, at LOD 0 the result is
    voxelMax - voxelMin.)doc";

static const char *__doc_OpenVDS_GetLayout =
R"doc(Get the VolumeDataLayout for a VDS

Parameters:
-----------

handle :
    The handle of the VDS

Returns:
--------
    The VolumeDataLayout of the VDS)doc";

static const char *__doc_OpenVDS_GlobalState = R"doc(Object that contains global runtime data)doc";

static const char *__doc_OpenVDS_GlobalState_2 = R"doc()doc";

static const char *__doc_OpenVDS_GlobalState_GetBytesDecompressed =
R"doc(Get the global amount of decompressed bytes. This amount might be
smaller than the amount of downloaded bytes because of a small header
pr chunk. It can also be larger for non compressed data sets since
chunks can be cached.

Parameters:
-----------

connectionType :
    $Returns:
--------

Amount of decompressed bytes served the process.)doc";

static const char *__doc_OpenVDS_GlobalState_GetBytesDownloaded =
R"doc(Get the global amount of downloaded bytes from a cloud vendor.

Parameters:
-----------

connectionType :
    The counter to be retireved.

Returns:
--------
    Global number of bytes downloaded from the connection. This does
    not include any http header data.)doc";

static const char *__doc_OpenVDS_GlobalState_GetChunksDecompressed =
R"doc(Get the global count of decompressed chunks.

Parameters:
-----------

connectionType :
    The counter to be retireved.

Returns:
--------
    Number of chunks decompressed.)doc";

static const char *__doc_OpenVDS_GlobalState_GetChunksDownloaded =
R"doc(Get the global count of downloaded chunks.

Parameters:
-----------

connectionType :
    The counter to be retireved.

Returns:
--------
    Number of chunks downloaded.)doc";

static const char *__doc_OpenVDS_GoogleCredentialsJson =
R"doc(Credentials for opening a VDS in Google Cloud Storage by the string
containing json with credentials Using OAuth)doc";

static const char *__doc_OpenVDS_GoogleCredentialsJson_GoogleCredentialsJson =
R"doc(GoogleCredentialsJson constructor

Parameters:
-----------

json :
    The string containing json with credentials)doc";

static const char *__doc_OpenVDS_GoogleCredentialsJson_GoogleCredentialsJson_2 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsJson_json = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsPath =
R"doc(Credentials for opening a VDS in Google Cloud Storage by path to the
service account json file Using OAuth)doc";

static const char *__doc_OpenVDS_GoogleCredentialsPath_GoogleCredentialsPath =
R"doc(GoogleCredentialsPath constructor

Parameters:
-----------

path :
    The path to the service account json file)doc";

static const char *__doc_OpenVDS_GoogleCredentialsPath_GoogleCredentialsPath_2 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsPath_path = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrl =
R"doc(Credentials for opening a VDS in Google Cloud Storage by using the
default credentials Using signed URL mechanism)doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlJson =
R"doc(Credentials for opening a VDS in Google Cloud Storage by the string
containing json with credentials Using signed URL mechanism)doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlJson_GoogleCredentialsSignedUrlJson =
R"doc(GoogleCredentialsSignedUrlJson constructor

Parameters:
-----------

region :
    The string containing the region required for signature generation

json :
    The string containing json with credentials)doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlJson_GoogleCredentialsSignedUrlJson_2 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlJson_GoogleCredentialsSignedUrlJson_3 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlJson_GoogleCredentialsSignedUrlJson_4 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlJson_json = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlJson_region = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlPath =
R"doc(Credentials for opening a VDS in Google Cloud Storage by path to the
service account json file Using signed URL mechanism)doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlPath_GoogleCredentialsSignedUrlPath =
R"doc(GoogleCredentialsSignedUrlPath constructor

Parameters:
-----------

region :
    The string containing the region required for signature generation

path :
    The path to the service account json file)doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlPath_GoogleCredentialsSignedUrlPath_2 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlPath_GoogleCredentialsSignedUrlPath_3 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlPath_GoogleCredentialsSignedUrlPath_4 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlPath_path = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrlPath_region = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrl_GoogleCredentialsSignedUrl =
R"doc(GoogleCredentialsSignedUrl constructor

Parameters:
-----------

region :
    The string containing the region required for signature generation)doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrl_GoogleCredentialsSignedUrl_2 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsSignedUrl_region = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsToken =
R"doc(Credentials for opening a VDS in Google Cloud Storage by using the
string containing an access token Using OAuth)doc";

static const char *__doc_OpenVDS_GoogleCredentialsToken_GoogleCredentialsToken =
R"doc(GoogleCredentialsToken constructor

Parameters:
-----------

token :
    The string containing an access token)doc";

static const char *__doc_OpenVDS_GoogleCredentialsToken_GoogleCredentialsToken_2 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleCredentialsToken_token = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions = R"doc(Options for opening a VDS in Google Cloud Storage)doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_CredentialsType = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_CredentialsType_AccessToken = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_CredentialsType_Default = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_CredentialsType_Json = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_CredentialsType_Path = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_CredentialsType_SignedUrl = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_CredentialsType_SignedUrlJson = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_CredentialsType_SignedUrlPath = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_GoogleOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_GoogleOpenOptions_2 =
R"doc(GoogleOpenOptions constructor

Parameters:
-----------

bucket :
    The bucket of the VDS

pathPrefix :
    The prefix of the VDS

credentials :
    Google Cloud Storage access credentials)doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_GoogleOpenOptions_3 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_GoogleOpenOptions_4 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_GoogleOpenOptions_5 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_GoogleOpenOptions_6 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_GoogleOpenOptions_7 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_GoogleOpenOptions_8 = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_SetSignedUrl = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_bucket = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_credentials = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_credentialsType = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_pathPrefix = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_region = R"doc()doc";

static const char *__doc_OpenVDS_GoogleOpenOptions_storageClass = R"doc()doc";

static const char *__doc_OpenVDS_HttpOpenOptions =
R"doc(Options for opening a VDS with a plain http url. If there are query
parameters in then they will be appended to the different sub urls.
The resulting IO backend will not support uploading data.)doc";

static const char *__doc_OpenVDS_HttpOpenOptions_HttpOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_HttpOpenOptions_HttpOpenOptions_2 =
R"doc(HttpOpenOptions constructor

Parameters:
-----------

url :
    The http base url of the VDS)doc";

static const char *__doc_OpenVDS_HttpOpenOptions_url = R"doc()doc";

static const char *__doc_OpenVDS_IHasVolumeDataAccess = R"doc()doc";

static const char *__doc_OpenVDS_IHasVolumeDataAccess_GetVolumeDataAccessManager = R"doc()doc";

static const char *__doc_OpenVDS_IHasVolumeDataAccess_IHasVolumeDataAccess = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_IJKGridDefinition = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_IJKGridDefinition_2 = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_iUnitStep = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_jUnitStep = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_kUnitStep = R"doc()doc";

static const char *__doc_OpenVDS_IJKGridDefinition_origin = R"doc()doc";

static const char *__doc_OpenVDS_IOManager = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_2 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_AddRef = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Cancel =
R"doc(Try to cancel the request. You still have to call
WaitForCompletion/IsCanceled to make sure the buffer is not being
written to and to take the job out of the system. It is possible that
the request has completed concurrently with the call to Cancel in
which case WaitForCompletion will return True.

Parameters:
-----------

requestID :
    The RequestID to cancel.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_CancelAndWaitForCompletion =
R"doc(Cancel the request and wait for it to complete. This call will block
until the request has completed so you can be sure the buffer is not
being written to and the job is taken out of the system.

Parameters:
-----------

requestID :
    The RequestID to cancel.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_ClearUploadErrors = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_CloneVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DInterpolatingVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DVolumeDataAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DVolumeDataAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DVolumeDataAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DVolumeDataAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create2DVolumeDataAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DInterpolatingVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DVolumeDataAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DVolumeDataAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DVolumeDataAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DVolumeDataAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create3DVolumeDataAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DInterpolatingVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DVolumeDataAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DVolumeDataAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DVolumeDataAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DVolumeDataAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DVolumeDataAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DVolumeDataAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Create4DVolumeDataAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_CreateVolumeDataPageAccessor =
R"doc(Create a VolumeDataPageAccessor object for the VDS.

Parameters:
-----------

dimensionsND :
    The dimensions group that the volume data page accessor will
    access.

LOD :
    The LOD level that the volume data page accessor will access.

channel :
    The channel index that the volume data page accessor will access.

maxPages :
    The maximum number of pages that the volume data page accessor
    will cache.

accessMode :
    This specifies the access mode (ReadOnly/ReadWrite/Create) of the
    volume data page accessor.

chunkMetadataPageSize :
    The chunk metadata page size of the layer. This controls how many
    chunk metadata entries are written per page, and is only used when
    the access mode is Create. If this number is too low it will
    degrade performance, but in certain situations it can be
    advantageous to make this number a multiple of the number of
    chunks in some of the dimensions. Do not change this from the
    default (1024) unless you know exactly what you are doing.

Returns:
--------
    A VolumeDataPageAccessor object for the VDS.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_DestroyVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_DestroyVolumeDataPageAccessor =
R"doc(Destroy a volume data page accessor object.

Parameters:
-----------

volumeDataPageAccessor :
    The VolumeDataPageAccessor object to destroy.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_FlushUploadQueue =
R"doc(Flush any pending writes and write updated layer status

Parameters:
-----------

writeUpdatedLayerStatus :
    Write the updated layer status (or only flush pending writes of
    chunks and chunk-metadata).)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_ForceClearAllUploadErrors = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetCompletionFactor =
R"doc(Get the completion factor (between 0 and 1) of the request.

Parameters:
-----------

requestID :
    The RequestID to get the completion factor of.

Returns:
--------
    A factor (between 0 and 1) indicating how much of the request has
    been completed.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetCurrentDownloadError = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetCurrentUploadError = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetProjectedVolumeSubsetBufferSize =
R"doc(Compute the buffer size (in bytes) for a projected volume subset
request.

Parameters:
-----------

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

projectedDimensions :
    The 2D dimension group that the plane in the source dimensiongroup
    is projected into. It must be a 2D subset of the source
    dimensions.

format :
    Voxel format of the destination buffer.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

Returns:
--------
    The buffer size needed.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetVDSProduceStatus =
R"doc(Get the produce status for the specific DimensionsND/LOD/Channel
combination.

Parameters:
-----------

dimensionsND :
    The dimensions group we're getting the produce status for.

LOD :
    The LOD level we're getting the produce status for.

channel :
    The channel index we're getting the produce status for.

Returns:
--------
    The produce status for the specific DimensionsND/LOD/Channel
    combination.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetVolumeDataLayout =
R"doc(Get the VolumeDataLayout object for a VDS.

Returns:
--------
    The VolumeDataLayout object associated with the VDS or NULL if
    there is no valid VolumeDataLayout.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetVolumeSamplesBufferSize =
R"doc(Compute the buffer size (in bytes) for a volume samples request.

Parameters:
-----------

sampleCount :
    Number of samples to request.

channel :
    The channel index the requested data is read from.

Returns:
--------
    The buffer size needed.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetVolumeSubsetBufferSize =
R"doc(Compute the buffer size (in bytes) for a volume subset request.

Parameters:
-----------

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

format :
    Voxel format of the destination buffer.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

Returns:
--------
    The buffer size needed.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_GetVolumeTracesBufferSize =
R"doc(Compute the buffer size (in bytes) for a volume traces request.

Parameters:
-----------

traceCount :
    Number of traces to request.

traceDimension :
    The dimension to trace

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

Returns:
--------
    The buffer size needed.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_IVolumeDataAccessManager = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_IsCanceled =
R"doc(Check if a request was canceled (e.g. the VDS was invalidated before
the request was processed). If the request was canceled, the buffer
does not contain valid data.

Parameters:
-----------

requestID :
    The RequestID to check for cancellation.

Returns:
--------
    Either IsCompleted, IsCanceled or WaitForCompletion will return
    True a single time, after that the request is taken out of the
    system.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_IsCompleted =
R"doc(Check if a request completed successfully. If the request completed,
the buffer now contains valid data.

Parameters:
-----------

requestID :
    The RequestID to check for completion.

Returns:
--------
    Either IsCompleted, IsCanceled or WaitForCompletion will return
    True a single time, after that the request is taken out of the
    system.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_PrefetchVolumeChunk =
R"doc(Force production of a specific volume data chunk

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested chunk belongs to.

LOD :
    The LOD level the requested chunk belongs to.

channel :
    The channel index the requested chunk belongs to.

chunkIndex :
    The index of the chunk to prefetch.

Returns:
--------
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_RefCount = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_Release = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_RequestProjectedVolumeSubset =
R"doc(Request a subset projected from an arbitrary 3D plane through the
subset onto one of the sides of the subset.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by minVoxelCoordinates and
    maxVoxelCoordinates for the projected dimensions.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

voxelPlane :
    The plane equation for the projection from the dimension source to
    the projected dimensions (which must be a 2D subset of the source
    dimensions).

projectedDimensions :
    The 2D dimension group that the plane in the source dimensiongroup
    is projected into. It must be a 2D subset of the source
    dimensions.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

format :
    Voxel format of the destination buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_RequestVolumeSamples =
R"doc(Request sampling of the input VDS at the specified coordinates.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least sampleCount
    elements.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

samplePositions :
    Pointer to array of VolumeDataLayout::Dimensionality_Max-elements
    indicating the positions to sample. May be deleted once
    RequestVolumeSamples return, as HueSpace makes a deep copy of the
    data.

sampleCount :
    Number of samples to request.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_RequestVolumeSubset =
R"doc(Request a subset of the input VDS.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by minVoxelCoordinates and
    maxVoxelCoordinates.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

format :
    Voxel format of the destination buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_RequestVolumeTraces =
R"doc(Request traces from the input VDS.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least traceCount *
    number of samples in the traceDimension.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

tracePositions :
    Pointer to array of traceCount
    VolumeDataLayout::Dimensionality_Max-elements indicating the trace
    positions.

traceCount :
    Number of traces to request.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

traceDimension :
    The dimension to trace

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    The RequestID which can be used to query the status of the
    request, cancel the request or wait for the request to complete.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_UploadErrorCount = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessManager_WaitForCompletion =
R"doc(Wait for a request to complete successfully. If the request completed,
the buffer now contains valid data.

Parameters:
-----------

requestID :
    The RequestID to wait for completion of.

millisecondsBeforeTimeout :
    The number of milliseconds to wait before timing out (optional). A
    value of 0 indicates there is no timeout and we will wait for
    however long it takes. Note that the request is not automatically
    canceled if the wait times out, you can also use this mechanism to
    e.g. update a progress bar while waiting. If you want to cancel
    the request you have to explicitly call CancelRequest() and then
    wait for the request to stop writing to the buffer.

Returns:
--------
    Either IsCompleted, IsCanceled or WaitForCompletion will return
    True a single time, after that the request is taken out of the
    system. Whenever WaitForCompletion returns False you need to call
    IsCanceled() to know if that was because of a timeout or if the
    request was canceled.)doc";

static const char *__doc_OpenVDS_IVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessorWithRegions = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessorWithRegions_CurrentRegion = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessor_GetLayout = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessor_GetManager = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessor_IVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessor_Manager = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessor_Manager_CloneVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessor_Manager_DestroyVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataAccessor_Manager_Manager = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataReadAccessor = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataReadAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataReadAccessor_GetValue = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataReadWriteAccessor = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataReadWriteAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataReadWriteAccessor_Cancel = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataReadWriteAccessor_Commit = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataReadWriteAccessor_SetValue = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataRegions = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataRegions_Region = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataRegions_RegionCount = R"doc()doc";

static const char *__doc_OpenVDS_IVolumeDataRegions_RegionFromIndex = R"doc()doc";

static const char *__doc_OpenVDS_InMemoryOpenOptions = R"doc(Options for opening a VDS which is stored in memory (for testing))doc";

static const char *__doc_OpenVDS_InMemoryOpenOptions_InMemoryOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_InMemoryOpenOptions_InMemoryOpenOptions_2 = R"doc()doc";

static const char *__doc_OpenVDS_InMemoryOpenOptions_InMemoryOpenOptions_3 = R"doc()doc";

static const char *__doc_OpenVDS_InMemoryOpenOptions_name = R"doc()doc";

static const char *__doc_OpenVDS_IndexOutOfRangeException = R"doc()doc";

static const char *__doc_OpenVDS_IndexOutOfRangeException_GetErrorMessage = R"doc()doc";

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

static const char *__doc_OpenVDS_InvalidArgument = R"doc()doc";

static const char *__doc_OpenVDS_InvalidArgument_GetErrorMessage = R"doc()doc";

static const char *__doc_OpenVDS_InvalidArgument_GetParameterName = R"doc()doc";

static const char *__doc_OpenVDS_InvalidArgument_InvalidArgument = R"doc()doc";

static const char *__doc_OpenVDS_InvalidArgument_m_errorMessage = R"doc()doc";

static const char *__doc_OpenVDS_InvalidArgument_m_parameterName = R"doc()doc";

static const char *__doc_OpenVDS_InvalidOperation = R"doc()doc";

static const char *__doc_OpenVDS_InvalidOperation_GetErrorMessage = R"doc()doc";

static const char *__doc_OpenVDS_InvalidOperation_InvalidOperation = R"doc()doc";

static const char *__doc_OpenVDS_InvalidOperation_m_errorMessage = R"doc()doc";

static const char *__doc_OpenVDS_IsCompressionMethodSupported =
R"doc(Check if a compression method is supported. Not all compression
methods might be supported when creating VDSs, and this method checks
if a particular compression methods is supported by this
implementation.

Parameters:
-----------

compressionMethod :
    The compression method to check

Returns:
--------
    True if the compression method is supported when creating VDSs
    with this implementation.)doc";

static const char *__doc_OpenVDS_IsSupportedProtocol =
R"doc(Verifies that the url is a supported protocol

Returns:
--------
    Returnes True if the protocol specifier of the url is recognised
    by OpenVDS, otherwise returns False)doc";

static const char *__doc_OpenVDS_KnownAxisNames = R"doc()doc";

static const char *__doc_OpenVDS_KnownAxisNames_Crossline =
R"doc(String representing the name of the axis corresponding to the
crossline spacing.)doc";

static const char *__doc_OpenVDS_KnownAxisNames_Depth =
R"doc(String representing the name of the axis corresponding to the negative
z direction.)doc";

static const char *__doc_OpenVDS_KnownAxisNames_I =
R"doc(String representing the name of the axis corresponding to the I step
vector (SurveyCoordinateSystemIStep))doc";

static const char *__doc_OpenVDS_KnownAxisNames_Inline =
R"doc(String representing the name of the axis corresponding to the inline
spacing.)doc";

static const char *__doc_OpenVDS_KnownAxisNames_J =
R"doc(String representing the name of the axis corresponding to the J step
vector (SurveyCoordinateSystemIStep))doc";

static const char *__doc_OpenVDS_KnownAxisNames_K =
R"doc(String representing the name of the axis corresponding to the K step
vector (SurveyCoordinateSystemKStep))doc";

static const char *__doc_OpenVDS_KnownAxisNames_Sample =
R"doc(String representing the name of the axis corresponding to the negative
z direction.)doc";

static const char *__doc_OpenVDS_KnownAxisNames_Time =
R"doc(String representing the name of the axis corresponding to the negative
z direction.)doc";

static const char *__doc_OpenVDS_KnownAxisNames_X =
R"doc(String representing the name of the axis that maps directly to the X
coordinate in the XYZ coordinate system)doc";

static const char *__doc_OpenVDS_KnownAxisNames_Y =
R"doc(String representing the name of the axis that maps directly to the Y
coordinate in the XYZ coordinate system)doc";

static const char *__doc_OpenVDS_KnownAxisNames_Z =
R"doc(String representing the name of the axis that maps directly to the Z
coordinate in the XYZ coordinate system)doc";

static const char *__doc_OpenVDS_KnownMetadata = R"doc()doc";

static const char *__doc_OpenVDS_KnownMetadata_CategoryImportInformation =
R"doc(The metadata in the ImportInformation category contains information
about the initial import to VDS. That is, information about the
original file (file name, last modification time etc.) and when/how it
was imported. The intended use is e.g. to give a default file name for
an export operation or to inform the user about whether the VDS was
imported from some particular file.)doc";

static const char *__doc_OpenVDS_KnownMetadata_CategorySEGY =
R"doc(The metadata in the SEGY category is intended to capture information
required to re-create the original SEG-Y file. It is only present if
the VDS was imported from a SEG-Y file.)doc";

static const char *__doc_OpenVDS_KnownMetadata_CategorySurveyCoordinateSystem =
R"doc(The metadata in the survey coordinate system category contains either
information used to transform from an Inline/Crossline annotation
system to XY (Easting, Northing) coordinates or information used to
transform from an I/J/K annotation system to XYZ coordinates. <para>
In the Inline/Crossline system, the step vector for dimension 0 (Which
can be named either Time, Depth or Sample) is always negative Z
direction. The origin is the position of (inline, crossline) (0, 0),
the inline spacing is the (X, Y) step when the inline position
increments by 1 and the crossline spacing is the (X, Y) step when the
crossline position increments by 1. </para> <para> In the I/J/K
system, all three dimensions have 3D step vectors allowing for any
affine tranform to XYZ coordinates. The IJK origin is the position of
(I, J, K) (0, 0, 0), the I step vector is the (X, Y, Z) step when the
I position increments by 1, the J step vector is the (X, Y, Z) step
when the J position increments by 1 and the K step vector is the (X,
Y, Z) step when the K position increments by 1. </para> <para> It is
also possible to define the X, Y, Z coordinates directly by using
X/Y/Z as the axis names. </para> <para> Any dimensions which have
names that are not recognized or are missing the Spacing metadata will
be given a unit vector perpendicular to the recognized dimensions.
</para>)doc";

static const char *__doc_OpenVDS_KnownMetadata_CategoryTraceCoordinates =
R"doc(The metadata in the TraceCoordinates category is used for positioning
of 2D seismic. It relates to the 'Trace' dimension of the VDS where
the annotation coordinate is used to look up in the arrays in this
category. This allows using the same positions with subsetted and
interpolated or decimated data.)doc";

static const char *__doc_OpenVDS_KnownMetadata_CategoryWrittenRegion =
R"doc(The metadata in the WrittenRegion category is used to indicate which
area is written in a sparsely populated VDS. The guarantee is that
values outside the written area will be NoValue if UseNoValue is True,
and ValueRange.Min if UseNoValue is False.)doc";

static const char *__doc_OpenVDS_KnownMetadata_EnergySourcePointNumbers =
R"doc(An array of scalar int32 values defining the energy source point
number for each trace.)doc";

static const char *__doc_OpenVDS_KnownMetadata_EnsembleNumbers =
R"doc(An array of scalar int32 values defining the ensemble number for each
trace.)doc";

static const char *__doc_OpenVDS_KnownMetadata_ImportInformationDisplayName =
R"doc(An informative name that can be displayed to a user but is not
necessarily a valid file name.)doc";

static const char *__doc_OpenVDS_KnownMetadata_ImportInformationImportTimeStamp = R"doc(The time in ISO8601 format when the data was imported to VDS.)doc";

static const char *__doc_OpenVDS_KnownMetadata_ImportInformationInputFileName =
R"doc(The original input file name. In cases where the input is not a simple
file this should still be a valid file name that can be used as the
default for a subsequent export operation.)doc";

static const char *__doc_OpenVDS_KnownMetadata_ImportInformationInputFileSize =
R"doc(The total size (in bytes) of the input file(s), which is an integer
stored as a double because there is no 64-bit integer metadata type.)doc";

static const char *__doc_OpenVDS_KnownMetadata_ImportInformationInputTimeStamp = R"doc(The last modified time of the input in ISO8601 format.)doc";

static const char *__doc_OpenVDS_KnownMetadata_SEGYBinaryHeader = R"doc(The original binary header of the SEG-Y file)doc";

static const char *__doc_OpenVDS_KnownMetadata_SEGYDataEndianness =
R"doc(The endianness of the data samples in the SEG-Y file if different from
the header endianness. This is a non-standard feature, but some SEG-Y
files use it. (0=BigEndian)(1=LittleEndian))doc";

static const char *__doc_OpenVDS_KnownMetadata_SEGYDataSampleFormatCode =
R"doc(The data sample format code of the data samples imported from the
SEG-Y file. (0=Unknown)(1=IBMFloat)(2=Int32)(3=Int16)(4=FixedPoint)(5=
IEEEFloat)(6=IEEEDouble)(7=Int24)(8=Int8)(9=Int64)(10=UInt32)(11=UInt1
6)(12=UInt64)(15=UInt24)(16=UInt8))doc";

static const char *__doc_OpenVDS_KnownMetadata_SEGYEndianness = R"doc(The endianness of the SEG-Y file (0=BigEndian)(1=LittleEndian))doc";

static const char *__doc_OpenVDS_KnownMetadata_SEGYPrimaryKey =
R"doc(The primary key (i.e. sort order) of the original SEG-Y file. VDSs are
expected to transpose crossline-oriented seismic to use the standard
(Sample, Crossline, Inline) axis triple, so in order to export the
file in the original order we need to know which dimension to loop
over.)doc";

static const char *__doc_OpenVDS_KnownMetadata_SEGYTextHeader = R"doc(The original text header of the SEG-Y file)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemCRSWkt = R"doc(The well-known text representation of the coordinate reference system)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemCrosslineSpacing = R"doc(The XY spacing between units in the Crossline annotation dimension.)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint0 =
R"doc(An optional reference point in the grid, the annotation coordinates
for this point are GridPoint0Inline and GridPoint0Crossline)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint0Crossline = R"doc(The inline annotation coordinate for the point defined by GridPoint0)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint0Inline = R"doc(The inline annotation coordinate for the point defined by GridPoint0)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint1 =
R"doc(An optional reference point in the grid, the annotation coordinates
for this point are GridPoint1Inline and GridPoint1Crossline)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint1Crossline = R"doc(The inline annotation coordinate for the point defined by GridPoint1)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint1Inline = R"doc(The inline annotation coordinate for the point defined by GridPoint1)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint2 =
R"doc(An optional reference point in the grid, the annotation coordinates
for this point are GridPoint2Inline and GridPoint2Crossline)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint2Crossline = R"doc(The inline annotation coordinate for the point defined by GridPoint2)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint2Inline = R"doc(The inline annotation coordinate for the point defined by GridPoint2)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint3 =
R"doc(An optional reference point in the grid, the annotation coordinates
for this point are GridPoint3Inline and GridPoint3Crossline)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint3Crossline = R"doc(The inline annotation coordinate for the point defined by GridPoint3)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemGridPoint3Inline = R"doc(The inline annotation coordinate for the point defined by GridPoint3)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemIJKOrigin =
R"doc(The XYZ position of the origin of the annotation (I/J/K) coordinate
system.)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemIStepVector = R"doc(The step vector corresponding to dimension named 'I')doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemInlineSpacing = R"doc(The XY spacing between units in the Inline annotation dimension.)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemJStepVector = R"doc(The step vector corresponding to dimension named 'J')doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemKStepVector = R"doc(The step vector corresponding to dimension named 'K')doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemOrigin =
R"doc(The XY position of the origin of the annotation
(Inline/Crossline/Time) coordinate system.)doc";

static const char *__doc_OpenVDS_KnownMetadata_SurveyCoordinateSystemUnit = R"doc(The unit of the coordinate system)doc";

static const char *__doc_OpenVDS_KnownMetadata_TracePositions =
R"doc(An array of DoubleVector2 defining the position for each trace, where
(0, 0) is treated as an undefined position.)doc";

static const char *__doc_OpenVDS_KnownMetadata_TraceVerticalOffsets =
R"doc(An array of doubles defining the offset for each trace from the
vertical start position in the Time/Depth/Sample dimension of the VDS)doc";

static const char *__doc_OpenVDS_KnownMetadata_WrittenRegion =
R"doc(An array of scalar int32 values defining the 6D min and max (NDBox) of
the written region)doc";

static const char *__doc_OpenVDS_KnownUnitNames = R"doc()doc";

static const char *__doc_OpenVDS_KnownUnitNames_FeetPerSecond = R"doc(String representing the feet per second unit)doc";

static const char *__doc_OpenVDS_KnownUnitNames_Foot = R"doc(String representing the foot unit)doc";

static const char *__doc_OpenVDS_KnownUnitNames_Meter = R"doc(String representing the SI meter unit)doc";

static const char *__doc_OpenVDS_KnownUnitNames_MetersPerSecond = R"doc(String representing the meters per second unit)doc";

static const char *__doc_OpenVDS_KnownUnitNames_Millisecond = R"doc(String representing the SI milliseconds unit)doc";

static const char *__doc_OpenVDS_KnownUnitNames_Second = R"doc(String representing the SI second unit)doc";

static const char *__doc_OpenVDS_KnownUnitNames_USSurveyFeetPerSecond = R"doc(String representing the US survey feet per second unit)doc";

static const char *__doc_OpenVDS_KnownUnitNames_USSurveyFoot = R"doc(String representing the US survey foot unit)doc";

static const char *__doc_OpenVDS_KnownUnitNames_Unitless = R"doc(String representing unitless)doc";

static const char *__doc_OpenVDS_M4 = R"doc()doc";

static const char *__doc_OpenVDS_M4_data = R"doc()doc";

static const char *__doc_OpenVDS_MessageBufferException = R"doc()doc";

static const char *__doc_OpenVDS_MessageBufferException_AddToBuffer = R"doc()doc";

static const char *__doc_OpenVDS_MessageBufferException_MESSAGE_BUFFER_SIZE = R"doc()doc";

static const char *__doc_OpenVDS_MessageBufferException_MessageBufferException = R"doc()doc";

static const char *__doc_OpenVDS_MessageBufferException_m_messageBuffer = R"doc()doc";

static const char *__doc_OpenVDS_MessageBufferException_m_usedSize = R"doc()doc";

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

static const char *__doc_OpenVDS_MetadataContainer_GetMetadataKeys = R"doc()doc";

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

static const char *__doc_OpenVDS_MetadataKey = R"doc(A metadata key uniquely identifies a metadata item)doc";

static const char *__doc_OpenVDS_MetadataKeyRange =
R"doc(A range of metadata keys that can be iterated over using range-based
'for')doc";

static const char *__doc_OpenVDS_MetadataKeyRange_MetadataKeyRange = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKeyRange_begin = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKeyRange_end = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKeyRange_m_begin = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKeyRange_m_end = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey_GetCategory =
R"doc(Get the category of metadata that this key identifies

Returns:
--------
    The category of the metadata that this key identifies)doc";

static const char *__doc_OpenVDS_MetadataKey_GetName =
R"doc(Get the name of metadata that this key identifies

Returns:
--------
    The name of the metadata that this key identifies)doc";

static const char *__doc_OpenVDS_MetadataKey_GetType =
R"doc(Get the type of metadata that this key identifies

Returns:
--------
    The type of the metadata that this key identifies)doc";

static const char *__doc_OpenVDS_MetadataKey_MetadataKey = R"doc(Default constructor)doc";

static const char *__doc_OpenVDS_MetadataKey_MetadataKey_2 =
R"doc(Constructor

Parameters:
-----------

type :
    The type of the metadata that this key identifies. The possible
    types are (Int, Float, Double, {Int,Float,Double}Vector{2,3,4},
    String or BLOB).

category :
    The category of the metadata that this key identifies. A category
    is a collection of related metadata items, e.g.
    SurveyCoordinateSystem consists of Origin, InlineSpacing,
    CrosslineSpacing and Unit metadata.

name :
    The name of the metadata that this key identifies)doc";

static const char *__doc_OpenVDS_MetadataKey_m_category = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey_m_name = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey_m_type = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey_operator_eq = R"doc()doc";

static const char *__doc_OpenVDS_MetadataKey_operator_ne = R"doc()doc";

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

static const char *__doc_OpenVDS_MetadataReadAccess_GetMetadataKeys = R"doc()doc";

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

Parameters:
-----------

category :
    the metadata category to copy

metadataReadAccess :
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

static const char *__doc_OpenVDS_NoiseKernel = R"doc()doc";

static const char *__doc_OpenVDS_NoiseKernel_Do = R"doc()doc";

static const char *__doc_OpenVDS_Open =
R"doc(Open an existing VDS

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

connectionString :
    The cloud provider specific connection string Specifies additional
    arguments for the cloud provider

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Open_2 =
R"doc(Open an existing VDS. This is a simple wrapper that uses an empty
connectionString

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Open_3 =
R"doc(Open an existing VDS

Parameters:
-----------

options :
    The options for the connection

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_Open_4 =
R"doc(Open an existing VDS

Parameters:
-----------

ioManager :
    The IOManager for the connection, it will be deleted automatically
    when the VDS handle is closed

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_OpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_AWS = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_Azure = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_AzurePresigned = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_ConnectionTypeCount = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_DMS = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_GoogleStorage = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_Http = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_InMemory = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_Other = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_ConnectionType_VDSFile = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_OpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_OpenOptions_2 = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_connectionType = R"doc()doc";

static const char *__doc_OpenVDS_OpenOptions_waveletAdaptiveMode =
R"doc(< This property (only relevant when using Wavelet compression) is used
to control how the wavelet adaptive compression determines which level
of wavelet compressed data to load. Depending on the setting, either
the global or local WaveletAdaptiveTolerance or the
WaveletAdaptiveRatio can be used.)doc";

static const char *__doc_OpenVDS_OpenOptions_waveletAdaptiveRatio =
R"doc(< Wavelet adaptive ratio, this setting will be used whenever the
WavletAdaptiveMode is set to Ratio. A compression ratio of 5.0
corresponds to compressed data which is 20% of the original.)doc";

static const char *__doc_OpenVDS_OpenOptions_waveletAdaptiveTolerance =
R"doc(< Wavelet adaptive tolerance, this setting will be used whenever the
WavletAdaptiveMode is set to Tolerance.)doc";

static const char *__doc_OpenVDS_OpenWithAdaptiveCompressionRatio =
R"doc(Open an existing VDS with adaptive compression ratio.

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

connectionString :
    The cloud provider specific connection string Specifies additional
    arguments for the cloud provider

waveletAdaptiveRatio :
    Wavelet adaptive ratio. This will try to read the dataset as-if it
    was compressed with the given ratio even if it was compressed with
    a lower ratio or lossless. A compression ratio of 5.0 corresponds
    to compressed data which is 20% of the original.

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_OpenWithAdaptiveCompressionTolerance =
R"doc(Open an existing VDS with adaptive compression tolerance.

Parameters:
-----------

url :
    The url scheme specific to each cloud provider Available schemes
    are s3:// azure://

connectionString :
    The cloud provider specific connection string Specifies additional
    arguments for the cloud provider

waveletAdaptiveTolerance :
    Wavelet adaptive tolerance. This will try to read the dataset as-
    if it was compressed with the given tolerance even if it was
    compressed with a lower tolerance or lossless.

error :
    If an error occured, the error code and message will be written to
    this output parameter

Returns:
--------
    The VDS handle that can be used to get the VolumeDataLayout and
    the VolumeDataAccessManager)doc";

static const char *__doc_OpenVDS_PitchScale =
R"doc(Template function to get the scalar for the pitch value (which is in
bytes for 1-bit data) used to scale the pitch to get the number of
elements

Returns:
--------
    The scale factor for the pitch for the given type)doc";

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

static const char *__doc_OpenVDS_ReadElement =
R"doc(Read element from buffer. This function has a template specialisation
for bool making it suitable to read single bits from a buffer with
packed bits.

Parameters:
-----------

buffer :
    Buffer to read from

element :
    The element index to read from buffer

Returns:
--------
    Element at position "element" is returned.)doc";

static const char *__doc_OpenVDS_ReadElement_2 = R"doc()doc";

static const char *__doc_OpenVDS_ReadErrorException = R"doc()doc";

static const char *__doc_OpenVDS_ReadErrorException_GetErrorCode = R"doc()doc";

static const char *__doc_OpenVDS_ReadErrorException_GetErrorMessage = R"doc()doc";

static const char *__doc_OpenVDS_ReadErrorException_ReadErrorException = R"doc()doc";

static const char *__doc_OpenVDS_ReadErrorException_errorCode = R"doc()doc";

static const char *__doc_OpenVDS_ReadErrorException_message = R"doc()doc";

static const char *__doc_OpenVDS_RequestFormat = R"doc()doc";

static const char *__doc_OpenVDS_RequestFormat_2 = R"doc()doc";

static const char *__doc_OpenVDS_RequestFormat_3 = R"doc()doc";

static const char *__doc_OpenVDS_RequestFormat_4 = R"doc()doc";

static const char *__doc_OpenVDS_RequestFormat_5 = R"doc()doc";

static const char *__doc_OpenVDS_RequestFormat_6 = R"doc()doc";

static const char *__doc_OpenVDS_ResultConverter = R"doc()doc";

static const char *__doc_OpenVDS_ResultConverter_ConvertValueT = R"doc()doc";

static const char *__doc_OpenVDS_ResultConverter_ReciprocalScale = R"doc()doc";

static const char *__doc_OpenVDS_SimplexNoise = R"doc()doc";

static const char *__doc_OpenVDS_StringWrapper = R"doc()doc";

static const char *__doc_OpenVDS_StringWrapper_StringWrapper = R"doc()doc";

static const char *__doc_OpenVDS_StringWrapper_StringWrapper_2 = R"doc()doc";

static const char *__doc_OpenVDS_StringWrapper_StringWrapper_3 = R"doc()doc";

static const char *__doc_OpenVDS_StringWrapper_data = R"doc()doc";

static const char *__doc_OpenVDS_StringWrapper_size = R"doc()doc";

static const char *__doc_OpenVDS_VDS = R"doc()doc";

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

static const char *__doc_OpenVDS_VDSFileOpenOptions = R"doc(Options for opening a VDS file)doc";

static const char *__doc_OpenVDS_VDSFileOpenOptions_VDSFileOpenOptions = R"doc()doc";

static const char *__doc_OpenVDS_VDSFileOpenOptions_VDSFileOpenOptions_2 =
R"doc(VDSFileOpenOptions constructor

Parameters:
-----------

fileName :
    The name of the VDS file)doc";

static const char *__doc_OpenVDS_VDSFileOpenOptions_fileName = R"doc()doc";

static const char *__doc_OpenVDS_VDSIJKGridDefinition = R"doc()doc";

static const char *__doc_OpenVDS_VDSIJKGridDefinition_VDSIJKGridDefinition = R"doc()doc";

static const char *__doc_OpenVDS_VDSIJKGridDefinition_VDSIJKGridDefinition_2 = R"doc()doc";

static const char *__doc_OpenVDS_VDSIJKGridDefinition_dimensionMap = R"doc()doc";

static const char *__doc_OpenVDS_VDSProduceStatus = R"doc()doc";

static const char *__doc_OpenVDS_VDSProduceStatus_Normal = R"doc()doc";

static const char *__doc_OpenVDS_VDSProduceStatus_Remapped = R"doc()doc";

static const char *__doc_OpenVDS_VDSProduceStatus_Unavailable = R"doc()doc";

static const char *__doc_OpenVDS_Vector = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper_VectorWrapper = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper_VectorWrapper_2 = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper_data = R"doc()doc";

static const char *__doc_OpenVDS_VectorWrapper_size = R"doc()doc";

static const char *__doc_OpenVDS_Vector_Vector = R"doc()doc";

static const char *__doc_OpenVDS_Vector_Vector_2 = R"doc()doc";

static const char *__doc_OpenVDS_Vector_data = R"doc()doc";

static const char *__doc_OpenVDS_Vector_operator_array = R"doc()doc";

static const char *__doc_OpenVDS_Vector_operator_array_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_ClearUploadErrors = R"doc(Clear all upload errors that have been retrieved)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateInterpolatingVolumeDataAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateInterpolatingVolumeDataAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DInterpolatingAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DInterpolatingAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DInterpolatingAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DInterpolatingAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessor1Bit_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorU16_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorU32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorU64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadAccessorU8_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessor1Bit_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorU16_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorU32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorU64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData2DReadWriteAccessorU8_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DInterpolatingAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DInterpolatingAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DInterpolatingAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DInterpolatingAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessor1Bit_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorU16_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorU32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorU64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadAccessorU8_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessor1Bit_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorU16_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorU32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorU64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData3DReadWriteAccessorU8_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DInterpolatingAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DInterpolatingAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DInterpolatingAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DInterpolatingAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessor1Bit_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorU16_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorU32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorU64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadAccessorU8_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessor1Bit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessor1Bit_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorR32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorR32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorR64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorR64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorU16 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorU16_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorU32 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorU32_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorU64 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorU64_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorU8 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeData4DReadWriteAccessorU8_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeDataPageAccessor =
R"doc(Create a VolumeDataPageAccessor object for the VDS.

Parameters:
-----------

dimensionsND :
    The dimensions group that the volume data page accessor will
    access.

LOD :
    The LOD level that the volume data page accessor will access.

channel :
    The channel index that the volume data page accessor will access.

maxPages :
    The maximum number of pages that the volume data page accessor
    will cache.

accessMode :
    This specifies the access mode (ReadOnly/ReadWrite/Create) of the
    volume data page accessor.

chunkMetadataPageSize :
    The chunk metadata page size of the layer. This controls how many
    chunk metadata entries are written per page, and is only used when
    the access mode is Create. If this number is too low it will
    degrade performance, but in certain situations it can be
    advantageous to make this number a multiple of the number of
    chunks in some of the dimensions. Do not change this from the
    default (1024) unless you know exactly what you are doing.

Returns:
--------
    A VolumeDataPageAccessor object for the VDS.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeDataReadAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeDataReadAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeDataReadWriteAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_CreateVolumeDataReadWriteAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_DestroyVolumeDataPageAccessor =
R"doc(Destroy a volume data page accessor object.

Parameters:
-----------

volumeDataPageAccessor :
    The VolumeDataPageAccessor object to destroy.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_Dispose = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_DoRequestProjectedVolumeSubset = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_DoRequestVolumeSamples = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_DoRequestVolumeSubset = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_DoRequestVolumeTraces = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_EnsureValid = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_FlushUploadQueue =
R"doc(Flush any pending writes and write updated layer status

Parameters:
-----------

writeUpdatedLayerStatus :
    Write the updated layer status (or only flush pending writes of
    chunks and chunk-metadata).)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_ForceClearAllUploadErrors = R"doc(Clear all upload errors)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetCurrentDownloadError = R"doc(Get the download error from the most recent operation that failed)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetCurrentUploadError =
R"doc(Get the next unretrieved upload error or an empty error if there are
no more errors to retrieve)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetProjectedVolumeSubsetBufferSize =
R"doc(Compute the buffer size (in bytes) for a projected volume subset
request.

Parameters:
-----------

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

projectedDimensions :
    The 2D dimension group that the plane in the source dimensiongroup
    is projected into. It must be a 2D subset of the source
    dimensions.

format :
    Voxel format of the destination buffer.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

Returns:
--------
    The buffer size needed.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetProjectedVolumeSubsetBufferSize_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetVDSProduceStatus =
R"doc(Get the produce status for the specific DimensionsND/LOD/Channel
combination.

Parameters:
-----------

dimensionsND :
    The dimensions group we're getting the produce status for.

LOD :
    The LOD level we're getting the produce status for.

channel :
    The channel index we're getting the produce status for.

Returns:
--------
    The produce status for the specific DimensionsND/LOD/Channel
    combination.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetVolumeDataLayout =
R"doc(Get the VolumeDataLayout object for a VDS.

Returns:
--------
    The VolumeDataLayout object associated with the VDS or NULL if
    there is no valid VolumeDataLayout.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetVolumeSamplesBufferSize =
R"doc(Compute the buffer size (in bytes) for a volume samples request.

Parameters:
-----------

sampleCount :
    Number of samples to request.

channel :
    The channel index the requested data is read from.

Returns:
--------
    The buffer size needed.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetVolumeSubsetBufferSize =
R"doc(Compute the buffer size (in bytes) for a volume subset request.

Parameters:
-----------

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

format :
    Voxel format of the destination buffer.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

Returns:
--------
    The buffer size needed.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetVolumeSubsetBufferSize_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_GetVolumeTracesBufferSize =
R"doc(Compute the buffer size (in bytes) for a volume traces request.

Parameters:
-----------

traceCount :
    Number of traces to request.

traceDimension :
    The dimension to trace

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

Returns:
--------
    The buffer size needed.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_IsValid = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_PrefetchVolumeChunk =
R"doc(Force production of a specific volume data chunk

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested chunk belongs to.

LOD :
    The LOD level the requested chunk belongs to.

channel :
    The channel index the requested chunk belongs to.

chunkIndex :
    The index of the chunk to prefetch.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestProjectedVolumeSubset =
R"doc(Request a subset projected from an arbitrary 3D plane through the
subset onto one of the sides of the subset.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by minVoxelCoordinates and
    maxVoxelCoordinates for the projected dimensions.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

voxelPlane :
    The plane equation for the projection from the dimension source to
    the projected dimensions (which must be a 2D subset of the source
    dimensions).

projectedDimensions :
    The 2D dimension group that the plane in the source dimensiongroup
    is projected into. It must be a 2D subset of the source
    dimensions.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

format :
    Voxel format of the destination buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestProjectedVolumeSubset_2 =
R"doc(Request a subset projected from an arbitrary 3D plane through the
subset onto one of the sides of the subset, using an automatically
allocated buffer.

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

voxelPlane :
    The plane equation for the projection from the dimension source to
    the projected dimensions (which must be a 2D subset of the source
    dimensions).

projectedDimensions :
    The 2D dimension group that the plane in the source dimensiongroup
    is projected into. It must be a 2D subset of the source
    dimensions.

format :
    Voxel format of the destination buffer.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestProjectedVolumeSubset_3 =
R"doc(Request a subset projected from an arbitrary 3D plane through the
subset onto one of the sides of the subset, using an automatically
allocated typed buffer.

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

voxelPlane :
    The plane equation for the projection from the dimension source to
    the projected dimensions (which must be a 2D subset of the source
    dimensions).

projectedDimensions :
    The 2D dimension group that the plane in the source dimensiongroup
    is projected into. It must be a 2D subset of the source
    dimensions.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSamples =
R"doc(Request sampling of the input VDS at the specified coordinates.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least sampleCount
    elements.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

samplePositions :
    Pointer to array of VolumeDataLayout::Dimensionality_Max-elements
    indicating the positions to sample. May be deleted once
    RequestVolumeSamples return, as HueSpace makes a deep copy of the
    data.

sampleCount :
    Number of samples to request.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSamples_2 =
R"doc(Request sampling of the input VDS at the specified coordinates, using
an automatically allocated buffer.

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

samplePositions :
    Pointer to array of VolumeDataLayout::Dimensionality_Max-elements
    indicating the positions to sample. May be deleted once
    RequestVolumeSamples return, as HueSpace makes a deep copy of the
    data.

sampleCount :
    Number of samples to request.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSubset =
R"doc(Request a subset of the input VDS.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by minVoxelCoordinates and
    maxVoxelCoordinates.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

format :
    Voxel format of the destination buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSubset_2 =
R"doc(Request a subset of the input VDS.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by minVoxelCoordinates and
    maxVoxelCoordinates.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSubset_3 =
R"doc(Request a subset of the input VDS, using an automatically allocated
buffer.

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

format :
    Voxel format of the destination buffer.

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSubset_4 =
R"doc(Request a subset of the input VDS, using an automatically allocated
typed buffer.

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSubset1Bit =
R"doc(Request a subset of the input VDS.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least as many elements
    of format as indicated by minVoxelCoordinates and
    maxVoxelCoordinates.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeSubset1Bit_2 =
R"doc(Request a subset of the input VDS, using an automatically allocated
buffer.

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

minVoxelCoordinates :
    The minimum voxel coordinates to request in each dimension
    (inclusive).

maxVoxelCoordinates :
    The maximum voxel coordinates to request in each dimension
    (exclusive).

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeTraces =
R"doc(Request traces from the input VDS, using an automatically allocated
buffer.

Parameters:
-----------

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

tracePositions :
    Pointer to array of traceCount
    VolumeDataLayout::Dimensionality_Max-elements indicating the trace
    positions.

traceCount :
    Number of traces to request.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

traceDimension :
    The dimension to trace

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_RequestVolumeTraces_2 =
R"doc(Request traces from the input VDS.

Parameters:
-----------

buffer :
    Pointer to a preallocated buffer holding at least traceCount *
    number of samples in the traceDimension.

bufferByteSize :
    The size of the provided buffer, in bytes.

dimensionsND :
    The dimensiongroup the requested data is read from.

LOD :
    The LOD level the requested data is read from.

channel :
    The channel index the requested data is read from.

tracePositions :
    Pointer to array of traceCount
    VolumeDataLayout::Dimensionality_Max-elements indicating the trace
    positions.

traceCount :
    Number of traces to request.

interpolationMethod :
    Interpolation method to use when sampling the buffer.

traceDimension :
    The dimension to trace

replacementNoValue :
    If specified, this value is used to replace regions of the input
    VDS that has no data.

Returns:
--------
    A VolumeDataRequest instance encapsulating the request status and
    buffer.)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_UploadErrorCount = R"doc(Get the number of unretrieved upload errors)doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_VolumeDataAccessManager = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_VolumeDataAccessManager_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_VolumeDataAccessManager_3 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_m_IVolumeDataAccessManager = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAccessManager_operator_assign = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor =
R"doc(Describes the number of samples, name, unit and coordinates
(annotation) of an axis (dimension) of the volume)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_2 =
R"doc(Describes the number of samples, name, unit and coordinates
(annotation) of an axis (dimension) of the volume)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_CoordinateToSampleIndex =
R"doc(Convert a coordinate to a sample index (rounding to the closest index)

Parameters:
-----------

coordinate :
    The coordinate to get the sample index of

Returns:
--------
    The sample index of the coordinate)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_CoordinateToSamplePosition =
R"doc(Convert a coordinate to a sample position (to use with
RequestVolumeSamples or an InterpolatingVolumeDataAccessor)

Parameters:
-----------

coordinate :
    The coordinate to get the sample position of

Returns:
--------
    The sample position of the coordinate)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetCoordinateMax =
R"doc(Get the coordinate of the last sample of this axis

Returns:
--------
    The coordinate of the last sample of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetCoordinateMin =
R"doc(Get the coordinate of the first sample of this axis

Returns:
--------
    The coordinate of the first sample of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetCoordinateStep =
R"doc(Get the coordiante step size per sample

Returns:
--------
    The coordiante step size per sample)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetName =
R"doc(Get the name of this axis

Returns:
--------
    The name of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetNumSamples =
R"doc(Get the number of samples along this axis

Returns:
--------
    The number of samples along this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_GetUnit =
R"doc(Get the unit of the coordinates of this axis

Returns:
--------
    The unit of the coordinates of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_SampleIndexToCoordinate =
R"doc(Convert a sample index on this axis to a coordinate

Parameters:
-----------

sampleIndex :
    The sample index get the coordinate of

Returns:
--------
    The coordinate of the sample index)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_VolumeDataAxisDescriptor = R"doc(Default constructor)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_VolumeDataAxisDescriptor_2 =
R"doc(Constructor

Parameters:
-----------

numSamples :
    The number of samples along this axis

name :
    The name of this axis

unit :
    The unit of the coordinates of this axis

coordinateMin :
    The coordinate of the first sample of this axis

coordinateMax :
    The coordinate of the last sample of this axis)doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_coordinateMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_coordinateMin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_name = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_numSamples = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataAxisDescriptor_m_unit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor = R"doc(\class VolumeDataChannelDescriptor Describes a channel of a VDS)doc";

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

Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

mappedValueCount :
    When using per trace mapping, the number of values to store per
    trace

flags :
    the flags for this channel

Returns:
--------
    a trace mapped descriptor)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_TraceMappedVolumeDataChannelDescriptor_2 =
R"doc(Named constructor for a trace mapped channel

Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

mappedValueCount :
    When using per trace mapping, the number of values to store per
    trace

flags :
    the flags for this channel

noValue :
    the No Value for this channel

Returns:
--------
    a trace mapped descriptor)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_2 =
R"doc(The minimum constructor for a VolumeDataChannelDescriptor. This will
use direct mapping, default flags, and no No Value

Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_3 =
R"doc(Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

mapping :
    the mapping for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_4 =
R"doc(Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

flags :
    the flags for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_5 =
R"doc(Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

mapping :
    the mapping for this channel

flags :
    the flags for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_6 =
R"doc(Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

mapping :
    the mapping for this channel

mappedValueCount :
    When using per trace mapping, the number of values to store per
    trace

flags :
    the flags for this channel

integerScale :
    the scale to use for integer types

integerOffset :
    the offset to use for integer types)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_7 =
R"doc(Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

noValue :
    the No Value for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_8 =
R"doc(Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

noValue :
    the No Value for this channel

mapping :
    the mapping for this channel

flags :
    the flags for this channel)doc";

static const char *__doc_OpenVDS_VolumeDataChannelDescriptor_VolumeDataChannelDescriptor_9 =
R"doc(Parameters:
-----------

format :
    the data format for this channel

components :
    the vector count for this channel

name :
    the name of this channel

unit :
    the unit of this channel

valueRangeMin :
    the value range minimum of this channel

valueRangeMax :
    the value range maximum of this channel

mapping :
    the mapping for this channel

mappedValueCount :
    When using per trace mapping, the number of values to store per
    trace

flags :
    the flags for this channel

noValue :
    the No Value for this channel

integerScale :
    the scale to use for integer types

integerOffset :
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

static const char *__doc_OpenVDS_VolumeDataLayout =
R"doc(\class VolumeDataLayout a class that contains axis and channel
information for a VDS A layout is associated with a VDS object and
contains information about axis and channels in the VDS. A
VolumeDataLayout can be invalidated when the VDS is invalidated.)doc";

static const char *__doc_OpenVDS_VolumeDataLayout_2 =
R"doc(\class VolumeDataLayout a class that contains axis and channel
information for a VDS A layout is associated with a VDS object and
contains information about axis and channels in the VDS. A
VolumeDataLayout can be invalidated when the VDS is invalidated.)doc";

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

static const char *__doc_OpenVDS_VolumeDataLayout_GetLayoutDescriptor = R"doc()doc";

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

static const char *__doc_OpenVDS_VolumeDataPageAccessor_AccessMode = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_AccessMode_AccessMode_Create = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_AccessMode_AccessMode_ReadOnly = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_AccessMode_AccessMode_ReadWrite = R"doc()doc";

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

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetMappedChunkIndex =
R"doc(Get the chunk index for this VolumeDataPageAccessor corresponding to
the given chunk index in the primary channel. Because some channels
can have mappings (e.g. one value per trace), the number of chunks can
be less than in the primary channel and we need to have a mapping to
figure out the chunk index in each channel that is produced together.

Parameters:
-----------

primaryChannelChunkIndex :
    The index of the chunk in the primary channel (channel 0) that we
    want to map to a chunk index for this VolumeDataPageAccessor.

Returns:
--------
    The chunk index for this VolumeDataPageAccessor corresponding to
    the given chunk index in the primary channel.)doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetMaxPages = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetNumSamples = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_GetPrimaryChannelChunkIndex =
R"doc(Get the primary channel chunk index corresponding to the given chunk
index of this VolumeDataPageAccessor. In order to avoid creating
duplicates requests when a channel is mapped, we need to know which
primary channel chunk index is representative of a particular mapped
chunk index.

Parameters:
-----------

chunkIndex :
    The chunk index for this VolumeDataPageAccessor that we want the
    representative primary channel chunk index of.

Returns:
--------
    The primary channel chunk index corresponding to the given chunk
    index for this VolumeDataPageAccessor.)doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_ReadPage = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_ReadPageAtPosition = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_RemoveReference = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_SetMaxPages = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPageAccessor_VolumeDataPageAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_Error = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_Error_errorCode = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_Error_message = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetBuffer = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetError = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetMinMax = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetMinMaxExcludingMargin = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetVolumeDataPageAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_GetWritableBuffer = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_Release = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_UpdateWrittenRegion = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataPage_VolumeDataPage = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor =
R"doc(\class VolumeDataReadAccessor A class that provides random read access
to the voxel values of a VDS)doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_CurrentRegion = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_GetLayout = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_GetValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_Region = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_RegionCount = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_RegionFromIndex = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_VolumeDataReadAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_VolumeDataReadAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_VolumeDataReadAccessor_3 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadAccessor_m_accessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor =
R"doc(\class VolumeDataReadWriteAccessor A class that provides random
read/write access to the voxel values of a VDS)doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_Accessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_Cancel = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_Commit = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_SetValue = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_VolumeDataReadWriteAccessor = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_VolumeDataReadWriteAccessor_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataReadWriteAccessor_VolumeDataReadWriteAccessor_3 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_Buffer =
R"doc(Get the pointer to the buffer the request is writing to.

Returns:
--------
    The pointer to the buffer the request is writing to.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_BufferByteSize =
R"doc(Get the size of the buffer the request is writing to.

Returns:
--------
    The size of the buffer the request is writing to.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_BufferDataType =
R"doc(Get the volume data format of the buffer the request is writing to.

Returns:
--------
    The volume data format of the buffer the request is writing to.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_Cancel =
R"doc(Try to cancel the request. You still have to call
WaitForCompletion/IsCanceled to make sure the buffer is not being
written to and to take the job out of the system. It is possible that
the request has completed concurrently with the call to Cancel in
which case WaitForCompletion will return True.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_CancelAndWaitForCompletion =
R"doc(Cancel the request and wait for it to complete. This call will block
until the request has completed so you can be sure the buffer is not
being written to and the job is taken out of the system.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_CancelInternal = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_Deleter = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_GetCompletionFactor =
R"doc(Get the completion factor (between 0 and 1) of the request.

Returns:
--------
    A factor (between 0 and 1) indicating how much of the request has
    been completed.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_IsCanceled =
R"doc(Check if the request was canceled (e.g. the VDS was invalidated before
the request was processed). If the request was canceled, the buffer
does not contain valid data.

Returns:
--------
    The request is active until either IsCompleted, IsCanceled or
    WaitForCompletion returns True.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_IsCompleted =
R"doc(Check if the request completed successfully. If the request completed,
the buffer now contains valid data.

Returns:
--------
    The request is active until either IsCompleted, IsCanceled or
    WaitForCompletion returns True.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_IsDataOwner = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_RequestFormat = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_RequestID =
R"doc(Get the ID of the request.

Returns:
--------
    The ID of the request.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_SetJobID = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_ValidateRequest =
R"doc(Check if the request object is valid. Throws an InvalidOperation
exception if the request object is not valid.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_VolumeDataRequest = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_VolumeDataRequest_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_VolumeDataRequest_3 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_VolumeDataRequest_4 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_WaitForCompletion =
R"doc(Wait for the VolumeDataRequest to complete successfully. If the
request completed, the buffer now contains valid data.

Parameters:
-----------

millisecondsBeforeTimeout :
    The number of milliseconds to wait before timing out (optional). A
    value of 0 indicates there is no timeout and we will wait for
    however long it takes. Note that the request is not automatically
    canceled if the wait times out, you can also use this mechanism to
    e.g. update a progress bar while waiting. If you want to cancel
    the request you have to explicitly call CancelRequest() and then
    wait for the request to stop writing to the buffer.

Returns:
--------
    The request is active until either IsCompleted, IsCanceled or
    WaitForCompletion returns True. Whenever WaitForCompletion returns
    False you need to call IsCanceled() to know if that was because of
    a timeout or if the request was canceled.)doc";

static const char *__doc_OpenVDS_VolumeDataRequest_m_Buffer = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_m_BufferByteSize = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_m_BufferDataType = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_m_Data = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_m_IsCanceled = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_m_IsCompleted = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_m_JobID = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_m_Manager = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_t = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_t_Data = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_t_EnsureRequestCompleted = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_t_IsDataOwner = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_t_VolumeDataRequest_t = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_t_VolumeDataRequest_t_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeDataRequest_t_m_TypedData = R"doc()doc";

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
@return True if the index is within this chunk, False otherwise)doc";

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
created with @param iLocalIndex the local index to check @return True
if the index is within this chunk, False otherwise)doc";

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
R"doc(Converts a local index to world coordinates using the coordinate
transformer's IJK grid definition and IJK dimension map @param
iLocalIndex the local index to convert @param
cVDSCoordinateTransformer the coordinate transformer to use @return
the world coordinates)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_RelativeAxisPositionToLocalIndex =
R"doc(Converts a relative position along the volume axes to a local index
@param rPosition the relative postion to convert @return the local
index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_RelativeAxisPositionToVoxelIndex =
R"doc(Converts a relative volume axis position to a voxel index, rounding to
the nears integer @param rPosition the axis position @param iDimension
the volume dimension @return the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VolumeIndexerBase = R"doc(////////////////////////// Constructors /////////////////////////////)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VolumeIndexerBase_2 = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexFloatToCoordinate =
R"doc(Converts a float voxel index for a specific volume dimension to a
coordinate value @param rVoxelIndex the voxel index to convert @param
iDimension the volume dimension @return the coordinate)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexInProcessArea =
R"doc(Checks if a voxel index is within the chunk this indexer was created
with @param iVoxelIndex the voxel index to check @return True if the
index is within this chunk, False otherwise)doc";

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
R"doc(Converts a voxel index to world coordinates using the coordinate
transformer's IJK grid definition and IJK dimension map @param
iVoxelIndex the voxel index to convert @param
cVDSCoordinateTransformer the coordinate transformer to use @return
the world coordinates)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_VoxelIndexToWorldCoordinates_2 =
R"doc(Converts a float voxel index to world coordinates using the coordinate
transformer's IJK grid definition and IJK dimension map @param
rVoxelIndex the float voxel index to convert @param
cVDSCoordinateTransformer the coordinate transformer to use @return
the world coordinates)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_WorldCoordinatesToLocalIndex =
R"doc(Converts world coordinates to a local index, rounding to the nearest
integer @param rWorldCoords the world coordinates to convert @param
cVDSCoordinateTransformer the coordinate transformer to use @return
the local index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_WorldCoordinatesToVoxelIndex =
R"doc(Converts world coordinates to a voxel index, rounding to the nearest
integer @param rWorldCoords the world coordinates to convert @param
cVDSCoordinateTransformer the coordinate transformer to use @return
the voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerBase_WorldCoordinatesToVoxelIndexFloat =
R"doc(Converts world coordinates to a float voxel index without rounding
@param rWorldCoords the world coordinates to convert @param
cVDSCoordinateTransformer the coordinate transformer to use @return
the float voxel index)doc";

static const char *__doc_OpenVDS_VolumeIndexerData = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerData_GetDataBlockDimensionality = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerData_GetDataBlockDimensionality_Dimensionality = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerData_VolumeIndexerData = R"doc()doc";

static const char *__doc_OpenVDS_VolumeIndexerData_VolumeIndexerData_2 = R"doc()doc";

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

Parameters:
-----------

ptBuffer :
    pointer to the buffer to be samples

localIndex :
    the local 2D index into the buffer

Returns:
--------
    a TREAL sampled using the ::InterpolationType at the given index)doc";

static const char *__doc_OpenVDS_VolumeSampler_Sample3D =
R"doc(Sample the given buffer at the the given 3D index\n The returned TREAL
type is double when T is double, int32, or int64 and float for all
other types

Parameters:
-----------

ptBuffer :
    pointer to the buffer to be samples

localIndex :
    the local 3D index into the buffer

Returns:
--------
    a TREAL sampled using the ::InterpolationType at the given index)doc";

static const char *__doc_OpenVDS_VolumeSampler_VolumeSampler = R"doc()doc";

static const char *__doc_OpenVDS_VolumeSampler_VolumeSampler_2 =
R"doc(Parameters:
-----------

anSize :
    the (at least 3D) size of the buffer to be sampled

anPitch :
    the (at least 3D) pitch of the buffer to be sampled

rangeMin :
    the value range minimum of the data to be sampled

rangemax :
    the value range maximum of the data to be sampled

noValalue :
    the no value for the data to be sampled

replacementNoValue :
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

static const char *__doc_OpenVDS_WaveletAdaptiveMode = R"doc()doc";

static const char *__doc_OpenVDS_WaveletAdaptiveMode_BestQuality =
R"doc(< The best quality available data is loaded (this is the only setting
which will load lossless data).)doc";

static const char *__doc_OpenVDS_WaveletAdaptiveMode_Ratio =
R"doc(< An adaptive level closest to the global compression ratio is
selected when loading wavelet compressed data.)doc";

static const char *__doc_OpenVDS_WaveletAdaptiveMode_Tolerance =
R"doc(< An adaptive level closest to the global compression tolerance is
selected when loading wavelet compressed data.)doc";

static const char *__doc_OpenVDS_WriteElement =
R"doc(Write element into buffer. This function has a template specialisation
for bool making it suitable to write single bits into a buffer with
packed bits.

Parameters:
-----------

buffer :
    Buffer to write to

element :
    The element index in the buffer to write to

value :
    Value to write)doc";

static const char *__doc_OpenVDS_WriteElement_2 = R"doc()doc";

static const char *__doc_OpenVDS_fastInvert = R"doc()doc";

static const char *__doc_OpenVDS_operator_bor = R"doc()doc";

static const char *__doc_OpenVDS_operator_bor_2 = R"doc()doc";

static const char *__doc_OpenVDS_optional = R"doc()doc";

static const char *__doc_OpenVDS_optional_emplace = R"doc()doc";

static const char *__doc_OpenVDS_optional_has_value = R"doc()doc";

static const char *__doc_OpenVDS_optional_m_HasValue = R"doc()doc";

static const char *__doc_OpenVDS_optional_m_Value = R"doc()doc";

static const char *__doc_OpenVDS_optional_operator_bool = R"doc()doc";

static const char *__doc_OpenVDS_optional_optional = R"doc()doc";

static const char *__doc_OpenVDS_optional_optional_2 = R"doc()doc";

static const char *__doc_OpenVDS_optional_value = R"doc()doc";

static const char *__doc_OpenVDS_optional_value_or = R"doc()doc";

static const char *__doc_OpenVDS_rangeSize = R"doc()doc";

static const char *__doc_PyMetadataAccess = R"doc()doc";

static const char *__doc_std_hash = R"doc()doc";

static const char *__doc_std_hash_operator_call = R"doc()doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

