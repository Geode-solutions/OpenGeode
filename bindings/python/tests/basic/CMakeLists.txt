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
)
add_geode_python_test(
    SOURCE "test-py-python-requirements.py"
    ARGUMENTS
        "${PROJECT_SOURCE_DIR}/bindings/python/requirements.in"
        "${PROJECT_BINARY_DIR}/${CMAKE_CONFIG_DESTINATION}/python_requirements.in"
        "${PROJECT_BINARY_DIR}/python_requirements.txt"
        "${PROJECT_BINARY_DIR}/python_geode_requirements.txt"
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
