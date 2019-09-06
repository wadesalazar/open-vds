set(SOURCE_FILES
  OpenVDS.cpp
  IO/File.cpp
  IO/Linux_File.cpp
  IO/Win_File.cpp
  IO/S3_Downloader.cpp
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
  )

set (PRIVATE_HEADER_FILES
  IO/File.h
  IO/S3_Downloader.h
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

find_package(OpenMP REQUIRED)
add_dependencies(openvds_objects aws-cpp-sdk)
target_include_directories(openvds_objects PRIVATE "${aws-cpp-sdk_INCLUDE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(openvds_objects PRIVATE openvds_EXPORTS)

target_link_libraries(openvds_objects Threads::Threads)
target_link_libraries(openvds_objects jsoncpp_lib_static)
target_link_libraries(openvds_objects ${aws-cpp-sdk_EXTERNAL_LIBS} ${aws-cpp-sdk_LIBS})
target_link_libraries(openvds_objects OpenMP::OpenMP_CXX)

if (BUILD_ZLIB)
  add_dependencies(openvds_objects zlib)
  target_include_directories(openvds_objects PRIVATE "${zlib_INCLUDE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}")
  target_link_libraries(openvds_objects ${zlib_LIBS})
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

if (${MSVC_TOOLSET_VERSION})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION}")
endif()

install(TARGETS openvds
  ARCHIVE
    DESTINATION lib${LIB_TOOLSET_DIR}
  LIBRARY
    DESTINATION lib${LIB_TOOLSET_DIR}
  RUNTIME
    DESTINATION bin${LIB_TOOLSET_DIR}
  FRAMEWORK
    DESTINATION framework
  PUBLIC_HEADER
    DESTINATION include/OpenVDS
  PRIVATE_HEADER
    DESTINATION include/OpenVDS/private)

  install(FILES ${aws-cpp-sdk_LIBS} ${zlib_LIBS}
    DESTINATION lib${LIB_TOOLSET_DIR})
  if (aws-cpp-sdk_RUNTIME)
    install(FILES ${aws-cpp-sdk_RUNTIME} ${zlib_RUNTIME}
      DESTINATION bin${LIB_TOOLSET_DIR})
  endif()

add_subdirectory(SEG-Y)
