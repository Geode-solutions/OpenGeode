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

set(folder "geode/basic")
set(lib_source_dir ${PROJECT_SOURCE_DIR}/src/${folder})
set(lib_include_dir ${PROJECT_SOURCE_DIR}/include/${folder})

add_geode_library(
    NAME basic
    FOLDER "${folder}"
    SOURCES
        "${lib_source_dir}/assert.cpp"
        "${lib_source_dir}/attribute_manager.cpp"
        "${lib_source_dir}/bitsery_archive.cpp"
        "${lib_source_dir}/common.cpp"
        "${lib_source_dir}/logger.cpp"
        "${lib_source_dir}/singleton.cpp"
        "${lib_source_dir}/uuid.cpp"
        "${lib_source_dir}/zip_file.cpp"
    PUBLIC_HEADERS
        "${lib_include_dir}/algorithm.h"
        "${lib_include_dir}/assert.h"
        "${lib_include_dir}/attribute_manager.h"
        "${lib_include_dir}/attribute.h"
        "${lib_include_dir}/bitsery_archive.h"
        "${lib_include_dir}/common.h"
        "${lib_include_dir}/factory.h"
        "${lib_include_dir}/logger.h"
        "${lib_include_dir}/named_type.h"
        "${lib_include_dir}/pimpl.h"
        "${lib_include_dir}/pimpl_impl.h"
        "${lib_include_dir}/range.h"
        "${lib_include_dir}/singleton.h"
        "${lib_include_dir}/types.h"
        "${lib_include_dir}/uuid.h"
        "${lib_include_dir}/zip_file.h"
    ADVANCED_HEADERS
        "${lib_include_dir}/detail/mapping_after_deletion.h"
    PUBLIC_DEPENDENCIES
        Bitsery::bitsery
    PRIVATE_DEPENDENCIES
        spdlog::spdlog_header_only
        ghcFilesystem::ghc_filesystem
        MINIZIP::minizip
)
if(WIN32)
    set_target_properties(basic
        PROPERTIES
            LINK_FLAGS_DEBUG "/NODEFAULTLIB:MSVCRTD"
    )
endif()
