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

if(NOT CPACK_PACKAGE_VERSION)
    set(CPACK_PACKAGE_VERSION "master")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING 
        "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if("${CMAKE_CXX_STANDARD}" STREQUAL "")
    set(CMAKE_CXX_STANDARD 11)
endif()

# Begin configuration of the superbuild project
project(SuperBuild NONE)

# Additional cmake modules
include(ExternalProject)

include(${PROJECT_SOURCE_DIR}/cmake/ConfigureAbseil.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ConfigureAsync++.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ConfigureBitsery.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ConfigureFilesystem.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ConfigureMinizip.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ConfigureNanoflann.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ConfigureSpdlog.cmake)

if(OPENGEODE_WITH_PYTHON OR INCLUDE_PYBIND11)
    include(${PROJECT_SOURCE_DIR}/cmake/ConfigurePybind11.cmake)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/ConfigureOpenGeode.cmake)

install(DIRECTORY ${OpenGeode_PATH_INSTALL}/ DESTINATION .)

#------------------------------------------------------------------------------------------------
# Configure CPack
set(CPACK_PACKAGE_NAME OpenGeode)
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

# This must always be last!
include(CPack)
