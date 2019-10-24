/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>
#include <geode/basic/point.h>

#include <geode/mesh/builder/geode_point_set_builder.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/io/point_set_input.h>
#include <geode/mesh/io/point_set_output.h>

void test_create_vertices(
    const geode::PointSet3D& point_set, geode::PointSetBuilder3D& builder )
{
    builder.create_point( { { 0.1, 0.2, 0.3 } } );
    builder.create_point( { { 2.1, 9.4, 6.7 } } );
    OPENGEODE_EXCEPTION( point_set.nb_vertices() == 2,
        "[Test] PointSet should have 2 vertices" );
    builder.create_vertices( 2 );
    OPENGEODE_EXCEPTION( point_set.nb_vertices() == 4,
        "[Test] PointSet should have 4 vertices" );
    OPENGEODE_EXCEPTION( point_set.point( 2 ) == geode::Point3D(),
        "[Test] Default coordinates are not correct" );
    builder.set_point( 2, { { 2.3, 5.0, -1.2 } } );
    OPENGEODE_EXCEPTION(
        point_set.point( 2 ) == geode::Point3D( { 2.3, 5.0, -1.2 } ),
        "[Test] Point coordinates have not been correctly set" );
}

void test_create_vertex_attribute( const geode::PointSet3D& point_set )
{
    auto attribute =
        point_set.vertex_attribute_manager()
            .find_or_create_attribute< geode::ConstantAttribute, bool >(
                "test" );
    attribute->value() = true;
    OPENGEODE_EXCEPTION( attribute->value() == true,
        "[Test] PointSet attribute value should be true" );
}

void test_delete_vertex(
    const geode::PointSet3D& point_set, geode::PointSetBuilder3D& builder )
{
    std::vector< bool > to_delete( point_set.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION( point_set.nb_vertices() == 3,
        "[Test] PointSet should have 3 vertices" );
    geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( point_set.point( 0 ) == answer,
        "[Test] PointSet vertex coordinates are not correct" );
}

void test_io( const geode::PointSet3D& point_set, const std::string& filename )
{
    DEBUG( point_set.nb_vertices() );
    save_point_set( point_set, filename );
    auto new_point_set = geode::PointSet3D::create(
        geode::OpenGeodePointSet3D::type_name_static() );
    load_point_set( *new_point_set, filename );
}

void test_clone( const geode::PointSet3D& point_set )
{
    auto point_set2 = point_set.clone();
    OPENGEODE_EXCEPTION( point_set2->nb_vertices() == 3,
        "[Test] PointSet2 should have 3 vertices" );

    auto attribute =
        point_set2->vertex_attribute_manager().find_attribute< bool >( "test" );
    OPENGEODE_EXCEPTION( attribute->value( 0 ) == true,
        "[Test] PointSet2 attribute value should be true" );

    geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( point_set2->point( 0 ) == answer,
        "[Test] PointSet2 vertex coordinates are not correct" );
}

int main()
{
    using namespace geode;

    try
    {
        auto point_set =
            PointSet3D::create( OpenGeodePointSet3D::type_name_static() );
        auto builder = PointSetBuilder3D::create( *point_set );
        test_create_vertices( *point_set, *builder );
        test_create_vertex_attribute( *point_set );
        auto base_file = "test." + point_set->native_extension();
        test_io( *point_set, base_file );
        test_delete_vertex( *point_set, *builder );
        test_clone( *point_set );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
