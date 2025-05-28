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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#include <geode/mesh/helpers/mesh_quality.hpp>

#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

namespace
{
    template < geode::index_t dimension >
    void check_polygon_id( const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t polygon_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        OPENGEODE_ASSERT( polygon_id < surface.nb_polygons(),
            "[check_polygon_id] Trying to access an invalid polygon" );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    bool is_edge_to_small( const EdgedCurve< dimension >& mesh,
        index_t edge_index,
        double threshold )
    {
        const auto edge_vertices = mesh.edge_vertices( edge_index );
        const auto p1 = mesh.point( edge_vertices[0] );
        const auto p2 = mesh.point( edge_vertices[1] );
        return point_point_distance( p1, p2 ) < threshold;
    }

    template < index_t dimension >
    bool is_polygon_minimum_height_too_small(
        const SurfaceMesh< dimension >& mesh,
        index_t polygon_id,
        double threshold )
    {
        check_polygon_id( mesh, polygon_id );
        const auto polygon = mesh.polygon( polygon_id );
        return polygon.is_polygon_minimum_height_too_small( threshold );
    }

    template < index_t dimension >
    bool is_polyhedron_minimum_height_too_small(
        const SolidMesh< dimension >& mesh,
        index_t polyhedron_id,
        double threshold )
    {
        double max_area{ 0. };
        local_index_t max_area_facet{ 0 };
        for( const auto f :
            LRange{ mesh.nb_polyhedron_facets( polyhedron_id ) } )
        {
            const auto cur_area =
                mesh.polyhedron_facet_area( { polyhedron_id, f } );
            if( cur_area > max_area )
            {
                max_area = cur_area;
                max_area_facet = f;
            }
        }
        const auto vertices = mesh.polyhedron_vertices( polyhedron_id );
        const auto normal =
            mesh.polyhedron_facet_normal( { polyhedron_id, max_area_facet } );
        if( !normal )
        {
            return true;
        }
        const auto facet_vertices =
            mesh.polyhedron_facet_vertices( { polyhedron_id, max_area_facet } );
        Plane plane{ normal.value(), mesh.point( facet_vertices[0] ) };
        for( const auto vertex_id : vertices )
        {
            if( absl::c_contains( facet_vertices, vertex_id ) )
            {
                continue;
            }
            if( std::get< 0 >(
                    point_plane_distance( mesh.point( vertex_id ), plane ) )
                > threshold )
            {
                return false;
            }
        }
        return true;
    }

    template bool opengeode_mesh_api is_edge_to_small(
        const EdgedCurve2D&, index_t, double );
    template bool opengeode_mesh_api is_edge_to_small(
        const EdgedCurve3D&, index_t, double );
    template bool opengeode_mesh_api is_polygon_minimum_height_too_small(
        const SurfaceMesh2D&, index_t, double );
    template bool opengeode_mesh_api is_polygon_minimum_height_too_small(
        const SurfaceMesh3D&, index_t, double );
    template bool opengeode_mesh_api is_polyhedron_minimum_height_too_small(
        const SolidMesh3D&, index_t, double );
} // namespace geode