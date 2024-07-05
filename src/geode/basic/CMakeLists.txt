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

add_geode_library(
    NAME basic
    FOLDER "geode/basic"
    SOURCES
        "assert.cpp"
        "attribute_manager.cpp"
        "bitsery_archive.cpp"
        "bitsery_input.cpp"
        "bitsery_output.cpp"
        "cell_array.cpp"
        "common.cpp"
        "console_logger_client.cpp"
        "console_progress_logger_client.cpp"
        "file.cpp"
        "filename.cpp"
        "identifier.cpp"
        "identifier_builder.cpp"
        "library.cpp"
        "logger.cpp"
        "logger_manager.cpp"
        "permutation.cpp"
        "progress_logger.cpp"
        "progress_logger_manager.cpp"
        "singleton.cpp"
        "string.cpp"
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
        "cell_array.h"
        "common.h"
        "console_logger_client.h"
        "console_progress_logger_client.h"
        "factory.h"
        "file.h"
        "filename.h"
        "identifier.h"
        "input.h"
        "io.h"
        "identifier_builder.h"
        "library.h"
        "logger.h"
        "logger_client.h"
        "logger_manager.h"
        "mapping.h"
        "named_type.h"
        "output.h"
        "passkey.h"
        "permutation.h"
        "pimpl.h"
        "pimpl_impl.h"
        "progress_logger.h"
        "progress_logger_client.h"
        "progress_logger_manager.h"
        "range.h"
        "singleton.h"
        "string.h"
        "timer.h"
        "types.h"
        "uuid.h"
        "zip_file.h"
    ADVANCED_HEADERS
        "detail/bitsery_archive.h"
        "detail/disable_debug_logger.h"
        "detail/enable_debug_logger.h"
        "detail/geode_input_impl.h"
        "detail/geode_output_impl.h"
        "detail/mapping_after_deletion.h"
    PRIVATE_HEADERS
        "private/array_impl.h"
    PUBLIC_DEPENDENCIES
        absl::flat_hash_map
        absl::strings
        ghcFilesystem::ghc_filesystem
        Bitsery::bitsery
    PRIVATE_DEPENDENCIES
        Async++
        absl::random_random
        absl::time
        spdlog::spdlog_header_only
        MINIZIP::minizip-ng
)
if(WIN32)
    target_link_libraries(basic PUBLIC absl::abseil_dll)
endif()