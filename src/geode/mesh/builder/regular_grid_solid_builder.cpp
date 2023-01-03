/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/builder/regular_grid_solid_builder.h>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/regular_grid_solid.h>

namespace geode
{
    RegularGridBuilder< 3 >::RegularGridBuilder( RegularGrid3D& mesh )
        : SolidMeshBuilder< 3 >( mesh ), GridBuilder< 3 >( mesh ), grid_( mesh )
    {
    }

    std::unique_ptr< RegularGridBuilder< 3 > > RegularGridBuilder< 3 >::create(
        RegularGrid3D& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            RegularGridBuilder< 3 > >( mesh );
    }

    void RegularGridBuilder< 3 >::initialize_grid( const Point3D& origin,
        std::array< index_t, 3 > cells_number,
        std::array< double, 3 > cells_length )
    {
        set_grid_dimensions(
            std::move( cells_number ), std::move( cells_length ) );
        const auto nb_u = grid_.nb_cells_in_direction( 0 );
        const auto nb_v = grid_.nb_cells_in_direction( 1 );
        const auto nb_w = grid_.nb_cells_in_direction( 2 );
        grid_.vertex_attribute_manager().resize(
            ( nb_u + 1 ) * ( nb_v + 1 ) * ( nb_w + 1 ) );
        grid_.polyhedron_attribute_manager().resize( grid_.nb_cells() );
        for( const auto w : Range{ nb_w } )
        {
            for( const auto v : Range{ nb_v } )
            {
                for( const auto u : Range{ nb_u } )
                {
                    const auto cell = u + v * nb_u + w * nb_u * nb_v;
                    const auto vertex =
                        u + v * ( nb_u + 1 ) + w * ( nb_u + 1 ) * ( nb_v + 1 );
                    associate_polyhedron_vertex_to_vertex(
                        { cell, 0 }, vertex );
                }
            }
        }
        // Last Face U
        for( const auto w : Range{ nb_w } )
        {
            for( const auto v : Range{ nb_v } )
            {
                const auto cell = ( nb_u - 1 ) + v * nb_u + w * nb_u * nb_v;
                const auto vertex =
                    nb_u + v * ( nb_u + 1 ) + w * ( nb_u + 1 ) * ( nb_v + 1 );
                associate_polyhedron_vertex_to_vertex( { cell, 1 }, vertex );
            }
        }
        // Last Face V
        for( const auto w : Range{ nb_w } )
        {
            for( const auto u : Range{ nb_u } )
            {
                const auto cell = u + ( nb_v - 1 ) * nb_u + w * nb_u * nb_v;
                const auto vertex =
                    u + nb_v * ( nb_u + 1 ) + w * ( nb_u + 1 ) * ( nb_v + 1 );
                associate_polyhedron_vertex_to_vertex( { cell, 2 }, vertex );
            }
        }
        // Last Face W
        for( const auto v : Range{ nb_v } )
        {
            for( const auto u : Range{ nb_u } )
            {
                const auto cell = u + v * nb_u + ( nb_w - 1 ) * nb_u * nb_v;
                const auto vertex =
                    u + v * ( nb_u + 1 ) + nb_w * ( nb_u + 1 ) * ( nb_v + 1 );
                associate_polyhedron_vertex_to_vertex( { cell, 4 }, vertex );
            }
        }
        // Last Line U-V
        for( const auto w : Range{ nb_w } )
        {
            const auto cell =
                ( nb_u - 1 ) + ( nb_v - 1 ) * nb_u + w * nb_u * nb_v;
            const auto vertex =
                nb_u + nb_v * ( nb_u + 1 ) + w * ( nb_u + 1 ) * ( nb_v + 1 );
            associate_polyhedron_vertex_to_vertex( { cell, 3 }, vertex );
        }
        // Last Line U-W
        for( const auto v : Range{ nb_v } )
        {
            const auto cell =
                ( nb_u - 1 ) + v * nb_u + ( nb_w - 1 ) * nb_u * nb_v;
            const auto vertex =
                nb_u + v * ( nb_u + 1 ) + nb_w * ( nb_u + 1 ) * ( nb_v + 1 );
            associate_polyhedron_vertex_to_vertex( { cell, 5 }, vertex );
        }
        // Last Line V-W
        for( const auto u : Range{ nb_u } )
        {
            const auto cell =
                u + ( nb_v - 1 ) * nb_u + ( nb_w - 1 ) * nb_u * nb_v;
            const auto vertex =
                u + nb_v * ( nb_u + 1 ) + nb_w * ( nb_u + 1 ) * ( nb_v + 1 );
            associate_polyhedron_vertex_to_vertex( { cell, 6 }, vertex );
        }
        // Last Corner U-V-W
        const auto cell =
            ( nb_u - 1 ) + ( nb_v - 1 ) * nb_u + ( nb_w - 1 ) * nb_u * nb_v;
        const auto vertex =
            nb_u + nb_v * ( nb_u + 1 ) + nb_w * ( nb_u + 1 ) * ( nb_v + 1 );
        associate_polyhedron_vertex_to_vertex( { cell, 7 }, vertex );
        update_origin( origin );
    }

    void RegularGridBuilder< 3 >::initialize_grid( const Point3D& origin,
        std::array< index_t, 3 > cells_number,
        double cells_length )
    {
        initialize_grid( origin, std::move( cells_number ),
            { cells_length, cells_length, cells_length } );
    }

    void RegularGridBuilder< 3 >::copy( const RegularGrid3D& grid )
    {
        OPENGEODE_EXCEPTION(
            grid_.nb_vertices() == 0 && grid_.nb_polyhedra() == 0,
            "[RegularGridBuilder::copy] Cannot copy a mesh into an "
            "already initialized mesh." );
        SolidMeshBuilder3D::copy( grid );
        GridBuilder3D::copy( grid );
    }
} // namespace geode
