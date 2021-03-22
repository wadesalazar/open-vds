include(CMake/BuildExternal.cmake)
include(CMake/BuildAWSSDK.cmake)
include(CMake/BuildJsonCpp.cmake)
include(CMake/BuildFmt.cmake)
include(CMake/BuildCppRestSdk.cmake)
include(CMake/BuildAzure.cmake)
include(CMake/BuildCurl.cmake)
include(CMake/BuildLibUV.cmake)
include(CMake/BuildLibreSSL.cmake)
include(CMake/BuildCrc32c.cmake)
include(CMake/BuildGoogleCloud.cmake)
include(CMake/BuildDms.cmake)
if (BUILD_ZLIB)
  include(CMake/BuildZlib.cmake)
endif()

macro(build3rdparty)
  if (NOT DISABLE_AWS_IOMANAGER)
    BuildAwsSdk()
  endif()
  BuildJsonCPP()
  if (BUILD_ZLIB)
    BuildZlib()
  endif()
  BuildFmt()
  if (NOT DISABLE_AZURE_IOMANAGER)
    BuildCppRestSdk()
    BuildAzure()
  endif()
  if (Python3_FOUND)
    include(CMake/FindPythonLibsNew.cmake)
    add_subdirectory(${pybind11_SOURCE_DIR} ${PROJECT_BINARY_DIR}/pybind11_${pybind11_VERSION} EXCLUDE_FROM_ALL)
  endif()
  if (NOT DISABLE_CURL_IOMANAGER OR NOT DISABLE_GCP_IOMANAGER OR NOT DISABLE_DMS_IOMANAGER)
    BuildCurl()
    BuildLibUV()
  endif()
  if (NOT DISABLE_GCP_IOMANAGER OR NOT DISABLE_DMS_IOMANAGER)
    BuildLibreSSL()
    BuildCrc32c()
    BuildGoogleCloud()
  endif()
  if (NOT DISABLE_DMS_IOMANAGER)
    BuildDms()
  endif()
endmacro()
