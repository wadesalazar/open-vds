add_executable(VDSInfo VDSInfo.cpp Base64.cpp)

target_link_libraries(VDSInfo PUBLIC openvds fmt::fmt jsoncpp_lib_static)

setCompilerFlagsForTools(VDSInfo)
