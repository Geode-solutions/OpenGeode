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

set(folder "geode/geometry")
set(lib_source_dir ${PROJECT_SOURCE_DIR}/src/${folder})
set(lib_include_dir ${PROJECT_SOURCE_DIR}/include/${folder})

add_geode_library(
    NAME geometry
    FOLDER "${folder}"
    SOURCES
        "${lib_source_dir}/bitsery_archive.cpp"
        "${lib_source_dir}/bounding_box.cpp"
        "${lib_source_dir}/common.cpp"
        "${lib_source_dir}/nn_search.cpp"
    PUBLIC_HEADERS
        "${lib_include_dir}/bitsery_archive.h"
        "${lib_include_dir}/bounding_box.h"
        "${lib_include_dir}/common.h"
        "${lib_include_dir}/nn_search.h"
        "${lib_include_dir}/point.h"
        "${lib_include_dir}/vector.h"
    PUBLIC_DEPENDENCIES
        Bitsery::bitsery
        Threads::Threads
        ${PROJECT_NAME}::basic
    PRIVATE_DEPENDENCIES
        Async++
        nanoflann::nanoflann
)
