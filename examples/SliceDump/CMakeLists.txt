add_executable(slicedump main.cpp)
target_link_libraries(slicedump openvds fmt::fmt)
set_target_properties(slicedump PROPERTIES FOLDER examples)

copyDllForTarget(slicedump)
