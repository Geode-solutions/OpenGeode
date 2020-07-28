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

#include <geode/mesh/helpers/convert_surface_mesh.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/triangulated_surface.h>

namespace
{
    template < typename MeshFrom, typename Builder >
    void copy_points( const MeshFrom& from, Builder& builder )
    {
        builder.create_vertices( from.nb_vertices() );
        for( const auto v : geode::Range{ from.nb_vertices() } )
        {
            builder.set_point( v, from.point( v ) );
        }
    }

    template < geode::index_t dimension >
    bool all_polygons_are_simplex(
        const geode::SurfaceMesh< dimension >& surface )
    {
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            if( surface.nb_polygon_vertices( p ) > 3 )
            {
                return false;
            }
        }
        return true;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    absl::optional< std::unique_ptr< TriangulatedSurface< dimension > > >
        convert_surface_mesh_into_triangulated_surface(
            const SurfaceMesh< dimension >& surface )
    {
        if( !all_polygons_are_simplex( surface ) )
        {
            return absl::nullopt;
        }
        auto tri_surface = TriangulatedSurface< dimension >::create();
        auto builder =
            TriangulatedSurfaceBuilder< dimension >::create( *tri_surface );
        copy_points( surface, *builder );
        builder->reserve_triangles( surface.nb_polygons() );
        for( const auto p : Range{ surface.nb_polygons() } )
        {
            const std::array< index_t, 3 > vertices{ surface.polygon_vertex(
                                                         { p, 0 } ),
                surface.polygon_vertex( { p, 1 } ),
                surface.polygon_vertex( { p, 2 } ) };
            builder->create_triangle( vertices );
        }
        for( const auto p : Range{ surface.nb_polygons() } )
        {
            for( const auto e : Range{ 3 } )
            {
                if( const auto adjacent = surface.polygon_adjacent( { p, e } ) )
                {
                    builder->set_polygon_adjacent( { p, e }, adjacent.value() );
                }
            }
        }
        return std::move( tri_surface );
    }

    template < index_t dimension >
    void triangulate_surface_mesh( SurfaceMesh< dimension >& surface )
    {
        triangulate_surface_mesh(
            surface, *SurfaceMeshBuilder< dimension >::create( surface ) );
    }

    template < index_t dimension >
    void triangulate_surface_mesh( const SurfaceMesh< dimension >& surface,
        SurfaceMeshBuilder< dimension >& builder )
    {
        std::vector< bool > to_delete( surface.nb_polygons(), false );
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            if( surface.nb_polygon_vertices( p ) > 3 )
            {
                to_delete[p] = true;
                const auto v0 = surface.polygon_vertex( { p, 0 } );
                for( const auto v :
                    Range{ 2, surface.nb_polygon_vertices( p ) } )
                {
                    builder.create_polygon(
                        { v0, surface.polygon_vertex( { p, v - 1 } ),
                            surface.polygon_vertex( { p, v } ) } );
                }
            }
        }
        to_delete.resize( surface.nb_polygons(), false );
        builder.delete_polygons( to_delete );
        builder.compute_polygon_adjacencies();
    }

    template absl::optional< std::unique_ptr< TriangulatedSurface2D > >
        opengeode_mesh_api convert_surface_mesh_into_triangulated_surface(
            const SurfaceMesh2D& );
    template absl::optional< std::unique_ptr< TriangulatedSurface3D > >
        opengeode_mesh_api convert_surface_mesh_into_triangulated_surface(
            const SurfaceMesh3D& );

    template void opengeode_mesh_api triangulate_surface_mesh( SurfaceMesh2D& );
    template void opengeode_mesh_api triangulate_surface_mesh( SurfaceMesh3D& );

    template void opengeode_mesh_api triangulate_surface_mesh(
        const SurfaceMesh2D&, SurfaceMeshBuilder2D& );
    template void opengeode_mesh_api triangulate_surface_mesh(
        const SurfaceMesh3D&, SurfaceMeshBuilder3D& );

} // namespace geode
