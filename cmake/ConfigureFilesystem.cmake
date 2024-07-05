# Copyright (c) 2019 - 2024 Geode-solutions
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

set(FILESYSTEM_PATH ${PROJECT_BINARY_DIR}/third_party/ghcFilesystem)
set(FILESYSTEM_INSTALL_PREFIX ${FILESYSTEM_PATH}/install)
ExternalProject_Add(ghcFilesystem
    PREFIX ${FILESYSTEM_PATH}
    SOURCE_DIR ${FILESYSTEM_PATH}/src
    BINARY_DIR ${FILESYSTEM_PATH}/build
    STAMP_DIR ${FILESYSTEM_PATH}/stamp
    GIT_REPOSITORY https://github.com/gulrak/filesystem
    GIT_TAG v1.5.14
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS
        -DCMAKE_INSTALL_MESSAGE=LAZY
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    CMAKE_CACHE_ARGS
        -DGHC_FILESYSTEM_BUILD_TESTING:BOOL=OFF
        -DGHC_FILESYSTEM_BUILD_EXAMPLES:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${FILESYSTEM_INSTALL_PREFIX}
)

ExternalProject_Add_StepTargets(ghcFilesystem download)
