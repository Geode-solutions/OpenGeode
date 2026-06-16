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
        template < typename Model >
        void load_brep_files( Model& brep, std::string_view directory )
        {
            DEBUG( "load_brep_files" );
            DEBUG( "BRepBuilder" );
            BRepBuilder builder{ brep };
            DEBUG( "BRepBuilder done" );
            const auto level = Logger::level();
            Logger::set_level( Logger::LEVEL::warn );
            DEBUG( "load_identifier" );
            builder.load_identifier( directory );
            DEBUG( "load_corners" );
            builder.load_corners( directory );
            DEBUG( "load_lines" );
            builder.load_lines( directory );
            DEBUG( "load_surfaces" );
            builder.load_surfaces( directory );
            DEBUG( "load_blocks" );
            builder.load_blocks( directory );
            DEBUG( "load_model_boundaries" );
            builder.load_model_boundaries( directory );
            DEBUG( "load_corner_collections" );
            builder.load_corner_collections( directory );
            DEBUG( "load_line_collections" );
            builder.load_line_collections( directory );
            DEBUG( "load_surface_collections" );
            builder.load_surface_collections( directory );
            DEBUG( "load_block_collections" );
            builder.load_block_collections( directory );
            DEBUG( "load_relationships" );
            builder.load_relationships( directory );
            DEBUG( "load_unique_vertices" );
            builder.load_unique_vertices( directory );
            Logger::set_level( level );
            DEBUG( "register_all_components" );
            detail::register_all_components( brep );
            DEBUG( "filter_unsupported_components" );
            detail::filter_unsupported_components( brep );
            DEBUG( "load_brep_files done" );
        }
    } // namespace detail
} // namespace geode
