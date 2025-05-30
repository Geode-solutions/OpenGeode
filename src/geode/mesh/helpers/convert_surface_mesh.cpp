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

#include <geode/mesh/helpers/convert_surface_mesh.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/polygonal_surface_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/detail/surface_merger.hpp>
#include <geode/mesh/helpers/internal/copy.hpp>

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
        geode::internal::copy_points( input, *builder );
        output.vertex_attribute_manager().copy(
            input.vertex_attribute_manager() );
        copy_polygons( input, *builder );
        output.polygon_attribute_manager().copy(
            input.polygon_attribute_manager() );
        geode::internal::copy_meta_info( input, *builder );
    }

    template < typename SurfaceIn, typename SurfaceOut >
    void copy_surface_attributes(
        const SurfaceIn& surface_in, const SurfaceOut& surface_out )
    {
        geode::internal::copy_attributes( surface_in.vertex_attribute_manager(),
            surface_out.vertex_attribute_manager() );
        geode::internal::copy_attributes(
            surface_in.polygon_attribute_manager(),
            surface_out.polygon_attribute_manager() );
        if( surface_in.are_edges_enabled() )
        {
            surface_out.enable_edges();
            geode::internal::copy_attributes(
                surface_in.edges().edge_attribute_manager(),
                surface_out.edges().edge_attribute_manager() );
        }
    }

    template < geode::index_t dimension >
    std::unique_ptr< geode::TriangulatedSurface< dimension > >
        create_triangulated_surface_from_grid(
            const geode::Grid< dimension >& /*unused*/,
            absl::Span< const geode::index_t > /*unused*/ )
    {
        throw geode::OpenGeodeException{
            "[create_triangulated_surface_from_grid] Cannot convert "
            "SurfaceMesh from RegularGrid3D"
        };
    }

    std::array< geode::index_t, 4 > create_triangles_from_cross_pattern(
        geode::TriangulatedSurfaceBuilder2D& builder,
        const geode::Grid2D& grid,
        const geode::Grid2D::CellIndices& cell_indices,
        geode::index_t additional_vertex_id )
    {
        const auto cell_vertices = grid.cell_vertices( cell_indices );
        std::array< geode::index_t, 4 > cell_mesh_vertices;
        for( const auto vertex_id : geode::LIndices{ cell_mesh_vertices } )
        {
            const auto cell_vertex_id =
                grid.vertex_index( cell_vertices[vertex_id] );
            cell_mesh_vertices[vertex_id] = cell_vertex_id;
        }
        std::array< geode::index_t, 4 > created_triangles;
        created_triangles[0] = builder.create_triangle( { cell_mesh_vertices[0],
            cell_mesh_vertices[1], additional_vertex_id } );
        created_triangles[1] = builder.create_triangle( { cell_mesh_vertices[1],
            cell_mesh_vertices[3], additional_vertex_id } );
        created_triangles[2] = builder.create_triangle( { cell_mesh_vertices[3],
            cell_mesh_vertices[2], additional_vertex_id } );
        created_triangles[3] = builder.create_triangle( { cell_mesh_vertices[2],
            cell_mesh_vertices[0], additional_vertex_id } );
        return created_triangles;
    }

    std::array< geode::index_t, 2 > create_triangles_from_diagonal_pattern(
        geode::TriangulatedSurfaceBuilder2D& builder,
        const geode::Grid2D& grid,
        const geode::Grid2D::CellIndices& cell_indices )
    {
        const auto cell_vertices = grid.cell_vertices( cell_indices );
        std::array< geode::index_t, 4 > cell_mesh_vertices;
        for( const auto vertex_id : geode::LIndices{ cell_mesh_vertices } )
        {
            const auto cell_vertex_id =
                grid.vertex_index( cell_vertices[vertex_id] );
            cell_mesh_vertices[vertex_id] = cell_vertex_id;
        }
        std::array< geode::index_t, 2 > created_triangles;
        created_triangles[0] = builder.create_triangle( { cell_mesh_vertices[0],
            cell_mesh_vertices[1], cell_mesh_vertices[3] } );
        created_triangles[1] = builder.create_triangle( { cell_mesh_vertices[0],
            cell_mesh_vertices[3], cell_mesh_vertices[2] } );
        return created_triangles;
    }

    void create_triangles_from_grid_cells(
        const geode::TriangulatedSurface2D& surface,
        geode::TriangulatedSurfaceBuilder2D& builder,
        const geode::Grid2D& grid,
        absl::Span< const geode::index_t > cells_to_densify )
    {
        builder.reserve_triangles(
            2 * grid.nb_cells() + 2 * cells_to_densify.size() );
        std::vector< bool > to_densify( grid.nb_cells(), false );
        for( const auto& cell_id : cells_to_densify )
        {
            to_densify[cell_id] = true;
        }
        geode::GenericMapping< geode::index_t > old2new_mapping;
        for( const auto cell_id : geode::Range{ grid.nb_cells() } )
        {
            if( to_densify[cell_id] )
            {
                continue;
            }
            const auto cell_indices = grid.cell_indices( cell_id );
            for( const auto triangle_id :
                create_triangles_from_diagonal_pattern(
                    builder, grid, cell_indices ) )
            {
                old2new_mapping.map( cell_id, triangle_id );
            }
        }
        for( const auto cell_index : geode::Indices{ cells_to_densify } )
        {
            const auto cell_indices =
                grid.cell_indices( cells_to_densify.at( cell_index ) );
            const auto additional_vertex_id =
                grid.nb_grid_vertices() + cell_index;
            for( const auto triangle_id : create_triangles_from_cross_pattern(
                     builder, grid, cell_indices, additional_vertex_id ) )
            {
                old2new_mapping.map(
                    cells_to_densify.at( cell_index ), triangle_id );
            }
        }
        builder.compute_polygon_adjacencies();
        surface.polygon_attribute_manager().import(
            grid.cell_attribute_manager(), old2new_mapping );
    }

    void create_vertices_from_grid( const geode::TriangulatedSurface2D& surface,
        geode::TriangulatedSurfaceBuilder2D& builder,
        const geode::Grid2D& grid,
        absl::Span< const geode::index_t > cells_to_densify )
    {
        builder.create_vertices(
            grid.nb_grid_vertices() + cells_to_densify.size() );
        auto& surface_attribute_manager = surface.vertex_attribute_manager();
        geode::internal::copy_attributes(
            grid.grid_vertex_attribute_manager(), surface_attribute_manager );
        for( const auto vertex_id : geode::Range{ grid.nb_grid_vertices() } )
        {
            builder.set_point( vertex_id,
                grid.grid_point( grid.vertex_indices( vertex_id ) ) );
        }
        geode::index_t counter{ grid.nb_grid_vertices() };
        for( const auto cell_id : cells_to_densify )
        {
            const auto cell_indices = grid.cell_indices( cell_id );
            builder.set_point( counter, grid.cell_barycenter( cell_indices ) );
            std::vector< geode::index_t > cell_vertices;
            std::vector< double > lambdas;
            cell_vertices.reserve( 4 );
            lambdas.reserve( 4 );
            geode::Point2D position{ { 0., 0. } };
            for( const auto& vertex_indices :
                grid.cell_vertices( cell_indices ) )
            {
                cell_vertices.push_back( grid.vertex_index( vertex_indices ) );
                lambdas.push_back( 0.25 );
                position += grid.grid_point( vertex_indices ) * 0.25;
            }
            surface_attribute_manager.interpolate_attribute_value(
                { cell_vertices, lambdas }, counter );
            builder.set_point( counter, position );
            counter++;
        }
    }

    template <>
    std::unique_ptr< geode::TriangulatedSurface2D >
        create_triangulated_surface_from_grid( const geode::Grid2D& grid,
            absl::Span< const geode::index_t > cells_to_densify )
    {
        auto surface = geode::TriangulatedSurface2D::create();
        auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
        create_vertices_from_grid( *surface, *builder, grid, cells_to_densify );
        create_triangles_from_grid_cells(
            *surface, *builder, grid, cells_to_densify );
        return surface;
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
    std::optional< std::unique_ptr< TriangulatedSurface< dimension > > >
        convert_surface_mesh_into_triangulated_surface(
            const SurfaceMesh< dimension >& surface )
    {
        if( surface.type_name()
            == TriangulatedSurface< dimension >::type_name_static() )
        {
            return dynamic_cast< const TriangulatedSurface< dimension >& >(
                surface )
                .clone();
        }
        if( surface.type_name()
            == RegularGrid< dimension >::type_name_static() )
        {
            std::optional< std::unique_ptr< TriangulatedSurface< dimension > > >
                result{ create_triangulated_surface_from_grid(
                    dynamic_cast< const RegularGrid< dimension >& >( surface ),
                    {} ) };
            auto builder = TriangulatedSurfaceBuilder< dimension >::create(
                *result->get() );
            internal::copy_meta_info( surface, *builder );
            return result;
        }
        if( !all_polygons_are_simplex( surface ) )
        {
            Logger::info( "[convert_surface_mesh_into_triangulated_surface] "
                          "SurfaceMesh is not made of only triangles." );
            return std::nullopt;
        }
        std::optional< std::unique_ptr< TriangulatedSurface< dimension > > >
            tri_surface{ TriangulatedSurface< dimension >::create() };
        convert_surface( surface, *tri_surface->get() );
        return tri_surface;
    }

    std::unique_ptr< TriangulatedSurface2D >
        convert_grid_into_triangulated_surface( const Grid2D& grid )
    {
        return create_triangulated_surface_from_grid( grid, {} );
    }

    std::unique_ptr< TriangulatedSurface2D >
        convert_grid_into_densified_triangulated_surface( const Grid2D& grid,
            absl::Span< const geode::index_t > cells_to_densify )
    {
        return create_triangulated_surface_from_grid( grid, cells_to_densify );
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
        for( const auto p : Range{ surface.nb_polygons() } )
        {
            const auto nb_vertices = surface.nb_polygon_vertices( p );
            to_delete[p] = nb_vertices != 3;
            if( nb_vertices > 3 )
            {
                using Edge = std::array< index_t, 2 >;
                absl::flat_hash_map< Edge, PolygonEdge > adjacents;
                const auto vertices = surface.polygon_vertices( p );
                for( const auto e : LRange{ nb_vertices } )
                {
                    if( const auto adj =
                            surface.polygon_adjacent_edge( { p, e } ) )
                    {
                        adjacents.emplace(
                            Edge{ vertices[e],
                                vertices[e + 1 == nb_vertices ? 0 : e + 1] },
                            adj.value() );
                    }
                }
                const auto new_triangles = surface.polygon( p ).triangulate();
                absl::FixedArray< index_t > new_polygons(
                    new_triangles.size() );
                for( const auto trgl : LIndices{ new_polygons } )
                {
                    const auto& new_triangle = new_triangles[trgl];
                    const std::array triangle{ vertices[new_triangle[0]],
                        vertices[new_triangle[1]], vertices[new_triangle[2]] };
                    new_polygons[trgl] = builder.create_polygon( triangle );
                    for( const auto e : LRange{ 3 } )
                    {
                        const auto vertex0 = triangle[e];
                        const auto vertex1 = triangle[e == 2 ? 0 : e + 1];
                        const auto adj_it =
                            adjacents.find( { vertex0, vertex1 } );
                        if( adj_it == adjacents.end() )
                        {
                            continue;
                        }
                        builder.set_polygon_adjacent(
                            adj_it->second, new_polygons[trgl] );
                        builder.set_polygon_adjacent( { new_polygons[trgl], e },
                            adj_it->second.polygon_id );
                    }
                }
                builder.compute_polygon_adjacencies( new_polygons );
            }
        }
        to_delete.resize( surface.nb_polygons(), false );
        builder.delete_polygons( to_delete );
    }

    std::unique_ptr< SurfaceMesh3D > convert_surface_mesh2d_into_3d(
        const SurfaceMesh2D& surface2d,
        index_t axis_to_add,
        double axis_coordinate )
    {
        auto surface3d = SurfaceMesh3D::create();
        auto builder3d = SurfaceMeshBuilder3D::create( *surface3d );
        internal::copy_meta_info( surface2d, *builder3d );
        internal::copy_points2d_into_3d(
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
        internal::copy_meta_info( surface3d, *builder2d );
        internal::copy_points3d_into_2d(
            surface3d, *builder2d, axis_to_remove );
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
        internal::copy_meta_info( surface2d, *builder3d );
        internal::copy_points2d_into_3d(
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
        internal::copy_meta_info( surface3d, *builder2d );
        internal::copy_points3d_into_2d(
            surface3d, *builder2d, axis_to_remove );
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
        internal::copy_points2d_into_3d(
            surface2d, *builder3d, axis_to_add, axis_coordinate );
        internal::copy_meta_info( surface2d, *builder3d );
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

        internal::copy_points3d_into_2d(
            surface3d, *builder2d, axis_to_remove );
        internal::copy_meta_info( surface3d, *builder2d );
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
        detail::SurfaceMeshMerger< dimension > merger{ surfaces };
        return merger.merge( GLOBAL_EPSILON );
    }

    template std::unique_ptr< PolygonalSurface2D > opengeode_mesh_api
        convert_surface_mesh_into_polygonal_surface( const SurfaceMesh2D& );
    template std::unique_ptr< PolygonalSurface3D > opengeode_mesh_api
        convert_surface_mesh_into_polygonal_surface( const SurfaceMesh3D& );

    template std::optional< std::unique_ptr< TriangulatedSurface2D > >
        opengeode_mesh_api convert_surface_mesh_into_triangulated_surface(
            const SurfaceMesh2D& );
    template std::optional< std::unique_ptr< TriangulatedSurface3D > >
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
