set(SOURCE_FILES
  File.cpp
  Linux_File.cpp
  Win_File.cpp
  S3_Downloader.cpp
  OpenVDS.cpp)

set (PRIVATE_HEADER_FILES
  File.h
  S3_Downloader.h)

set (EXPORTED_HEADER_FILES
  OpenVDS/OpenVDS.h
  OpenVDS/openvds_export.h)

add_library(openvds SHARED
  ${PRIVATE_HEADER_FILES}
  ${EXPORTED_HEADER_FILES}
  ${SOURCE_FILES})

add_dependencies(openvds aws-cpp-sdk)
target_include_directories(openvds PRIVATE "${AWS_INCLUDE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(openvds PRIVATE openvds_EXPORTS)
target_link_libraries(openvds PUBLIC Threads::Threads ${AWS_LIBS})

if (WIN32)
  set_source_files_properties(Linux_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
elseif (UNIX)
  set_source_files_properties(Win_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
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

