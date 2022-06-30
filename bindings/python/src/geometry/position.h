/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/information.h>
#include <geode/geometry/position.h>

#define PYTHON_POSITION( dimension )                                           \
    const auto point_segment##dimension =                                      \
        "point_segment_position" + std::to_string( dimension ) + "D";          \
    module.def( point_segment##dimension.c_str(),                              \
        ( Position( * )(                                                       \
            const Point< dimension >&, const Segment< dimension >& ) )         \
            & point_segment_position );                                        \
    const auto point_triangle##dimension =                                     \
        "point_triangle_position" + std::to_string( dimension ) + "D";         \
    module.def( point_triangle##dimension.c_str(),                             \
        ( Position( * )(                                                       \
            const Point< dimension >&, const Triangle< dimension >& ) )        \
            & point_triangle_position )

namespace geode
{
    void define_position( pybind11::module& module )
    {
        PYTHON_POSITION( 2 );
        PYTHON_POSITION( 3 );
        module.def( "point_side_to_segment2D", &point_side_to_segment );
        module.def( "point_side_to_line2D", &point_side_to_line );
        module.def( "point_side_to_plane3D", &point_side_to_plane );
        module.def( "point_side_to_triangle3D", &point_side_to_triangle );
        module.def(
            "point_tetrahedron_position3D", &point_tetrahedron_position );
    }
} // namespace geode
