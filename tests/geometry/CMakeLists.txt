# Copyright (c) 2019 - 2025 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

add_geode_test(
    SOURCE "test-triangle.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-aabb.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-angle.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-barycentric-coordinates.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-basic-objects.cpp" 
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-bounding-box.cpp" 
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-coordinate-system.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-distance.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-dynamic-nnsearch.cpp" 
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-intersection.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
    ESSENTIAL
)
add_geode_test(
    SOURCE "test-intersection-detection.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
    ESSENTIAL
)
add_geode_test(
    SOURCE "test-mensuration.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-nnsearch.cpp" 
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)

add_geode_test(
    SOURCE "test-normal-frame-transform.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)

add_geode_test(
    SOURCE "test-perpendicular.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-point.cpp" 
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-points-sort.cpp" 
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-position.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-projection.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-quality.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-rotation.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-radial-sort.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-sign.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-square-matrix.cpp" 
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
add_geode_test(
    SOURCE "test-vector.cpp" 
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)

