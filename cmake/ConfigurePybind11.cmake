# Copyright (c) 2019 - 2021 Geode-solutions
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

set(PYBIND11_PATH ${PROJECT_BINARY_DIR}/third_party/pybind11)
set(PYBIND11_INSTALL_PREFIX ${PYBIND11_PATH}/install)
if(WIN32)
    if(CMAKE_CXX_FLAGS_DEBUG)
        string(REPLACE "/MDd" "/MD" NEW_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
    endif()
endif()
ExternalProject_Add(pybind11
    PREFIX ${PYBIND11_PATH}
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG c9a319c607a5fcddf14417483a451bd85083da72
    GIT_PROGRESS ON
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_INSTALL_MESSAGE=LAZY
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    CMAKE_CACHE_ARGS
        -DCMAKE_C_FLAGS_DEBUG:INTERNAL=${NEW_FLAGS}
        -DCMAKE_CXX_FLAGS_DEBUG:INTERNAL=${NEW_FLAGS}
        -DPYBIND11_PYTHON_VERSION:STRING=${PYTHON_VERSION}
        -DPYBIND11_INSTALL:BOOL=ON
        -DPYBIND11_TEST:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${PYBIND11_INSTALL_PREFIX}
        -DPYBIND11_CMAKECONFIG_INSTALL_DIR:STRING=lib/cmake/pybind11
)
