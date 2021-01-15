cmake_minimum_required(VERSION 3.12.0)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(OpenVDS VERSION 1.1.999)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(GNUInstallDirs)

set (TEST_DATA_PATH "" CACHE PATH "Test data path")
set (TEST_SEGY_FILE "" CACHE FILEPATH "SEGY test file")
set (TEST_SEGY_HEADER_FILE "" CACHE FILEPATH "Json file describing the header data of SEGY test file")
set (TEST_URL "" CACHE STRING "Test url for OpenOptions")
set (TEST_CONNECTION "" CACHE STRING "Test connection string for OpenOptions")

set (OPENVDS_LIBDIR_SUFFIX "" CACHE STRING "Suffix to add to lib directories on linux")
set (OPENVDS_PYTHON_VERSION "" CACHE STRING "Exact Python 3 version to discover")
set (OPENVDS_JAVA_VERSION "" CACHE STRING "Exact Java version to discover")
set (OPENVDS_3RD_PARTY_DIR "" CACHE PATH "Path to put 3rd party source code, should be absolute path")

option(FORCE_NO_LIBDIR_SUFFIX "Force there to be no suffix on lib directories linux")

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if ("${LIB64}" STREQUAL "TRUE" AND NOT FORCE_NO_LIBDIR_SUFFIX)
  set(LIBSUFFIX 64)
else()
  set(LIBSUFFIX "")
endif()

set(BUILD_PYTHON_DEFAULT ON)
if (Python_ROOT_DIR)
  set(PYTHON_ROOT_DIR_HINT "HINTS ${Python_ROOT_DIR}")
  set(PYTHON_REQUIRED "REQUIRED")
endif()

if (OPENVDS_PYTHON_VERSION)
  find_package(Python3 ${OPENVDS_PYTHON_VERSION} EXACT REQUIRED COMPONENTS Interpreter Development ${PYTHON_ROOT_DIR_HINT})
else()
  find_package(Python3 ${PYTHON_REQUIRED} COMPONENTS Interpreter Development ${PYTHON_ROOT_DIR_HINT})
endif()

if (Python3_FOUND)
  set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
  set(PYTHONINTERP_FOUND ON)
else()
  message("Failed to find Python3 interpreter. Disabling Python build")
  set(BUILD_PYTHON_DEFAULT OFF)
endif()

if (OPENVDS_JAVA_VERSION)
  find_package(Java ${OPENVDS_JAVA_VERSION} COMPONENTS Development)
  find_package(JNI)
else()
  find_package(Java 1.8 COMPONENTS Development)
  find_package(JNI)
endif()

if (Java_FOUND AND JNI_FOUND)
  set(BUILD_JAVA_DEFAULT ON)
else()
  message("Failed to find Java compiler. Disabling Java build")
  set(BUILD_JAVA_DEFAULT OFF)
endif()

if (WIN32)
  set(DEFAULT_BUILD_ZLIB ON)
  set(DEFAULT_BUILD_CURL ON)
  set(DEFAULT_BUILD_UV ON)
  set(DEFAULT_USE_LIBRESSL ON)
else()
  set(DEFAULT_BUILD_ZLIB OFF)
  set(DEFAULT_BUILD_CURL OFF)
  set(DEFAULT_BUILD_UV OFF)
  set(DEFAULT_USE_LIBRESSL OFF)
endif()

option(BUILD_PYTHON "Build python bindings" ${BUILD_PYTHON_DEFAULT})
option(BUILD_JAVA "Build java bindings" ${BUILD_JAVA_DEFAULT})
option(ENABLE_OPENMP "Enable the OpenMP codepath" ON)
option(BUILD_SHARED_LIBS "Build OpenVDS as a shared library" ON)
option(BUILD_EXAMPLES "Build OpenVDS example projects" ON)
option(BUILD_TESTS "Build OpenVDS tests" ON)
option(BUILD_DOCS  "Build OpenVDS docs" OFF)
option(ENABLE_RUNPATH_ORIGIN "On Linux make the install have ORIGIN as RUNPATH" ON)
option(DISABLE_STRICT_WARNINGS "Disable strict warnings settings. This includes making warnings to errors." OFF)
option(DISABLE_AWS_IOMANAGER "Disable compiling the Aws-cpp-sdk IOManager backend." OFF)
option(DISABLE_AZURE_IOMANAGER "Disable compiling Azure-storage-cpp and cpprestapi." OFF)
option(DISABLE_AZURE_PRESIGNED_IOMANAGER "Disable compiling Azure presigned IOManager backend." OFF)
option(DISABLE_GCP_IOMANAGER "Disable compiling GCP IOManager backend." OFF)
option(BUILD_ZLIB   "Build zlib" ${DEFAULT_BUILD_ZLIB})
option(BUILD_CURL   "Build libcurl as part of the openVDS build" ${DEFAULT_BUILD_CURL})
option(BUILD_UV     "Build libuv as part of the openVDS build" ${DEFAULT_BUILD_UV})
option(USE_LIBRESSL "Build libressl as part of the openVDS build and use it instead of OpenSSL" ${DEFAULT_USE_LIBRESSL})

include(CMakeDependentOption)

cmake_dependent_option(
    DISABLE_CURL_IOMANAGER "Disable all IOManagers based on CURL."
    ON "DISABLE_AZURE_PRESIGNED_IOMANAGER;DISABLE_GCP_IOMANAGER" OFF)
mark_as_advanced(DISABLE_CURL_IOMANAGER)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

define_property(GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE BRIEF_DOCS "Runtime libraries" FULL_DOCS "Files that will be copied to test folders")
define_property(GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG   BRIEF_DOCS "Runtime libraries" FULL_DOCS "Files that will be copied to test folders")
define_property(GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_RELEASE BRIEF_DOCS "OpenVDS link libraries" FULL_DOCS "Files OpenVDS will link with")
define_property(GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_DEBUG   BRIEF_DOCS "OpenVDS link libraries" FULL_DOCS "Files OpenVDS will link with")
define_property(GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES BRIEF_DOCS "OpenVDS include libraries" FULL_DOCS "Include paths for 3rdparty")
define_property(GLOBAL PROPERTY OPENVDS_DEPENDENCY_TARGETS BRIEF_DOCS "Dependency targets for OpenVDS" FULL_DOCS "Targets that OpenVDS needs to build first")

if(WIN32)
  if(CMAKE_VS_PLATFORM_TOOLSET)
    string(REGEX REPLACE "^v" "" MSVC_TOOLSET_VERSION_LOCAL "${CMAKE_VS_PLATFORM_TOOLSET}")
  else()
    set(MSVC_TOOLSET_VERSION_LOCAL ${MSVC_TOOLSET_VERSION})
  endif()
endif()

if (${MSVC_TOOLSET_VERSION_LOCAL})
  set(LIB_TOOLSET_DIR "/msvc_${MSVC_TOOLSET_VERSION_LOCAL}")
endif()

set(LIB_TOOLSET_DIR "${LIB_TOOLSET_DIR}$<$<NOT:$<CONFIG:Release>>:/$<CONFIG>>")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads)
if (ENABLE_OPENMP)
    find_package(OpenMP)
endif()
if (MSVC)
  if (MSVC_VERSION GREATER_EQUAL 1910 AND NOT OpenMP_CXX_FOUND)
    add_compile_options(/permissive-)
  endif()
  if(MSVC_VERSION GREATER_EQUAL 1915)
    add_compile_options( /JMC )
  endif()
endif()

if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
  add_compile_options(-Werror=return-type)
endif()

include(CMake/SetWarnings.cmake)

include(CMake/Fetch3rdPartyInBuild.cmake)
Fetch3rdParty()

include(CMake/Build3rdParty.cmake)
build3rdparty()

include(CMake/CopyDllForTarget.cmake)
include(CMake/AddSystemHeadersToTarget.cmake)

#this is disabled in Java and Python since they match dl open with a string
set(CMAKE_DEBUG_POSTFIX d)

add_subdirectory(src)
add_subdirectory(tools)

if (BUILD_PYTHON)
  add_subdirectory(python)
endif ()

if (BUILD_JAVA)
  add_subdirectory(java)
endif()

if (BUILD_DOCS)
  add_subdirectory(docs)
endif()

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

file(RELATIVE_PATH INSTALLED_LIB_RELATIVE_TO_EXAMPLES "${PROJECT_SOURCE_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/openvds/examples" "${PROJECT_SOURCE_DIR}/${CMAKE_INSTALL_DATAROOTDIR}")
configure_file(CMake/InstalledPaths.cmake.in ${CMAKE_BINARY_DIR}/CMake/InstalledPaths.cmake)
install(FILES CMake/CopyDllForTarget.cmake CMake/SetWarnings.cmake ${CMAKE_BINARY_DIR}/CMake/InstalledPaths.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/openvds/examples/modules)
install(DIRECTORY examples DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/openvds)

if (NOT DEFINED CPACK_GENERATOR)
  if (UNIX)
    set (CPACK_GENERATOR "TGZ")
  elseif(WIN32)
    set (CPACK_GENERATOR "ZIP")
  endif()
endif()

if (UNIX)
    find_program(LSB_RELEASE_EXEC lsb_release)
    execute_process(COMMAND ${LSB_RELEASE_EXEC} -irs
        OUTPUT_VARIABLE OS_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # OS_NAME is not empty, check for "CentOS" and replace it with "redhat-"
    if (NOT "${OS_NAME}" STREQUAL "")
        STRING(REPLACE "CentOS " "redhat-" OS_NAME ${OS_NAME})
        STRING(REGEX MATCH "(redhat-)[1-9]" OS_NAME ${OS_NAME})
    endif ()

    # if OS_NAME is still empty, set it to Linux
    if ("${OS_NAME}" STREQUAL "")
        set(OS_NAME "Linux")
    endif()
else ()
    # Always set OS_NAME to "win" for windows
    set(OS_NAME "win-${MSVC_TOOLSET_VERSION_LOCAL}")
endif ()

if (NOT DEFINED PACKAGE_RELEASE)
   set(PACKAGE_RELEASE "1")
endif()

set (CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set (CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package-install")
set (CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${PACKAGE_RELEASE}-${OS_NAME}")

include(CPack)

