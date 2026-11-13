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

# The bulk access functions (numpy arrays) are tested with numpy, which is a
# runtime dependency of the wheel (bindings/python/requirements.in)
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import numpy"
    RESULT_VARIABLE numpy_missing
    OUTPUT_QUIET
    ERROR_QUIET
)
if(numpy_missing)
    message(FATAL_ERROR
        "The Python tests need numpy in ${PYTHON_EXECUTABLE}. Install it with:\n"
        "  ${PYTHON_EXECUTABLE} -m pip install numpy"
    )
endif()

add_subdirectory(basic)
add_subdirectory(geometry)
add_subdirectory(image)
add_subdirectory(mesh)
add_subdirectory(model)

# The stubs of every binding module must pass the quality gates (named
# parameters, no unresolved or raw C++ types, stubtest)
add_geode_python_stub_test(
    MODULES
        opengeode_py_basic
        opengeode_py_geometry
        opengeode_py_image
        opengeode_py_gdal
        opengeode_py_mesh
        opengeode_py_model
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_gdal
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)

# The pure-Python modules (their stubs are only generated and checked: stubtest
# is run on the whole package by the wheel target)
add_geode_python_stub_test(
    NAME test-py-wrapper-stubs
    NO_STUBTEST
    MODULES
        basic
        geometry
        image
        mesh
        model
        gdal
    DEPENDENCIES
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_image
        ${PROJECT_NAME}::py_gdal
        ${PROJECT_NAME}::py_mesh
        ${PROJECT_NAME}::py_model
)
