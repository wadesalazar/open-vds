add_library(openvds
    File.h
    File.cpp
    Linux_File.cpp
    Win_File.cpp
    )

add_dependencies(openvds aws-sdk-cpp)
target_include_directories(openvds PRIVATE "${AWS_INCLUDE_PATH}")
target_link_libraries(openvds Threads::Threads ${AWS_LIBS})

if (WIN32)
    set_source_files_properties(Linux_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
elseif (UNIX)
    set_source_files_properties(Win_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
