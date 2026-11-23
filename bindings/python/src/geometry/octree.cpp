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

#include "../common.hpp"

#include <geode/basic/attribute_manager.hpp>

#include <geode/geometry/octree.hpp>

namespace
{
    template < geode::index_t dimension >
    void check_cell( std::string_view context,
        const geode::Octree< dimension >& octree,
        geode::index_t cell )
    {
        geode::check_index( context, "cell", cell, octree.nb_cells() );
    }

    template < geode::index_t dimension >
    void define_octree( pybind11::module& module )
    {
        using Octree = geode::Octree< dimension >;
        const auto name = absl::StrCat( "Octree", dimension, "D" );
        pybind11::class_< Octree > octree( module, name.c_str(),
            "Adaptive octree (quadtree in 2D) covering an axis-aligned box. "
            "The box is split into a grid of root cells (along each axis, "
            "round( side / smallest side ) cells, at least 1), then each leaf "
            "can be refined into NB_CHILDREN children. Cells have stable "
            "indices: roots first (axis 0 varying fastest), then children "
            "appended by blocks of NB_CHILDREN each time a leaf is refined; "
            "there is no coarsening. Each cell has an integer anchor (its "
            "minimal corner) on the finest lattice: a cell at level L spans "
            "2^(max_depth() - L) lattice units along each axis" );
        octree.attr( "NB_CHILDREN" ) = Octree::NB_CHILDREN;
        octree.attr( "NB_FACE_CHILDREN" ) = Octree::NB_FACE_CHILDREN;
        pybind11::native_enum< typename Octree::FACE_SIDE >( octree, "FaceSide",
            "enum.Enum",
            "With an axis, one of the 2 * dimension faces of a cell: the face "
            "of minimal (lower) or maximal (upper) coordinate along the axis" )
            .value( "lower", Octree::FACE_SIDE::lower )
            .value( "upper", Octree::FACE_SIDE::upper )
            .finalize();
        pybind11::native_enum< typename Octree::BALANCE_TYPE >( octree,
            "BalanceType", "enum.Enum",
            "Adjacency enforced by balance(): faces (leaves sharing a face, "
            "an edge in 2D) or full (leaves sharing a face, an edge or a "
            "vertex)" )
            .value( "faces", Octree::BALANCE_TYPE::faces )
            .value( "full", Octree::BALANCE_TYPE::full )
            .finalize();
        octree.def( pybind11::init<>(), "Create an empty octree (no cell)" )
            .def( pybind11::init< const geode::BoundingBox< dimension >& >(),
                pybind11::arg( "bbox" ),
                "Create an octree covering the box with only root cells. "
                "Raise an OpenGeodeDataError if a box side is not larger "
                "than GLOBAL_EPSILON or if the box is too elongated (more "
                "root cells than the lattice can hold)" )
            .def( "max_depth", &Octree::max_depth,
                "Return the maximal level a cell can have (roots are at "
                "level 0)" )
            .def( "nb_cells", &Octree::nb_cells,
                "Return the number of cells, roots and refined cells "
                "included" )
            .def( "nb_root_cells", &Octree::nb_root_cells,
                "Return the number of root cells along each axis" )
            .def(
                "is_leaf",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "is_leaf", self, cell );
                    return self.is_leaf( cell );
                },
                pybind11::arg( "cell" ), "Return True if the cell is a leaf" )
            .def(
                "level",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "level", self, cell );
                    return self.level( cell );
                },
                pybind11::arg( "cell" ),
                "Return the level of the cell (roots are at level 0)" )
            .def(
                "parent",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "parent", self, cell );
                    return self.parent( cell );
                },
                pybind11::arg( "cell" ),
                "Return the parent cell, None for a root cell" )
            .def(
                "children",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "children", self, cell );
                    return self.children( cell );
                },
                pybind11::arg( "cell" ),
                "Return the index of the first child, None for a leaf. The "
                "NB_CHILDREN children are contiguous; bit d of (child - "
                "first child) is set when the child is in the upper half of "
                "the cell along axis d" )
            .def(
                "cell_bounding_box",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "cell_bounding_box", self, cell );
                    return self.cell_bounding_box( cell );
                },
                pybind11::arg( "cell" ),
                "Return the bounding box of the cell, computed from lattice "
                "values only: cells sharing a face have exactly the same face "
                "coordinates" )
            .def(
                "cell_center",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "cell_center", self, cell );
                    return self.cell_center( cell );
                },
                pybind11::arg( "cell" ), "Return the center of the cell" )
            .def(
                "cell_lengths",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "cell_lengths", self, cell );
                    return self.cell_lengths( cell );
                },
                pybind11::arg( "cell" ),
                "Return the cell length along each axis: root cell length / "
                "2^level" )
            .def(
                "cell_anchor",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "cell_anchor", self, cell );
                    return self.cell_anchor( cell );
                },
                pybind11::arg( "cell" ),
                "Return the minimal corner of the cell on the finest lattice "
                "(a copy)" )
            .def( "cell", &Octree::cell, pybind11::arg( "level" ),
                pybind11::arg( "anchor" ),
                "Return the cell with the given level and anchor, None if it "
                "does not exist (not created, level above max_depth(), "
                "anchor outside the domain or not aligned on the cell size "
                "of this level)" )
            .def( "leaves",
                pybind11::overload_cast<>( &Octree::leaves, pybind11::const_ ),
                "Return all the leaf cells, sorted by cell index" )
            .def(
                "leaves",
                []( const Octree& self, geode::index_t cell ) {
                    check_cell( "leaves", self, cell );
                    return self.leaves( cell );
                },
                pybind11::arg( "cell" ),
                "Return the leaf cells of the subtree of the cell (the cell "
                "itself if it is a leaf), in depth-first order, children in "
                "index order" )
            .def( "domain_bounding_box", &Octree::domain_bounding_box,
                "Return the domain covered by the root cells: exactly the "
                "bounding box given at construction" )
            .def( "leaf_containing", &Octree::leaf_containing,
                pybind11::arg( "point" ),
                "Return the leaf containing the point, None if the point is "
                "outside domain_bounding_box() (no tolerance). Cells are "
                "closed: on a face shared by several leaves, the leaf on the "
                "upper side is returned, except on the domain upper border" )
            .def(
                "traverse",
                []( const Octree& self, const pybind11::function& filter,
                    const pybind11::function& action ) {
                    self.traverse(
                        [&filter]( geode::index_t cell,
                            const geode::BoundingBox< dimension >& box ) {
                            return filter( cell, box ).template cast< bool >();
                        },
                        [&action]( geode::index_t leaf ) {
                            action( leaf );
                        } );
                },
                pybind11::arg( "filter" ), pybind11::arg( "action" ),
                "Depth-first traversal from the roots, in index order. "
                "filter(cell, box) is called on each visited cell with its "
                "bounding box: if it returns False, the cell and its subtree "
                "are skipped. action(leaf) is called on each leaf accepted by "
                "the filter" )
            .def(
                "refine",
                []( Octree& self, geode::index_t leaf ) {
                    check_cell( "refine", self, leaf );
                    return self.refine( leaf );
                },
                pybind11::arg( "leaf" ),
                "Split a leaf into NB_CHILDREN children appended at the end "
                "of the cells, copying all its cell attribute values to them, "
                "and return the index of the first child. Raise an "
                "OpenGeodeDataError if the cell is not a leaf or is at "
                "max_depth()" )
            .def(
                "face_neighbor_leaves",
                []( const Octree& self, geode::index_t cell,
                    geode::local_index_t axis,
                    typename Octree::FACE_SIDE side ) {
                    check_cell( "face_neighbor_leaves", self, cell );
                    geode::check_direction(
                        "face_neighbor_leaves", axis, dimension );
                    const auto neighbors =
                        self.face_neighbor_leaves( cell, axis, side );
                    return std::vector< geode::index_t >(
                        neighbors.begin(), neighbors.end() );
                },
                pybind11::arg( "cell" ), pybind11::arg( "axis" ),
                pybind11::arg( "side" ),
                "Return the leaves outside the cell (a leaf or not) sharing a "
                "part of its face given by axis and side: the same-size leaf "
                "on the other side, else the coarser leaf containing it, else "
                "all the finer leaves touching the face (depth-first). Empty "
                "on the domain border" )
            .def( "balance", &Octree::balance,
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                pybind11::arg( "type" ),
                "Refine leaves until any two adjacent leaves (see "
                "BalanceType) differ by at most one level. Existing indices "
                "are unchanged, new cells inherit attribute values as in "
                "refine(); the result is deterministic" )
            .def( "cell_attribute_manager", &Octree::cell_attribute_manager,
                pybind11::return_value_policy::reference_internal,
                "Return the manager of the attributes on all the cells "
                "(refined cells included); refine() copies the values of a "
                "leaf to its children" );
    }
} // namespace

namespace geode
{
    void define_octree( pybind11::module& module )
    {
        ::define_octree< 2 >( module );
        ::define_octree< 3 >( module );
    }
} // namespace geode
