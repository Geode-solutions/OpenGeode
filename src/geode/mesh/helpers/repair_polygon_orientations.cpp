/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/mesh/helpers/repair_polygon_orientations.hpp>

#include <queue>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/sign.hpp>

#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

namespace
{
    template < geode::index_t dimension >
    class PolygonOrientationChecker
    {
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
                const auto vertices = mesh_.polygon_vertices( cur_polygon );
                for( const auto e : geode::LIndices{ vertices } )
                {
                    const auto adj =
                        mesh_.polygon_adjacent_edge( { cur_polygon, e } );
                    if( !adj || visited[adj->polygon_id] )
                    {
                        continue;
                    }
                    const auto e_next = e == vertices.size() - 1 ? 0 : e + 1;
                    const auto adj_vertices =
                        mesh_.polygon_edge_vertices( adj.value() );
                    const auto same_orientation =
                        ( vertices[e] == adj_vertices[1]
                            && vertices[e_next] == adj_vertices[0] );
                    const auto adj_polygon = adj->polygon_id;
                    visited[adj_polygon] = true;
                    reorient_polygon_[adj_polygon] =
                        cur_polygon_reorient == same_orientation;
                    if( reorient_polygon_[adj_polygon] )
                    {
                        nb_++;
                    }
                    queue_.emplace( adj_polygon );
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

    struct polygons_area_sign_info
    {
        polygons_area_sign_info( geode::index_t nb_init,
            geode::index_t nb_polygons,
            geode::Sign sign_init )
            : nb_bad_polygons{ nb_init }, area_sign{ nb_polygons, sign_init }
        {
        }

        geode::index_t nb_bad_polygons;
        std::queue< geode::index_t > queue;
        absl::FixedArray< geode::Sign > area_sign;
    };

    polygons_area_sign_info compute_polygon_area_sign(
        const geode::SurfaceMesh2D& mesh )
    {
        polygons_area_sign_info area_sign_info{ 0, mesh.nb_polygons(),
            geode::Sign::zero };
        for( const auto polygon_id : geode::Range{ mesh.nb_polygons() } )
        {
            const auto& p1 =
                mesh.point( mesh.polygon_vertex( { polygon_id, 0 } ) );
            for( const auto i :
                geode::LRange{ 1, mesh.nb_polygon_vertices( polygon_id ) - 1 } )
            {
                const auto& p2 =
                    mesh.point( mesh.polygon_vertex( { polygon_id, i } ) );
                const auto& p3 = mesh.point( mesh.polygon_vertex( { polygon_id,
                    static_cast< geode::local_index_t >( i + 1 ) } ) );
                const auto sign = geode::triangle_area_sign( { p1, p2, p3 } );
                if( sign == geode::Sign::positive )
                {
                    area_sign_info.area_sign[polygon_id] =
                        geode::Sign::positive;
                    break;
                }
                if( sign == geode::Sign::negative )
                {
                    area_sign_info.area_sign[polygon_id] =
                        geode::Sign::negative;
                    area_sign_info.nb_bad_polygons++;
                    break;
                }
            }
            if( area_sign_info.area_sign[polygon_id] == geode::Sign::zero )
            {
                area_sign_info.queue.emplace( polygon_id );
            }
        }
        OPENGEODE_EXCEPTION( area_sign_info.queue.size() != mesh.nb_polygons(),
            "[repair_polygon_bad_orientations] Cannot repair orientations of a "
            "SurfaceMesh2D where all polygons have a null area." );
        return area_sign_info;
    }

    void process_null_area_polygons( const geode::SurfaceMesh2D& mesh,
        polygons_area_sign_info& area_sign_info )
    {
        while( !area_sign_info.queue.empty() )
        {
            const auto cur_polygon = area_sign_info.queue.front();
            area_sign_info.queue.pop();
            for( const auto e :
                geode::LRange{ mesh.nb_polygon_edges( cur_polygon ) } )
            {
                const geode::PolygonEdge edge{ cur_polygon, e };
                if( mesh.is_edge_on_border( edge ) )
                {
                    continue;
                }
                const auto adj = mesh.polygon_adjacent_edge( edge ).value();
                if( area_sign_info.area_sign[adj.polygon_id]
                    == geode::Sign::zero )
                {
                    continue;
                }
                const auto vertices = mesh.polygon_edge_vertices( edge );
                const auto adj_vertices = mesh.polygon_edge_vertices( adj );
                const auto same_orientation =
                    ( vertices[0] == adj_vertices[1]
                        && vertices[1] == adj_vertices[0] );
                if( same_orientation )
                {
                    area_sign_info.area_sign[cur_polygon] =
                        area_sign_info.area_sign[adj.polygon_id];
                    break;
                }
                if( area_sign_info.area_sign[adj.polygon_id]
                    == geode::Sign::positive )
                {
                    area_sign_info.area_sign[cur_polygon] =
                        geode::Sign::negative;
                }
                else
                {
                    area_sign_info.area_sign[cur_polygon] =
                        geode::Sign::positive;
                }
                break;
            }
            if( area_sign_info.area_sign[cur_polygon] == geode::Sign::zero )
            {
                area_sign_info.queue.emplace( cur_polygon );
            }
            else if( area_sign_info.area_sign[cur_polygon]
                     == geode::Sign::negative )
            {
                area_sign_info.nb_bad_polygons++;
            }
        }
    }

    template < geode::index_t dimension >
    absl::FixedArray< geode::index_t > identify_badly_oriented_polygons(
        const geode::SurfaceMesh< dimension >& mesh );

    template <>
    absl::FixedArray< geode::index_t > identify_badly_oriented_polygons< 2 >(
        const geode::SurfaceMesh2D& mesh )
    {
        auto area_sign_info = compute_polygon_area_sign( mesh );
        process_null_area_polygons( mesh, area_sign_info );

        absl::FixedArray< geode::index_t > bad_polygons(
            area_sign_info.nb_bad_polygons );
        geode::index_t count{ 0 };
        for( const auto p : geode::Range{ mesh.nb_polygons() } )
        {
            if( area_sign_info.area_sign[p] == geode::Sign::negative )
            {
                bad_polygons[count++] = p;
            }
        }
        return bad_polygons;
    }

    template <>
    absl::FixedArray< geode::index_t > identify_badly_oriented_polygons< 3 >(
        const geode::SurfaceMesh3D& mesh )
    {
        PolygonOrientationChecker< 3 > checker{ mesh };
        return checker.compute_bad_oriented_polygons();
    }

    template < geode::index_t dimension >
    void reorient_bad_polygons( geode::SurfaceMeshBuilder< dimension >& builder,
        const geode::SurfaceMesh< dimension >& mesh,
        absl::Span< const geode::index_t > bad_polygons )
    {
        for( const auto p : bad_polygons )
        {
            absl::FixedArray< geode::index_t > vertices(
                mesh.nb_polygon_vertices( p ) );
            absl::FixedArray< std::optional< geode::index_t > > adjacents(
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
            }
            for( const auto v : geode::LRange{ mesh.nb_polygon_vertices( p ) } )
            {
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
        repair_polygon_orientations( mesh, *builder );
    }

    template < index_t dimension >
    void repair_polygon_orientations( const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder )
    {
        const auto polygons_to_reorient =
            identify_badly_oriented_polygons( mesh );
        reorient_bad_polygons( builder, mesh, polygons_to_reorient );
        if( mesh.are_edges_enabled() )
        {
            builder.edges_builder().delete_isolated_edges();
        }
        Logger::debug( "Repair polygons orientations: ",
            polygons_to_reorient.size(), " polygons reoriented" );
    }

    template void opengeode_mesh_api repair_polygon_orientations(
        SurfaceMesh2D& );
    template void opengeode_mesh_api repair_polygon_orientations(
        SurfaceMesh3D& );

    template void opengeode_mesh_api repair_polygon_orientations(
        const SurfaceMesh2D&, SurfaceMeshBuilder2D& );
    template void opengeode_mesh_api repair_polygon_orientations(
        const SurfaceMesh3D&, SurfaceMeshBuilder3D& );
} // namespace geode
