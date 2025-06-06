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
        -DCPACK_SYSTEM_NAME=${CPACK_SYSTEM_NAME}
        -DCPACK_PACKAGE_VERSION=${CPACK_PACKAGE_VERSION}
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DCMAKE_INSTALL_MESSAGE=LAZY
    CMAKE_CACHE_ARGS
        -DWHEEL_VERSION:STRING=${WHEEL_VERSION}
        -DOPENGEODE_WITH_TESTS:BOOL=${OPENGEODE_WITH_TESTS}
        -DOPENGEODE_WITH_PYTHON:BOOL=${OPENGEODE_WITH_PYTHON}
        -DINCLUDE_PYBIND11:BOOL=${INCLUDE_PYBIND11}
        -DUSE_SUPERBUILD:BOOL=OFF
        -DASYNCPLUSPLUS_INSTALL_PREFIX:PATH=${ASYNCPLUSPLUS_INSTALL_PREFIX}
        -DBITSERY_INSTALL_PREFIX:PATH=${BITSERY_INSTALL_PREFIX}
        -DEARCUT_INSTALL_PREFIX:PATH=${EARCUT_INSTALL_PREFIX}
        -DMINIZIP_INSTALL_PREFIX:PATH=${MINIZIP_INSTALL_PREFIX}
        -DNANOFLANN_INSTALL_PREFIX:PATH=${NANOFLANN_INSTALL_PREFIX}
        -DSPDLOG_INSTALL_PREFIX:PATH=${SPDLOG_INSTALL_PREFIX}
        -DABSEIL_INSTALL_PREFIX:PATH=${ABSEIL_INSTALL_PREFIX}
        -DPROJ_INSTALL_PREFIX:PATH=${PROJ_INSTALL_PREFIX}
        -DGDAL_INSTALL_PREFIX:PATH=${GDAL_INSTALL_PREFIX}
        -DSQLITE_INSTALL_PREFIX:PATH=${SQLITE_INSTALL_PREFIX}
        -DPYBIND11_INSTALL_PREFIX:PATH=${PYBIND11_INSTALL_PREFIX}
        -DPYBIND11_PYTHON_VERSION:STRING=${PYTHON_VERSION}
        -DCMAKE_INSTALL_PREFIX:PATH=${OpenGeode_PATH_INSTALL}
    BINARY_DIR ${OpenGeode_PATH_BIN}
    DEPENDS
        abseil
        asyncplusplus
        bitsery
        earcut
        gdal
        minizip
        nanoflann
        spdlog
        ${bindings}
)

ExternalProject_Add_StepTargets(opengeode configure)

add_custom_target(third_party
    DEPENDS
        opengeode-configure
)

add_custom_target(download
    DEPENDS
        abseil-download
        asyncplusplus-download
        bitsery-download
        earcut-download
        gdal-download
        minizip-download
        nanoflann-download
        proj-download
        spdlog-download
        sqlite-download
)
if(OPENGEODE_WITH_PYTHON OR INCLUDE_PYBIND11)
    add_dependencies(download pybind11-download)
endif()

add_custom_target(post_install 
    ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${OpenGeode_PATH_INSTALL}/bin ${PROJECT_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${OpenGeode_PATH_INSTALL}/lib ${PROJECT_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${OpenGeode_PATH_INSTALL}/cmake ${PROJECT_BINARY_DIR}/cmake
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${OpenGeode_PATH_INSTALL}/include ${PROJECT_BINARY_DIR}/include
    DEPENDS
        opengeode)

install(DIRECTORY ${OpenGeode_PATH_INSTALL}/ DESTINATION .)
