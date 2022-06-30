/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/information.h>
#include <geode/geometry/sign.h>

namespace geode
{
    void define_sign( pybind11::module& module )
    {
        module.def( "tetrahedron_volume_sign", &tetrahedron_volume_sign );
        module.def( "triangle_area_sign2D",
            ( Sign( * )( const Triangle2D& ) ) & triangle_area_sign );
        module.def( "triangle_area_sign3D",
            ( Sign( * )( const Triangle3D&, local_index_t ) )
                & triangle_area_sign );
    }
} // namespace geode
