set(SOURCE_FILES
  OpenVDS.cpp
  IO/File.cpp
  IO/Linux_File.cpp
  IO/Win_File.cpp
  IO/IOManager.cpp
  IO/IOManagerAWS.cpp
  IO/IOManagerAzure.cpp
  IO/IOManagerInMemory.cpp
  IO/IOManagerCurl.cpp
  VDS/VolumeDataPartition.cpp
  VDS/VolumeDataChannelMapping.cpp
  VDS/VolumeDataLayer.cpp
  VDS/VolumeDataLayoutImpl.cpp
  VDS/VolumeDataRegion.cpp
  VDS/VolumeDataHash.cpp
  VDS/VolumeDataPageAccessorImpl.cpp
  VDS/VolumeDataAccessManagerImpl.cpp
  VDS/VolumeDataPageImpl.cpp
  VDS/VolumeDataAccessor.cpp
  VDS/VolumeDataAccessorRequest.cpp
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
  VDS/VolumeIndexer.cpp
  VDS/Env.cpp
  )

set (PRIVATE_HEADER_FILES
  IO/File.h
  IO/IOManager.h
  IO/IOManagerAWS.h
  IO/IOManagerAzure.h
  IO/IOManagerInMemory.h
  IO/IOManagerCurl.h
  VDS/VDS.h
  VDS/VolumeDataPartition.h
  VDS/VolumeDataChannelMapping.h
  VDS/VolumeDataLayer.h
  VDS/VolumeDataLayoutImpl.h
  VDS/VolumeDataChunk.h
  VDS/VolumeDataRegion.h
  VDS/VolumeDataHash.h
  VDS/VolumeDataPageAccessorImpl.h
  VDS/VolumeDataAccessManagerImpl.h
  VDS/VolumeDataAccessor.h
  VDS/VolumeDataPageImpl.h
  VDS/DimensionGroup.h
  VDS/Hash.h
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
  VDS/VolumeDataRequestProcessor.h
  VDS/ThreadPool.h
  VDS/Env.h
  )

set (EXPORTED_HEADER_FILES
  OpenVDS/GlobalMetadataCommon.h
  OpenVDS/KnownMetadata.h
  OpenVDS/Metadata.h
  OpenVDS/OpenVDS.h
  OpenVDS/openvds_export.h
  OpenVDS/Range.h
  OpenVDS/ValueConversion.h
  OpenVDS/Vector.h
  OpenVDS/VolumeData.h
  OpenVDS/VolumeDataAccess.h
  OpenVDS/VolumeDataAxisDescriptor.h
  OpenVDS/VolumeDataChannelDescriptor.h
  OpenVDS/VolumeDataLayout.h
  OpenVDS/VolumeDataLayoutDescriptor.h
  OpenVDS/VolumeSampler.h
  OpenVDS/CoordinateTransformer.h
  OpenVDS/VolumeIndexer.h
  OpenVDS/SimplexNoiseKernel.h
  )

add_library(openvds_objects OBJECT
  ${PRIVATE_HEADER_FILES}
  ${EXPORTED_HEADER_FILES}
  ${SOURCE_FILES})

function(compileInTarget to_target source_target)
  get_target_property(src_sources ${source_target} SOURCES)
  get_target_property(src_include ${source_target} INTERFACE_INCLUDE_DIRECTORIES)
  get_target_property(src_dir ${source_target} SOURCE_DIR)
  foreach (src_file ${src_sources})
    set(complete_file "${src_dir}/${src_file}")
    target_sources(${to_target} PRIVATE ${complete_file})
  endforeach()
  target_include_directories(${to_target} PRIVATE ${src_include})
endfunction()

compileInTarget(openvds_objects fmt::fmt)
compileInTarget(openvds_objects jsoncpp_lib_static)

if(MSVC_VERSION)
  set_source_files_properties(VDS/VolumeDataAccessor.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()

get_property(link_3rdparty_release GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_RELEASE)
get_property(link_3rdparty_debug   GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_DEBUG)
get_property(runtime_3rdparty_release GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE)
get_property(runtime_3rdparty_debug   GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG)
get_property(include_3rdparty      GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES)

add_dependencies(openvds_objects aws-cpp-sdk azure-storage)

target_include_directories(openvds_objects SYSTEM PRIVATE ${include_3rdparty} "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(openvds_objects PRIVATE openvds_EXPORTS)

target_link_libraries(openvds_objects PUBLIC Threads::Threads)

if (aws-cpp-sdk_EXTERNAL_LIBS)
  target_link_libraries(openvds_objects PUBLIC ${aws-cpp-sdk_EXTERNAL_LIBS})
endif()
target_link_libraries(openvds_objects PUBLIC $<$<NOT:$<CONFIG:Debug>>:${link_3rdparty_release}> $<$<CONFIG:Debug>:${link_3rdparty_debug}>)

# this need to be explicitly included on centos7 for some reason
if (NOT WIN32)
  target_link_libraries(openvds_objects PUBLIC ssl)
endif()

#Azure storage exposes boost_system on linux et al.
if (UNIX)
    target_link_libraries(openvds_objects PUBLIC boost_system)
endif()

if (OpenMP_CXX_FOUND)
    target_link_libraries(openvds_objects PUBLIC OpenMP::OpenMP_CXX)
endif()

if (BUILD_ZLIB)
  add_dependencies(openvds_objects zlib)
else()
  find_package(ZLIB REQUIRED)
  target_link_libraries(openvds_objects PUBLIC ZLIB::ZLIB)
endif()

if (BUILD_CURL_IO_MANAGER)
  if (NOT BUILD_CURL)
    find_package(CURL REQUIRED)
    target_link_libraries(openvds_objects PUBLIC ${CURL_LIBRARIES})
    target_include_directories(openvds_objects PUBLIC ${CURL_INCLUDE_DIRS})
  endif()
  message("GOODBY")
  if (NOT BUILD_UV)
    message("HELLO WORLD")
    find_package(LibUV REQUIRED)
    target_link_libraries(openvds_objects PUBLIC LibUV::LibUV)
  endif()

  if (WIN32)
    target_link_libraries(openvds_objects PUBLIC bcrypt)
  else()
    find_package(OpenSSL REQUIRED)
    target_link_libraries(openvds_objects PUBLIC ${OPENSSL_CRYPTO_LIBRARY})
    target_include_directories(openvds_objects PUBLIC ${OPENSSL_INCLUDE_DIR})
  endif()
endif()
if (WIN32)
  set_source_files_properties(IO/Linux_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
elseif (UNIX)
  set_source_files_properties(IO/Win_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Dist/OpenVDS CACHE STRING "" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

add_library(openvds $<TARGET_OBJECTS:openvds_objects>)
target_link_libraries(openvds openvds_objects)
set_target_properties(openvds
  PROPERTIES
  FRAMEWORK ON
  SOVERSION 0
  VERSION 0.1.0
  PUBLIC_HEADER "${EXPORTED_HEADER_FILES}"
  #OUTPUT_NAME ${PROJECT_NAME}
)
target_include_directories(openvds
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

if (${MSVC_TOOLSET_VERSION_LOCAL})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
endif()

install(TARGETS openvds openvds_objects
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

install(
    EXPORT openvds-export
    NAMESPACE openvds::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/openvds/cmake
    FILE openvds-config.cmake
)

install(FILES ${runtime_3rdparty_release} CONFIGURATIONS Release RelWithDebInfo MinSizeRel DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR})
install(FILES ${runtime_3rdparty_debug} CONFIGURATIONS Debug DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR})

export(
    TARGETS
        openvds
        openvds_objects
    NAMESPACE
        openvds::
    FILE
        openvds-config.cmake
)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR})
include (InstallRequiredSystemLibraries)

set(OPENVDS_LIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "")

add_subdirectory(SEG-Y)
