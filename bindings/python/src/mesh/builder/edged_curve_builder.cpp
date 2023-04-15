/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include "../../common.h"

#include <geode/geometry/point.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/core/edged_curve.h>

#define PYTHON_EDGED_CURVE_BUILDER( dimension )                                \
    const auto name##dimension =                                               \
        "EdgedCurveBuilder" + std::to_string( dimension ) + "D";               \
    pybind11::class_< EdgedCurveBuilder##dimension##D, GraphBuilder,           \
        CoordinateReferenceSystemManagersBuilder##dimension##D >(              \
        module, name##dimension.c_str() )                                      \
        .def_static( "create",                                                 \
            ( std::unique_ptr< EdgedCurveBuilder##dimension##D >( * )(         \
                EdgedCurve< dimension >& ) )                                   \
                & EdgedCurveBuilder##dimension##D::create )                    \
        .def( "create_point", &EdgedCurveBuilder##dimension##D::create_point )

namespace geode
{
    void define_edged_curve_builder( pybind11::module& module )
    {
        PYTHON_EDGED_CURVE_BUILDER( 2 );
        PYTHON_EDGED_CURVE_BUILDER( 3 );
    }
} // namespace geode
