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

#include <geode/mesh/helpers/rasterize.h>

#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>

#include <geode/mesh/core/grid.h>
#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    void define_rasterize( pybind11::module& module )
    {
        module.def( "rasterize_segment2D", &rasterize_segment< 2 > );
        module.def( "rasterize_segment3D", &rasterize_segment< 3 > );
        module.def( "conservative_rasterize_segment2D",
            &conservative_rasterize_segment< 2 > );
        module.def( "conservative_rasterize_segment3D",
            &conservative_rasterize_segment< 3 > );
        module.def( "rasterize_triangle2D", &rasterize_triangle< 2 > );
        module.def( "rasterize_triangle3D", &rasterize_triangle< 3 > );
        module.def( "rasterize_tetrahedron", &rasterize_tetrahedron );
        module.def( "rasterize_closed_surface", &rasterize_closed_surface );
    }

} // namespace geode