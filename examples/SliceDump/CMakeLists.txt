add_executable(slicedump main.cpp)
target_link_libraries(slicedump PRIVATE openvds::openvds)
if (OpenMP_CXX_FOUND)
    target_link_libraries(slicedump PRIVATE OpenMP::OpenMP_CXX)
endif()
set_target_properties(slicedump PROPERTIES FOLDER examples)

setWarningFlagsForTarget(slicedump)
copyDllForTarget(slicedump)
