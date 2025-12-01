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
    template < typename Mesh, typename Line >
    geode::Point< Mesh::dim > begin( const Mesh& mesh, const Line& line )
    {
        auto bbox = mesh.bounding_box();
        bbox.add_point( line.origin() );
        const auto diagonal = bbox.diagonal();
        return line.origin() - line.direction() * diagonal.length();
    }

    template < typename Mesh, typename Line >
    geode::Point< Mesh::dim > end( const Mesh& mesh, const Line& line )
    {
        auto bbox = mesh.bounding_box();
        bbox.add_point( line.origin() );
        const auto diagonal = bbox.diagonal();
        return line.origin() + line.direction() * diagonal.length();
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

    bool test_vertex_mode( const geode::SurfaceMesh3D& mesh,
        const geode::RayTracing3D::PolygonDistance& polygon0,
        const geode::RayTracing3D::PolygonDistance& polygon1 )
    {
        geode::PolygonVertex polygon_vertex0{ polygon0.polygon, geode::NO_LID };
        if( polygon0.position == geode::POSITION::vertex0 )
        {
            polygon_vertex0.vertex_id = 0;
        }
        else if( polygon0.position == geode::POSITION::vertex1 )
        {
            polygon_vertex0.vertex_id = 1;
        }
        else if( polygon0.position == geode::POSITION::vertex2 )
        {
            polygon_vertex0.vertex_id = 2;
        }
        else
        {
            return false;
        }
        geode::PolygonVertex polygon_vertex1{ polygon1.polygon, geode::NO_LID };
        if( polygon1.position == geode::POSITION::vertex0 )
        {
            polygon_vertex1.vertex_id = 0;
        }
        else if( polygon1.position == geode::POSITION::vertex1 )
        {
            polygon_vertex1.vertex_id = 1;
        }
        else if( polygon1.position == geode::POSITION::vertex2 )
        {
            polygon_vertex1.vertex_id = 2;
        }
        else
        {
            return false;
        }
        return mesh.polygon_vertex( polygon_vertex0 )
               == mesh.polygon_vertex( polygon_vertex1 );
    }

    bool test_edge_mode( const geode::SurfaceMesh3D& mesh,
        const geode::RayTracing3D::PolygonDistance& polygon0,
        const geode::RayTracing3D::PolygonDistance& polygon1 )
    {
        geode::PolygonEdge polygon_edge0{ polygon0.polygon, geode::NO_LID };
        if( polygon0.position == geode::POSITION::edge0 )
        {
            polygon_edge0.edge_id = 0;
        }
        else if( polygon0.position == geode::POSITION::edge1 )
        {
            polygon_edge0.edge_id = 1;
        }
        else if( polygon0.position == geode::POSITION::edge2 )
        {
            polygon_edge0.edge_id = 2;
        }
        else
        {
            return false;
        }
        geode::PolygonEdge polygon_edge1{ polygon1.polygon, geode::NO_LID };
        if( polygon1.position == geode::POSITION::edge0 )
        {
            polygon_edge1.edge_id = 0;
        }
        else if( polygon1.position == geode::POSITION::edge1 )
        {
            polygon_edge1.edge_id = 1;
        }
        else if( polygon1.position == geode::POSITION::edge2 )
        {
            polygon_edge1.edge_id = 2;
        }
        else
        {
            return false;
        }
        return mesh.polygon_adjacent_edge( polygon_edge0 ) == polygon_edge1;
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
        const geode::RayTracing3D::PolygonDistance& polygon0,
        const geode::RayTracing3D::PolygonDistance& polygon1 )
    {
        if( std::fabs( polygon0.distance - polygon1.distance )
            > geode::GLOBAL_EPSILON )
        {
            return false;
        }
        if( polygon0.position == geode::POSITION::inside
            || polygon0.position == geode::POSITION::parallel )
        {
            return false;
        }
        if( polygon1.position == geode::POSITION::inside
            || polygon1.position == geode::POSITION::parallel )
        {
            return false;
        }
        if( test_vertex_mode( mesh, polygon0, polygon1 ) )
        {
            return true;
        }
        return test_edge_mode( mesh, polygon0, polygon1 );
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
              segment_{ ray.origin(), end( mesh, ray ) }
        {
        }

        Impl( const EdgedCurve2D& mesh,
            const Point2D& origin,
            const OwnerSegment2D& segment )
            : mesh_( mesh ), origin_( origin ), segment_{ segment }
        {
        }

        Impl( const EdgedCurve2D& mesh, const InfiniteLine2D& infinite_line )
            : mesh_( mesh ),
              origin_( infinite_line.origin() ),
              segment_{ begin( mesh, infinite_line ),
                  end( mesh, infinite_line ) }
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
            const auto result =
                segment_segment_intersection_detection( segment_, segment );
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
                std::lock_guard< std::mutex > lock{ mutex_ };
                results_.emplace_back( edge_id, distance, result.second,
                    std::move( intersection_result ) );
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
        const Point2D& origin_;
        DEBUG_CONST OwnerSegment2D segment_;
        mutable std::vector< EdgeDistance > results_;
        mutable bool are_results_sorted_{ false };
        std::mutex mutex_;
    };

    RayTracing2D::RayTracing2D( const EdgedCurve2D& mesh, const Ray2D& ray )
        : impl_{ mesh, ray }
    {
    }

    RayTracing2D::RayTracing2D( const EdgedCurve2D& mesh,
        const Point2D& origin,
        const OwnerSegment2D& segment )
        : impl_{ mesh, origin, segment }
    {
    }

    RayTracing2D::RayTracing2D(
        const EdgedCurve2D& mesh, const InfiniteLine2D& infinite_line )
        : impl_{ mesh, infinite_line }
    {
    }

    RayTracing2D::RayTracing2D( RayTracing2D&& ) noexcept = default;

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
              segment_{ ray.origin(), end( mesh, ray ) }
        {
        }

        Impl( const SurfaceMesh3D& mesh, const InfiniteLine3D& infinite_line )
            : mesh_( mesh ),
              origin_( infinite_line.origin() ),
              segment_{ begin( mesh, infinite_line ),
                  end( mesh, infinite_line ) }
        {
        }

        Impl( const SurfaceMesh3D& mesh,
            const Point3D& origin,
            const OwnerSegment3D& segment )
            : mesh_( mesh ), origin_( origin ), segment_{ segment }
        {
        }

        std::optional< PolygonDistance > closest_polygon() const
        {
            if( results_.empty() )
            {
                return std::nullopt;
            }
            sort_results();
            return results_.front();
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
                closest_polygons->at( i ) = results_[i];
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
            return results_;
        }

        bool compute( index_t polygon_id )
        {
            const auto& p0 =
                mesh_.point( mesh_.polygon_vertex( { polygon_id, 0 } ) );
            for( const auto e :
                LRange{ 1, mesh_.nb_polygon_edges( polygon_id ) - 1 } )
            {
                const auto edge_vertices =
                    mesh_.polygon_edge_vertices( { polygon_id, e } );
                const Triangle3D triangle{ p0,
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
                    std::lock_guard< std::mutex > lock{ mutex_ };
                    results_.emplace_back( polygon_id, distance, result.second,
                        std::move( intersection_result ) );
                    break;
                }
                for( const auto e2 : LRange{ 3 } )
                {
                    auto [ray_edge_distance, point, _] =
                        segment_segment_distance( Segment3D{ segment_ },
                            { triangle.vertices()[e2].get(),
                                triangle.vertices()[( e2 + 1 ) % 3].get() } );
                    if( ray_edge_distance > GLOBAL_EPSILON )
                    {
                        continue;
                    }
                    if( Vector3D{ origin_, point }.dot( segment_.direction() )
                        < 0 )
                    {
                        ray_edge_distance *= -1.;
                    }
                    std::lock_guard< std::mutex > lock{ mutex_ };
                    results_.emplace_back( polygon_id, ray_edge_distance,
                        result.second, std::move( point ) );
                }
                const auto [distance, __, triangle_point] =
                    segment_triangle_distance( segment_, triangle );
                if( distance < GLOBAL_EPSILON )
                {
                    std::lock_guard< std::mutex > lock{ mutex_ };
                    results_.emplace_back(
                        polygon_id, distance, result.second, triangle_point );
                }
                break;
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
                [this]( const PolygonDistance& polygon0,
                    const PolygonDistance& polygon1 ) {
                    return are_equal( this->mesh_, polygon0, polygon1 );
                } );
            results_.erase( last, results_.end() );
            are_results_sorted_ = true;
        }

    private:
        const SurfaceMesh3D& mesh_;
        const Point3D& origin_;
        DEBUG_CONST OwnerSegment3D segment_;
        mutable std::vector< PolygonDistance > results_;
        mutable bool are_results_sorted_{ false };
        std::mutex mutex_;
    };

    RayTracing3D::RayTracing3D( const SurfaceMesh3D& mesh, const Ray3D& ray )
        : impl_{ mesh, ray }
    {
    }

    RayTracing3D::RayTracing3D(
        const SurfaceMesh3D& mesh, const InfiniteLine3D& infinite_line )
        : impl_{ mesh, infinite_line }
    {
    }

    RayTracing3D::RayTracing3D( const SurfaceMesh3D& mesh,
        const Point3D& origin,
        const OwnerSegment3D& segment )
        : impl_{ mesh, origin, segment }
    {
    }

    RayTracing3D::RayTracing3D( RayTracing3D&& ) noexcept = default;

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
