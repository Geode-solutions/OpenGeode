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

set(ZLIB_PATH ${PROJECT_BINARY_DIR}/third_party/zlib)
set(ZLIB_INSTALL_PREFIX ${ZLIB_PATH}/install)
ExternalProject_Add(zlib
    PREFIX ${ZLIB_PATH}
    SOURCE_DIR ${ZLIB_PATH}/src
    BINARY_DIR ${ZLIB_PATH}/build
    STAMP_DIR ${ZLIB_PATH}/stamp
    GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng
    GIT_TAG 2.2.1
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_INSTALL_MESSAGE=LAZY
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DBUILD_SHARED_LIBS:BOOL=OFF
    CMAKE_CACHE_ARGS
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
        -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -DZLIB_ENABLE_TESTS:BOOL=OFF
        -DZLIB_COMPAT:BOOL=ON
        -DWITH_NEW_STRATEGIES:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${ZLIB_INSTALL_PREFIX}
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
)
