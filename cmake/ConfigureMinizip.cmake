# Copyright (c) 2019 - 2025 Geode-solutions
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

set(MINIZIP_PATH ${PROJECT_BINARY_DIR}/third_party/minizip)
set(MINIZIP_INSTALL_PREFIX ${MINIZIP_PATH}/install)
ExternalProject_Add(minizip
    PREFIX ${MINIZIP_PATH}
    SOURCE_DIR ${MINIZIP_PATH}/src
    BINARY_DIR ${MINIZIP_PATH}/build
    STAMP_DIR ${MINIZIP_PATH}/stamp
    GIT_REPOSITORY https://github.com/zlib-ng/minizip-ng
    GIT_TAG 4.0.7
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_MESSAGE=LAZY
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    CMAKE_CACHE_ARGS
        -DMZ_COMPAT:BOOL=OFF
        -DMZ_ZLIB:BOOL=OFF
        -DMZ_BZIP2:BOOL=OFF
        -DMZ_LZMA:BOOL=OFF
        -DMZ_PKCRYPT:BOOL=OFF
        -DMZ_WZAES:BOOL=OFF
        -DMZ_ZSTD:BOOL=OFF
        -DMZ_OPENSSL:BOOL=OFF
        -DMZ_LIBBSD:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${MINIZIP_INSTALL_PREFIX}
)

ExternalProject_Add_StepTargets(minizip download)
