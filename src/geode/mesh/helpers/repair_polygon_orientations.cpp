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

#include <geode/mesh/helpers/repair_polygon_orientations.h>

#include <queue>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/logger.h>

#include <geode/mesh/builder/surface_edges_builder.h>
#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/surface_mesh.h>

namespace
{
    template < geode::index_t dimension >
    class PolygonOrientationChecker
    {
        static constexpr auto reorient_attribute_name = "reorient";

    public:
        PolygonOrientationChecker( const geode::SurfaceMesh< dimension >& mesh )
            : mesh_( mesh ), reorient_polygon_( mesh.nb_polygons(), false )
        {
        }

        absl::FixedArray< geode::index_t > compute_bad_oriented_polygons()
        {
            absl::FixedArray< bool > visited( mesh_.nb_polygons(), false );
            for( const auto p : geode::Range{ mesh_.nb_polygons() } )
            {
                if( visited[p] )
                {
                    continue;
                }
                queue_.emplace( p );
                visited[p] = true;
                process_polygon_queue( visited );
            }
            return get_bad_oriented_polygons();
        }

    private:
        void process_polygon_queue( absl::FixedArray< bool >& visited )
        {
            while( !queue_.empty() )
            {
                const auto cur_polygon = queue_.front();
                queue_.pop();
                const auto cur_polygon_reorient =
                    reorient_polygon_[cur_polygon];
                for( const auto e :
                    geode::LRange{ mesh_.nb_polygon_edges( cur_polygon ) } )
                {
                    const geode::PolygonEdge edge{ cur_polygon, e };
                    if( mesh_.is_edge_on_border( edge ) )
                    {
                        continue;
                    }
                    const auto adj =
                        mesh_.polygon_adjacent_edge( edge ).value();
                    if( visited[adj.polygon_id] )
                    {
                        continue;
                    }
                    const auto vertices = mesh_.polygon_edge_vertices( edge );
                    const auto adj_vertices =
                        mesh_.polygon_edge_vertices( adj );
                    const auto same_orientation =
                        ( vertices[0] == adj_vertices[1]
                            && vertices[1] == adj_vertices[0] );
                    visited[adj.polygon_id] = true;
                    reorient_polygon_[adj.polygon_id] =
                        cur_polygon_reorient == same_orientation;
                    if( reorient_polygon_[adj.polygon_id] )
                    {
                        nb_++;
                    }
                    queue_.emplace( adj.polygon_id );
                }
            }
        }

        absl::FixedArray< geode::index_t > get_bad_oriented_polygons() const
        {
            absl::FixedArray< geode::index_t > bad_polygons( nb_ );
            geode::index_t count{ 0 };
            for( const auto p : geode::Range{ mesh_.nb_polygons() } )
            {
                if( reorient_polygon_[p] )
                {
                    bad_polygons[count++] = p;
                }
            }
            return bad_polygons;
        }

    private:
        const geode::SurfaceMesh< dimension >& mesh_;
        std::queue< geode::index_t > queue_;
        std::vector< bool > reorient_polygon_;
        geode::index_t nb_{ 0 };
    };

    template < geode::index_t dimension >
    absl::FixedArray< geode::index_t > identify_bad_orented_polygons(
        const geode::SurfaceMesh< dimension >& mesh )
    {
        PolygonOrientationChecker< dimension > checker{ mesh };
        return checker.compute_bad_oriented_polygons();
    }

    template < geode::index_t dimension >
    void reorient_bad_polygons( geode::SurfaceMeshBuilder< dimension >& builder,
        geode::SurfaceMesh< dimension >& mesh,
        absl::Span< const geode::index_t > bad_polygons )
    {
        for( const auto p : bad_polygons )
        {
            absl::FixedArray< geode::index_t > vertices(
                mesh.nb_polygon_vertices( p ) );
            absl::FixedArray< absl::optional< geode::index_t > > adjacents(
                mesh.nb_polygon_edges( p ) );
            for( const auto v : geode::LRange{ mesh.nb_polygon_vertices( p ) } )
            {
                vertices[v] = mesh.polygon_vertex( { p, v } );
                adjacents[v] = mesh.polygon_adjacent( { p, v } );
            }
            std::reverse( vertices.begin() + 1, vertices.end() );
            absl::c_reverse( adjacents );
            for( const auto v : geode::LRange{ mesh.nb_polygon_vertices( p ) } )
            {
                builder.disassociate_polygon_vertex_to_vertex(
                    mesh.polygon_vertex( { p, v } ) );
                builder.set_polygon_vertex( { p, v }, vertices[v] );
                builder.associate_polygon_vertex_to_vertex(
                    { p, v }, vertices[v] );
                if( adjacents[v] )
                {
                    builder.set_polygon_adjacent(
                        { p, v }, adjacents[v].value() );
                }
                else
                {
                    builder.unset_polygon_adjacent( { p, v } );
                }
            }
        }
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    void repair_polygon_orientations( SurfaceMesh< dimension >& mesh )
    {
        auto builder = SurfaceMeshBuilder< dimension >::create( mesh );
        const auto polygons_to_reorient = identify_bad_orented_polygons( mesh );
        reorient_bad_polygons( *builder, mesh, polygons_to_reorient );
        if( mesh.are_edges_enabled() )
        {
            builder->edges_builder().delete_isolated_edges();
        }
        Logger::info( "Repair polygons orientations: ",
            polygons_to_reorient.size(), " polygons reoriented" );
    }

    template void opengeode_mesh_api repair_polygon_orientations(
        SurfaceMesh2D& );
    template void opengeode_mesh_api repair_polygon_orientations(
        SurfaceMesh3D& );
} // namespace geode
