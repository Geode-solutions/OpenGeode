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

#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

#include <absl/container/inlined_vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/point.hpp>

namespace geode
{
    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * Adaptive octree (quadtree in 2D) covering an axis-aligned domain.
     *
     * The domain is first split into a grid of root cells, built from a
     * BoundingBox: along each axis, the number of root cells is
     * max( 1, round( side / smallest_side ) ) and the root cell length is
     * side / nb, so that the root cells fit the box exactly and have an
     * aspect ratio in [1, 2).
     * Each leaf cell can then be refined into 2^dimension children.
     *
     * Cells are identified by stable indices: all cells (roots, internal
     * cells and leaves) are stored in a single vector. Root cells come first
     * (indices 0 to nb_roots - 1, row-major with axis 0 varying fastest),
     * then children are appended by blocks of 2^dimension contiguous cells
     * each time a leaf is refined. Indices never change (no coarsening).
     * Within a block of children, bit d of the local child index (0 to
     * 2^dimension - 1) is set when the child is in the upper half of its
     * parent along axis d.
     *
     * Each cell has an integer anchor (its minimal corner) in the finest
     * lattice. The lattice uses B bits per axis: B = 21 in 3D and B = 31 in
     * 2D (32 bits would overflow the 64-bit locational code: 1 sentinel bit +
     * 2 * 32 bits). The root grid uses root_bits = ceil( log2( max number of
     * root cells along an axis ) ) of these bits, so max_depth() = B -
     * root_bits. A cell at level L (roots are at level 0) spans 2^(
     * max_depth() - L ) lattice units along each axis.
     *
     * @note The finest cell size is about the largest domain side times
     * 2^-B. Refining a cell at max_depth() throws. This structure bounds the
     * resolution, not the memory: refining everywhere down to max_depth() is
     * not possible in practice.
     */
    template < index_t dimension >
    class Octree
    {
        OPENGEODE_DISABLE_COPY( Octree );

    public:
        static constexpr index_t NB_CHILDREN{ 1u << dimension };

        /*!
         * Identifies, with an axis, one of the 2 * dimension faces of a cell:
         * the face of minimal coordinate along the axis (lower) or of maximal
         * coordinate (upper). face_neighbor_leaves( cell, axis, side ) uses
         * it to select the face whose neighbors are returned: lower gives the
         * leaves on the side of decreasing coordinates, upper on the side of
         * increasing coordinates. Looping over every axis and both sides
         * visits all the face neighbors of a cell, e.g. to propagate values
         * between adjacent leaves.
         */
        enum struct FACE_SIDE
        {
            lower,
            upper
        };

        /*!
         * Adjacency enforced by balance(), which refines leaves until any
         * two adjacent leaves differ by at most one level:
         * - faces: leaves sharing a face (an edge in 2D). It bounds the
         * level jump across faces only, which is enough for algorithms
         * walking from leaf to leaf through faces: face_neighbor_leaves() of
         * a leaf then returns at most 2^(dimension-1) finer leaves.
         * - full: leaves sharing a face, an edge or a vertex (in 2D: an
         * edge or a vertex). It also bounds the level jump along edges and
         * at vertices, for algorithms using these neighbors too.
         */
        enum struct BALANCE_TYPE
        {
            faces,
            full
        };

        /*!
         * Leaves returned by face_neighbor_leaves(), inline up to the
         * 2^(dimension-1) finer leaves of a one-level-finer neighbor.
         */
        using FaceNeighbors =
            absl::InlinedVector< index_t, 1u << ( dimension - 1 ) >;

        /*!
         * Creates an empty octree (no cell).
         */
        Octree();

        /*!
         * Creates an octree covering the given box with only root cells.
         * @exception OpenGeodeException if a box side is smaller than or
         * equal to GLOBAL_EPSILON, or if the box is too elongated for the
         * lattice (max_depth() would be lower than 1).
         */
        explicit Octree( const BoundingBox< dimension >& bbox );
        Octree( Octree&& other ) noexcept;
        ~Octree();

        Octree& operator=( Octree&& other ) noexcept;

        /*!
         * Maximal level a cell can have (roots are at level 0).
         */
        [[nodiscard]] local_index_t max_depth() const;

        /*!
         * Number of cells, including roots and internal (refined) cells.
         */
        [[nodiscard]] index_t nb_cells() const;

        [[nodiscard]] const std::array< index_t, dimension >&
            nb_root_cells() const;

        [[nodiscard]] bool is_leaf( index_t cell ) const;

        [[nodiscard]] local_index_t level( index_t cell ) const;

        /*!
         * Parent cell, std::nullopt for a root cell.
         */
        [[nodiscard]] std::optional< index_t > parent( index_t cell ) const;

        /*!
         * Index of the first child, std::nullopt for a leaf.
         * The NB_CHILDREN children are contiguous: their indices are
         * [first_child, first_child + NB_CHILDREN). Bit d of
         * ( child - first_child ) is set when the child is in the upper half
         * of the cell along axis d.
         */
        [[nodiscard]] std::optional< index_t > children( index_t cell ) const;

        /*!
         * Bounding box of the cell. Coordinates are computed from lattice
         * values only, hence cells sharing a face have exactly the same face
         * coordinates. Faces on the domain upper border are exactly at the
         * input bounding box max.
         */
        [[nodiscard]] BoundingBox< dimension > cell_bounding_box(
            index_t cell ) const;

        [[nodiscard]] Point< dimension > cell_center( index_t cell ) const;

        /*!
         * Cell length along each axis: root cell length / 2^level.
         */
        [[nodiscard]] std::array< double, dimension > cell_lengths(
            index_t cell ) const;

        /*!
         * Minimal corner of the cell in the finest lattice.
         */
        [[nodiscard]] const std::array< uint32_t, dimension >& cell_anchor(
            index_t cell ) const;

        /*!
         * Finds the cell with the given level and anchor.
         * @return std::nullopt if such a cell does not exist (not created,
         * level above max_depth(), anchor outside the domain or not aligned
         * on the cell size of this level).
         */
        [[nodiscard]] std::optional< index_t > cell( local_index_t level,
            const std::array< uint32_t, dimension >& anchor ) const;

        /*!
         * All the leaf cells, sorted by cell index.
         */
        [[nodiscard]] std::vector< index_t > leaves() const;

        /*!
         * Domain covered by the root cells: exactly the bounding box given at
         * construction.
         */
        [[nodiscard]] BoundingBox< dimension > domain_bounding_box() const;

        /*!
         * Finds the leaf containing the point.
         * Cells are considered as closed boxes. When a point lies on a face
         * (or edge/corner) shared by several leaves, the leaf on the upper
         * side along each such axis is returned, unless the point is on the
         * upper border of the domain, in which case the last leaf along this
         * axis is returned.
         * @return std::nullopt if the point is outside domain_bounding_box()
         * (BoundingBox::contains semantics, no tolerance).
         */
        [[nodiscard]] std::optional< index_t > leaf_containing(
            const Point< dimension >& point ) const;

        /*!
         * Depth-first traversal of the tree, starting from the roots in index
         * order, children in index order.
         * @param[in] filter Functor called on each visited cell as
         * bool filter( index_t cell, const BoundingBox< dimension >& box );
         * if it returns false, the cell and its subtree are skipped.
         * @param[in] action Functor called on each leaf accepted by the
         * filter as void action( index_t leaf ).
         */
        template < typename Filter, typename Action >
        void traverse( Filter&& filter, Action&& action ) const;

        /*!
         * Splits a leaf into NB_CHILDREN children, appended at the end of
         * the cell vector. All cell attribute values of the leaf are copied
         * to its children.
         * @return Index of the first child.
         * @exception OpenGeodeException if the cell is not a leaf or is
         * already at max_depth().
         */
        index_t refine( index_t leaf );

        /*!
         * Leaves sharing a part (with positive measure) of the given face of
         * the cell, found by lattice lookups only (no geometry).
         * The cell may be a leaf or an internal cell: in both cases, the
         * returned leaves are outside the cell and touch the given face:
         * - the same-size leaf on the other side of the face if it exists,
         * - else the coarser leaf containing the neighboring region if this
         * region is not refined down to the cell level,
         * - else all the finer leaves (at any depth) touching the face, in
         * depth-first order, children in index order.
         * @return Empty on the domain border.
         */
        [[nodiscard]] FaceNeighbors face_neighbor_leaves(
            index_t cell, local_index_t axis, FACE_SIDE side ) const;

        /*!
         * Refines leaves until any two adjacent leaves (see BALANCE_TYPE)
         * differ by at most one level (2:1 balance). Only coarser leaves are
         * refined, never coarsened: existing indices are unchanged and new
         * cells are appended and inherit attribute values as in refine().
         * Leaves are processed in a FIFO queue seeded in index order, hence
         * the result is deterministic.
         */
        void balance( BALANCE_TYPE type );

        /*!
         * Attributes on all cells (internal cells included).
         */
        [[nodiscard]] AttributeManager& cell_attribute_manager() const;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& serializer );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Octree );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    template < typename Filter, typename Action >
    void Octree< dimension >::traverse( Filter&& filter, Action&& action ) const
    {
        if( nb_cells() == 0 )
        {
            return;
        }
        index_t nb_roots{ 1 };
        for( const auto nb : nb_root_cells() )
        {
            nb_roots *= nb;
        }
        std::vector< index_t > to_visit;
        to_visit.reserve( nb_roots );
        for( const auto root : Range{ nb_roots } )
        {
            to_visit.push_back( nb_roots - 1 - root );
        }
        while( !to_visit.empty() )
        {
            const auto cell = to_visit.back();
            to_visit.pop_back();
            if( !filter( cell, cell_bounding_box( cell ) ) )
            {
                continue;
            }
            const auto first_child = children( cell );
            if( !first_child )
            {
                action( cell );
                continue;
            }
            for( const auto child : Range{ NB_CHILDREN } )
            {
                to_visit.push_back(
                    first_child.value() + NB_CHILDREN - 1 - child );
            }
        }
    }
} // namespace geode
