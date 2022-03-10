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

add_geode_library(
    NAME basic
    FOLDER "geode/basic"
    SOURCES
        "assert.cpp"
        "attribute_manager.cpp"
        "bitsery_input.cpp"
        "bitsery_output.cpp"
        "common.cpp"
        "filename.cpp"
        "identifier.cpp"
        "identifier_builder.cpp"
        "logger.cpp"
        "permutation.cpp"
        "progress_logger.cpp"
        "singleton.cpp"
        "timer.cpp"
        "uuid.cpp"
        "zip_file.cpp"
    PUBLIC_HEADERS
        "algorithm.h"
        "assert.h"
        "attribute_manager.h"
        "attribute_utils.h"
        "attribute.h"
        "bitsery_archive.h"
        "common.h"
        "factory.h"
        "filename.h"
        "identifier.h"
        "identifier_builder.h"
        "logger.h"
        "mapping.h"
        "named_type.h"
        "passkey.h"
        "permutation.h"
        "pimpl.h"
        "pimpl_impl.h"
        "progress_logger.h"
        "range.h"
        "singleton.h"
        "timer.h"
        "types.h"
        "uuid.h"
        "zip_file.h"
    ADVANCED_HEADERS
        "detail/bitsery_archive.h"
        "detail/mapping_after_deletion.h"
    PUBLIC_DEPENDENCIES
        absl::flat_hash_map
        absl::strings
        ghcFilesystem::ghc_filesystem
        Bitsery::bitsery
    PRIVATE_DEPENDENCIES
        Async++
        absl::time
        spdlog::spdlog_header_only
        MINIZIP::minizip-ng
)
if(WIN32)
    set_target_properties(basic
        PROPERTIES
            LINK_FLAGS_DEBUG "/NODEFAULTLIB:MSVCRTD"
    )
endif()
