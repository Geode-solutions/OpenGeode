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

set(SQLITE_PATH ${PROJECT_BINARY_DIR}/third_party/sqlite)
set(SQLITE_INSTALL_PREFIX ${SQLITE_PATH}/install)
ExternalProject_Add(sqlite
    PREFIX ${SQLITE_PATH}
    SOURCE_DIR ${SQLITE_PATH}/src
    BINARY_DIR ${SQLITE_PATH}/build
    STAMP_DIR ${SQLITE_PATH}/stamp
    GIT_REPOSITORY https://github.com/Geode-solutions/sqlite-amalgamation
    GIT_TAG e22d590f9af3e8cf8e61b9967ef447e29ef5c099
    GIT_PROGRESS ON
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_MESSAGE=LAZY
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
        -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${SQLITE_INSTALL_PREFIX}
        -DBUILD_SHELL:BOOL=ON
)

ExternalProject_Add_StepTargets(sqlite download)
