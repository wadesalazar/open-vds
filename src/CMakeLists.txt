add_library(openvds
    File.h
    File.cpp
    Linux_File.cpp
    Win_File.cpp
    )

if (WIN32)
    set_source_files_properties(Linux_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
elseif (LINUX)
    set_source_files_properties(Win_File.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
