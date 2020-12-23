add_executable(SEGYImport
  SEGYImport.cpp
  )

target_link_libraries(SEGYImport PRIVATE Threads::Threads openvds segyutils jsoncpp_lib_static fmt::fmt)

setCompilerFlagsForTools(SEGYImport)

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} setargv.obj")
endif()