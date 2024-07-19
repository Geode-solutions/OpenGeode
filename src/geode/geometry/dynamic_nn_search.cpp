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

#include <geode/geometry/dynamic_nn_search.hpp>

#include <numeric>

#include <absl/algorithm/container.h>

#include <async++.h>

#include <nanoflann.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class DynamicNNSearch< dimension >::Impl
    {
    public:
        explicit Impl( std::vector< Point< dimension > > points )
            : cloud_{ std::move( points ) },
              dynamic_nn_tree_{ dimension, cloud_ }
        {
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
            nanoflann::RadiusResultSet< double, index_t > resultSet(
                threshold_distance * threshold_distance, results );
            dynamic_nn_tree_.findNeighbors( resultSet, &copy( point )[0] );
            std::sort(
                results.begin(), results.end(), nanoflann::IndexDist_Sorter() );
            std::vector< index_t > indices;
            indices.reserve( results.size() );
            for( const auto& result : results )
            {
                indices.emplace_back( result.first );
            }
            return indices;
        }

        void add_point( const Point< dimension >& point )
        {
            const auto nb_points = cloud_.kdtree_get_point_count();
            cloud_.points.emplace_back( point );
            dynamic_nn_tree_.addPoints( nb_points, nb_points );
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
        PointCloud cloud_;
        nanoflann::KDTreeSingleIndexDynamicAdaptor<
            nanoflann::L2_Simple_Adaptor< double, PointCloud >,
            PointCloud,
            dimension,
            index_t >
            dynamic_nn_tree_;
    };

    template < index_t dimension >
    DynamicNNSearch< dimension >::DynamicNNSearch(
        std::vector< Point< dimension > > points )
        : impl_( std::move( points ) )
    {
    }

    template < index_t dimension >
    DynamicNNSearch< dimension >::DynamicNNSearch(
        DynamicNNSearch&& ) noexcept = default;

    template < index_t dimension >
    DynamicNNSearch< dimension >::~DynamicNNSearch() = default;

    template < index_t dimension >
    const Point< dimension >& DynamicNNSearch< dimension >::point(
        index_t index ) const
    {
        return impl_->point( index );
    }

    template < index_t dimension >
    index_t DynamicNNSearch< dimension >::nb_points() const
    {
        return impl_->nb_points();
    }

    template < index_t dimension >
    std::vector< index_t > DynamicNNSearch< dimension >::radius_neighbors(
        const Point< dimension >& point, double threshold_distance ) const
    {
        return impl_->radius_neighbors( point, threshold_distance );
    }

    template < index_t dimension >
    void DynamicNNSearch< dimension >::add_point(
        const Point< dimension >& point )
    {
        return impl_->add_point( point );
    }

    template class opengeode_geometry_api DynamicNNSearch< 2 >;
    template class opengeode_geometry_api DynamicNNSearch< 3 >;
} // namespace geode
