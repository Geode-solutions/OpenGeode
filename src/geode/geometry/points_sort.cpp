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
#include <limits>

#include <absl/algorithm/container.h>
#include <absl/container/inlined_vector.h>

#include <geode/geometry/point.hpp>

namespace
{
    using itr = std::vector< geode::index_t >::iterator;

    template < geode::index_t dimension >
    class Morton_cmp
    {
    public:
        Morton_cmp( absl::Span< const geode::Point< dimension > > points,
            geode::local_index_t coord )
            : points_( points ), coord_( coord )
        {
        }

        bool operator()( geode::index_t box1, geode::index_t box2 ) const
        {
            return points_[box1].value( coord_ )
                   < points_[box2].value( coord_ );
        }

    private:
        absl::Span< const geode::Point< dimension > > points_;
        geode::local_index_t coord_;
    };
    ALIAS_1D_AND_2D_AND_3D( Morton_cmp );

    template < geode::index_t dimension >
    class Hilbert_cmp
    {
    public:
        Hilbert_cmp( absl::Span< const geode::Point< dimension > > points,
            geode::local_index_t coord )
            : points_( points ), coord_( coord )
        {
        }

        bool operator()( geode::index_t box1, geode::index_t box2 ) const
        {
            return points_[box1].value( coord_ )
                   > points_[box2].value( coord_ );
        }

    private:
        absl::Span< const geode::Point< dimension > > points_;
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
    template < typename Compare >
    itr split_container( const itr& begin, const itr& end, const Compare& cmp )
    {
        if( begin >= end )
        {
            return begin;
        }
        const itr middle = begin + ( end - begin ) / 2;
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
    void morton_mapping( absl::Span< const geode::Point3D > points,
        const itr& begin,
        const itr& end );

    template < template < geode::index_t > typename Comparator >
    void morton_mapping_from_axis( geode::local_index_t axis,
        absl::Span< const geode::Point3D > points,
        const itr& begin,
        const itr& end )
    {
        if( axis == 0 )
        {
            morton_mapping< 0, Comparator >( points, begin, end );
        }
        else if( axis == 1 )
        {
            morton_mapping< 1, Comparator >( points, begin, end );
        }
        else
        {
            morton_mapping< 2, Comparator >( points, begin, end );
        }
    }

    // Splitting along an axis where all points have (almost) the same
    // coordinate interleaves points that are far apart along the other
    // axes, which breaks the curve locality (e.g. flat 3D data).
    // Such axes are skipped and the range is split as in 2D/1D.
    template < template < geode::index_t > typename Comparator >
    bool morton_mapping_skipping_flat_axes(
        const std::array< geode::local_index_t, 3 >& axes,
        absl::Span< const geode::Point3D > points,
        const itr& begin,
        const itr& end )
    {
        static constexpr double FLAT_RATIO{ 1e-3 };
        std::array< double, 3 > min;
        min.fill( std::numeric_limits< double >::max() );
        std::array< double, 3 > max;
        max.fill( std::numeric_limits< double >::lowest() );
        for( auto it = begin; it != end; ++it )
        {
            const auto& point = points[*it];
            for( const auto d : geode::LRange{ 3 } )
            {
                min[d] = std::min( min[d], point.value( d ) );
                max[d] = std::max( max[d], point.value( d ) );
            }
        }
        double max_extent{ 0 };
        for( const auto d : geode::LRange{ 3 } )
        {
            max_extent = std::max( max_extent, max[d] - min[d] );
        }
        if( max_extent == 0 )
        {
            // All points are colocated: any order is fine
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
            const Comparator< 3 > compX{ points, kept_axes[0] };
            const auto m1 = split_container( begin, end, compX );
            morton_mapping_from_axis< Comparator >(
                kept_axes[0], points, begin, m1 );
            morton_mapping_from_axis< Comparator >(
                kept_axes[0], points, m1, end );
            return true;
        }
        const Comparator< 3 > compX{ points, kept_axes[0] };
        const Comparator< 3 > compY{ points, kept_axes[1] };
        const auto m0 = begin;
        const auto m4 = end;
        const auto m2 = split_container( m0, m4, compX );
        const auto m1 = split_container( m0, m2, compY );
        const auto m3 = split_container( m2, m4, compY );
        morton_mapping_from_axis< Comparator >( kept_axes[1], points, m0, m1 );
        morton_mapping_from_axis< Comparator >( kept_axes[0], points, m1, m2 );
        morton_mapping_from_axis< Comparator >( kept_axes[0], points, m2, m3 );
        morton_mapping_from_axis< Comparator >( kept_axes[1], points, m3, m4 );
        return true;
    }

    template < geode::local_index_t COORDX,
        template < geode::index_t > typename Comparator >
    void morton_mapping( absl::Span< const geode::Point3D > points,
        const itr& begin,
        const itr& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        constexpr geode::local_index_t COORDY = COORDX == 2 ? 0 : COORDX + 1;
        constexpr geode::local_index_t COORDZ = COORDY == 2 ? 0 : COORDY + 1;
        if( morton_mapping_skipping_flat_axes< Comparator >(
                { COORDX, COORDY, COORDZ }, points, begin, end ) )
        {
            return;
        }

        const Comparator< 3 > compX{ points, COORDX };
        const Comparator< 3 > compY{ points, COORDY };
        const Comparator< 3 > compZ{ points, COORDZ };

        const auto m0 = begin;
        const auto m8 = end;
        const auto m4 = split_container( m0, m8, compX );
        const auto m2 = split_container( m0, m4, compY );
        const auto m1 = split_container( m0, m2, compZ );
        const auto m3 = split_container( m2, m4, compZ );
        const auto m6 = split_container( m4, m8, compY );
        const auto m5 = split_container( m4, m6, compZ );
        const auto m7 = split_container( m6, m8, compZ );
        morton_mapping< COORDZ, Comparator >( points, m0, m1 );
        morton_mapping< COORDY, Comparator >( points, m1, m2 );
        morton_mapping< COORDY, Comparator >( points, m2, m3 );
        morton_mapping< COORDX, Comparator >( points, m3, m4 );
        morton_mapping< COORDX, Comparator >( points, m4, m5 );
        morton_mapping< COORDY, Comparator >( points, m5, m6 );
        morton_mapping< COORDY, Comparator >( points, m6, m7 );
        morton_mapping< COORDZ, Comparator >( points, m7, m8 );
    }

    template < geode::local_index_t COORDX,
        template < geode::index_t > typename Comparator >
    void morton_mapping( absl::Span< const geode::Point2D > points,
        const itr& begin,
        const itr& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        constexpr geode::local_index_t COORDY = COORDX == 1 ? 0 : COORDX + 1;

        const Comparator< 2 > compX{ points, COORDX };
        const Comparator< 2 > compY{ points, COORDY };

        const auto m0 = begin;
        const auto m4 = end;
        const auto m2 = split_container( m0, m4, compX );
        const auto m1 = split_container( m0, m2, compY );
        const auto m3 = split_container( m2, m4, compY );
        morton_mapping< COORDY, Comparator >( points, m0, m1 );
        morton_mapping< COORDX, Comparator >( points, m1, m2 );
        morton_mapping< COORDX, Comparator >( points, m2, m3 );
        morton_mapping< COORDY, Comparator >( points, m3, m4 );
    }

    template < geode::local_index_t COORDX,
        template < geode::index_t > typename Comparator >
    void morton_mapping( absl::Span< const geode::Point1D > points,
        const itr& begin,
        const itr& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        const Comparator< 1 > compX{ points, COORDX };

        const auto m0 = begin;
        const auto m2 = end;
        const auto m1 = split_container( m0, m2, compX );
        morton_mapping< COORDX, Comparator >( points, m0, m1 );
        morton_mapping< COORDX, Comparator >( points, m1, m2 );
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
        absl::c_sort( mapping, [&points]( index_t i, index_t j ) {
            return lexicographic_compare( points[i], points[j] );
        } );
        return mapping;
    }

    template < index_t dimension >
    std::vector< index_t > morton_mapping(
        absl::Span< const Point< dimension > > points )
    {
        std::vector< index_t > mapping( points.size() );
        absl::c_iota( mapping, index_t{ 0 } );
        ::morton_mapping< 0_uc, Morton_cmp >(
            points, mapping.begin(), mapping.end() );
        return mapping;
    }

    template < index_t dimension >
    std::vector< index_t > hilbert_mapping(
        absl::Span< const Point< dimension > > points )
    {
        std::vector< index_t > mapping( points.size() );
        absl::c_iota( mapping, index_t{ 0 } );
        ::morton_mapping< 0_uc, Hilbert_cmp >(
            points, mapping.begin(), mapping.end() );
        return mapping;
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
