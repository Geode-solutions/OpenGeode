/*
 * Copyright (c) 2019 - 2024 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.h>
#include <geode/mesh/core/attribute_coordinate_reference_system.h>
#include <geode/mesh/core/coordinate_reference_system.h>
#include <geode/mesh/core/coordinate_reference_system_manager.h>

#include <geode/tests/common.h>

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    geode::CoordinateReferenceSystemManager3D crs_manager;
    geode::CoordinateReferenceSystemManagerBuilder3D crs_manager_builder{
        crs_manager
    };
    geode::AttributeManager att_manager;
    att_manager.resize( 42 );
    const auto crs_name = "test";
    crs_manager_builder.register_coordinate_reference_system( crs_name,
        std::make_shared< geode::AttributeCoordinateReferenceSystem3D >(
            att_manager ) );
    OPENGEODE_EXCEPTION( crs_manager.nb_coordinate_reference_systems() == 1,
        "[Test] Wrong number of CRS" );
    OPENGEODE_EXCEPTION(
        crs_manager.coordinate_reference_system_exists( crs_name ),
        "[Test] CRS should exist" );
    OPENGEODE_EXCEPTION(
        !crs_manager.coordinate_reference_system_exists( "not a good test" ),
        "[Test] CRS should not exist" );
    auto& crs = crs_manager_builder.coordinate_reference_system( crs_name );
    const geode::Point3D test_point{ { 12, 12, 12 } };
    crs.set_point( 0, test_point );
    crs_manager_builder.set_active_coordinate_reference_system( crs_name );
    const auto& const_crs = crs_manager.active_coordinate_reference_system();
    OPENGEODE_EXCEPTION(
        const_crs.point( 0 ) == test_point, "[Test] Wrong CRS point value" );
    OPENGEODE_EXCEPTION(
        const_crs.point( 1 ) != test_point, "[Test] Wrong CRS point value" );

    const auto crs_name2 = "test2";
    crs_manager_builder.register_coordinate_reference_system( crs_name2,
        std::make_shared< geode::AttributeCoordinateReferenceSystem3D >(
            att_manager, "another crs" ) );
    OPENGEODE_EXCEPTION( crs_manager.nb_coordinate_reference_systems() == 2,
        "[Test] Wrong number of CRS" );
}

OPENGEODE_TEST( "coordinate-reference-manager" )
