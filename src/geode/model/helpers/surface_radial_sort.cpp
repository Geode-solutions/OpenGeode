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

#include <geode/model/helpers/surface_radial_sort.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/radial_sort.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/component_mesh_vertices.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/mixin/core/vertex_identifier.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    geode::Point3D opposite(
        const geode::Surface3D& surface, const geode::PolygonEdge& edge )
    {
        const auto& mesh = surface.mesh();
        auto vertex = mesh.polygon_vertex( mesh.previous_polygon_edge( edge ) );
        return mesh.point( vertex );
    }

    struct BorderPolygon
    {
        BorderPolygon( const geode::Surface3D& surface_in,
            bool same_orientation_in,
            geode::PolygonEdge edge_in )
            : surface( surface_in.id() ),
              same_orientation( same_orientation_in ),
              edge( std::move( edge_in ) ),
              opposite_point{ opposite( surface_in, edge_in ) }
        {
        }

        bool operator==( const BorderPolygon& rhs ) const
        {
            return surface == rhs.surface
                   && edge.polygon_id == rhs.edge.polygon_id
                   && edge.edge_id == rhs.edge.edge_id;
        }

        bool operator<( const BorderPolygon& rhs ) const
        {
            if( surface != rhs.surface )
            {
                return surface < rhs.surface;
            }
            if( edge.polygon_id != rhs.edge.polygon_id )
            {
                return edge.polygon_id < rhs.edge.polygon_id;
            }
            return edge.edge_id < rhs.edge.edge_id;
        }

        geode::uuid surface;
        bool same_orientation;
        geode::PolygonEdge edge;
        geode::Point3D opposite_point;
    };

    bool is_polygon_degenerate( const geode::SurfaceMesh3D& mesh,
        const geode::PolygonEdge& edge,
        const geode::Point3D& opposite_point )
    {
        const auto edge_vertices = mesh.polygon_edge_vertices( edge );
        return std::get< 0 >( geode::point_segment_distance(
                   opposite_point, { mesh.point( edge_vertices[0] ),
                                       mesh.point( edge_vertices[1] ) } ) )
               <= geode::global_epsilon;
    }

    std::pair< bool, std::vector< BorderPolygon > > border_polygons(
        const geode::BRep& brep,
        const geode::Line3D& line,
        geode::index_t e0,
        geode::index_t e1 )
    {
        const auto line_v0 = brep.unique_vertex( { line.component_id(), e0 } );
        const auto line_v1 = brep.unique_vertex( { line.component_id(), e1 } );
        const auto surface_vertices0 = brep.component_mesh_vertices(
            line_v0, geode::Surface3D::component_type_static() );
        const auto surface_vertices1 = brep.component_mesh_vertices(
            line_v1, geode::Surface3D::component_type_static() );
        std::vector< BorderPolygon > polygons;
        bool degenerate_polygon{ false };
        for( const auto& vertex_pairs : geode::component_mesh_vertex_pairs(
                 surface_vertices0, surface_vertices1 ) )
        {
            const auto& surface_id = vertex_pairs.first.id();
            const auto& surface = brep.surface( surface_id );
            const auto& surface_mesh = surface.mesh();
            for( const auto& pair : vertex_pairs.second )
            {
                if( auto edge0 = surface_mesh.polygon_edge_from_vertices(
                        pair.first, pair.second ) )
                {
                    polygons.emplace_back(
                        surface, true, std::move( edge0.value() ) );
                    degenerate_polygon =
                        degenerate_polygon
                        || is_polygon_degenerate( surface_mesh, edge0.value(),
                            polygons.back().opposite_point );
                }
                if( auto edge1 = surface_mesh.polygon_edge_from_vertices(
                        pair.second, pair.first ) )
                {
                    polygons.emplace_back(
                        surface, false, std::move( edge1.value() ) );
                    degenerate_polygon =
                        degenerate_polygon
                        || is_polygon_degenerate( surface_mesh, edge1.value(),
                            polygons.back().opposite_point );
                }
            }
        }
        geode::sort_unique( polygons );
        return std::make_pair( !degenerate_polygon, polygons );
    }

    geode::SortedSurfaces sort( const geode::Segment3D& segment,
        absl::Span< const BorderPolygon > polygons )
    {
        geode::SortedSurfaces sorted_surfaces( polygons.size() );
        absl::FixedArray< geode::Point3D > points( polygons.size() );
        for( const auto p : geode::Indices{ polygons } )
        {
            points[p] = polygons[p].opposite_point;
        }
        geode::index_t count{ 0 };
        for( const auto sorted : geode::radial_sort( segment, points ) )
        {
            const auto& polygon = polygons[sorted];
            sorted_surfaces.surfaces[count++] = { polygon.surface,
                !polygon.same_orientation, polygon.edge };
            sorted_surfaces.surfaces[count++] = { polygon.surface,
                polygon.same_orientation, polygon.edge };
        }
        return sorted_surfaces;
    }
} // namespace

namespace geode
{
    SortedSurfaces::SortedSurfaces( index_t nb_surfaces )
        : surfaces( 2 * nb_surfaces )
    {
    }

    index_t SortedSurfaces::opposite( index_t position ) const
    {
        const auto side_position = position % 2;
        if( side_position == 1 )
        {
            return ( position + surfaces.size() - 1 ) % surfaces.size();
        }
        return ( position + 1 ) % surfaces.size();
    }

    index_t SortedSurfaces::next( index_t position ) const
    {
        const auto side_position = position % 2;
        if( side_position == 1 )
        {
            return ( position + 1 ) % surfaces.size();
        }
        return ( position + surfaces.size() - 1 ) % surfaces.size();
    }

    absl::InlinedVector< index_t, 1 > SortedSurfaces::find(
        const SidedSurface& surface ) const
    {
        absl::InlinedVector< index_t, 1 > positions;
        index_t id{ 0 };
        for( const auto& s : surfaces )
        {
            if( s == surface )
            {
                positions.push_back( id );
            }
            id++;
        }
        return positions;
    }

    SortedSurfaces surface_radial_sort( const BRep& brep, const Line3D& line )
    {
        const auto& mesh = line.mesh();
        for( const auto edge_id : Range{ mesh.nb_edges() } )
        {
            const auto e0 = mesh.edge_vertex( { 0, 0 } );
            const auto e1 = mesh.edge_vertex( { 0, 1 } );
            auto polygons = border_polygons( brep, line, e0, e1 );
            if( !polygons.first )
            {
                if( edge_id == mesh.nb_edges() - 1 )
                {
                    Logger::warn( "[surface_radial_sort] Degenerated polygons "
                                  "has been found on all the edges of Line ",
                        line.id().string(),
                        ". The result of surface_radial_sort is not "
                        "guaranteed." );
                }
                else
                {
                    continue;
                }
            }
            const auto& p0 = line.mesh().point( e0 );
            const auto& p1 = line.mesh().point( e1 );
            return sort( { p0, p1 }, polygons.second );
        }
        OPENGEODE_ASSERT_NOT_REACHED(
            "[surface_radial_sort] Cannot find sorted surfaces on a Line" );
        return { 0 };
    }
} // namespace geode
