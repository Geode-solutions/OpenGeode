/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/geometry/nn_search.h>

#include <numeric>

#include <absl/algorithm/container.h>

#include <async++.h>

#include <nanoflann.hpp>

#include <geode/basic/pimpl_impl.h>

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
            std::vector< std::pair< index_t, double > > results;
            const auto nb_results = nn_tree_.radiusSearch( &copy( point )[0],
                threshold_distance * threshold_distance, results, {} );
            std::vector< index_t > indices;
            indices.reserve( nb_results );
            for( auto&& result : results )
            {
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

    private:
        std::array< double, dimension > copy(
            const Point< dimension >& point ) const
        {
            std::array< double, dimension > result;
            for( const auto i : Range{ dimension } )
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
    NNSearch< dimension >::~NNSearch() // NOLINT
    {
    }

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
    std::vector< index_t > NNSearch< dimension >::neighbors(
        const Point< dimension >& point, index_t nb_neighbors ) const
    {
        return impl_->neighbors( point, nb_neighbors );
    }

    template < index_t dimension >
    typename NNSearch< dimension >::ColocatedInfo
        NNSearch< dimension >::colocated_index_mapping( double epsilon ) const
    {
        std::vector< index_t > mapping( nb_points() );
        absl::c_iota( mapping, 0 );
        async::parallel_for( async::irange( index_t{ 0 }, nb_points() ),
            [&epsilon, &mapping, this]( index_t p ) {
                if( mapping[p] == p )
                {
                    const auto vertices =
                        radius_neighbors( point( p ), epsilon );
                    const auto min_index = *absl::c_min_element( vertices );
                    for( const auto id : vertices )
                    {
                        mapping[id] = min_index;
                    }
                }
            } );
        index_t nb_colocated{ 0 };
        std::vector< Point< dimension > > unique_points;
        for( const auto p : Range{ nb_points() } )
        {
            if( mapping[p] == p )
            {
                mapping[p] -= nb_colocated;
                unique_points.push_back( point( p ) );
            }
            else
            {
                nb_colocated++;
                mapping[p] = mapping[mapping[p]];
            }
        }
        return { mapping, unique_points };
    }

    template class opengeode_geometry_api NNSearch< 2 >;
    template class opengeode_geometry_api NNSearch< 3 >;
} // namespace geode
