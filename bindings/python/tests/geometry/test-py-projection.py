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

if __name__ == '__main__':
    a = geom.Point2D( [1.0, 5.0] )
    b = geom.Point2D( [-1.0, -5.0] )
    segment2D = geom.Segment2D( a, b )
    closest_point = geom.point_segment_projection2D( a, segment2D )
    if closest_point != a:
        raise ValueError( "[Test] Wrong result for point_segment_projection with query point a" )
    closest_point = geom.point_segment_projection2D( b, segment2D )
    if closest_point != b:
        raise ValueError( "[Test] Wrong result for point_segment_projection with query point b" )
    q1 = geom.Point2D( [0.0, 0.0] )
    closest_point = geom.point_segment_projection2D( q1, segment2D )
    if closest_point != q1:
        raise ValueError( "[Test] Wrong result for point_segment_projection with query point q1" )
    q2 = geom.Point2D( [10.0, 10.0] )
    closest_point = geom.point_segment_projection2D( q2, segment2D )
    if closest_point != a:
        raise ValueError( "[Test] Wrong result for point_segment_projection with query point q2" )
    q3 = geom.Point2D( [5.0, -1.0] )
    closest_point = geom.point_segment_projection2D( q3, segment2D )
    result_q3 = geom.Point2D( [0.0, 0.0] )
    if closest_point != result_q3:
        raise ValueError( "[Test] Wrong result for point_segment_projection with query point q3" )
    q4 = geom.Point2D( [5.5, 1.5] )
    closest_point = geom.point_segment_projection2D( q4, segment2D )
    result_q4 = geom.Point2D( [0.5, 2.5] )
    if closest_point != result_q4:
        raise ValueError( "[Test] Wrong result for point_segment_projection with query point q3" )
