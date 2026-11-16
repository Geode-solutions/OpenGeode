# Copyright (c) 2019 - 2026 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

add_geode_python_test(
    SOURCE "test-py-attribute.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-attribute-numpy.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-gil-helper.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-uuid.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-input.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-logger.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-exception.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-additional-files.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-logger-bridge.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-stub-gates.py"
    ARGUMENTS "${PROJECT_SOURCE_DIR}/cmake/check_python_stubs.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-package-tools.py"
    ARGUMENTS "${PROJECT_SOURCE_DIR}/cmake/geode_python_package.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-python-requirements.py"
    ARGUMENTS
        "${PROJECT_SOURCE_DIR}/bindings/python/requirements.in"
        "${PROJECT_BINARY_DIR}/${CMAKE_CONFIG_DESTINATION}/${PROJECT_NAME}_python_requirements.in"
        "${PROJECT_BINARY_DIR}/python_requirements.txt"
        "${PROJECT_BINARY_DIR}/python_geode_requirements.txt"
)
# Compile flags of a downstream binding (pybind11, OpenGeode mesh and model)
# for the test compiling the installed helpers: the transitive include
# directories and definitions, without bindings/python/src
find_package(pybind11 REQUIRED)
add_library(geode_installed_helpers_flags INTERFACE)
target_link_libraries(geode_installed_helpers_flags
    INTERFACE pybind11::module ${PROJECT_NAME}::mesh ${PROJECT_NAME}::model
)
set(helpers_flags "${PROJECT_BINARY_DIR}/installed_helpers_flags.cmake")
file(GENERATE OUTPUT "${helpers_flags}" CONTENT
"set(CHECK_INCLUDES \"$<TARGET_PROPERTY:geode_installed_helpers_flags,INTERFACE_INCLUDE_DIRECTORIES>\")
set(CHECK_DEFINITIONS \"$<TARGET_PROPERTY:geode_installed_helpers_flags,INTERFACE_COMPILE_DEFINITIONS>\")
set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
set(CMAKE_CXX_COMPILER \"${CMAKE_CXX_COMPILER}\")
set(CMAKE_BUILD_TYPE Release)
")
add_geode_python_test(
    SOURCE "test-py-installed-helpers.py"
    ARGUMENTS
        "${CMAKE_COMMAND}"
        "${PROJECT_BINARY_DIR}/bindings/python/helpers/cmake_install.cmake"
        "${PROJECT_SOURCE_DIR}/bindings/python/src"
        "${helpers_flags}"
        "${CMAKE_GENERATOR}"
)
add_geode_python_test(
    SOURCE "test-py-helpers-python-off.py"
    ARGUMENTS
        "${CMAKE_COMMAND}"
        "${PROJECT_SOURCE_DIR}/bindings/python/helpers"
        "${PROJECT_SOURCE_DIR}/bindings/python/src"
        "${PROJECT_SOURCE_DIR}/cmake/OpenGeode.cmake"
        "${CMAKE_GENERATOR}"
)
add_geode_python_test(
    SOURCE "test-py-installed-wheels.py"
    ARGUMENTS
        "${CMAKE_COMMAND}"
        "${PROJECT_SOURCE_DIR}/cmake/PythonTargets.cmake"
        "${CMAKE_GENERATOR}"
)
add_geode_python_test(
    SOURCE "test-py-python-environments.py"
    ARGUMENTS "${CMAKE_COMMAND}" "${PROJECT_SOURCE_DIR}/cmake/PythonTargets.cmake"
)
# Test module standing for a downstream binding (import_binding_dependency of
# a module of another package), built aside the binding modules
pybind11_add_module(geode_test_import_dependency MODULE
    "import_dependency_module.cpp"
)
target_link_libraries(geode_test_import_dependency PRIVATE ${PROJECT_NAME}::basic)
set_target_properties(geode_test_import_dependency
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python_test_modules"
        FOLDER "Bindings/Python/Tests"
)
add_geode_python_test(
    SOURCE "test-py-import-dependency.py"
    ARGUMENTS "$<TARGET_FILE:geode_test_import_dependency>"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
)
add_geode_python_test(
    SOURCE "test-py-signatures.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_gdal
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-keyword-arguments.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-module-import.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_gdal
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-package-layout.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_gdal
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-wheel-layout.py"
    ARGUMENTS "${PROJECT_BINARY_DIR}/wheel/${project_name}"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_gdal
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-repr.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-naming.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-generic-api.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-generic-api-typing.py"
    ARGUMENTS
        "${PROJECT_SOURCE_DIR}/cmake/check_python_stubs.py"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_gdal
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
add_geode_python_test(
    SOURCE "test-py-package-docs.py"
    ARGUMENTS
        "${PROJECT_SOURCE_DIR}/bindings/python/opengeode_doc.txt"
        "${PROJECT_BINARY_DIR}/wheel/${project_name}"
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_gdal
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
