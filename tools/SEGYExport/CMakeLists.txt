add_executable(SEGYExport
  SEGYExport.cpp
  )

target_link_libraries(SEGYExport PUBLIC Threads::Threads openvds segyutils jsoncpp_lib_static fmt::fmt)

setCompilerFlagsForTools(SEGYExport)
