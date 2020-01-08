/*
 * Copyright (c) 2019 Geode-solutions
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

#include <ghc/filesystem.hpp>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/section_output.h>

namespace geode
{
    class opengeode_model_api OpenGeodeSectionOutput final
        : public SectionOutput
    {
    public:
        OpenGeodeSectionOutput(
            const Section& section, absl::string_view filename )
            : SectionOutput( section, filename )
        {
        }

        static absl::string_view extension()
        {
            return Section::native_extension_static();
        }

        void save_section_files( absl::string_view directory ) const
        {
            section().save_relationships( directory );
            section().save_unique_vertices( directory );
            section().save_corners( directory );
            section().save_lines( directory );
            section().save_surfaces( directory );
        }

        void archive_section_files( const ZipFile& zip_writer ) const
        {
            for( const auto& file : ghc::filesystem::directory_iterator(
                     zip_writer.directory().data() ) )
            {
                zip_writer.archive_file( file.path().native() );
            }
        }

        void write() const final
        {
            const ZipFile zip_writer{ filename(), uuid{}.string() };
            save_section_files( zip_writer.directory().data() );
            archive_section_files( zip_writer );
        }
    };
} // namespace geode
