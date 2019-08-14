# Copyright (c) 2019 Geode-solutions
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

# Define the project
project(OpenGeode CXX)

set(UTILS_FILE "${PROJECT_SOURCE_DIR}/cmake/utils.cmake")
file(READ "${UTILS_FILE}" OPENGEODE_UTILS)
include("${UTILS_FILE}")

# Get OpenGeode dependencies
find_package(Async++ REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${ASYNCPLUSPLUS_INSTALL_PREFIX})
find_package(Bitsery REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${BITSERY_INSTALL_PREFIX})
find_package(ghcfilesystem REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${FILESYSTEM_INSTALL_PREFIX})
find_package(minizip REQUIRED CONFIGS "minizip.cmake" NO_DEFAULT_PATH PATHS ${MINIZIP_INSTALL_PREFIX})
find_package(nanoflann REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${NANOFLANN_INSTALL_PREFIX})
find_package(spdlog REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${SPDLOG_INSTALL_PREFIX})
find_package(Threads REQUIRED)

copy_windows_binaries(Async++)

# Install OpenGeode third-parties
install(
    DIRECTORY
        ${ASYNCPLUSPLUS_INSTALL_PREFIX}/
        ${BITSERY_INSTALL_PREFIX}/
        ${MINIZIP_INSTALL_PREFIX}/
    DESTINATION
        .
)

#------------------------------------------------------------------------------------------------
# Configure the OpenGeode libraries
add_geode_library(geode/basic)
add_geode_library(geode/mesh)
add_geode_library(geode/georepresentation)

#------------------------------------------------------------------------------------------------
# Optional modules configuration
if(OPENGEODE_WITH_TESTS)
    message(STATUS "Configuring OpenGeode with tests")
    enable_testing()
    add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------------------------
# Configure CPack
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

# This must always be last!
include(CPack)
