/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/mensuration.h>

#define PYTHON_MENSURATION( dimension )                                        \
    const auto triangle_area##dimension =                                      \
        "triangle_area" + std::to_string( dimension ) + "D";                   \
    module.def( triangle_area##dimension.c_str(), &triangle_area< dimension > )

namespace geode
{
    void define_mensuration( pybind11::module& module )
    {
        PYTHON_MENSURATION( 2 );
        PYTHON_MENSURATION( 3 );
        module.def( "triangle_signed_area2D",
            ( double ( * )( const Triangle2D& ) ) & triangle_signed_area );
        module.def( "triangle_signed_area3D",
            ( double ( * )( const Triangle3D&, const Vector3D& ) )
                & triangle_signed_area );
        module.def( "tetrahedron_signed_volume", &tetrahedron_signed_volume );
        module.def( "tetrahedron_volume", &tetrahedron_volume );
    }
} // namespace geode
