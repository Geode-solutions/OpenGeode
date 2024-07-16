# Copyright (c) 2019 - 2024 Geode-solutions
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
        "basic_objects/cylinder.cpp"
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
        "coordinate_system.cpp"
        "distance.cpp"
        "frame.cpp"
        "information.cpp"
        "intersection.cpp"
        "intersection_detection.cpp"
        "mensuration.cpp"
        "nn_search.cpp"
        "perpendicular.cpp"
        "points_sort.cpp"
        "position.cpp"
        "internal/predicates.cpp"
        "projection.cpp"
        "radial_sort.cpp"
        "rotation.cpp"
        "sign.cpp"
        "square_matrix.cpp"
    PUBLIC_HEADERS
        "aabb.hpp"
        "barycentric_coordinates.hpp"
        "basic_objects/circle.hpp"
        "basic_objects/cylinder.hpp"
        "basic_objects/infinite_line.hpp"
        "basic_objects/plane.hpp"
        "basic_objects/segment.hpp"
        "basic_objects/sphere.hpp"
        "basic_objects/tetrahedron.hpp"
        "basic_objects/triangle.hpp"
        "bitsery_archive.hpp"
        "bounding_box.hpp"
        "common.hpp"
        "coordinate_system.hpp"
        "distance.hpp"
        "frame.hpp"
        "information.hpp"
        "intersection.hpp"
        "intersection_detection.hpp"
        "mensuration.hpp"
        "nn_search.hpp"
        "perpendicular.hpp"
        "point.hpp"
        "points_sort.hpp"
        "position.hpp"
        "projection.hpp"
        "radial_sort.hpp"
        "rotation.hpp"
        "sign.hpp"
        "vector.hpp"
        "square_matrix.hpp"
    ADVANCED_HEADERS
        "detail/aabb_impl.hpp"
        "detail/bitsery_archive.hpp"
    INTERNAL_HEADERS
        "internal/intersection_from_sides.hpp"
        "internal/position_from_sides.hpp"
        "internal/predicates.hpp"
    PUBLIC_DEPENDENCIES
        Bitsery::bitsery
        Threads::Threads
        ${PROJECT_NAME}::basic
    PRIVATE_DEPENDENCIES
        Async++
        nanoflann::nanoflann
)
