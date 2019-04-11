cmake_minimum_required(VERSION 3.12)
project(Open-VDS)

set (CMAKE_CXX_STANDARD 11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)

add_subdirectory(tests)
