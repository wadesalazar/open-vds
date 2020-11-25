add_executable(slicedump main.cpp)
target_link_libraries(slicedump openvds::openvds)
set_target_properties(slicedump PROPERTIES FOLDER examples)

setWarningFlagsForTarget(slicedump)
copyDllForTarget(slicedump)
