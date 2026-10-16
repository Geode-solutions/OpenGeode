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

#include <geode/mesh/helpers/ray_tracing.hpp>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/intersection.hpp>
#include <geode/geometry/intersection_detection.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

namespace
{
    template < geode::index_t dimension, typename Line >
    geode::Point< dimension > begin(
        const geode::BoundingBox< dimension >& bbox, const Line& line )
    {
        auto bbox_with_line = bbox;
        bbox_with_line.add_point( line.origin() );
        const auto diagonal = bbox_with_line.diagonal();
        return line.origin() - ( line.direction() * diagonal.length() );
    }

    template < geode::index_t dimension, typename Line >
    geode::Point< dimension > end(
        const geode::BoundingBox< dimension >& bbox, const Line& line )
    {
        auto bbox_with_line = bbox;
        bbox_with_line.add_point( line.origin() );
        const auto diagonal = bbox_with_line.diagonal();
        return line.origin() + ( line.direction() * diagonal.length() );
    }

    bool test_vertex_mode( const geode::EdgedCurve2D& mesh,
        const geode::RayTracing2D::EdgeDistance& edge0,
        const geode::RayTracing2D::EdgeDistance& edge1 )
    {
        geode::EdgeVertex edge_vertex0{ edge0.edge, geode::NO_LID };
        if( edge0.position == geode::POSITION::vertex0 )
        {
            edge_vertex0.vertex_id = 0;
        }
        else if( edge0.position == geode::POSITION::vertex1 )
        {
            edge_vertex0.vertex_id = 1;
        }
        else
        {
            return false;
        }
        geode::EdgeVertex edge_vertex1{ edge1.edge, geode::NO_LID };
        if( edge1.position == geode::POSITION::vertex0 )
        {
            edge_vertex1.vertex_id = 0;
        }
        else if( edge1.position == geode::POSITION::vertex1 )
        {
            edge_vertex1.vertex_id = 1;
        }
        else
        {
            return false;
        }
        return mesh.edge_vertex( edge_vertex0 )
               == mesh.edge_vertex( edge_vertex1 );
    }

    /*!
     * Polygon-local vertex or edge hit by a ray, if any.
     */
    struct PolygonFeature
    {
        geode::local_index_t vertex{ geode::NO_LID };
        geode::local_index_t edge{ geode::NO_LID };
    };

    struct PolygonHit
    {
        geode::RayTracing3D::PolygonDistance distance;
        PolygonFeature feature;

        bool operator<( const PolygonHit& other ) const
        {
            return distance < other.distance;
        }
    };

    geode::POSITION polygon_vertex_position( geode::local_index_t vertex )
    {
        static constexpr std::array< geode::POSITION, 4 > POSITIONS{
            geode::POSITION::vertex0, geode::POSITION::vertex1,
            geode::POSITION::vertex2, geode::POSITION::vertex3
        };
        // POSITION cannot express vertices after the fourth one
        return vertex < POSITIONS.size() ? POSITIONS[vertex]
                                         : geode::POSITION::parallel;
    }

    geode::POSITION polygon_edge_position( geode::local_index_t edge )
    {
        static constexpr std::array< geode::POSITION, 3 > POSITIONS{
            geode::POSITION::edge0, geode::POSITION::edge1,
            geode::POSITION::edge2
        };
        // POSITION cannot express edges after the third one
        return edge < POSITIONS.size() ? POSITIONS[edge]
                                       : geode::POSITION::parallel;
    }

    /*!
     * Convert a position in the fan triangle {0, fan_edge, fan_edge + 1} of
     * a polygon into a position in the polygon.
     */
    std::tuple< geode::POSITION, PolygonFeature > polygon_position(
        geode::POSITION fan_position,
        geode::local_index_t fan_edge,
        geode::local_index_t nb_polygon_vertices )
    {
        PolygonFeature feature;
        switch( fan_position )
        {
            case geode::POSITION::vertex0:
                feature.vertex = 0;
                break;
            case geode::POSITION::vertex1:
                feature.vertex = fan_edge;
                break;
            case geode::POSITION::vertex2:
                feature.vertex = fan_edge + 1;
                break;
            case geode::POSITION::edge0:
                if( fan_edge != 1 )
                {
                    // Diagonal between two fan triangles
                    return { geode::POSITION::inside, feature };
                }
                feature.edge = 0;
                break;
            case geode::POSITION::edge1:
                feature.edge = fan_edge;
                break;
            case geode::POSITION::edge2:
                if( fan_edge + 2 != nb_polygon_vertices )
                {
                    // Diagonal between two fan triangles
                    return { geode::POSITION::inside, feature };
                }
                feature.edge = nb_polygon_vertices - 1;
                break;
            default:
                return { fan_position, feature };
        }
        if( feature.vertex != geode::NO_LID )
        {
            return { polygon_vertex_position( feature.vertex ), feature };
        }
        return { polygon_edge_position( feature.edge ), feature };
    }

    bool are_equal( const geode::EdgedCurve2D& mesh,
        const geode::RayTracing2D::EdgeDistance& edge0,
        const geode::RayTracing2D::EdgeDistance& edge1 )
    {
        if( std::fabs( edge0.distance - edge1.distance )
            > geode::GLOBAL_EPSILON )
        {
            return false;
        }
        if( edge0.position == geode::POSITION::inside
            || edge0.position == geode::POSITION::parallel )
        {
            return false;
        }
        if( edge1.position == geode::POSITION::inside
            || edge1.position == geode::POSITION::parallel )
        {
            return false;
        }
        return test_vertex_mode( mesh, edge0, edge1 );
    }

    bool are_equal( const geode::SurfaceMesh3D& mesh,
        const PolygonHit& hit0,
        const PolygonHit& hit1 )
    {
        if( std::fabs( hit0.distance.distance - hit1.distance.distance )
            > geode::GLOBAL_EPSILON )
        {
            return false;
        }
        const auto& feature0 = hit0.feature;
        const auto& feature1 = hit1.feature;
        if( feature0.vertex != geode::NO_LID
            && feature1.vertex != geode::NO_LID )
        {
            return mesh.polygon_vertex(
                       { hit0.distance.polygon, feature0.vertex } )
                   == mesh.polygon_vertex(
                       { hit1.distance.polygon, feature1.vertex } );
        }
        if( feature0.edge != geode::NO_LID && feature1.edge != geode::NO_LID )
        {
            return mesh.polygon_adjacent_edge(
                       { hit0.distance.polygon, feature0.edge } )
                   == geode::PolygonEdge{ hit1.distance.polygon,
                          feature1.edge };
        }
        return false;
    }
} // namespace

namespace geode
{
    class RayTracing2D::Impl
    {
    public:
        Impl( const EdgedCurve2D& mesh, const Ray2D& ray )
            : mesh_( mesh ),
              origin_( ray.origin() ),
              segment_{ ray.origin(), end( mesh.bounding_box(), ray ) }
        {
        }

        Impl( const EdgedCurve2D& mesh,
            const BoundingBox2D& bbox,
            const Ray2D& ray )
            : mesh_( mesh ),
              origin_( ray.origin() ),
              segment_{ ray.origin(), end( bbox, ray ) }
        {
        }

        Impl( const EdgedCurve2D& mesh,
            const BoundingBox2D& bbox,
            const InfiniteLine2D& infinite_line )
            : mesh_( mesh ),
              origin_( infinite_line.origin() ),
              segment_{ begin( bbox, infinite_line ),
                  end( bbox, infinite_line ) }
        {
        }

        Impl( const EdgedCurve2D& mesh,
            const Point2D& origin,
            OwnerSegment2D segment )
            : mesh_( mesh ), origin_( origin ), segment_{ std::move( segment ) }
        {
        }

        Impl( const EdgedCurve2D& mesh, const InfiniteLine2D& infinite_line )
            : mesh_( mesh ),
              origin_( infinite_line.origin() ),
              segment_{ begin( mesh.bounding_box(), infinite_line ),
                  end( mesh.bounding_box(), infinite_line ) }
        {
        }

        std::optional< EdgeDistance > closest_edge() const
        {
            if( results_.empty() )
            {
                return std::nullopt;
            }
            sort_results();
            return results_.front();
        }

        std::optional< absl::FixedArray< RayTracing2D::EdgeDistance > >
            closest_edges( index_t size ) const
        {
            if( results_.empty() )
            {
                return std::nullopt;
            }
            sort_results();
            std::optional< absl::FixedArray< RayTracing2D::EdgeDistance > >
                closest_edges{ std::min(
                    size, static_cast< index_t >( results_.size() ) ) };
            for( const auto i : Indices{ closest_edges.value() } )
            {
                closest_edges->at( i ) = results_[i];
            }
            return closest_edges;
        }

        std::vector< EdgeDistance > all_intersections() const
        {
            if( results_.empty() )
            {
                return {};
            }
            sort_results();
            return results_;
        }

        bool compute( index_t edge_id )
        {
            const auto segment = mesh_.segment( edge_id );
            const auto result = segment_segment_intersection_detection< 2 >(
                segment_, segment );
            if( result.first == POSITION::outside )
            {
                return false;
            }
            if( auto intersection =
                    segment_segment_intersection( segment_, segment ) )
            {
                auto& intersection_result = intersection.result.value();
                auto distance =
                    point_point_distance( origin_, intersection_result );
                if( Vector2D{ origin_, intersection_result }.dot(
                        segment_.direction() )
                    < 0 )
                {
                    distance *= -1.;
                }
                results_.emplace_back(
                    edge_id, distance, result.second, intersection_result );
            }
            return false;
        }

    private:
        void sort_results() const
        {
            if( are_results_sorted_ )
            {
                return;
            }
            absl::c_sort( results_ );
            const auto last = std::unique( results_.begin(), results_.end(),
                [this]( const EdgeDistance& edge0, const EdgeDistance& edge1 ) {
                    return are_equal( this->mesh_, edge0, edge1 );
                } );
            results_.erase( last, results_.end() );
            are_results_sorted_ = true;
        }

    private:
        const EdgedCurve2D& mesh_;
        Point2D origin_;
        OwnerSegment2D segment_;
        mutable std::vector< EdgeDistance > results_;
        mutable bool are_results_sorted_{ false };
    };

    RayTracing2D::RayTracing2D(
        const EdgedCurve2D& mesh, const BoundingBox2D& bbox, const Ray2D& ray )
        : impl_{ mesh, bbox, ray }
    {
    }

    RayTracing2D::RayTracing2D( const EdgedCurve2D& mesh,
        const BoundingBox2D& bbox,
        const InfiniteLine2D& infinite_line )
        : impl_{ mesh, bbox, infinite_line }
    {
    }

    RayTracing2D::RayTracing2D( const EdgedCurve2D& mesh,
        const Point2D& origin,
        OwnerSegment2D segment )
        : impl_{ mesh, origin, std::move( segment ) }
    {
    }

    RayTracing2D::RayTracing2D( RayTracing2D&& ) noexcept = default;

    RayTracing2D& RayTracing2D::operator=( RayTracing2D&& ) noexcept = default;

    RayTracing2D::~RayTracing2D() = default;

    std::optional< RayTracing2D::EdgeDistance >
        RayTracing2D::closest_edge() const
    {
        return impl_->closest_edge();
    }

    std::optional< absl::FixedArray< RayTracing2D::EdgeDistance > >
        RayTracing2D::closest_edges( index_t size ) const
    {
        return impl_->closest_edges( size );
    }

    std::vector< RayTracing2D::EdgeDistance >
        RayTracing2D::all_intersections() const
    {
        return impl_->all_intersections();
    }

    bool RayTracing2D::operator()( index_t edge_id )
    {
        return impl_->compute( edge_id );
    }

    class RayTracing3D::Impl
    {
    public:
        Impl( const SurfaceMesh3D& mesh, const Ray3D& ray )
            : mesh_( mesh ),
              origin_( ray.origin() ),
              segment_{ ray.origin(), end( mesh.bounding_box(), ray ) }
        {
        }

        Impl( const SurfaceMesh3D& mesh, const InfiniteLine3D& infinite_line )
            : mesh_( mesh ),
              origin_( infinite_line.origin() ),
              segment_{ begin( mesh.bounding_box(), infinite_line ),
                  end( mesh.bounding_box(), infinite_line ) }
        {
        }

        Impl( const SurfaceMesh3D& mesh,
            const BoundingBox3D& bbox,
            const Ray3D& ray )
            : mesh_( mesh ),
              origin_( ray.origin() ),
              segment_{ ray.origin(), end( bbox, ray ) }
        {
        }

        Impl( const SurfaceMesh3D& mesh,
            const BoundingBox3D& bbox,
            const InfiniteLine3D& infinite_line )
            : mesh_( mesh ),
              origin_( infinite_line.origin() ),
              segment_{ begin( bbox, infinite_line ),
                  end( bbox, infinite_line ) }
        {
        }

        Impl( const SurfaceMesh3D& mesh,
            const Point3D& origin,
            OwnerSegment3D segment )
            : mesh_( mesh ), origin_( origin ), segment_{ std::move( segment ) }
        {
        }

        std::optional< PolygonDistance > closest_polygon() const
        {
            if( results_.empty() )
            {
                return std::nullopt;
            }
            sort_results();
            return results_.front().distance;
        }

        std::optional< absl::FixedArray< RayTracing3D::PolygonDistance > >
            closest_polygons( index_t size ) const
        {
            if( results_.empty() )
            {
                return std::nullopt;
            }
            sort_results();
            std::optional< absl::FixedArray< RayTracing3D::PolygonDistance > >
                closest_polygons{ std::min(
                    size, static_cast< index_t >( results_.size() ) ) };
            for( const auto i : Indices{ closest_polygons.value() } )
            {
                closest_polygons->at( i ) = results_[i].distance;
            }
            return closest_polygons;
        }

        std::vector< PolygonDistance > all_intersections() const
        {
            if( results_.empty() )
            {
                return {};
            }
            sort_results();
            std::vector< PolygonDistance > intersections;
            intersections.reserve( results_.size() );
            for( const auto& result : results_ )
            {
                intersections.push_back( result.distance );
            }
            return intersections;
        }

        bool compute( index_t polygon_id )
        {
            const auto& point0 =
                mesh_.point( mesh_.polygon_vertex( { polygon_id, 0 } ) );
            for( const auto edge :
                LRange{ 1, mesh_.nb_polygon_edges( polygon_id ) - 1 } )
            {
                const auto edge_vertices =
                    mesh_.polygon_edge_vertices( { polygon_id, edge } );
                const Triangle3D triangle{ point0,
                    mesh_.point( edge_vertices.front() ),
                    mesh_.point( edge_vertices.back() ) };
                const auto result = segment_triangle_intersection_detection(
                    segment_, triangle );
                if( result.first == POSITION::outside )
                {
                    continue;
                }
                if( auto intersection =
                        segment_triangle_intersection( segment_, triangle ) )
                {
                    auto& intersection_result = intersection.result.value();
                    auto distance =
                        point_point_distance( origin_, intersection_result );
                    if( Vector3D{ origin_, intersection_result }.dot(
                            segment_.direction() )
                        < 0 )
                    {
                        distance *= -1.;
                    }
                    add_result( polygon_id, distance, result.second, edge,
                        intersection_result );
                    break;
                }
                for( const auto edge2 : LRange{ 3 } )
                {
                    auto [ray_edge_distance, ray_point,
                        _] = segment_segment_distance( Segment3D{ segment_ },
                        { triangle.vertices()[edge2].get(),
                            triangle.vertices()[( edge2 + 1 ) % 3].get() } );
                    if( ray_edge_distance > GLOBAL_EPSILON )
                    {
                        continue;
                    }
                    const Vector3D origin_target{ origin_, ray_point };
                    auto distance = origin_target.length();
                    if( origin_target.dot( segment_.direction() ) < 0 )
                    {
                        distance *= -1.;
                    }
                    add_result(
                        polygon_id, distance, result.second, edge, ray_point );
                }
                const auto [triangle_distance, ray_point, _] =
                    segment_triangle_distance( segment_, triangle );
                if( triangle_distance > GLOBAL_EPSILON )
                {
                    continue;
                }
                const Vector3D origin_target{ origin_, ray_point };
                auto distance = origin_target.length();
                if( origin_target.dot( segment_.direction() ) < 0 )
                {
                    distance *= -1.;
                }
                add_result(
                    polygon_id, distance, result.second, edge, ray_point );
                break;
            }
            return false;
        }

    private:
        void add_result( index_t polygon_id,
            double distance,
            POSITION fan_position,
            local_index_t fan_edge,
            const Point3D& point )
        {
            const auto [position, feature] = polygon_position( fan_position,
                fan_edge, mesh_.nb_polygon_vertices( polygon_id ) );
            results_.push_back(
                { { polygon_id, distance, position, point }, feature } );
        }

        void sort_results() const
        {
            if( are_results_sorted_ )
            {
                return;
            }
            absl::c_sort( results_ );
            const auto last = std::unique( results_.begin(), results_.end(),
                [this]( const PolygonHit& hit0, const PolygonHit& hit1 ) {
                    return are_equal( this->mesh_, hit0, hit1 );
                } );
            results_.erase( last, results_.end() );
            are_results_sorted_ = true;
        }

    private:
        const SurfaceMesh3D& mesh_;
        Point3D origin_;
        OwnerSegment3D segment_;
        mutable std::vector< PolygonHit > results_;
        mutable bool are_results_sorted_{ false };
    };

    RayTracing3D::RayTracing3D(
        const SurfaceMesh3D& mesh, const BoundingBox3D& bbox, const Ray3D& ray )
        : impl_{ mesh, bbox, ray }
    {
    }

    RayTracing3D::RayTracing3D( const SurfaceMesh3D& mesh,
        const BoundingBox3D& bbox,
        const InfiniteLine3D& infinite_line )
        : impl_{ mesh, bbox, infinite_line }
    {
    }

    RayTracing3D::RayTracing3D( const SurfaceMesh3D& mesh,
        const Point3D& origin,
        OwnerSegment3D segment )
        : impl_{ mesh, origin, std::move( segment ) }
    {
    }

    RayTracing3D::RayTracing3D( RayTracing3D&& ) noexcept = default;

    RayTracing3D& RayTracing3D::operator=( RayTracing3D&& ) noexcept = default;

    RayTracing3D::~RayTracing3D() = default;

    std::optional< RayTracing3D::PolygonDistance >
        RayTracing3D::closest_polygon() const
    {
        return impl_->closest_polygon();
    }

    std::optional< absl::FixedArray< RayTracing3D::PolygonDistance > >
        RayTracing3D::closest_polygons( index_t size ) const
    {
        return impl_->closest_polygons( size );
    }

    std::vector< RayTracing3D::PolygonDistance >
        RayTracing3D::all_intersections() const
    {
        return impl_->all_intersections();
    }

    bool RayTracing3D::operator()( index_t polygon_id )
    {
        return impl_->compute( polygon_id );
    }
} // namespace geode
