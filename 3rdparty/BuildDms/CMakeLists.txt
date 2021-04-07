
if(WIN32)
  list(APPEND CMAKE_MODULE_PATH ${dmsCMakeDir}/modules/version_info_win32)
endif()

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${dmsCMakeDir}/src/core)
include_directories(${dmsCMakeDir}/src/lib)
include_directories(${dmsCMakeDir}/src/lib/cloud/providers)

if (NOT DISABLE_AZURE_IOMANAGER)
  add_definitions( -DHAS_AZURE_BLOB_STORAGE_PROVIDER )
  set(BUILD_POLYCLOUD ON)
  add_definitions(-DPOLYCLOUD)
  file(GLOB SRC_LIB_PROVIDERS_AZURE ${dmsCMakeDir}/src/lib/cloud/providers/azure/*.cc)
endif()
# retrieve sources
file(GLOB SRC_CORE ${dmsCMakeDir}/src/core/*.cc)
file(GLOB SRC_LIB_ACCESSORS ${dmsCMakeDir}/src/lib/accessors/*.cc)
file(GLOB SRC_LIB_PROVIDERS ${dmsCMakeDir}/src/lib/cloud/providers/*.cc)
file(GLOB SRC_LIB_HTTP ${dmsCMakeDir}/src/lib/http/*.cc)
file(GLOB SRC_LIB_JSON ${dmsCMakeDir}/src/lib/json/*.cpp)
file(GLOB SRC_LIB_AUTH ${dmsCMakeDir}/src/lib/auth/*.cc)
file(GLOB SRC_LIB_SHARED ${dmsCMakeDir}/src/lib/shared/*.cc)
set(SOURCES ${SRC_CORE} ${SRC_LIB_ACCESSORS} ${SRC_LIB_PROVIDERS} ${SRC_LIB_HTTP} ${SRC_LIB_JSON} ${SRC_LIB_AUTH} ${SRC_LIB_SHARED})
if(BUILD_POLYCLOUD)
  set(SOURCES ${SOURCES} ${SRC_LIB_PROVIDERS_AZURE})
endif()

# set major library versions
if(NOT DEFINED LIB_VERSION_MAJOR)
  set(LIB_VERSION_MAJOR 0)
endif()

# set minor library versions
if(NOT DEFINED LIB_VERSION_MINOR)
  set(LIB_VERSION_MINOR 0)
endif()

# set patch library versions
if(NOT DEFINED LIB_VERSION_PATCH)
  if(DEFINED LIBVERSION) # backward compatibility (old naming convention for patch)
    set(LIB_VERSION_PATCH ${LIBVERSION})
  elseif(DEFINED SDAPI_WIN_VERSION) # backward compatibility (old naming convention for patch)
    set(LIB_VERSION_PATCH ${SDAPI_WIN_VERSION})
  else()
    set(LIB_VERSION_PATCH 0)
  endif()
endif()

# set the versioninfo file for win32
if(WIN32)
  include(generate_product_version)
  generate_product_version(
    VersionFilesOutputVariable
    NAME "sdapi"
    VERSION_MAJOR ${LIB_VERSION_MAJOR}
    VERSION_MINOR ${LIB_VERSION_MINOR}
    VERSION_PATCH ${LIB_VERSION_PATCH}
    COMPANY_NAME "Schlumberger"
  )
endif()


add_library (sdapi_objects OBJECT ${SOURCES} ${VersionFilesOutputVariable})

get_property(link_3rdparty_release GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_RELEASE)
get_property(link_3rdparty_debug   GLOBAL PROPERTY OPENVDS_LINK_LIBRARIES_DEBUG)
get_property(runtime_3rdparty_release GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_RELEASE)
get_property(runtime_3rdparty_debug   GLOBAL PROPERTY OPENVDS_RUNTIME_LIBS_DEBUG)
get_property(include_3rdparty      GLOBAL PROPERTY OPENVDS_INCLUDE_LIBRARIES)
get_property(dependency_targets_3rdparty      GLOBAL PROPERTY OPENVDS_DEPENDENCY_TARGETS)


addSystemHeadersToTarget(sdapi_objects "${include_3rdparty}")

if (UNIX)
  find_package(Boost REQUIRED COMPONENTS system date_time regex)
  target_link_libraries(sdapi_objects PRIVATE Boost::system)
endif()

add_library(sdapi SHARED $<TARGET_OBJECTS:sdapi_objects>)

foreach(DEP ${dependency_targets_3rdparty})
  add_dependencies(sdapi_objects ${DEP})
endforeach()

target_link_libraries(sdapi PUBLIC sdapi_objects)
foreach(item ${link_3rdparty_debug})
    target_link_libraries(sdapi PRIVATE debug ${item})
endforeach()
foreach(item ${link_3rdparty_release})
    target_link_libraries(sdapi PRIVATE optimized ${item})
endforeach()

setLTOForTarget(sdapi_objects)
setLTOForTarget(sdapi)

if (WIN32)
  if(DEFINED LIB_VERSION_ON_NAME)
    set_target_properties(sdapi PROPERTIES
      OUTPUT_NAME "sdapi_${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}"
      POSITION_INDEPENDENT_CODE ON)
  else()
    set_target_properties(sdapi PROPERTIES
      OUTPUT_NAME "sdapi"
      POSITION_INDEPENDENT_CODE ON)      
  endif()
else()
  set_target_properties(sdapi PROPERTIES
    SOVERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}
    VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
    CXX_VISIBILITY_PRESET hidden
    OUTPUT_NAME "sdapi"
    POSITION_INDEPENDENT_CODE ON)

endif()

install(TARGETS sdapi sdapi_objects
  EXPORT sdapi-export
  ARCHIVE
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR}
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_TOOLSET_DIR}
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}${LIB_TOOLSET_DIR}
  FRAMEWORK
    DESTINATION framework
)

