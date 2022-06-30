/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/intersection_detection.h>

namespace geode
{
    void define_intersection_detection( pybind11::module& module )
    {
        module.def( "segment_segment_intersection_detection2D",
            &segment_segment_intersection_detection );
        module.def( "colinear_segment_segment_intersection_detection2D",
            &colinear_segment_segment_intersection_detection );
        module.def( "segment_line_intersection_detection2D",
            &segment_line_intersection_detection );
        module.def( "line_triangle_intersection_detection3D",
            &line_triangle_intersection_detection );
        module.def( "segment_triangle_intersection_detection3D",
            &segment_triangle_intersection_detection );
        module.def( "segment_plane_intersection_detection3D",
            &segment_plane_intersection_detection );
    }
} // namespace geode
