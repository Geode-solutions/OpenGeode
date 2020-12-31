/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/logger.h>

#include <geode/mesh/builder/geode_vertex_set_builder.h>
#include <geode/mesh/builder/vertex_set_builder.h>
#include <geode/mesh/core/geode_vertex_set.h>
#include <geode/mesh/io/vertex_set_input.h>
#include <geode/mesh/io/vertex_set_output.h>

#include <geode/tests/common.h>

void test_default_vertex_set( const geode::VertexSet& vertex_set )
{
    const auto type_static = geode::OpenGeodeVertexSet::impl_name_static();
    OPENGEODE_EXCEPTION( vertex_set.impl_name() == type_static,
        "[Test] VertexSet type name is not correct" );

    const auto extension_static =
        geode::OpenGeodeVertexSet::native_extension_static();
    OPENGEODE_EXCEPTION( vertex_set.native_extension() == extension_static,
        "[Test] VertexSet native extension is not correct" );

    OPENGEODE_EXCEPTION( vertex_set.nb_vertices() == 0,
        "[Test] Default VertexSet should have no vertex" );
}

void test_create_vertices(
    const geode::VertexSet& vertex_set, geode::VertexSetBuilder& builder )
{
    builder.create_vertex();
    OPENGEODE_EXCEPTION( vertex_set.nb_vertices() == 1,
        "[Test] VertexSet should have 1 vertex" );
    builder.create_vertices( 5 );
    OPENGEODE_EXCEPTION( vertex_set.nb_vertices() == 6,
        "[Test] VertexSet should have 6 vertices" );
}

void test_delete_vertex(
    const geode::VertexSet& vertex_set, geode::VertexSetBuilder& builder )
{
    std::vector< bool > to_delete( vertex_set.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION( vertex_set.nb_vertices() == 5,
        "[Test] VertexSet should have 5 vertices" );
}

void test_io( const geode::VertexSet& vertex_set, absl::string_view filename )
{
    geode::save_vertex_set( vertex_set, filename );
    geode::load_vertex_set( filename );
    geode::load_vertex_set(
        geode::OpenGeodeVertexSet::impl_name_static(), filename );
}

void test_clone( const geode::VertexSet& vertex_set )
{
    const auto vertex_set2 = vertex_set.clone();
    OPENGEODE_EXCEPTION( vertex_set2->nb_vertices() == 5,
        "[Test] VertexSet2 should have 5 vertices" );
}

void test()
{
    geode::OpenGeodeVertexSet vertex_set;
    test_default_vertex_set( vertex_set );
    auto builder = geode::VertexSetBuilder::create( vertex_set );
    test_create_vertices( vertex_set, *builder );
    test_io(
        vertex_set, absl::StrCat( "test.", vertex_set.native_extension() ) );
    test_delete_vertex( vertex_set, *builder );
    test_clone( vertex_set );
}

OPENGEODE_TEST( "vertex-set" )