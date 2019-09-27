cmake_minimum_required(VERSION 3.5.0)
project(openvds-python-extension LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_C_VISIBILITY_PRESET   "hidden")

find_package(PythonExtensions REQUIRED)

find_package(Threads REQUIRED)
if(ENABLE_OPENMP)
  find_package(OpenMP REQUIRED)
endif()
find_package(ZLIB REQUIRED)
find_package(openvds REQUIRED)

add_library(core MODULE core.cpp)
target_include_directories(core
    PRIVATE
        ${PYBIND11_INCLUDE_DIRS}
)
python_extension_module(core)
target_link_libraries(core openvds::openvds)

if (MSVC)
    target_compile_options(core
        BEFORE
        PRIVATE
            /EHsc
    )
endif ()

install(TARGETS core LIBRARY DESTINATION openvds)
