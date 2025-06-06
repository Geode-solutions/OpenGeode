/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <fstream>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/constant_attribute.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/geode/geode_point_set_builder.hpp>
#include <geode/mesh/core/geode/geode_point_set.hpp>
#include <geode/mesh/io/point_set_input.hpp>
#include <geode/mesh/io/point_set_output.hpp>

#include <geode/tests/common.hpp>

void test_create_vertices(
    const geode::PointSet3D& point_set, geode::PointSetBuilder3D& builder )
{
    builder.create_point( geode::Point3D{ { 0.1, 0.2, 0.3 } } );
    builder.create_point( geode::Point3D{ { 2.1, 9.4, 6.7 } } );
    OPENGEODE_EXCEPTION( point_set.nb_vertices() == 2,
        "[Test] PointSet should have 2 vertices" );
    builder.create_vertices( 2 );
    OPENGEODE_EXCEPTION( point_set.nb_vertices() == 4,
        "[Test] PointSet should have 4 vertices" );
    OPENGEODE_EXCEPTION( point_set.point( 2 ) == geode::Point3D(),
        "[Test] Default coordinates are not correct" );
    builder.set_point( 2, geode::Point3D{ { 2.3, 5.0, -1.2 } } );
    OPENGEODE_EXCEPTION(
        point_set.point( 2 ) == geode::Point3D( { 2.3, 5.0, -1.2 } ),
        "[Test] Point coordinates have not been correctly set" );
}

void test_permutation(
    const geode::PointSet3D& point_set, geode::PointSetBuilder3D& builder )
{
    std::vector< geode::index_t > permutation{ 2, 0, 3, 1 };
    builder.permute_vertices( permutation );
    OPENGEODE_EXCEPTION( point_set.point( 2 ) == geode::Point3D(),
        "[Test] Point coordinates have not been correctly permuted" );
    OPENGEODE_EXCEPTION(
        point_set.point( 0 ) == geode::Point3D( { 2.3, 5.0, -1.2 } ),
        "[Test] Point coordinates have not been correctly permuted" );
}

void test_bounding_box( const geode::PointSet3D& point_set )
{
    geode::Point3D answer_min{ { 0.0, 0.0, -1.2 } };
    geode::Point3D answer_max{ { 2.3, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( point_set.bounding_box().min() == answer_min,
        "[Test] Wrong computation of bounding box (min)" );
    OPENGEODE_EXCEPTION( point_set.bounding_box().max() == answer_max,
        "[Test] Wrong computation of bounding box (max)" );
}

void test_create_vertex_attribute( const geode::PointSet3D& point_set )
{
    auto attribute =
        point_set.vertex_attribute_manager()
            .find_or_create_attribute< geode::ConstantAttribute, bool >(
                "test", true );
    OPENGEODE_EXCEPTION( attribute->value() == true,
        "[Test] PointSet attribute value should be true" );
}

void test_delete_vertex(
    const geode::PointSet3D& point_set, geode::PointSetBuilder3D& builder )
{
    std::vector< bool > to_delete( point_set.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION( point_set.nb_vertices() == 3,
        "[Test] PointSet should have 3 vertices" );
    geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( point_set.point( 2 ) == answer,
        "[Test] PointSet vertex coordinates are not correct" );
}

void test_io( const geode::PointSet3D& point_set, std::string_view filename )
{
    geode::save_point_set( point_set, filename );
    const auto reload = geode::load_point_set< 3 >( filename );
    geode_unused( reload );
    const auto point_set2 = geode::load_point_set< 3 >(
        geode::OpenGeodePointSet3D::impl_name_static(), filename );
    for( const auto vertex_id : geode::Range{ point_set.nb_vertices() } )
    {
        OPENGEODE_EXCEPTION(
            point_set.point( vertex_id )
                .inexact_equal( point_set2->point( vertex_id ) ),
            "[Test] Wrong reloaded mesh point coordinates." );
    }
}

void test_clone( const geode::PointSet3D& point_set )
{
    const auto point_set2 = point_set.clone();
    OPENGEODE_EXCEPTION( point_set2->nb_vertices() == 3,
        "[Test] PointSet2 should have 3 vertices" );

    const auto attribute =
        point_set2->vertex_attribute_manager().find_attribute< bool >( "test" );
    OPENGEODE_EXCEPTION( attribute->value( 0 ) == true,
        "[Test] PointSet2 attribute value should be true" );

    const geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( point_set2->point( 2 ) == answer,
        "[Test] PointSet2 vertex coordinates are not correct" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto point_set = geode::PointSet3D::create(
        geode::OpenGeodePointSet3D::impl_name_static() );
    auto builder = geode::PointSetBuilder3D::create( *point_set );
    test_create_vertices( *point_set, *builder );
    test_bounding_box( *point_set );
    test_create_vertex_attribute( *point_set );
    test_io(
        *point_set, absl::StrCat( "test.", point_set->native_extension() ) );

    test_permutation( *point_set, *builder );
    test_delete_vertex( *point_set, *builder );
    test_clone( *point_set );
}

OPENGEODE_TEST( "point-set" )