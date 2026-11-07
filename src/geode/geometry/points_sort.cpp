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

#include <geode/geometry/points_sort.hpp>

#include <algorithm>
#include <array>
#include <limits>

#include <absl/algorithm/container.h>
#include <absl/container/inlined_vector.h>

#include <geode/geometry/point.hpp>

namespace
{
    // The sorts move copies of the points coordinates with their index
    // (instead of indices into the points) for memory locality. The
    // comparisons are the same, so the resulting order is the same.
    // Ties are broken by index: the comparators are strict total orders, so
    // the set of elements std::nth_element puts on each side of the middle is
    // unique and the recursion goes down to ranges of size <= 1. The order is
    // therefore the same whatever the standard library implementation.
    template < geode::index_t dimension >
    struct SortedPoint
    {
        std::array< double, dimension > coordinates;
        geode::index_t index;
    };

    template < geode::index_t dimension >
    using itr = typename std::vector< SortedPoint< dimension > >::iterator;

    template < geode::index_t dimension >
    std::vector< SortedPoint< dimension > > sorted_points(
        absl::Span< const geode::Point< dimension > > points )
    {
        std::vector< SortedPoint< dimension > > result( points.size() );
        for( const auto p : geode::Indices{ points } )
        {
            for( const auto d : geode::LRange{ dimension } )
            {
                result[p].coordinates[d] = points[p].value( d );
            }
            result[p].index = p;
        }
        return result;
    }

    template < geode::index_t dimension >
    std::vector< geode::index_t > sorted_indices(
        const std::vector< SortedPoint< dimension > >& sorted )
    {
        std::vector< geode::index_t > mapping( sorted.size() );
        for( const auto p : geode::Indices{ sorted } )
        {
            mapping[p] = sorted[p].index;
        }
        return mapping;
    }

    template < geode::index_t dimension >
    class Morton_cmp
    {
    public:
        explicit Morton_cmp( geode::local_index_t coord ) : coord_( coord ) {}

        bool operator()( const SortedPoint< dimension >& box1,
            const SortedPoint< dimension >& box2 ) const
        {
            const auto value1 = box1.coordinates[coord_];
            const auto value2 = box2.coordinates[coord_];
            // Bitwise operators on bool: branchless, faster in
            // std::nth_element than the logical ones (or a ternary)
            return ( value1 < value2 )
                   | ( ( value1 == value2 ) & ( box1.index < box2.index ) );
        }

    private:
        geode::local_index_t coord_;
    };
    ALIAS_1D_AND_2D_AND_3D( Morton_cmp );

    template < geode::index_t dimension >
    class Hilbert_cmp
    {
    public:
        explicit Hilbert_cmp( geode::local_index_t coord ) : coord_( coord ) {}

        bool operator()( const SortedPoint< dimension >& box1,
            const SortedPoint< dimension >& box2 ) const
        {
            const auto value1 = box1.coordinates[coord_];
            const auto value2 = box2.coordinates[coord_];
            // Bitwise operators on bool: branchless, faster in
            // std::nth_element than the logical ones (or a ternary)
            return ( value1 > value2 )
                   | ( ( value1 == value2 ) & ( box1.index < box2.index ) );
        }

    private:
        geode::local_index_t coord_;
    };
    ALIAS_1D_AND_2D_AND_3D( Hilbert_cmp );

    /**
     * \brief Splits a sequence into two ordered halves.
     * \details The algorithm shuffles the sequence and
     *  partitions its into two halves with the same number of elements
     *  and such that the elements of the first half are smaller
     *  than the elements of the second half.
     * \param[in] begin an iterator to the first element
     * \param[in] end an iterator one position past the last element
     * \param[in] cmp the comparator object
     * \return an iterator to the middle of the sequence that separates
     *  the two halves
     */
    template < typename Iterator, typename Compare >
    Iterator split_container(
        const Iterator& begin, const Iterator& end, const Compare& cmp )
    {
        if( begin >= end )
        {
            return begin;
        }
        const Iterator middle = begin + ( ( end - begin ) / 2 );
        std::nth_element( begin, middle, end, cmp );
        return middle;
    }

    /**
     * \brief Generic class for sorting arbitrary elements in Morton order.
     * \details The implementation is inspired by:
     *  - Christophe Delage and Olivier Devillers. Spatial Sorting.
     *   In CGAL User and Reference Manual. CGAL Editorial Board,
     *   3.9 edition, 2011
     */
    template < geode::local_index_t COORDX,
        template < geode::index_t > typename Comparator >
    void morton_mapping( const itr< 3 >& begin, const itr< 3 >& end );

    template < template < geode::index_t > typename Comparator >
    void morton_mapping_from_axis(
        geode::local_index_t axis, const itr< 3 >& begin, const itr< 3 >& end )
    {
        if( axis == 0 )
        {
            morton_mapping< 0, Comparator >( begin, end );
        }
        else if( axis == 1 )
        {
            morton_mapping< 1, Comparator >( begin, end );
        }
        else
        {
            morton_mapping< 2, Comparator >( begin, end );
        }
    }

    // Splitting along an axis where all points have (almost) the same
    // coordinate interleaves points that are far apart along the other
    // axes, which breaks the curve locality (e.g. flat 3D data).
    // Such axes are skipped and the range is split as in 2D/1D.
    template < template < geode::index_t > typename Comparator >
    bool morton_mapping_skipping_flat_axes(
        const std::array< geode::local_index_t, 3 >& axes,
        const itr< 3 >& begin,
        const itr< 3 >& end )
    {
        static constexpr double FLAT_RATIO{ 1e-3 };
        std::array< double, 3 > min;
        min.fill( std::numeric_limits< double >::max() );
        std::array< double, 3 > max;
        max.fill( std::numeric_limits< double >::lowest() );
        for( auto it = begin; it != end; ++it )
        {
            const auto& point = it->coordinates;
            for( const auto d : geode::LRange{ 3 } )
            {
                min[d] = std::min( min[d], point[d] );
                max[d] = std::max( max[d], point[d] );
            }
        }
        double max_extent{ 0 };
        for( const auto d : geode::LRange{ 3 } )
        {
            max_extent = std::max( max_extent, max[d] - min[d] );
        }
        if( max_extent == 0 )
        {
            // All points are colocated: order them by index so that the order
            // does not depend on the previous std::nth_element calls
            std::sort( begin, end,
                []( const SortedPoint< 3 >& point1,
                    const SortedPoint< 3 >& point2 ) {
                    return point1.index < point2.index;
                } );
            return true;
        }
        absl::InlinedVector< geode::local_index_t, 3 > kept_axes;
        for( const auto axis : axes )
        {
            if( max[axis] - min[axis] > FLAT_RATIO * max_extent )
            {
                kept_axes.push_back( axis );
            }
        }
        if( kept_axes.size() == 3 )
        {
            return false;
        }
        if( kept_axes.size() == 1 )
        {
            const Comparator< 3 > compX{ kept_axes[0] };
            const auto m1 = split_container( begin, end, compX );
            morton_mapping_from_axis< Comparator >( kept_axes[0], begin, m1 );
            morton_mapping_from_axis< Comparator >( kept_axes[0], m1, end );
            return true;
        }
        const Comparator< 3 > compX{ kept_axes[0] };
        const Comparator< 3 > compY{ kept_axes[1] };
        const auto m0 = begin;
        const auto m4 = end;
        const auto m2 = split_container( m0, m4, compX );
        const auto m1 = split_container( m0, m2, compY );
        const auto m3 = split_container( m2, m4, compY );
        morton_mapping_from_axis< Comparator >( kept_axes[1], m0, m1 );
        morton_mapping_from_axis< Comparator >( kept_axes[0], m1, m2 );
        morton_mapping_from_axis< Comparator >( kept_axes[0], m2, m3 );
        morton_mapping_from_axis< Comparator >( kept_axes[1], m3, m4 );
        return true;
    }

    template < geode::local_index_t COORDX,
        template < geode::index_t > typename Comparator >
    void morton_mapping( const itr< 3 >& begin, const itr< 3 >& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        constexpr geode::local_index_t COORDY = COORDX == 2 ? 0 : COORDX + 1;
        constexpr geode::local_index_t COORDZ = COORDY == 2 ? 0 : COORDY + 1;
        if( morton_mapping_skipping_flat_axes< Comparator >(
                { COORDX, COORDY, COORDZ }, begin, end ) )
        {
            return;
        }

        const Comparator< 3 > compX{ COORDX };
        const Comparator< 3 > compY{ COORDY };
        const Comparator< 3 > compZ{ COORDZ };

        const auto m0 = begin;
        const auto m8 = end;
        const auto m4 = split_container( m0, m8, compX );
        const auto m2 = split_container( m0, m4, compY );
        const auto m1 = split_container( m0, m2, compZ );
        const auto m3 = split_container( m2, m4, compZ );
        const auto m6 = split_container( m4, m8, compY );
        const auto m5 = split_container( m4, m6, compZ );
        const auto m7 = split_container( m6, m8, compZ );
        morton_mapping< COORDZ, Comparator >( m0, m1 );
        morton_mapping< COORDY, Comparator >( m1, m2 );
        morton_mapping< COORDY, Comparator >( m2, m3 );
        morton_mapping< COORDX, Comparator >( m3, m4 );
        morton_mapping< COORDX, Comparator >( m4, m5 );
        morton_mapping< COORDY, Comparator >( m5, m6 );
        morton_mapping< COORDY, Comparator >( m6, m7 );
        morton_mapping< COORDZ, Comparator >( m7, m8 );
    }

    template < geode::local_index_t COORDX,
        template < geode::index_t > typename Comparator >
    void morton_mapping( const itr< 2 >& begin, const itr< 2 >& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        constexpr geode::local_index_t COORDY = COORDX == 1 ? 0 : COORDX + 1;

        const Comparator< 2 > compX{ COORDX };
        const Comparator< 2 > compY{ COORDY };

        const auto m0 = begin;
        const auto m4 = end;
        const auto m2 = split_container( m0, m4, compX );
        const auto m1 = split_container( m0, m2, compY );
        const auto m3 = split_container( m2, m4, compY );
        morton_mapping< COORDY, Comparator >( m0, m1 );
        morton_mapping< COORDX, Comparator >( m1, m2 );
        morton_mapping< COORDX, Comparator >( m2, m3 );
        morton_mapping< COORDY, Comparator >( m3, m4 );
    }

    template < geode::local_index_t COORDX,
        template < geode::index_t > typename Comparator >
    void morton_mapping( const itr< 1 >& begin, const itr< 1 >& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        const Comparator< 1 > compX{ COORDX };

        const auto m0 = begin;
        const auto m2 = end;
        const auto m1 = split_container( m0, m2, compX );
        morton_mapping< COORDX, Comparator >( m0, m1 );
        morton_mapping< COORDX, Comparator >( m1, m2 );
    }

    /*
     * Return true if p0 < p1 comparing first X, then Y.
     */
    template < geode::index_t dimension >
    bool lexicographic_compare( const geode::Point< dimension >& p0,
        const geode::Point< dimension >& p1 )
    {
        for( const auto d : geode::LRange{ dimension - 1 } )
        {
            if( p0.value( d ) < p1.value( d ) )
            {
                return true;
            }
            if( p0.value( d ) > p1.value( d ) )
            {
                return false;
            }
        }
        return p0.value( dimension - 1 ) < p1.value( dimension - 1 );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::vector< index_t > lexicographic_mapping(
        absl::Span< const Point< dimension > > points )
    {
        std::vector< index_t > mapping( points.size() );
        absl::c_iota( mapping, index_t{ 0 } );
        // Stable sort: identical points stay ordered by index, whatever the
        // standard library implementation
        absl::c_stable_sort( mapping, [&points]( index_t i, index_t j ) {
            return lexicographic_compare( points[i], points[j] );
        } );
        return mapping;
    }

    template < index_t dimension >
    std::vector< index_t > morton_mapping(
        absl::Span< const Point< dimension > > points )
    {
        auto sorted = sorted_points( points );
        ::morton_mapping< 0_uc, Morton_cmp >( sorted.begin(), sorted.end() );
        return sorted_indices( sorted );
    }

    template < index_t dimension >
    std::vector< index_t > hilbert_mapping(
        absl::Span< const Point< dimension > > points )
    {
        auto sorted = sorted_points( points );
        ::morton_mapping< 0_uc, Hilbert_cmp >( sorted.begin(), sorted.end() );
        return sorted_indices( sorted );
    }

    template std::vector< index_t > opengeode_geometry_api
        lexicographic_mapping( absl::Span< const Point< 1 > > );
    template std::vector< index_t > opengeode_geometry_api
        lexicographic_mapping( absl::Span< const Point< 2 > > );
    template std::vector< index_t > opengeode_geometry_api
        lexicographic_mapping( absl::Span< const Point< 3 > > );

    template std::vector< index_t > opengeode_geometry_api morton_mapping(
        absl::Span< const Point< 1 > > );
    template std::vector< index_t > opengeode_geometry_api morton_mapping(
        absl::Span< const Point< 2 > > );
    template std::vector< index_t > opengeode_geometry_api morton_mapping(
        absl::Span< const Point< 3 > > );

    template std::vector< index_t > opengeode_geometry_api hilbert_mapping(
        absl::Span< const Point< 1 > > );
    template std::vector< index_t > opengeode_geometry_api hilbert_mapping(
        absl::Span< const Point< 2 > > );
    template std::vector< index_t > opengeode_geometry_api hilbert_mapping(
        absl::Span< const Point< 3 > > );
} // namespace geode
