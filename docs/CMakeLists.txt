# Based on https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/
find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

# Find all the public headers
get_target_property(OPENVDS_PUBLIC_HEADER_DIR openvds  INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE OPENVDS_PUBLIC_HEADERS ${OPENVDS_PUBLIC_HEADER_DIR}/*.h)

set(DOXYGEN_INPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src/OpenVDS)
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml)
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)

doxygen_add_docs(Doxygen
    ${DOXYGEN_INPUT_DIRECTORY}
    COMMENT "Generating docs")

#set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
#set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
#configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
#file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY})

# Only regenerate Doxygen when the Doxyfile or public headers change
#add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
#                   DEPENDS ${OPENVDS_PUBLIC_HEADERS}
#                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
#                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
#                   COMMENT "Generating docs"
#                   VERBATIM)

# Nice named target so we can run the job easily
#add_custom_target(Doxygen DEPENDS ${DOXYGEN_INDEX_FILE})

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/html)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

if (UNIX)
  set(separator ":")
else ()
  set(separator ";")
endif()

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=$<TARGET_FILE_DIR:core>/.." "PATH=${separator}${CMAKE_CURRENT_BINARY_DIR}"
                     ${Python3_EXECUTABLE} -m sphinx -b html
                     # Tell Breathe where to find the Doxygen output
                     -Dbreathe_projects.OpenVDS=${DOXYGEN_OUTPUT_DIRECTORY}/xml
                   ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                   # Other docs files you want to track should go here (or in some variable)
                   ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                   ${CMAKE_CURRENT_SOURCE_DIR}/getting-started.rst
                   ${CMAKE_CURRENT_SOURCE_DIR}/python-api.rst
                   ${CMAKE_CURRENT_SOURCE_DIR}/cpp-api.rst
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")

# Nice named target so we can run the job easily
add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})
add_dependencies(Sphinx Doxygen)

# Add an install target to install the docs
install(DIRECTORY ${SPHINX_BUILD} DESTINATION ${CMAKE_INSTALL_DOCDIR})
