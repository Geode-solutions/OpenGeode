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

#include "../common.h"

#include <geode/geometry/nn_search.hpp>

#define PYTHON_NN_SEARCH( dimension )                                          \
    const auto name##dimension =                                               \
        "NNSearch" + std::to_string( dimension ) + "D";                        \
    pybind11::class_< NNSearch##dimension##D >(                                \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init< std::vector< Point< dimension > > >() )          \
        .def( "nb_points", &NNSearch##dimension##D::nb_points )                \
        .def( "point", &NNSearch##dimension##D::point )                        \
        .def( "closest_neighbor", &NNSearch##dimension##D::closest_neighbor )  \
        .def( "radius_neighbors", &NNSearch##dimension##D::radius_neighbors )  \
        .def( "neighbors", &NNSearch##dimension##D::neighbors )                \
        .def( "colocated_index_mapping",                                       \
            &NNSearch##dimension##D::colocated_index_mapping );                \
                                                                               \
    const auto info##dimension =                                               \
        "ColocatedInfo" + std::to_string( dimension ) + "D";                   \
    pybind11::class_< NNSearch##dimension##D::ColocatedInfo >(                 \
        module, info##dimension.c_str() )                                      \
        .def_readonly( "colocated_mapping",                                    \
            &NNSearch##dimension##D::ColocatedInfo::colocated_mapping )        \
        .def_readonly( "unique_points",                                        \
            &NNSearch##dimension##D::ColocatedInfo::unique_points )            \
        .def( "nb_unique_points",                                              \
            &NNSearch##dimension##D::ColocatedInfo::nb_unique_points )         \
        .def( "nb_colocated_points",                                           \
            &NNSearch##dimension##D::ColocatedInfo::nb_colocated_points )

namespace geode
{
    void define_nn_search( pybind11::module& module )
    {
        PYTHON_NN_SEARCH( 2 );
        PYTHON_NN_SEARCH( 3 );
    }
} // namespace geode
