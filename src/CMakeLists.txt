set(SOURCE_FILES
  OpenVDS.cpp
  IO/File.cpp
  IO/Linux_File.cpp
  IO/Win_File.cpp
  IO/S3_Downloader.cpp
  VDS/VolumeDataPartition.cpp
  VDS/VolumeDataLayer.cpp
  VDS/VolumeDataLayout.cpp
  VDS/VolumeDataRegion.cpp
  VDS/VolumeDataHash.cpp
  VDS/DimensionGroup.cpp
  VDS/ParseVDSJson.cpp)

set (PRIVATE_HEADER_FILES
  IO/File.h
  IO/S3_Downloader.h
  VDS/VolumeData.h
  VDS/VolumeDataPartition.h
  VDS/VolumeDataLayer.h
  VDS/VolumeDataLayout.h
  VDS/VolumeDataChunk.h
  VDS/VolumeDataRegion.h
  VDS/VolumeDataHash.h
  VDS/VolumeDataLayoutDescriptor.h
  VDS/VolumeDataChannelDescriptor.h
  VDS/VolumeDataAxisDescriptor.h
  VDS/VolumeDataAccess.h
  VDS/DimensionGroup.h
  VDS/Hash.h
  Math/Vector.h
  VDS/Bitmask.h
  VDS/ParseVDSJson.h)

set (EXPORTED_HEADER_FILES
  OpenVDS/OpenVDS.h
  OpenVDS/KnownMetadata.h
  OpenVDS/openvds_export.h)

add_library(openvds SHARED
  ${PRIVATE_HEADER_FILES}
  ${EXPORTED_HEADER_FILES}
  ${SOURCE_FILES})

add_dependencies(openvds aws-cpp-sdk)
target_include_directories(openvds PRIVATE "${aws-cpp-sdk_INCLUDE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(openvds PRIVATE openvds_EXPORTS)
target_link_libraries(openvds Threads::Threads)
target_link_libraries(openvds jsoncpp_lib_static)
target_link_libraries(openvds ${aws-cpp-sdk_EXTERNAL_LIBS} ${aws-cpp-sdk_LIBS})

if (WIN32)
  set_source_files_properties(IO/Linux_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
elseif (UNIX)
  set_source_files_properties(IO/Win_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Dist/OpenVDS CACHE STRING "" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

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
  PUBLIC_HEADER
    DESTINATION include/OpenVDS
  PRIVATE_HEADER
    DESTINATION include/OpenVDS/private)

  install(FILES ${aws-cpp-sdk_LIBS}
    DESTINATION lib${LIB_TOOLSET_DIR})
  if (aws-cpp-sdk_RUNTIME)
    install(FILES ${aws-cpp-sdk_RUNTIME}
      DESTINATION bin${LIB_TOOLSET_DIR})
  endif()

add_subdirectory(SEG-Y)
