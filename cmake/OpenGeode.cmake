# Copyright (c) 2019 - 2023 Geode-solutions
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

set(OPENGEODE_CXX_STANDARD ${CMAKE_CXX_STANDARD})

include(cmake/GlobalOptions.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/ProjectOptions.cmake)
include(cmake/CppTargets.cmake)
include(cmake/PythonTargets.cmake)

# Get OpenGeode dependencies
find_package(absl REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${ABSEIL_INSTALL_PREFIX})
find_package(Async++ REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${ASYNCPLUSPLUS_INSTALL_PREFIX})
find_package(Bitsery REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${BITSERY_INSTALL_PREFIX})
find_package(ghc_filesystem REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${FILESYSTEM_INSTALL_PREFIX})
find_package(minizip-ng REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${MINIZIP_INSTALL_PREFIX})
find_package(nanoflann REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${NANOFLANN_INSTALL_PREFIX})
find_package(spdlog REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${SPDLOG_INSTALL_PREFIX})
find_package(Threads REQUIRED)

# Install OpenGeode third-parties
install(
    DIRECTORY
        ${ABSEIL_INSTALL_PREFIX}/
        ${ASYNCPLUSPLUS_INSTALL_PREFIX}/
        ${BITSERY_INSTALL_PREFIX}/
        ${FILESYSTEM_INSTALL_PREFIX}/
        ${GDAL_INSTALL_PREFIX}/
        ${PROJ_INSTALL_PREFIX}/
    DESTINATION
        .
    COMPONENT
        public
)
if(NOT BUILD_SHARED_LIBS)
    install(
        DIRECTORY
            ${MINIZIP_INSTALL_PREFIX}/
            ${NANOFLANN_INSTALL_PREFIX}/
            ${SPDLOG_INSTALL_PREFIX}/
            ${SQLITE_INSTALL_PREFIX}/
        DESTINATION
            .
        COMPONENT
            public
    )
endif()

if(OPENGEODE_WITH_PYTHON OR INCLUDE_PYBIND11)
    find_package(pybind11 REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${PYBIND11_INSTALL_PREFIX})
    install(
        DIRECTORY
            ${PYBIND11_INSTALL_PREFIX}/
        DESTINATION
            .
        COMPONENT
            public
    )
endif()

install(
    FILES 
        "cmake/OpenGeodeModule-setup.py.in"
        "cmake/GlobalOptions.cmake"
        "cmake/CompilerWarnings.cmake"
        "cmake/ProjectOptions.cmake"
        "cmake/CppTargets.cmake"
        "cmake/PythonTargets.cmake"
    DESTINATION 
        ${CMAKE_CONFIG_DESTINATION}
    COMPONENT
        public
)

# Configure the OpenGeode sources
add_subdirectory(src/geode)

# Optional modules configuration
if(OPENGEODE_WITH_TESTS)
    message(STATUS "Configuring OpenGeode with tests")
    enable_testing()
    add_subdirectory(tests)
endif()

if(OPENGEODE_WITH_PYTHON)
    message(STATUS "Configuring OpenGeode with Python bindings")
    add_subdirectory(bindings/python)
endif()

# Configure CPacks
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

# This must always be last!
include(CPack)
