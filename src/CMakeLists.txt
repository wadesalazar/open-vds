set(SOURCE_FILES
  OpenVDS.cpp
  IO/File.cpp
  IO/Linux_File.cpp
  IO/Win_File.cpp
  IO/IOManager.cpp
  IO/IOManagerAWS.cpp
  VDS/VolumeDataPartition.cpp
  VDS/VolumeDataChannelMapping.cpp
  VDS/VolumeDataLayer.cpp
  VDS/VolumeDataLayout.cpp
  VDS/VolumeDataRegion.cpp
  VDS/VolumeDataHash.cpp
  VDS/VolumeDataPageAccessorImpl.cpp
  VDS/VolumeDataAccessManagerImpl.cpp
  VDS/VolumeDataPageImpl.cpp
  VDS/VolumeDataAccessor.cpp
  VDS/DimensionGroup.cpp
  VDS/ParseVDSJson.cpp
  VDS/MetadataManager.cpp
  VDS/Base64.cpp
  VDS/VolumeDataStore.cpp
  VDS/Wavelet.cpp
  VDS/WaveletAdaptiveLL.cpp
  VDS/DataBlock.cpp
  VDS/FSE/entropy_common.cpp
  VDS/FSE/fse_decompress.cpp
  VDS/Rle.cpp
  VDS/VolumeDataRequestProcessor.cpp
  )

set (PRIVATE_HEADER_FILES
  IO/File.h
  IO/IOManager.h
  IO/IOManagerAWS.h
  VDS/VolumeDataPartition.h
  VDS/VolumeDataChannelMapping.h
  VDS/VolumeDataLayer.h
  VDS/VolumeDataLayout.h
  VDS/VolumeDataChunk.h
  VDS/VolumeDataRegion.h
  VDS/VolumeDataHash.h
  VDS/VolumeDataPageAccessorImpl.h
  VDS/VolumeDataAccessManagerImpl.h
  VDS/VolumeDataAccessor.h
  VDS/VolumeDataPageImpl.h
  VDS/DimensionGroup.h
  VDS/Hash.h
  Math/Vector.h
  VDS/Bitmask.h
  VDS/ParseVDSJson.h
  VDS/MetadataManager.h
  VDS/IntrusiveList.h
  VDS/Base64.h
  VDS/VolumeDataStore.h
  VDS/Wavelet.h
  VDS/WaveletAdaptiveLL.h
  VDS/WaveletTypes.h
  VDS/DataBlock.h
  VDS/WaveletSSETransform.h
  VDS/FSE/bitstream.h
  VDS/FSE/error_private.h
  VDS/FSE/error_public.h
  VDS/FSE/mem.h
  VDS/FSE/fse.h
  VDS/Rle.h
  VDS/ValueConversion.h
  VDS/VolumeSampler.h
  VDS/VolumeDataRequestProcessor.h
  )

set (EXPORTED_HEADER_FILES
  OpenVDS/OpenVDS.h
  OpenVDS/KnownMetadata.h
  OpenVDS/Metadata.h
  OpenVDS/VolumeData.h
  OpenVDS/VolumeDataLayoutDescriptor.h
  OpenVDS/VolumeDataChannelDescriptor.h
  OpenVDS/VolumeDataAxisDescriptor.h
  OpenVDS/VolumeDataAccess.h
  OpenVDS/openvds_export.h)

add_library(openvds_objects OBJECT
  ${PRIVATE_HEADER_FILES}
  ${EXPORTED_HEADER_FILES}
  ${SOURCE_FILES})


if(MSVC_VERSION)
  set_source_files_properties(VDS/VolumeDataAccessor.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()

find_package(OpenMP REQUIRED)

get_property(link_3rdparty_release GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_RELEASE)
get_property(link_3rdparty_debug   GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_DEBUG)
get_property(include_3rdparty      GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES)

add_dependencies(openvds_objects aws-cpp-sdk)

target_include_directories(openvds_objects PRIVATE ${include_3rdparty} "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(openvds_objects PRIVATE openvds_EXPORTS)

target_link_libraries(openvds_objects Threads::Threads)
target_link_libraries(openvds_objects jsoncpp_lib_static)
if (aws-cpp-sdk_EXTERNAL_LIBS)
  target_link_libraries(openvds_objects ${aws-cpp-sdk_EXTERNAL_LIBS})
endif()
target_link_libraries(openvds_objects $<$<NOT:$<CONFIG:Debug>>:${link_3rdparty_release}> $<$<CONFIG:Debug>:${link_3rdparty_debug}>)
target_link_libraries(openvds_objects OpenMP::OpenMP_CXX)

if (BUILD_ZLIB)
  add_dependencies(openvds_objects zlib)
else()
  find_package(ZLIB REQUIRED)
  target_link_libraries(openvds_objects ZLIB::ZLIB)
endif()

if (WIN32)
  set_source_files_properties(IO/Linux_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
elseif (UNIX)
  set_source_files_properties(IO/Win_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Dist/OpenVDS CACHE STRING "" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

add_library(openvds SHARED $<TARGET_OBJECTS:openvds_objects>)
target_link_libraries(openvds openvds_objects)
set_target_properties(openvds
  PROPERTIES
  FRAMEWORK ON
  SOVERSION 0
  VERSION 0.1.0
  PUBLIC_HEADER "${EXPORTED_HEADER_FILES}"
  PRIVATE_HEADER "${PRIVATE_HEADER_FILES}"
  #OUTPUT_NAME ${PROJECT_NAME}
)
target_include_directories(openvds
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

if (${MSVC_TOOLSET_VERSION})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION}")
endif()

install(TARGETS openvds openvds_objects jsoncpp_lib_static
  EXPORT openvds-export
  ARCHIVE
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR}
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR}
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
  FRAMEWORK
    DESTINATION framework
  PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/OpenVDS
  PRIVATE_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/OpenVDS/private)

  install(FILES ${aws-cpp-sdk_LIBS} ${zlib_LIBS}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR})
  if (aws-cpp-sdk_RUNTIME)
    install(FILES ${aws-cpp-sdk_RUNTIME} ${zlib_RUNTIME}
      DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR})
  endif()

install(
    EXPORT openvds-export
    NAMESPACE openvds::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/openvds/cmake
    FILE openvds-config.cmake
)

export(
    TARGETS
        openvds
        openvds_objects
        jsoncpp_lib_static
    NAMESPACE
        openvds::
    FILE
        openvds-config.cmake
)
set(OPENVDS_LIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")

add_subdirectory(SEG-Y)
