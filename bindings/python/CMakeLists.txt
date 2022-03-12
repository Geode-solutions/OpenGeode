# Copyright (c) 2019 - 2022 Geode-solutions
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

add_subdirectory(src)

if(OPENGEODE_WITH_TESTS)
    add_subdirectory(tests)
endif()

configure_file(
    __init__.py
    ${PROJECT_BINARY_DIR}/wheel/opengeode/__init__.py
)
configure_file(
    setup.py
    ${PROJECT_BINARY_DIR}/wheel/setup.py
)
message(STATUS "WHEEL_VERSION = ${WHEEL_VERSION}")
message(STATUS "WHEEL_PLATFORM = ${WHEEL_PLATFORM}")
add_custom_target(wheel
    COMMAND ${CMAKE_COMMAND} 
        -DSOURCE_DIR="${CMAKE_INSTALL_PREFIX}"
        -DDEST_DIR="${CMAKE_BINARY_DIR}/wheel/opengeode"
        -P "${CMAKE_CURRENT_LIST_DIR}/copy_wheel.cmake"
    COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel --plat-name ${WHEEL_PLATFORM}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/wheel
)
