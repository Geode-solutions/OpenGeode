# Copyright (c) 2019 - 2020 Geode-solutions
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
        "basic_objects.cpp"
        "bitsery_archive.cpp"
        "bounding_box.cpp"
        "common.cpp"
        "distance.cpp"
        "nn_search.cpp"
        "perpendicular.cpp"
        "projection.cpp"
        "rotation.cpp"
        "signed_mensuration.cpp"
    PUBLIC_HEADERS
        "aabb.h"
        "barycentric_coordinates.h"
        "basic_objects.h"
        "bitsery_archive.h"
        "bounding_box.h"
        "common.h"
        "distance.h"
        "nn_search.h"
        "perpendicular.h"
        "point.h"
        "projection.h"
        "rotation.h"
        "signed_mensuration.h"
        "vector.h"
    ADVANCED_HEADERS
        "detail/aabb_impl.h"
    PUBLIC_DEPENDENCIES
        Bitsery::bitsery
        Threads::Threads
        ${PROJECT_NAME}::basic
    PRIVATE_DEPENDENCIES
        Async++
        nanoflann::nanoflann
)
