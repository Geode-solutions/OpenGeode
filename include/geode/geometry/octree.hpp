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

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl.hpp>

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

#include <geode/geometry/detail/octree_impl.hpp>
