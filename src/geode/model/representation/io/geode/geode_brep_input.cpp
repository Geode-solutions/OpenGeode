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

#include <geode/model/representation/io/geode/geode_brep_input.hpp>

#include <async++.h>

#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/detail/filter.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace geode
{
    void OpenGeodeBRepInput::load_brep_files(
        BRep& brep, std::string_view directory )
    {
        BRepBuilder builder{ brep };
        async::parallel_invoke(
            [&builder, &directory] {
                builder.load_identifier( directory );
            },
            [&builder, &directory] {
                builder.load_corners( directory );
                builder.load_lines( directory );
                builder.load_surfaces( directory );
                builder.load_blocks( directory );
            },
            [&builder, &directory] {
                builder.load_model_boundaries( directory );
                builder.load_corner_collections( directory );
                builder.load_line_collections( directory );
                builder.load_surface_collections( directory );
                builder.load_block_collections( directory );
            },
            [&builder, &directory] {
                builder.load_relationships( directory );
            },
            [&builder, &directory] {
                builder.load_unique_vertices( directory );
            } );
        for( const auto& corner : brep.corners() )
        {
            builder.register_mesh_component( corner );
        }
        for( const auto& line : brep.lines() )
        {
            builder.register_mesh_component( line );
        }
        for( const auto& surface : brep.surfaces() )
        {
            builder.register_mesh_component( surface );
        }
        for( const auto& block : brep.blocks() )
        {
            builder.register_mesh_component( block );
        }
    }

    BRep OpenGeodeBRepInput::read()
    {
        const UnzipFile zip_reader{ filename(), uuid{}.string() };
        zip_reader.extract_all();
        BRep brep;
        load_brep_files( brep, zip_reader.directory() );
        detail::filter_unsupported_components( brep );
        return brep;
    }
} // namespace geode
