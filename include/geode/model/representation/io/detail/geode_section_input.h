/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/section_input.h>

namespace geode
{
    class opengeode_model_api OpenGeodeSectionInput final : public SectionInput
    {
    public:
        OpenGeodeSectionInput( Section& section, absl::string_view filename )
            : SectionInput( section, filename )
        {
        }

        static absl::string_view extension()
        {
            return Section::native_extension_static();
        }

        void read() final
        {
            SectionBuilder builder( section() );
            const UnzipFile zip_reader{ filename(), uuid{}.string() };
            zip_reader.extract_all();

            builder.load_corners( zip_reader.directory() );
            builder.load_lines( zip_reader.directory() );
            builder.load_surfaces( zip_reader.directory() );
            builder.load_model_boundaries( zip_reader.directory() );
            builder.load_relationships( zip_reader.directory() );
            builder.load_unique_vertices( zip_reader.directory() );

            for( const auto& corner : section().corners() )
            {
                builder.register_mesh_component( corner );
            }
            for( const auto& line : section().lines() )
            {
                builder.register_mesh_component( line );
            }
            for( const auto& surface : section().surfaces() )
            {
                builder.register_mesh_component( surface );
            }
        }
    };
} // namespace geode
