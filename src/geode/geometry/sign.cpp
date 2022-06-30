/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/geometry/sign.h>

#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/private/position_from_sides.h>
#include <geode/geometry/private/predicates.h>

namespace
{
    const std::array< std::array< geode::local_index_t, 2 >, 3 > new_axis{
        { std::array< geode::local_index_t, 2 >{ { 1, 2 } },
            std::array< geode::local_index_t, 2 >{ { 2, 0 } },
            std::array< geode::local_index_t, 2 >{ { 0, 1 } } }
    };
} // namespace

namespace geode
{
    Sign tetrahedron_volume_sign( const Tetrahedron& tetra )
    {
        return detail::side( GEO::PCK::orient_3d( tetra.vertices()[0],
            tetra.vertices()[1], tetra.vertices()[2], tetra.vertices()[3] ) );
    }

    Sign triangle_area_sign( const Triangle2D& triangle )
    {
        return detail::side( GEO::PCK::orient_2d( triangle.vertices()[0],
            triangle.vertices()[1], triangle.vertices()[2] ) );
    }

    Sign triangle_area_sign( const Triangle3D& triangle, local_index_t axis )
    {
        const auto axis1 = new_axis[axis][0];
        const auto axis2 = new_axis[axis][1];
        const Point2D pt0{ { triangle.vertices()[0].get().value( axis1 ),
            triangle.vertices()[0].get().value( axis2 ) } };
        const Point2D pt1{ { triangle.vertices()[1].get().value( axis1 ),
            triangle.vertices()[1].get().value( axis2 ) } };
        const Point2D pt2{ { triangle.vertices()[2].get().value( axis1 ),
            triangle.vertices()[2].get().value( axis2 ) } };
        return triangle_area_sign( { pt0, pt1, pt2 } );
    }
} // namespace geode
