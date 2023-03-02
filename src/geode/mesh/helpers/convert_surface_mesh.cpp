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

#include <geode/mesh/helpers/convert_surface_mesh.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/polygonal_surface_builder.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/surface_edges.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/detail/surface_merger.h>
#include <geode/mesh/helpers/private/copy.h>

namespace
{
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

    template < typename Surface, typename Builder >
    void copy_polygons( const Surface& surface, Builder& builder )
    {
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            absl::FixedArray< geode::index_t > vertices(
                surface.nb_polygon_vertices( p ) );
            for( const auto v :
                geode::LRange{ surface.nb_polygon_vertices( p ) } )
            {
                vertices[v] = surface.polygon_vertex( { p, v } );
            }
            builder.create_polygon( vertices );
        }
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            for( const auto e : geode::LRange{ surface.nb_polygon_edges( p ) } )
            {
                const geode::PolygonEdge edge{ p, e };
                if( const auto adj = surface.polygon_adjacent( edge ) )
                {
                    builder.set_polygon_adjacent( edge, adj.value() );
                }
            }
        }
    }

    template < geode::index_t dimension >
    void convert_surface( const geode::SurfaceMesh< dimension >& input,
        geode::SurfaceMesh< dimension >& output )
    {
        auto builder = geode::SurfaceMeshBuilder< dimension >::create( output );
        geode::detail::copy_points( input, *builder );
        output.vertex_attribute_manager().copy(
            input.vertex_attribute_manager() );
        copy_polygons( input, *builder );
        output.polygon_attribute_manager().copy(
            input.polygon_attribute_manager() );
        geode::detail::copy_meta_info( input, *builder );
    }

    template < typename SurfaceIn, typename SurfaceOut >
    void copy_surface_attributes(
        const SurfaceIn& surface_in, const SurfaceOut& surface_out )
    {
        geode::detail::copy_attributes( surface_in.vertex_attribute_manager(),
            surface_out.vertex_attribute_manager() );
        geode::detail::copy_attributes( surface_in.polygon_attribute_manager(),
            surface_out.polygon_attribute_manager() );
        if( surface_in.are_edges_enabled() )
        {
            surface_out.enable_edges();
            geode::detail::copy_attributes(
                surface_in.edges().edge_attribute_manager(),
                surface_out.edges().edge_attribute_manager() );
        }
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > >
        convert_surface_mesh_into_polygonal_surface(
            const SurfaceMesh< dimension >& surface )
    {
        auto poly_surface = PolygonalSurface< dimension >::create();
        convert_surface( surface, *poly_surface );
        return poly_surface;
    }

    template < index_t dimension >
    absl::optional< std::unique_ptr< TriangulatedSurface< dimension > > >
        convert_surface_mesh_into_triangulated_surface(
            const SurfaceMesh< dimension >& surface )
    {
        if( !all_polygons_are_simplex( surface ) )
        {
            Logger::info( "[convert_surface_mesh_into_triangulated_surface] "
                          "Surface is not made of only triangles." );
            return absl::nullopt;
        }
        auto tri_surface = TriangulatedSurface< dimension >::create();
        convert_surface( surface, *tri_surface );
        return absl::optional<
            std::unique_ptr< TriangulatedSurface< dimension > > >{
            absl::in_place, tri_surface.release()
        };
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
            const auto nb_vertices = surface.nb_polygon_vertices( p );
            to_delete[p] = nb_vertices != 3;
            if( nb_vertices > 3 )
            {
                const auto vertices = surface.polygon_vertices( p );
                for( const auto v : LRange{ 2, nb_vertices } )
                {
                    builder.create_polygon(
                        { vertices[0], vertices[v - 1], vertices[v] } );
                }
            }
        }
        to_delete.resize( surface.nb_polygons(), false );
        builder.delete_polygons( to_delete );
        builder.compute_polygon_adjacencies();
    }

    std::unique_ptr< SurfaceMesh3D > convert_surface_mesh2d_into_3d(
        const SurfaceMesh2D& surface2d,
        index_t axis_to_add,
        double axis_coordinate )
    {
        auto surface3d = SurfaceMesh3D::create();
        auto builder3d = SurfaceMeshBuilder3D::create( *surface3d );
        detail::copy_meta_info( surface2d, *builder3d );
        detail::copy_points2d_into_3d(
            surface2d, *builder3d, axis_to_add, axis_coordinate );
        copy_polygons( surface2d, *builder3d );
        copy_surface_attributes( surface2d, *surface3d );
        return surface3d;
    }

    std::unique_ptr< SurfaceMesh2D > convert_surface_mesh3d_into_2d(
        const SurfaceMesh3D& surface3d, index_t axis_to_remove )
    {
        auto surface2d = SurfaceMesh2D::create();
        auto builder2d = SurfaceMeshBuilder2D::create( *surface2d );
        detail::copy_meta_info( surface3d, *builder2d );
        detail::copy_points3d_into_2d( surface3d, *builder2d, axis_to_remove );
        copy_polygons( surface3d, *builder2d );
        copy_surface_attributes( surface3d, *surface2d );
        return surface2d;
    }

    std::unique_ptr< PolygonalSurface3D > convert_polygonal_surface2d_into_3d(
        const PolygonalSurface2D& surface2d,
        index_t axis_to_add,
        double axis_coordinate )
    {
        auto surface3d = PolygonalSurface3D::create();
        auto builder3d = PolygonalSurfaceBuilder3D::create( *surface3d );
        detail::copy_meta_info( surface2d, *builder3d );
        detail::copy_points2d_into_3d(
            surface2d, *builder3d, axis_to_add, axis_coordinate );
        copy_polygons( surface2d, *builder3d );
        copy_surface_attributes( surface2d, *surface3d );
        return surface3d;
    }

    std::unique_ptr< PolygonalSurface2D > convert_polygonal_surface3d_into_2d(
        const PolygonalSurface3D& surface3d, index_t axis_to_remove )
    {
        auto surface2d = PolygonalSurface2D::create();
        auto builder2d = PolygonalSurfaceBuilder2D::create( *surface2d );
        detail::copy_meta_info( surface3d, *builder2d );
        detail::copy_points3d_into_2d( surface3d, *builder2d, axis_to_remove );
        copy_polygons( surface3d, *builder2d );
        copy_surface_attributes( surface3d, *surface2d );
        return surface2d;
    }

    std::unique_ptr< TriangulatedSurface3D >
        convert_triangulated_surface2d_into_3d(
            const TriangulatedSurface2D& surface2d,
            index_t axis_to_add,
            double axis_coordinate )
    {
        auto surface3d = TriangulatedSurface3D::create();
        auto builder3d = TriangulatedSurfaceBuilder3D::create( *surface3d );
        detail::copy_points2d_into_3d(
            surface2d, *builder3d, axis_to_add, axis_coordinate );
        detail::copy_meta_info( surface2d, *builder3d );
        copy_polygons( surface2d, *builder3d );
        copy_surface_attributes( surface2d, *surface3d );
        return surface3d;
    }

    std::unique_ptr< TriangulatedSurface2D >
        convert_triangulated_surface3d_into_2d(
            const TriangulatedSurface3D& surface3d, index_t axis_to_remove )
    {
        auto surface2d = TriangulatedSurface2D::create();
        auto builder2d = TriangulatedSurfaceBuilder2D::create( *surface2d );

        detail::copy_points3d_into_2d( surface3d, *builder2d, axis_to_remove );
        detail::copy_meta_info( surface3d, *builder2d );
        copy_polygons( surface3d, *builder2d );
        copy_surface_attributes( surface3d, *surface2d );
        return surface2d;
    }

    template < index_t dimension >
    std::unique_ptr< SurfaceMesh< dimension > > merge_surface_meshes(
        absl::Span<
            const std::reference_wrapper< const SurfaceMesh< dimension > > >
            surfaces )
    {
        detail::SurfaceMeshMerger< dimension > merger{ surfaces,
            global_epsilon };
        return merger.merge();
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

    template std::unique_ptr< SurfaceMesh< 2 > >
        opengeode_mesh_api merge_surface_meshes( absl::Span<
            const std::reference_wrapper< const SurfaceMesh< 2 > > > );
    template std::unique_ptr< SurfaceMesh< 3 > >
        opengeode_mesh_api merge_surface_meshes( absl::Span<
            const std::reference_wrapper< const SurfaceMesh< 3 > > > );
} // namespace geode
