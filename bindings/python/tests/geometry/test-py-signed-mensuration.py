#- * - coding : utf - 8 - * -
#Copyright( c ) 2019 - 2020 Geode - solutions
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files( the "Software" ), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

import opengeode_py_geometry as geom

def test_triangle_area():
    a = geom.Point2D( [0.0, 0.0] )
    b = geom.Point2D( [1.0, 0.0] )
    c = geom.Point2D( [1.0, 1.0] )

    triangle2D_1 = geom.Triangle2D( a, b, c )
    if geom.triangle_area2D( triangle2D_1 ) != 0.5:
        raise ValueError( "[Test] Wrong result for triangle_area with query triangle triangle2D_1" )

    triangle2D_2 = geom.Triangle2D( a, b, a )
    if geom.triangle_area2D( triangle2D_2 ) != 0.0:
        raise ValueError( "[Test] Wrong result for triangle_area with query triangle triangle2D_2" )

def test_triangle_signed_area():
    a = geom.Point2D( [0.0, 0.0] )
    b = geom.Point2D( [1.0, 0.0] )
    c = geom.Point2D( [1.0, 1.0] )

    triangle2D_1 = geom.Triangle2D( a, b, c )
    if geom.triangle_signed_area2D( triangle2D_1 ) != 0.5:
        raise ValueError( "[Test] Wrong result for triangle_signed_area with query triangle triangle2D_1" )

    triangle2D_2 = geom.Triangle2D( a, c, b )
    if geom.triangle_signed_area2D( triangle2D_2 ) != -0.5:
        raise ValueError( "[Test] Wrong result for triangle_signed_area with query triangle triangle2D_2" )

    triangle2D_3 = geom.Triangle2D( a, b, a )
    if geom.triangle_signed_area2D( triangle2D_3 ) != -0.0:
        raise ValueError( "[Test] Wrong result for triangle_signed_area with query triangle triangle2D_3" )

if __name__ == '__main__':
    test_triangle_area()
    test_triangle_signed_area()
