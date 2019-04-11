add_library(openvds
    File.h
    File.cpp
    File_Linux.cpp
    File_Win.cpp
    )

if (WIN32)
    set_source_files_properties(File_Linux.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
elseif (LINUX)
    set_source_files_properties(File_Win.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
