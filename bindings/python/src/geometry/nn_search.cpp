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

#include <geode/geometry/nn_search.hpp>

#define PYTHON_NN_SEARCH( dimension )                                          \
    const auto info##dimension =                                               \
        "ColocatedInfo" + std::to_string( dimension ) + "D";                   \
    pybind11::class_< NNSearch##dimension##D::ColocatedInfo >( module,         \
        info##dimension.c_str(),                                               \
        "Result of a colocation: mapping from the input points to the unique " \
        "points" )                                                             \
        .def_readonly( "colocated_mapping",                                    \
            &NNSearch##dimension##D::ColocatedInfo::colocated_mapping,         \
            "For each input point, the index of its unique point in "          \
            "unique_points" )                                                  \
        .def_readonly( "unique_points",                                        \
            &NNSearch##dimension##D::ColocatedInfo::unique_points,             \
            "Unique points: one point per group of colocated points" )         \
        .def( "nb_unique_points",                                              \
            &NNSearch##dimension##D::ColocatedInfo::nb_unique_points,          \
            "Return the number of unique points" )                             \
        .def( "nb_colocated_points",                                           \
            &NNSearch##dimension##D::ColocatedInfo::nb_colocated_points,       \
            "Return the number of points removed because they are colocated "  \
            "with another one" );                                              \
                                                                               \
    const auto name##dimension =                                               \
        "NNSearch" + std::to_string( dimension ) + "D";                        \
    pybind11::class_< NNSearch##dimension##D >( module,                        \
        name##dimension.c_str(),                                               \
        "Search tree returning the neighbors of a point among a list of "      \
        "points" )                                                             \
        .def( pybind11::init< std::vector< Point< dimension > > >(),           \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            pybind11::arg( "points" ),                                         \
            "Build the search tree from a list of points" )                    \
        .def( "nb_points", &NNSearch##dimension##D::nb_points,                 \
            "Return the number of points in the tree" )                        \
        .def( "point", &NNSearch##dimension##D::point,                         \
            pybind11::arg( "index" ),                                          \
            "Return the point at the given index in the input list" )          \
        .def( "closest_neighbor", &NNSearch##dimension##D::closest_neighbor,   \
            pybind11::arg( "point" ),                                          \
            "Return the index of the point closest to the given point, or "    \
            "NO_ID if the tree is empty" )                                     \
        .def( "radius_neighbors", &NNSearch##dimension##D::radius_neighbors,   \
            pybind11::arg( "point" ), pybind11::arg( "threshold_distance" ),   \
            "Return the indices of the points closer than threshold_distance " \
            "to the given point" )                                             \
        .def( "frame_neighbors", &NNSearch##dimension##D::frame_neighbors,     \
            pybind11::arg( "point" ), pybind11::arg( "epsilons_frame" ),       \
            "Return the indices of the points inside the ellipse described "   \
            "by epsilons_frame and centered on the given point" )              \
        .def( "neighbors", &NNSearch##dimension##D::neighbors,                 \
            pybind11::arg( "point" ), pybind11::arg( "nb_neighbors" ),         \
            "Return the indices of the nb_neighbors points closest to the "    \
            "given point (fewer if the tree is smaller)" )                     \
        .def( "radius_colocated_index_mapping",                                \
            static_cast< NNSearch##dimension##D::ColocatedInfo (               \
                NNSearch##dimension##D::* )( double ) const >(                 \
                &NNSearch##dimension##D::colocated_index_mapping ),            \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            pybind11::arg( "epsilon" ),                                        \
            "Return the ColocatedInfo grouping the points closer than "        \
            "epsilon. Raise an error if epsilon is smaller than the global "   \
            "epsilon" )                                                        \
        .def( "frame_colocated_index_mapping",                                 \
            static_cast< NNSearch##dimension##D::ColocatedInfo (               \
                NNSearch##dimension##D::* )( const Frame##dimension##D& )      \
                    const >(                                                   \
                &NNSearch##dimension##D::colocated_index_mapping ),            \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            pybind11::arg( "epsilon" ),                                        \
            "Return the ColocatedInfo grouping the points colocated within "   \
            "the ellipse described by the frame epsilon. Raise an error if "   \
            "one direction is shorter than the global epsilon" )

namespace geode
{
    void define_nn_search( pybind11::module& module )
    {
        PYTHON_NN_SEARCH( 2 );
        PYTHON_NN_SEARCH( 3 );
    }
} // namespace geode
