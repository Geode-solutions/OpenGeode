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

set(EARCUT_PATH ${PROJECT_BINARY_DIR}/third_party/earcut)
set(EARCUT_INSTALL_PREFIX ${EARCUT_PATH}/install)
ExternalProject_Add(earcut
    PREFIX ${EARCUT_PATH}
    SOURCE_DIR ${EARCUT_PATH}/src
    BINARY_DIR ${EARCUT_PATH}/build
    STAMP_DIR ${EARCUT_PATH}/stamp
    GIT_REPOSITORY https://github.com/mapbox/earcut.hpp
    GIT_TAG 7fa7aa30183849e988ae79ab2eef19f9ae97acf4
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS
        -DCMAKE_INSTALL_MESSAGE=LAZY
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${EARCUT_INSTALL_PREFIX}
        -DEARCUT_BUILD_TESTS:BOOL=OFF
        -DEARCUT_BUILD_BENCH:BOOL=OFF
        -DEARCUT_BUILD_VIZ:BOOL=OFF
)

ExternalProject_Add_StepTargets(earcut download)
