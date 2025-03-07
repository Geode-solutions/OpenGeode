/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/detail/filter.hpp>
#include <geode/model/representation/builder/detail/register.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>

namespace geode
{
    class opengeode_model_api OpenGeodeBRepInput final : public BRepInput
    {
    public:
        explicit OpenGeodeBRepInput( std::string_view filename )
            : BRepInput( filename )
        {
        }

        [[nodiscard]] static std::string_view extension()
        {
            return BRep::native_extension_static();
        }

        [[nodiscard]] BRep read() final;
    };

    namespace detail
    {
        template < typename Model >
        void load_brep_files( Model& brep, std::string_view directory )
        {
            BRepBuilder builder{ brep };
            const auto level = Logger::level();
            Logger::set_level( Logger::LEVEL::warn );
            async::parallel_invoke(
                [&builder, &directory] {
                    builder.load_identifier( directory );
                },
                [&builder, &directory] {
                    builder.load_corners( directory );
                },
                [&builder, &directory] {
                    builder.load_lines( directory );
                },
                [&builder, &directory] {
                    builder.load_surfaces( directory );
                },
                [&builder, &directory] {
                    builder.load_blocks( directory );
                },
                [&builder, &directory] {
                    builder.load_model_boundaries( directory );
                },
                [&builder, &directory] {
                    builder.load_corner_collections( directory );
                },
                [&builder, &directory] {
                    builder.load_line_collections( directory );
                },
                [&builder, &directory] {
                    builder.load_surface_collections( directory );
                },
                [&builder, &directory] {
                    builder.load_block_collections( directory );
                },
                [&builder, &directory] {
                    builder.load_relationships( directory );
                },
                [&builder, &directory] {
                    builder.load_unique_vertices( directory );
                } );
            Logger::set_level( level );
            detail::register_all_components( brep );
            detail::filter_unsupported_components( brep );
        }
    } // namespace detail
} // namespace geode
