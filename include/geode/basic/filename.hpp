/*
 * Copyright (c) 2019 - 2024 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once

#include <filesystem>

#include <geode/basic/common.hpp>

namespace geode
{
    [[nodiscard]] std::filesystem::path opengeode_basic_api
        filename_with_extension( const std::filesystem::path& path );

    [[nodiscard]] std::filesystem::path opengeode_basic_api
        filename_without_extension( const std::filesystem::path& path );

    [[nodiscard]] std::filesystem::path opengeode_basic_api
        filepath_without_extension( const std::filesystem::path& path );

    [[nodiscard]] std::filesystem::path opengeode_basic_api
        filepath_without_filename( const std::filesystem::path& path );

    [[nodiscard]] std::string_view opengeode_basic_api extension_from_filename(
        std::string_view filename );

    [[nodiscard]] std::string opengeode_basic_api expand_predefined_folders(
        std::string_view path );
} // namespace geode