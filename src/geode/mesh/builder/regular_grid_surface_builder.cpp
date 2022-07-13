/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/builder/regular_grid_surface_builder.h>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/regular_grid_surface.h>

namespace geode
{
    RegularGridBuilder< 2 >::RegularGridBuilder( RegularGrid2D& mesh )
        : SurfaceMeshBuilder< 2 >( mesh ),
          GridBuilder< 2 >( mesh ),
          grid_( mesh )
    {
    }

    std::unique_ptr< RegularGridBuilder< 2 > > RegularGridBuilder< 2 >::create(
        RegularGrid2D& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            RegularGridBuilder< 2 > >( mesh );
    }

    void RegularGridBuilder< 2 >::initialize_grid( const Point2D& origin,
        std::array< index_t, 2 > cells_number,
        std::array< double, 2 > cells_length )
    {
        set_grid_dimensions(
            std::move( cells_number ), std::move( cells_length ) );
        grid_.vertex_attribute_manager().resize(
            grid_.nb_vertices_in_direction( 0 )
            * grid_.nb_vertices_in_direction( 1 ) );
        grid_.polygon_attribute_manager().resize( grid_.nb_cells() );
        for( const auto p : Range{ grid_.nb_polygons() } )
        {
            for( const auto v : LRange{ grid_.nb_polygon_vertices( p ) } )
            {
                associate_polygon_vertex_to_vertex(
                    { p, v }, grid_.polygon_vertex( { p, v } ) );
            }
        }
        update_origin( origin );
    }

    void RegularGridBuilder< 2 >::initialize_grid( const Point2D& origin,
        std::array< index_t, 2 > cells_number,
        double cells_length )
    {
        initialize_grid(
            origin, std::move( cells_number ), { cells_length, cells_length } );
    }

    void RegularGridBuilder< 2 >::copy( const RegularGrid2D& grid )
    {
        OPENGEODE_EXCEPTION(
            grid_.nb_vertices() == 0 && grid_.nb_polygons() == 0,
            "[RegularGridBuilder::copy] Cannot copy a mesh into an "
            "already initialized mesh." );
        SurfaceMeshBuilder2D::copy( grid );
        GridBuilder2D::copy( grid );
    }
} // namespace geode
