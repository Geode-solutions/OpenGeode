/*
 * Copyright (c) 2019 - 2025 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
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

#include <string>
#include <vector>

#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/section_output.hpp>

namespace geode
{
    class ZipFile;
}

namespace geode
{
    class opengeode_model_api OpenGeodeSectionOutput final
        : public SectionOutput
    {
    public:
        explicit OpenGeodeSectionOutput( std::string_view filename )
            : SectionOutput( filename )
        {
        }

        [[nodiscard]] static std::string_view extension()
        {
            return Section::native_extension_static();
        }

        void save_section_files(
            const Section& section, std::string_view directory ) const;

        void archive_section_files( const ZipFile& zip_writer ) const;

        std::vector< std::string > write( const Section& section ) const final;
    };
} // namespace geode
