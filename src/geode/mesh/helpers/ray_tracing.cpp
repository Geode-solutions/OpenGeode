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

#include <geode/mesh/helpers/ray_tracing.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/bounding_box.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/information.h>
#include <geode/geometry/intersection.h>
#include <geode/geometry/intersection_detection.h>

#include <geode/mesh/core/surface_mesh.h>

namespace
{
    template < typename Line >
    geode::Point3D begin( const geode::SurfaceMesh3D& mesh, const Line& line )
    {
        auto bbox = mesh.bounding_box();
        bbox.add_point( line.origin() );
        const auto diagonal = bbox.diagonal();
        return line.origin() - line.direction() * diagonal.length();
    }

    template < typename Line >
    geode::Point3D end( const geode::SurfaceMesh3D& mesh, const Line& line )
    {
        auto bbox = mesh.bounding_box();
        bbox.add_point( line.origin() );
        const auto diagonal = bbox.diagonal();
        return line.origin() + line.direction() * diagonal.length();
    }

    bool test_vertex_mode( const geode::SurfaceMesh3D& mesh,
        const geode::RayTracing3D::PolygonDistance& polygon0,
        const geode::RayTracing3D::PolygonDistance& polygon1 )
    {
        geode::PolygonVertex polygon_vertex0{ polygon0.polygon, geode::NO_LID };
        if( polygon0.position == geode::Position::vertex0 )
        {
            polygon_vertex0.vertex_id = 0;
        }
        else if( polygon0.position == geode::Position::vertex1 )
        {
            polygon_vertex0.vertex_id = 1;
        }
        else if( polygon0.position == geode::Position::vertex2 )
        {
            polygon_vertex0.vertex_id = 2;
        }
        else
        {
            return false;
        }
        geode::PolygonVertex polygon_vertex1{ polygon1.polygon, geode::NO_LID };
        if( polygon1.position == geode::Position::vertex0 )
        {
            polygon_vertex1.vertex_id = 0;
        }
        else if( polygon1.position == geode::Position::vertex1 )
        {
            polygon_vertex1.vertex_id = 1;
        }
        else if( polygon1.position == geode::Position::vertex2 )
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
        if( polygon0.position == geode::Position::edge0 )
        {
            polygon_edge0.edge_id = 0;
        }
        else if( polygon0.position == geode::Position::edge1 )
        {
            polygon_edge0.edge_id = 1;
        }
        else if( polygon0.position == geode::Position::edge2 )
        {
            polygon_edge0.edge_id = 2;
        }
        else
        {
            return false;
        }
        geode::PolygonEdge polygon_edge1{ polygon1.polygon, geode::NO_LID };
        if( polygon1.position == geode::Position::edge0 )
        {
            polygon_edge1.edge_id = 0;
        }
        else if( polygon1.position == geode::Position::edge1 )
        {
            polygon_edge1.edge_id = 1;
        }
        else if( polygon1.position == geode::Position::edge2 )
        {
            polygon_edge1.edge_id = 2;
        }
        else
        {
            return false;
        }
        return mesh.polygon_adjacent_edge( polygon_edge0 ) == polygon_edge1;
    }

    bool are_equal( const geode::SurfaceMesh3D& mesh,
        const geode::RayTracing3D::PolygonDistance& polygon0,
        const geode::RayTracing3D::PolygonDistance& polygon1 )
    {
        if( std::fabs( polygon0.distance - polygon1.distance )
            > geode::global_epsilon )
        {
            return false;
        }
        if( polygon0.position == geode::Position::inside
            || polygon0.position == geode::Position::parallel )
        {
            return false;
        }
        if( polygon1.position == geode::Position::inside
            || polygon1.position == geode::Position::parallel )
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

        absl::optional< PolygonDistance > closest_polygon() const
        {
            if( results_.empty() )
            {
                return absl::nullopt;
            }
            sort_results();
            return results_.front();
        }

        absl::optional< absl::FixedArray< RayTracing3D::PolygonDistance > >
            closest_polygons( index_t size ) const
        {
            if( results_.empty() )
            {
                return absl::nullopt;
            }
            sort_results();
            absl::FixedArray< RayTracing3D::PolygonDistance > closest_polygons{
                std::min( size, static_cast< index_t >( results_.size() ) )
            };
            for( const auto i : Indices( closest_polygons ) )
            {
                closest_polygons[i] = results_[i];
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

        void compute( index_t polygon_id )
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
                if( result.first == Position::outside )
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
                    results_.emplace_back( polygon_id, distance, result.second,
                        std::move( intersection_result ) );
                }
                else
                {
                    for( const auto e2 : LRange{ 3 } )
                    {
                        auto point = std::get< 1 >( segment_segment_distance(
                            Segment3D{ segment_ },
                            { triangle.vertices()[e2].get(),
                                triangle.vertices()[( e2 + 1 ) % 3].get() } ) );
                        auto distance = point_point_distance( origin_, point );
                        if( Vector3D{ origin_, point }.dot(
                                segment_.direction() )
                            < 0 )
                        {
                            distance *= -1.;
                        }
                        results_.emplace_back( polygon_id, distance,
                            result.second, std::move( point ) );
                    }
                }
                break;
            }
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

    RayTracing3D::RayTracing3D( RayTracing3D&& other ) : impl_{ *other.impl_ }
    {
    }

    RayTracing3D::~RayTracing3D() {}

    absl::optional< RayTracing3D::PolygonDistance >
        RayTracing3D::closest_polygon() const
    {
        return impl_->closest_polygon();
    }

    absl::optional< absl::FixedArray< RayTracing3D::PolygonDistance > >
        RayTracing3D::closest_polygons( index_t size ) const
    {
        return impl_->closest_polygons( size );
    }

    std::vector< RayTracing3D::PolygonDistance >
        RayTracing3D::all_intersections() const
    {
        return impl_->all_intersections();
    }

    void RayTracing3D::operator()( index_t polygon_id )
    {
        impl_->compute( polygon_id );
    }
} // namespace geode
