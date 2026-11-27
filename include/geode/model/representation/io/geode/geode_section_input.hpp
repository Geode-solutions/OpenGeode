/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <async++.h>

#include <geode/basic/detail/input_files.hpp>

#include <geode/model/representation/builder/detail/filter.hpp>
#include <geode/model/representation/builder/detail/register.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/section_input.hpp>

namespace geode
{
    class opengeode_model_api OpenGeodeSectionInput final : public SectionInput
    {
    public:
        explicit OpenGeodeSectionInput( std::string_view filename )
            : SectionInput( filename )
        {
        }

        [[nodiscard]] static std::string_view extension()
        {
            return Section::native_extension_static();
        }

        [[nodiscard]] std::vector< Section > read() final;

        [[nodiscard]] AdditionalFiles additional_files() const final
        {
            return {};
        }

        [[nodiscard]] index_t object_priority() const final
        {
            return 0;
        }

        [[nodiscard]] Percentage is_loadable() const final
        {
            return Percentage{ 1 };
        }
    };

    namespace detail
    {
        /*!
         * Load the files of a Section (identifier, components, meshes,
         * relationships, unique vertices) into an empty model.
         */
        template < typename Model >
        void load_section_files(
            Model& section, const detail::InputFiles& files )
        {
            SectionBuilder builder{ section };
            const detail::ScopedLoggerLevel scoped_level{
                Logger::LEVEL::warning
            };
            async::parallel_invoke(
                [&builder, &files] {
                    builder.load_identifier( files );
                },
                [&builder, &files] {
                    builder.load_corners( files );
                },
                [&builder, &files] {
                    builder.load_lines( files );
                },
                [&builder, &files] {
                    builder.load_surfaces( files );
                },
                [&builder, &files] {
                    builder.load_model_boundaries( files );
                },
                [&builder, &files] {
                    builder.load_corner_collections( files );
                },
                [&builder, &files] {
                    builder.load_line_collections( files );
                },
                [&builder, &files] {
                    builder.load_surface_collections( files );
                },
                [&builder, &files] {
                    builder.load_relationships( files );
                },
                [&builder, &files] {
                    builder.load_unique_vertices( files );
                } );
            detail::register_all_components( section );
            detail::filter_unsupported_components( section );
            builder.repair_component_vertices();
        }
    } // namespace detail
} // namespace geode
