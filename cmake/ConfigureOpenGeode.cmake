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

if(OPENGEODE_WITH_PYTHON OR INCLUDE_PYBIND11)
    list(APPEND bindings pybind11)
endif()

set(OpenGeode_PATH_BIN ${PROJECT_BINARY_DIR}/opengeode)
set(OpenGeode_PATH_INSTALL ${OpenGeode_PATH_BIN}/install)
ExternalProject_Add(opengeode
    PREFIX ${OpenGeode_PATH_BIN}
    SOURCE_DIR ${PROJECT_SOURCE_DIR}
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
    CMAKE_ARGS
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_MESSAGE=LAZY
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    CMAKE_CACHE_ARGS
        -DPACKAGE_VERSION:STRING=${PACKAGE_VERSION}
        -DOPENGEODE_WITH_TESTS:BOOL=${OPENGEODE_WITH_TESTS}
        -DOPENGEODE_WITH_PYTHON:BOOL=${OPENGEODE_WITH_PYTHON}
        -DINCLUDE_PYBIND11:BOOL=${INCLUDE_PYBIND11}
        -DUSE_SUPERBUILD:BOOL=OFF
        -DASYNCPLUSPLUS_INSTALL_PREFIX:PATH=${ASYNCPLUSPLUS_INSTALL_PREFIX}
        -DBITSERY_INSTALL_PREFIX:PATH=${BITSERY_INSTALL_PREFIX}
        -DFILESYSTEM_INSTALL_PREFIX:PATH=${FILESYSTEM_INSTALL_PREFIX}
        -DMINIZIP_INSTALL_PREFIX:PATH=${MINIZIP_INSTALL_PREFIX}
        -DNANOFLANN_INSTALL_PREFIX:PATH=${NANOFLANN_INSTALL_PREFIX}
        -DSPDLOG_INSTALL_PREFIX:PATH=${SPDLOG_INSTALL_PREFIX}
        -DABSEIL_INSTALL_PREFIX:PATH=${ABSEIL_INSTALL_PREFIX}
        -DPYBIND11_INSTALL_PREFIX:PATH=${PYBIND11_INSTALL_PREFIX}
        -DPYBIND11_PYTHON_VERSION:STRING=${PYTHON_VERSION}
        -DCMAKE_INSTALL_PREFIX:PATH=${OpenGeode_PATH_INSTALL}    
    BINARY_DIR ${OpenGeode_PATH_BIN}
    DEPENDS
        abseil
        asyncplusplus
        bitsery
        ghcFilesystem
        minizip
        nanoflann
        spdlog
        ${bindings}
)

