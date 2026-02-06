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

#include <geode/mesh/helpers/detail/mesh_intersection_detection.hpp>

#include <absl/algorithm/container.h>

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/intersection_detection.hpp>
#include <geode/geometry/position.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

namespace
{
    geode::PolygonVertices triangles_common_points(
        const geode::PolygonVertices& vertices,
        const geode::PolygonVertices& other_vertices )
    {
        geode::PolygonVertices common_points;
        for( const auto vertex_id : vertices )
        {
            if( absl::c_find( other_vertices, vertex_id )
                != other_vertices.end() )
            {
                common_points.push_back( vertex_id );
            }
        }
        return common_points;
    }

    absl::InlinedVector< geode::PolygonVertices, 1 > fan_triangles(
        const geode::PolygonVertices& polygon, geode::local_index_t apex )
    {
        absl::InlinedVector< geode::PolygonVertices, 1 > triangles;
        for( const auto edge : geode::LRange{ 1, polygon.size() - 1 } )
        {
            const auto next_apex = ( apex + edge ) % polygon.size();
            const auto next_next_apex =
                next_apex + 1 == polygon.size() ? 0u : next_apex + 1;
            triangles.emplace_back( geode::PolygonVertices{
                polygon[apex], polygon[next_apex], polygon[next_next_apex] } );
        }
        return triangles;
    }

    geode::index_t third_point_index( const geode::PolygonVertices& vertices,
        const geode::PolygonVertices& vertices_to_avoid )
    {
        for( const auto vertex_id : vertices )
        {
            if( vertex_id == vertices_to_avoid[0]
                || vertex_id == vertices_to_avoid[1] )
            {
                continue;
            }
            return vertex_id;
        }
        OPENGEODE_ASSERT_NOT_REACHED( "Should have found a third point index" );
        return geode::NO_ID;
    }

    bool triangle_intersects_other( const geode::Triangle3D& triangle,
        const geode::Triangle3D& other_triangle,
        const geode::PolygonVertices& triangle_vertices,
        const geode::PolygonVertices& other_triangle_vertices,
        const geode::PolygonVertices& common_points )
    {
        for( const auto v_id : geode::LRange{ 3 } )
        {
            const auto v2_id = v_id == 2 ? 0 : v_id + 1;
            const auto intersection = segment_triangle_intersection_detection(
                { triangle.vertices()[v_id], triangle.vertices()[v2_id] },
                other_triangle );
            if( intersection.first == geode::POSITION::outside )
            {
                continue;
            }
            if( common_points.size() != 1 )
            {
                return true;
            }
            const auto edge_inter_pt_id =
                geode::detail::position_to_index( intersection.first );
            const auto triangle_inter_pt_id =
                geode::detail::position_to_index( intersection.second );
            if( edge_inter_pt_id == geode::NO_LID
                || triangle_inter_pt_id == geode::NO_LID )
            {
                return true;
            }
            if( triangle_vertices[edge_inter_pt_id == 0 ? v_id : v2_id]
                    != other_triangle_vertices[triangle_inter_pt_id]
                || other_triangle_vertices[triangle_inter_pt_id]
                       != common_points[0] )
            {
                return true;
            }
        }
        return false;
    }

    template < typename Mesh >
    geode::Triangle< Mesh::dim > mesh_triangle(
        const Mesh& mesh, const geode::PolygonVertices& triangle_vertices )
    {
        return { mesh.point( triangle_vertices[0] ),
            mesh.point( triangle_vertices[1] ),
            mesh.point( triangle_vertices[2] ) };
    }

    template < typename Mesh >
    typename std::enable_if< Mesh::dim == 3, bool >::type triangles_intersect(
        const Mesh& mesh,
        const geode::PolygonVertices& triangle_vertices,
        const geode::PolygonVertices& other_triangle_vertices,
        const geode::PolygonVertices& common_points )
    {
        const auto other_triangle =
            mesh_triangle( mesh, other_triangle_vertices );
        if( common_points.size() == 2 )
        {
            const auto& common_point0 = mesh.point( common_points[0] );
            const auto& common_point1 = mesh.point( common_points[1] );
            const auto& third_point = mesh.point(
                third_point_index( triangle_vertices, common_points ) );
            return geode::segment_triangle_intersection_detection(
                       { common_point0, third_point }, other_triangle )
                           .first
                       == geode::POSITION::parallel
                   || geode::segment_triangle_intersection_detection(
                          { common_point1, third_point }, other_triangle )
                              .first
                          == geode::POSITION::parallel;
        }
        const auto triangle = mesh_triangle( mesh, triangle_vertices );
        if( triangle_intersects_other( triangle, other_triangle,
                triangle_vertices, other_triangle_vertices, common_points )
            || triangle_intersects_other( other_triangle, triangle,
                other_triangle_vertices, triangle_vertices, common_points ) )
        {
            return true;
        }
        return false;
    }

    template < typename Mesh >
    typename std::enable_if< Mesh::dim == 2, bool >::type triangles_intersect(
        const Mesh& mesh,
        const geode::PolygonVertices& triangle_vertices,
        const geode::PolygonVertices& other_triangle_vertices,
        const geode::PolygonVertices& common_points )
    {
        if( common_points.size() == 2 )
        {
            const auto& common_point0 = mesh.point( common_points[0] );
            const auto& common_point1 = mesh.point( common_points[1] );
            const auto& third_point = mesh.point(
                third_point_index( triangle_vertices, common_points ) );
            const auto& other_third_point = mesh.point(
                third_point_index( other_triangle_vertices, common_points ) );
            if( geode::segment_segment_intersection_detection(
                    { common_point0, third_point },
                    { common_point1, other_third_point } )
                        .first
                    != geode::POSITION::outside
                || geode::segment_segment_intersection_detection(
                       { common_point1, third_point },
                       { common_point0, other_third_point } )
                           .first
                       != geode::POSITION::outside )
            {
                return true;
            }
            const auto triangle = mesh_triangle( mesh, triangle_vertices );
            const auto other_triangle =
                mesh_triangle( mesh, other_triangle_vertices );
            return geode::point_triangle_position( third_point, other_triangle )
                       != geode::POSITION::outside
                   || geode::point_triangle_position(
                          other_third_point, triangle )
                          != geode::POSITION::outside;
        }
        for( const auto edge_v : geode::LRange{ 3 } )
        {
            const std::array< geode::index_t, 2 > edge_vertices{
                triangle_vertices[edge_v],
                triangle_vertices[edge_v == 2 ? 0 : edge_v + 1]
            };
            const geode::Segment2D edge{ mesh.point( edge_vertices[0] ),
                mesh.point( edge_vertices[1] ) };
            for( const auto other_edge_v : geode::LRange{ 3 } )
            {
                const std::array< geode::index_t, 2 > other_edge_vertices{
                    other_triangle_vertices[other_edge_v],
                    other_triangle_vertices[other_edge_v == 2
                                                ? 0
                                                : other_edge_v + 1]
                };
                const geode::Segment2D other_edge{ mesh.point(
                                                       other_edge_vertices[0] ),
                    mesh.point( other_edge_vertices[1] ) };
                const auto edge_edge_inter =
                    geode::segment_segment_intersection_detection(
                        edge, other_edge );
                if( edge_edge_inter.first == geode::POSITION::outside
                    || edge_edge_inter.first == geode::POSITION::parallel )
                {
                    continue;
                }
                if( common_points.size() != 1 )
                {
                    return true;
                }
                const auto edge_inter_pt_id =
                    geode::detail::position_to_index( edge_edge_inter.first );
                const auto other_edge_inter_pt_id =
                    geode::detail::position_to_index( edge_edge_inter.second );
                if( edge_inter_pt_id == geode::NO_LID
                    || other_edge_inter_pt_id == geode::NO_LID )
                {
                    return true;
                }
                if( edge_vertices[edge_inter_pt_id]
                        != other_edge_vertices[other_edge_inter_pt_id]
                    || edge_vertices[edge_inter_pt_id] != common_points[0] )
                {
                    return true;
                }
            }
        }
        return false;
    }

    template < typename Mesh >
    bool triangles_intersection_detection( const Mesh& mesh,
        const geode::PolygonVertices& triangle,
        const geode::PolygonVertices& other_triangle )
    {
        const auto common_points =
            triangles_common_points( triangle, other_triangle );
        if( common_points.size() == 3 )
        {
            return true;
        }
        return triangles_intersect(
            mesh, triangle, other_triangle, common_points );
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < typename Mesh >
        bool polygons_intersection_detection( const Mesh& mesh,
            const PolygonVertices& polygon,
            const PolygonVertices& other_polygon )
        {
            if( polygon.size() < 3 || other_polygon.size() < 3 )
            {
                return false;
            }
            if( polygon.size() == 3 && other_polygon.size() == 3 )
            {
                return triangles_intersection_detection(
                    mesh, polygon, other_polygon );
            }
            for( const auto v : geode::LRange{ polygon.size() - 2 } )
            {
                for( const auto& triangle : fan_triangles( polygon, v ) )
                {
                    for( const auto other_v :
                        geode::LRange{ other_polygon.size() - 2 } )
                    {
                        for( const auto& other_triangle :
                            fan_triangles( other_polygon, other_v ) )
                        {
                            if( triangles_intersection_detection(
                                    mesh, triangle, other_triangle ) )
                            {
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
        }

        template bool opengeode_mesh_api polygons_intersection_detection(
            const SurfaceMesh2D&,
            const PolygonVertices&,
            const PolygonVertices& );
        template bool opengeode_mesh_api polygons_intersection_detection(
            const SurfaceMesh3D&,
            const PolygonVertices&,
            const PolygonVertices& );

        template bool opengeode_mesh_api polygons_intersection_detection(
            const SolidMesh3D&,
            const PolygonVertices&,
            const PolygonVertices& );
    } // namespace detail
} // namespace geode