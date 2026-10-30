# Copyright (c) 2019 - 2026 Geode-solutions
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

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} ${SQLITE_INSTALL_PREFIX} ${PROJ_INSTALL_PREFIX}")
find_package(GDAL REQUIRED CONFIG NO_DEFAULT_PATH PATHS ${GDAL_INSTALL_PREFIX})

add_subdirectory(src)

if(OPENGEODE_WITH_TESTS)
    add_subdirectory(tests)
endif()

add_geode_python_wheel(
    NAME "OpenGeode-core"
    DESCRIPTION 
        "Open source framework for representing and manipulating geometric models"
    MODULES
        "basic.py"
        "geometry.py"
        "image.py"
        "mesh.py"
        "model.py"
        "gdal.py"
    SUPERBUILD
    LICENSE "MIT"
)

# Imported by the Python tests of OpenGeode and of its dependents (see add_geode_python_test),
# next to the Python modules, in the build tree and in the installation
if(WIN32)
    set(test_env_folder "${CMAKE_INSTALL_BINDIR}")
else()
    set(test_env_folder "${CMAKE_INSTALL_LIBDIR}")
endif()
foreach(prefix "${PROJECT_BINARY_DIR}" "${CMAKE_INSTALL_PREFIX}")
    configure_file(geode_test_env.py "${prefix}/${test_env_folder}/geode_test_env.py" COPYONLY)
endforeach()
