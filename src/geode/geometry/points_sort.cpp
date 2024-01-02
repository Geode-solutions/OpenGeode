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

#include <geode/geometry/points_sort.h>

#include <algorithm>

#include <async++.h>

#include <absl/algorithm/container.h>

#include <geode/geometry/point.h>

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
    ALIAS_2D_AND_3D( Morton_cmp );

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
    itr split( const itr& begin, const itr& end, const Compare& cmp )
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
    template < geode::local_index_t COORDX >
    void morton_mapping( absl::Span< const geode::Point3D > points,
        const itr& begin,
        const itr& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        constexpr geode::local_index_t COORDY =
            COORDX + 1 == 3 ? 0 : COORDX + 1;
        constexpr geode::local_index_t COORDZ =
            COORDY + 1 == 3 ? 0 : COORDY + 1;

        const Morton_cmp3D compX{ points, COORDX };
        const Morton_cmp3D compY{ points, COORDY };
        const Morton_cmp3D compZ{ points, COORDZ };

        const auto m0 = begin;
        const auto m8 = end;
        const auto m4 = split( m0, m8, compX );
        const auto m2 = split( m0, m4, compY );
        const auto m1 = split( m0, m2, compZ );
        const auto m3 = split( m2, m4, compZ );
        const auto m6 = split( m4, m8, compY );
        const auto m5 = split( m4, m6, compZ );
        const auto m7 = split( m6, m8, compZ );
        morton_mapping< COORDZ >( points, m0, m1 );
        morton_mapping< COORDY >( points, m1, m2 );
        morton_mapping< COORDY >( points, m2, m3 );
        morton_mapping< COORDX >( points, m3, m4 );
        morton_mapping< COORDX >( points, m4, m5 );
        morton_mapping< COORDY >( points, m5, m6 );
        morton_mapping< COORDY >( points, m6, m7 );
        morton_mapping< COORDZ >( points, m7, m8 );
    }

    template < geode::local_index_t COORDX >
    void morton_mapping( absl::Span< const geode::Point2D > points,
        const itr& begin,
        const itr& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        constexpr geode::local_index_t COORDY =
            COORDX + 1 == 2 ? 0 : COORDX + 1;

        const Morton_cmp2D compX{ points, COORDX };
        const Morton_cmp2D compY{ points, COORDY };

        const auto m0 = begin;
        const auto m4 = end;
        const auto m2 = split( m0, m4, compX );
        const auto m1 = split( m0, m2, compY );
        const auto m3 = split( m2, m4, compY );
        morton_mapping< COORDY >( points, m0, m1 );
        morton_mapping< COORDX >( points, m1, m2 );
        morton_mapping< COORDX >( points, m2, m3 );
        morton_mapping< COORDY >( points, m3, m4 );
    }

    /*
     * Return true if p0 < p1 comparing first X, then Y.
     */
    bool lexicographic_compare(
        const geode::Point2D& p0, const geode::Point2D& p1 )
    {
        if( p0.value( 0 ) < p1.value( 0 ) )
        {
            return true;
        }
        if( p0.value( 0 ) > p1.value( 0 ) )
        {
            return false;
        }
        return p0.value( 1 ) < p1.value( 1 );
    }

    /*
     * Return true if p0 < p1 comparing first X, then Y and last Z.
     */
    bool lexicographic_compare(
        const geode::Point3D& p0, const geode::Point3D& p1 )
    {
        if( p0.value( 0 ) < p1.value( 0 ) )
        {
            return true;
        }
        if( p0.value( 0 ) > p1.value( 0 ) )
        {
            return false;
        }
        if( p0.value( 1 ) < p1.value( 1 ) )
        {
            return true;
        }
        if( p0.value( 1 ) > p1.value( 1 ) )
        {
            return false;
        }
        return p0.value( 2 ) < p1.value( 2 );
    }

} // namespace

namespace geode
{
    template < index_t dimension >
    std::vector< index_t > lexicographic_mapping(
        absl::Span< const Point< dimension > > points )
    {
        std::vector< index_t > mapping( points.size() );
        async::parallel_for( async::irange( size_t{ 0 }, mapping.size() ),
            [&mapping]( index_t i ) {
                mapping[i] = i;
            } );
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
        async::parallel_for( async::irange( size_t{ 0 }, mapping.size() ),
            [&mapping]( index_t i ) {
                mapping[i] = i;
            } );
        ::morton_mapping< 0_uc >( points, mapping.begin(), mapping.end() );
        return mapping;
    }

    template std::vector< index_t > opengeode_geometry_api
        lexicographic_mapping( absl::Span< const Point< 2 > > );
    template std::vector< index_t > opengeode_geometry_api
        lexicographic_mapping( absl::Span< const Point< 3 > > );

    template std::vector< index_t > opengeode_geometry_api morton_mapping(
        absl::Span< const Point< 2 > > );
    template std::vector< index_t > opengeode_geometry_api morton_mapping(
        absl::Span< const Point< 3 > > );
} // namespace geode
