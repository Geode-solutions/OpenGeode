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

#include <geode/geometry/nn_search.hpp>

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/intersection.hpp>

#include <mutex>
#include <numeric>

#include <absl/algorithm/container.h>

#include <async++.h>

#include <nanoflann.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class NNSearch< dimension >::Impl
    {
    public:
        explicit Impl( std::vector< Point< dimension > > points )
            : cloud_{ std::move( points ) }, nn_tree_{ dimension, cloud_ }
        {
            nn_tree_.buildIndex();
        }

        const Point< dimension >& point( const index_t index ) const
        {
            return cloud_.points.at( index );
        }

        index_t nb_points() const
        {
            return cloud_.kdtree_get_point_count();
        }

        std::vector< index_t > radius_neighbors(
            const Point< dimension >& point,
            const double threshold_distance ) const
        {
            std::vector< nanoflann::ResultItem< index_t, double > > results;
            nanoflann::SearchParameters params;
            params.sorted = true;
            const auto nb_results = nn_tree_.radiusSearch( &copy( point )[0],
                threshold_distance * threshold_distance, results, params );
            std::vector< index_t > indices;
            indices.reserve( nb_results );
            for( auto&& result : results )
            {
                indices.emplace_back( result.first );
            }
            return indices;
        }

        std::vector< index_t > frame_neighbors( const Point< dimension >& point,
            const Frame< dimension >& epsilons_frame,
            const double factor_distance ) const
        {
            std::vector< nanoflann::ResultItem< index_t, double > > results;
            nanoflann::SearchParameters params;
            params.sorted = true;
            const auto max_elongation_direction =
                epsilons_frame.max_elongation_direction();
            const auto max_elongation =
                epsilons_frame.direction( max_elongation_direction ).length();
            const auto radius = factor_distance * factor_distance
                                * max_elongation * max_elongation;
            const auto nb_results = nn_tree_.radiusSearch(
                &copy( point )[0], radius, results, params );
            std::vector< index_t > indices;
            indices.reserve( nb_results );
            for( auto&& result : results )
            {
                Segment< dimension > segment{ point,
                    this->point( result.first ) };
                if( segment_ellipse_intersection(
                        segment, Ellipse< dimension >{ point, epsilons_frame } )
                        .type
                    != INTERSECTION_TYPE::none )
                {
                    continue;
                }
                indices.emplace_back( result.first );
            }
            return indices;
        }

        std::vector< index_t > neighbors(
            const Point< dimension >& point, const index_t nb_neighbors ) const
        {
            std::vector< index_t > results( nb_neighbors );
            std::vector< double > distances( nb_neighbors );
            const auto new_nb_neighbors = nn_tree_.knnSearch(
                &copy( point )[0], nb_neighbors, &results[0], &distances[0] );
            results.resize( new_nb_neighbors );
            return results;
        }

        typename NNSearch< dimension >::ColocatedInfo colocated_index_mapping(
            const NNSearch< dimension >& nn_search, const double epsilon ) const
        {
            OPENGEODE_EXCEPTION( epsilon >= GLOBAL_EPSILON,
                "[NNSearch::colocated_index_mapping] Given epsilon too "
                "small, "
                "should be bigger than GLOBAL_EPSILON (i.e. ",
                GLOBAL_EPSILON, ")" );
            typename NNSearch< dimension >::ColocatedInfo result;
            const auto nb_points = nn_search.nb_points();
            std::vector< index_t > mapping( nb_points, NO_ID );
            std::mutex mutex;
            async::parallel_for( async::irange( index_t{ 0 }, nb_points ),
                [&nn_search, &epsilon, &mapping, &mutex, this](
                    index_t point_id ) {
                    if( mapping[point_id] != NO_ID )
                    {
                        return;
                    }
                    const auto vertices_around = nn_search.radius_neighbors(
                        nn_search.point( point_id ), epsilon );
                    std::lock_guard< std::mutex > lock( mutex );
                    if( mapping[point_id] != NO_ID )
                    {
                        Logger::trace( point_id,
                            " : correction 1 / mapping[p] ",
                            mapping[point_id] );
                        return;
                    }
                    for( const auto vertex_id : vertices_around )
                    {
                        Logger::trace( point_id, " : id ", vertex_id,
                            " / mapping[id] ", mapping[vertex_id] );
                        if( mapping[vertex_id] == NO_ID )
                        {
                            mapping[vertex_id] = point_id;
                        }
                    }
                } );
            result.colocated_input_points = mapping;
            index_t nb_unique_points{ 0 };
            for( const auto point_id : Range{ nb_points } )
            {
                if( mapping[point_id] == point_id )
                {
                    nb_unique_points++;
                }
            }
            result.unique_points.resize( nb_unique_points );
            std::vector< index_t > old2new( nb_points, NO_ID );
            index_t count{ 0 };
            for( const auto point_id : Range{ nb_points } )
            {
                if( mapping[point_id] == point_id )
                {
                    result.unique_points[count] = nn_search.point( point_id );
                    old2new[point_id] = count;
                    count++;
                }
            }
            for( const auto point_id : Range{ nb_points } )
            {
                mapping[point_id] = old2new[mapping[point_id]];
            }
            result.colocated_mapping = mapping;
            return result;
        }

        typename geode::NNSearch< dimension >::ColocatedInfo
            colocated_index_mapping(
                const geode::NNSearch< dimension >& nn_search,
                const Frame< dimension >& epsilons_frame,
                const double factor_distance ) const
        {
            for( const auto d : LRange{ dimension } )
            {
                OPENGEODE_EXCEPTION(
                    epsilons_frame.direction( d ).length() * factor_distance
                        >= GLOBAL_EPSILON,
                    "[NNSearch::colocated_index_mapping] Given epsilon too "
                    "small, "
                    "should be bigger than GLOBAL_EPSILON (i.e. ",
                    GLOBAL_EPSILON, ")" );
            }
            typename NNSearch< dimension >::ColocatedInfo result;
            const auto nb_points = nn_search.nb_points();
            std::vector< index_t > mapping( nb_points, NO_ID );
            std::mutex mutex;
            async::parallel_for( async::irange( index_t{ 0 }, nb_points ),
                [&nn_search, &epsilons_frame, &factor_distance, &mapping,
                    &mutex, this]( index_t point_id ) {
                    if( mapping[point_id] != NO_ID )
                    {
                        return;
                    }
                    const auto vertices_around =
                        frame_neighbors( nn_search.point( point_id ),
                            epsilons_frame, factor_distance );
                    std::lock_guard< std::mutex > lock( mutex );
                    if( mapping[point_id] != NO_ID )
                    {
                        Logger::trace( point_id,
                            " : correction 1 / mapping[p] ",
                            mapping[point_id] );
                        return;
                    }
                    for( const auto vertex_id : vertices_around )
                    {
                        Logger::trace( point_id, " : id ", vertex_id,
                            " / mapping[id] ", mapping[vertex_id] );
                        if( mapping[vertex_id] == NO_ID )
                        {
                            mapping[vertex_id] = point_id;
                        }
                    }
                } );
            result.colocated_input_points = mapping;
            index_t nb_unique_points{ 0 };
            for( const auto point_id : Range{ nb_points } )
            {
                if( mapping[point_id] == point_id )
                {
                    nb_unique_points++;
                }
            }
            result.unique_points.resize( nb_unique_points );
            std::vector< index_t > old2new( nb_points, NO_ID );
            index_t count{ 0 };
            for( const auto point_id : Range{ nb_points } )
            {
                if( mapping[point_id] == point_id )
                {
                    result.unique_points[count] = nn_search.point( point_id );
                    old2new[point_id] = count;
                    count++;
                }
            }
            for( const auto point_id : Range{ nb_points } )
            {
                mapping[point_id] = old2new[mapping[point_id]];
            }
            result.colocated_mapping = mapping;
            return result;
        }

    private:
        std::array< double, dimension > copy(
            const Point< dimension >& point ) const
        {
            std::array< double, dimension > result;
            for( const auto i : LRange{ dimension } )
            {
                result[i] = point.value( i );
            }
            return result;
        }
        struct PointCloud
        {
            std::vector< Point< dimension > > points;

            size_t kdtree_get_point_count() const
            {
                return points.size();
            }

            double kdtree_get_pt( size_t idx, size_t dim ) const
            {
                return points[idx].value( dim );
            }

            // Optional bounding-box computation: return false to default to a
            // standard bbox computation loop.
            //   Return true if the BBOX was already computed by the class and
            //   returned in "bb" so it can be avoided to redo it again. Look at
            //   bb.size() to find out the expected dimensionality (e.g. 2 or 3
            //   for point clouds)
            template < class BBOX >
            bool kdtree_get_bbox( BBOX& /* bb */ ) const
            {
                return false;
            }
        };

    private:
        const PointCloud cloud_;
        nanoflann::KDTreeSingleIndexAdaptor<
            nanoflann::L2_Simple_Adaptor< double, PointCloud >,
            PointCloud,
            dimension,
            index_t >
            nn_tree_;
    };

    template < index_t dimension >
    NNSearch< dimension >::NNSearch( std::vector< Point< dimension > > points )
        : impl_( std::move( points ) )
    {
    }

    template < index_t dimension >
    NNSearch< dimension >::NNSearch( NNSearch&& ) noexcept = default;

    template < index_t dimension >
    NNSearch< dimension >::~NNSearch() = default;

    template < index_t dimension >
    const Point< dimension >& NNSearch< dimension >::point(
        index_t index ) const
    {
        return impl_->point( index );
    }

    template < index_t dimension >
    index_t NNSearch< dimension >::nb_points() const
    {
        return impl_->nb_points();
    }

    template < index_t dimension >
    index_t NNSearch< dimension >::closest_neighbor(
        const Point< dimension >& point ) const
    {
        return impl_->neighbors( point, 1 ).front();
    }

    template < index_t dimension >
    std::vector< index_t > NNSearch< dimension >::radius_neighbors(
        const Point< dimension >& point, double threshold_distance ) const
    {
        return impl_->radius_neighbors( point, threshold_distance );
    }

    template < index_t dimension >
    std::vector< index_t > NNSearch< dimension >::frame_neighbors(
        const Point< dimension >& point,
        const Frame< dimension >& epsilons_frame,
        const double factor_distance ) const
    {
        return impl_->frame_neighbors( point, epsilons_frame, factor_distance );
    }

    template < index_t dimension >
    std::vector< index_t > NNSearch< dimension >::neighbors(
        const Point< dimension >& point, index_t nb_neighbors ) const
    {
        return impl_->neighbors( point, nb_neighbors );
    }

    template < geode::index_t dimension >
    typename geode::NNSearch< dimension >::ColocatedInfo
        NNSearch< dimension >::colocated_index_mapping(
            const double epsilon ) const
    {
        return impl_->colocated_index_mapping( *this, epsilon );
    }

    template < geode::index_t dimension >
    typename geode::NNSearch< dimension >::ColocatedInfo
        NNSearch< dimension >::colocated_index_mapping(
            const Frame< dimension >& epsilons_frame,
            const double factor_distance ) const
    {
        return impl_->colocated_index_mapping(
            *this, epsilons_frame, factor_distance );
    }

    template class opengeode_geometry_api NNSearch< 2 >;
    template class opengeode_geometry_api NNSearch< 3 >;
} // namespace geode
