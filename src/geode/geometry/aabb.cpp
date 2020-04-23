/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#include <geode/geometry/aabb.h>

#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

#include <algorithm>
#include <numeric>

namespace
{
    template < geode::index_t dimension >
    double inner_point_box_distance( const geode::Point< dimension >& point,
        const geode::BoundingBox< dimension >& box )
    {
        OPENGEODE_ASSERT( box.contains( point ), "point out of the box" );
        const auto Pmin = point - box.min();
        const auto Pmax = point - box.max();
        auto result = std::numeric_limits< double >::max();
        for( const auto c : geode::Range{ dimension } )
        {
            const auto local_result = std::min(
                std::abs( Pmin.value( c ) ), std::abs( Pmax.value( c ) ) );
            result = std::min( result, local_result );
        }
        return result;
    }

    using const_itr = const std::vector< geode::index_t >::iterator;

    template < geode::index_t dimension >
    class Morton_cmp
    {
    public:
        Morton_cmp( absl::Span< const geode::BoundingBox< dimension > > bboxes,
            geode::index_t coord )
            : bboxes_( bboxes ), coord_( coord )
        {
        }

        bool operator()( geode::index_t box1, geode::index_t box2 ) const
        {
            const auto& bbox1 = bboxes_[box1];
            const auto& bbox2 = bboxes_[box2];
            return bbox1.min().value( coord_ ) + bbox1.max().value( coord_ )
                   < bbox2.min().value( coord_ ) + bbox2.max().value( coord_ );
        }

    private:
        absl::Span< const geode::BoundingBox< dimension > > bboxes_;
        geode::index_t coord_;
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
    template < typename CMP >
    const_itr split( const_itr& begin, const_itr& end, const CMP& cmp )
    {
        if( begin >= end )
        {
            return begin;
        }
        const_itr middle = begin + ( end - begin ) / 2;
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
    template < geode::index_t COORDX >
    void morton_sort( absl::Span< const geode::BoundingBox3D > bboxes,
        const_itr& begin,
        const_itr& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        constexpr auto COORDY = ( COORDX + 1 ) % 3;
        constexpr auto COORDZ = ( COORDY + 1 ) % 3;

        const Morton_cmp3D compX{ bboxes, COORDX };
        const Morton_cmp3D compY{ bboxes, COORDY };
        const Morton_cmp3D compZ{ bboxes, COORDZ };

        const auto m0 = begin;
        const auto m8 = end;
        const auto m4 = split( m0, m8, compX );
        const auto m2 = split( m0, m4, compY );
        const auto m1 = split( m0, m2, compZ );
        const auto m3 = split( m2, m4, compZ );
        const auto m6 = split( m4, m8, compY );
        const auto m5 = split( m4, m6, compZ );
        const auto m7 = split( m6, m8, compZ );
        morton_sort< COORDZ >( bboxes, m0, m1 );
        morton_sort< COORDY >( bboxes, m1, m2 );
        morton_sort< COORDY >( bboxes, m2, m3 );
        morton_sort< COORDX >( bboxes, m3, m4 );
        morton_sort< COORDX >( bboxes, m4, m5 );
        morton_sort< COORDY >( bboxes, m5, m6 );
        morton_sort< COORDY >( bboxes, m6, m7 );
        morton_sort< COORDZ >( bboxes, m7, m8 );
    }

    template < geode::index_t COORDX >
    void morton_sort( absl::Span< const geode::BoundingBox2D > bboxes,
        const_itr& begin,
        const_itr& end )
    {
        if( end - begin <= 1 )
        {
            return;
        }
        constexpr auto COORDY = ( COORDX + 1 ) % 2;

        const Morton_cmp2D compX{ bboxes, COORDX };
        const Morton_cmp2D compY{ bboxes, COORDY };

        const auto m0 = begin;
        const auto m4 = end;
        const auto m2 = split( m0, m4, compX );
        const auto m1 = split( m0, m2, compY );
        const auto m3 = split( m2, m4, compY );
        morton_sort< COORDY >( bboxes, m0, m1 );
        morton_sort< COORDX >( bboxes, m1, m2 );
        morton_sort< COORDX >( bboxes, m2, m3 );
        morton_sort< COORDY >( bboxes, m3, m4 );
    }

    template < geode::index_t dimension >
    std::vector< geode::index_t > morton_sort(
        absl::Span< const geode::BoundingBox< dimension > > bboxes )
    {
        std::vector< geode::index_t > mapping_morton( bboxes.size() );
        absl::c_iota( mapping_morton, 0 );
        morton_sort< 0 >(
            bboxes, mapping_morton.begin(), mapping_morton.end() );
        return mapping_morton;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension >::AABBTree(
        absl::Span< const BoundingBox< dimension > > bboxes )
    {
        mapping_morton_ = morton_sort( bboxes );
        const auto nb_bboxes = static_cast< index_t >( bboxes.size() );
        tree_.resize( max_node_index( ROOT_INDEX, 0, nb_bboxes ) + ROOT_INDEX );
        initialize_tree_recursive( bboxes, ROOT_INDEX, 0, nb_bboxes );
    }

    template < index_t dimension >
    index_t AABBTree< dimension >::max_node_index(
        index_t node_index, index_t box_begin, index_t box_end ) const
    {
        OPENGEODE_ASSERT( box_end > box_begin,
            "End box index should be after Begin box index" );
        if( is_leaf( box_begin, box_end ) )
        {
            return node_index;
        }
        index_t element_middle;
        index_t child_left;
        index_t child_right;
        get_recursive_iterators( node_index, box_begin, box_end, element_middle,
            child_left, child_right );
        return std::max(
            max_node_index( child_left, box_begin, element_middle ),
            max_node_index( child_right, element_middle, box_end ) );
    }

    /**
     * \brief Computes the hierarchy of bounding boxes recursively.
     * \param[in] bboxes the array of bounding boxes
     * \param[in] node_index the index of the root of the subtree
     * \param[in] element_begin first box index in the vector \p bboxes
     * \param[in] element_end one position past the last box index in the vector
     * \p
     * bboxes
     */
    template < index_t dimension >
    void AABBTree< dimension >::initialize_tree_recursive(
        absl::Span< const BoundingBox< dimension > > bboxes,
        index_t node_index,
        index_t element_begin,
        index_t element_end )
    {
        OPENGEODE_ASSERT( node_index < tree_.size(), "Node index out of tree" );
        OPENGEODE_ASSERT( element_begin != element_end,
            "Begin and End indices should be different" );
        if( is_leaf( element_begin, element_end ) )
        {
            set_node( node_index, bboxes[mapping_morton_[element_begin]] );
            return;
        }
        index_t element_middle;
        index_t child_left;
        index_t child_right;
        get_recursive_iterators( node_index, element_begin, element_end,
            element_middle, child_left, child_right );
        OPENGEODE_ASSERT( child_left < tree_.size(), "Left index out of tree" );
        OPENGEODE_ASSERT(
            child_right < tree_.size(), "Right index out of tree" );
        initialize_tree_recursive(
            bboxes, child_left, element_begin, element_middle );
        initialize_tree_recursive(
            bboxes, child_right, element_middle, element_end );
        // before box_union
        add_box( node_index, node( child_left ) );
        add_box( node_index, node( child_right ) );
    }

    template < index_t dimension >
    double point_box_signed_distance(
        const Point< dimension >& point, const BoundingBox< dimension >& box )
    {
        bool inside{ true };
        Vector< dimension > result;
        for( const auto c : Range{ dimension } )
        {
            if( point.value( c ) < box.min().value( c ) )
            {
                inside = false;
                result.set_value( c, point.value( c ) - box.min().value( c ) );
            }
            else if( point.value( c ) > box.max().value( c ) )
            {
                inside = false;
                result.set_value( c, point.value( c ) - box.max().value( c ) );
            }
        }
        if( inside )
        {
            return -inner_point_box_distance( point, box );
        }
        return result.length();
    }

    template double opengeode_geometry_api point_box_signed_distance(
        const Point2D&, const BoundingBox2D& );
    template class opengeode_geometry_api AABBTree< 2 >;

    template double opengeode_geometry_api point_box_signed_distance(
        const Point3D&, const BoundingBox3D& );
    template class opengeode_geometry_api AABBTree< 3 >;
} // namespace geode
