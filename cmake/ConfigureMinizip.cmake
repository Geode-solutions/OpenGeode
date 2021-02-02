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

set(MINIZIP_PATH ${PROJECT_BINARY_DIR}/third_party/minizip)
set(MINIZIP_INSTALL_PREFIX ${MINIZIP_PATH}/install)
ExternalProject_Add(minizip
    PREFIX ${MINIZIP_PATH}
    BINARY_DIR ${MINIZIP_PATH}/src/minizip
    GIT_REPOSITORY https://github.com/nmoinvaz/minizip
    GIT_TAG 318e848843667f982f81dbcb72334e415ba482a5
    GIT_PROGRESS ON
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_MESSAGE=LAZY
        ${STANDARD}
    CMAKE_CACHE_ARGS
        -DMZ_COMPAT:BOOL=OFF
        -DMZ_ZLIB:BOOL=OFF
        -DMZ_BZIP2:BOOL=OFF
        -DMZ_LZMA:BOOL=OFF
        -DMZ_PKCRYPT:BOOL=OFF
        -DMZ_WZAES:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${MINIZIP_INSTALL_PREFIX}
)
