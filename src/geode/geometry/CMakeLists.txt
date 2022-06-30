# Copyright (c) 2019 - 2022 Geode-solutions
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

add_geode_library(
    NAME geometry
    FOLDER "geode/geometry"
    SOURCES
        "aabb.cpp"
        "barycentric_coordinates.cpp"
        "basic_objects/circle.cpp"
        "basic_objects/infinite_line.cpp"
        "basic_objects/plane.cpp"
        "basic_objects/segment.cpp"
        "basic_objects/sphere.cpp"
        "basic_objects/tetrahedron.cpp"
        "basic_objects/triangle.cpp"
        "bitsery_input.cpp"
        "bitsery_output.cpp"
        "bounding_box.cpp"
        "common.cpp"
        "distance.cpp"
        "intersection.cpp"
        "mensuration.cpp"
        "nn_search.cpp"
        "perpendicular.cpp"
        "points_sort.cpp"
        "position.cpp"
        "private/predicates.cpp"
        "projection.cpp"
        "rotation.cpp"
        "sign.cpp"
    PUBLIC_HEADERS
        "aabb.h"
        "barycentric_coordinates.h"
        "basic_objects/circle.h"
        "basic_objects/infinite_line.h"
        "basic_objects/plane.h"
        "basic_objects/segment.h"
        "basic_objects/sphere.h"
        "basic_objects/tetrahedron.h"
        "basic_objects/triangle.h"
        "bitsery_archive.h"
        "bounding_box.h"
        "common.h"
        "distance.h"
        "information.h"
        "intersection.h"
        "mensuration.h"
        "nn_search.h"
        "perpendicular.h"
        "point.h"
        "points_sort.h"
        "position.h"
        "projection.h"
        "rotation.h"
        "sign.h"
        "vector.h"
    ADVANCED_HEADERS
        "detail/aabb_impl.h"
        "detail/bitsery_archive.h"
    PRIVATE_HEADERS
        "private/intersection_from_sides.h"
        "private/position_from_sides.h"
        "private/predicates.h"
    PUBLIC_DEPENDENCIES
        Bitsery::bitsery
        Threads::Threads
        ${PROJECT_NAME}::basic
    PRIVATE_DEPENDENCIES
        Async++
        nanoflann::nanoflann
)
