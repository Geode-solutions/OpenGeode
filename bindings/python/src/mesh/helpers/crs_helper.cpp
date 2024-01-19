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

#include "../../common.h"

#include <geode/geometry/coordinate_system.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/helpers/create_coordinate_system.h>

#define PYTHON_MESH_CRS_HELPER( mesh, dimension )                              \
    const auto name##mesh##dimension = absl::StrCat(                           \
        "create_", #mesh, "_coordinate_system", dimension, "D" );              \
    module.def( name##mesh##dimension.c_str(),                                 \
        &create_##mesh##_coordinate_system< dimension > )

namespace geode
{
    void define_mesh_crs_helper( pybind11::module& module )
    {
        PYTHON_MESH_CRS_HELPER( edged_curve, 2 );
        PYTHON_MESH_CRS_HELPER( edged_curve, 3 );
        PYTHON_MESH_CRS_HELPER( point_set, 2 );
        PYTHON_MESH_CRS_HELPER( point_set, 3 );
        PYTHON_MESH_CRS_HELPER( surface_mesh, 2 );
        PYTHON_MESH_CRS_HELPER( surface_mesh, 3 );
        PYTHON_MESH_CRS_HELPER( solid_mesh, 3 );
    }
} // namespace geode
