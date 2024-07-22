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
        "algorithm.hpp"
        "assert.hpp"
        "attribute_manager.hpp"
        "attribute_utils.hpp"
        "attribute.hpp"
        "bitsery_archive.hpp"
        "cell_array.hpp"
        "common.hpp"
        "console_logger_client.hpp"
        "console_progress_logger_client.hpp"
        "factory.hpp"
        "file.hpp"
        "filename.hpp"
        "identifier.hpp"
        "input.hpp"
        "io.hpp"
        "identifier_builder.hpp"
        "library.hpp"
        "logger.hpp"
        "logger_client.hpp"
        "logger_manager.hpp"
        "mapping.hpp"
        "named_type.hpp"
        "output.hpp"
        "passkey.hpp"
        "permutation.hpp"
        "pimpl.hpp"
        "pimpl_impl.hpp"
        "progress_logger.hpp"
        "progress_logger_client.hpp"
        "progress_logger_manager.hpp"
        "range.hpp"
        "singleton.hpp"
        "string.hpp"
        "timer.hpp"
        "types.hpp"
        "uuid.hpp"
        "zip_file.hpp"
    ADVANCED_HEADERS
        "detail/bitsery_archive.hpp"
        "detail/disable_debug_logger.hpp"
        "detail/enable_debug_logger.hpp"
        "detail/geode_input_impl.hpp"
        "detail/geode_output_impl.hpp"
        "detail/mapping_after_deletion.hpp"
    INTERNAL_HEADERS
        "internal/array_impl.hpp"
    PUBLIC_DEPENDENCIES
        absl::flat_hash_map
        absl::strings
        absl::stacktrace
        absl::symbolize
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