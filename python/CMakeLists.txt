cmake_minimum_required(VERSION 3.5.0)
project(OpenVDS-python)

find_package(PythonInterp REQUIRED)

set(python ${PYTHON_EXECUTABLE})
set(setup.py ${CMAKE_CURRENT_SOURCE_DIR}/setup.py)

add_custom_target(
    openvds-python ALL
    COMMENT "Building python library with setup.py"
    SOURCES ${setup.py}
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}

    COMMAND ${python} ${setup.py}
        # build the extension inplace (really, once its built, copy it to the
        # source tree) so that post-build, the directory can be used to run
        # tests against
        build_ext --inplace
        build # setup.py build args
            --cmake-executable ${CMAKE_COMMAND}
            --generator ${CMAKE_GENERATOR}
        -- # cmake to the extension
            -Dopenvds_DIR=${OPENVDS_LIB_BINARY_DIR}
            # "install" to the python/openvds dir with rpath, so there's no need
            # to fiddle with environment in ctest to load the core library from
            # the build tree
            -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON
            -DCMAKE_INSTALL_RPATH=$<TARGET_FILE_DIR:openvds>
            -DCMAKE_INSTALL_NAME_DIR=$<TARGET_FILE_DIR:openvds>
)

add_dependencies(openvds-python openvds)

# run tests with setup.py test
# this is very slow compared to invoking pytest directly, but setuptools will
# copy the built extension into the tree as it sees fit
add_test(NAME python.unit
    COMMAND ${python} ${setup.py} --skip-cmake test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
