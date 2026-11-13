/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <absl/hash/hash.h>
#include <absl/strings/match.h>

#include <geode/basic/logger.hpp>

#include <geode/mesh/builder/geode/geode_vertex_set_builder.hpp>
#include <geode/mesh/builder/vertex_set_builder.hpp>
#include <geode/mesh/core/geode/geode_vertex_set.hpp>
#include <geode/mesh/core/mesh_element.hpp>
#include <geode/mesh/io/vertex_set_input.hpp>
#include <geode/mesh/io/vertex_set_output.hpp>

#include <geode/tests/common.hpp>

void test_default_vertex_set( const geode::VertexSet& vertex_set )
{
    const auto type_static = geode::OpenGeodeVertexSet::impl_name_static();
    geode::OpenGeodeMeshException::test( vertex_set.impl_name() == type_static,
        "VertexSet type name is not correct" );

    const auto extension_static =
        geode::OpenGeodeVertexSet::native_extension_static();
    geode::OpenGeodeMeshException::test(
        vertex_set.native_extension() == extension_static,
        "VertexSet native extension is not correct" );

    geode::OpenGeodeMeshException::test( vertex_set.nb_vertices() == 0,
        "Default VertexSet should have no vertex" );
}

void test_create_vertices(
    const geode::VertexSet& vertex_set, geode::VertexSetBuilder& builder )
{
    builder.create_vertex();
    geode::OpenGeodeMeshException::test(
        vertex_set.nb_vertices() == 1, "VertexSet should have 1 vertex" );
    builder.create_vertices( 5 );
    geode::OpenGeodeMeshException::test(
        vertex_set.nb_vertices() == 6, "VertexSet should have 6 vertices" );
}

void test_delete_vertex(
    const geode::VertexSet& vertex_set, geode::VertexSetBuilder& builder )
{
    std::vector< bool > to_delete( vertex_set.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    geode::OpenGeodeMeshException::test(
        vertex_set.nb_vertices() == 5, "VertexSet should have 5 vertices" );
}

void test_io( const geode::VertexSet& vertex_set, std::string_view filename )
{
    geode::save_vertex_set( vertex_set, filename );
    const auto reload =
        geode::load_one( geode::load_vertex_set( filename ), filename );
    geode_unused( reload );
    const auto vertex_set2 = geode::load_one(
        geode::load_vertex_set(
            geode::OpenGeodeVertexSet::impl_name_static(), filename ),
        filename );
    geode::OpenGeodeMeshException::test( vertex_set2->nb_vertices() == 6,
        "Reloaded VertexSet should have 6 vertices" );
}

template < typename Action >
void test_io_failure( const Action& action,
    geode::OpenGeodeException::TYPE expected_type,
    std::string_view expected_message )
{
    try
    {
        action();
    }
    catch( const geode::OpenGeodeException& exception )
    {
        geode::OpenGeodeMeshException::test( exception.type() == expected_type,
            "[Test] Wrong exception type for: ", exception.what() );
        geode::OpenGeodeMeshException::test(
            absl::StrContains( exception.what(), expected_message ),
            "[Test] Wrong exception message: ", exception.what() );
        return;
    }
    throw geode::OpenGeodeMeshException{ nullptr,
        geode::OpenGeodeException::TYPE::internal,
        "[Test] IO should have failed with: ", expected_message };
}

void test_io_failures( const geode::VertexSet& vertex_set )
{
    test_io_failure(
        [] {
            return geode::load_vertex_set( "unknown.extension" );
        },
        geode::OpenGeodeException::TYPE::data,
        "Cannot load VertexSet from file: unknown.extension: Unknown "
        "extension" );
    test_io_failure(
        [] {
            return geode::load_vertex_set( "missing.og_vts" );
        },
        geode::OpenGeodeException::TYPE::data,
        "Cannot load VertexSet from file: missing.og_vts: [VertexSet] File "
        "\"missing.og_vts\" does not exist" );
    test_io_failure(
        [&vertex_set] {
            return geode::save_vertex_set( vertex_set, "unknown.extension" );
        },
        geode::OpenGeodeException::TYPE::data,
        "Cannot save VertexSet in file: unknown.extension: Unknown "
        "extension" );
}

void test_clone( const geode::VertexSet& vertex_set )
{
    const auto vertex_set_clone = vertex_set.clone();
    geode::OpenGeodeMeshException::test( vertex_set_clone->nb_vertices() == 5,
        "VertexSet2 should have 5 vertices" );
}

void test_move( geode::OpenGeodeVertexSet& vertex_set )
{
    geode::OpenGeodeVertexSet vertex_set_2{ std::move( vertex_set ) };
    geode::OpenGeodeMeshException::test(
        vertex_set_2.nb_vertices() == 5, "VertexSet2 should have 5 vertices" );
}

void test_mesh_element_hash()
{
    const geode::uuid mesh_id;
    for( const auto e : geode::Range{ 10 } )
    {
        const geode::MeshElement element{ mesh_id, e };
        geode::OpenGeodeMeshException::test(
            std::hash< geode::MeshElement >{}( element )
                == absl::Hash< geode::MeshElement >{}( element ),
            "[Test] std::hash of MeshElement should combine its members like "
            "absl::Hash" );
        geode::OpenGeodeMeshException::test(
            std::hash< geode::MeshVertex >{}( geode::MeshVertex{ mesh_id, e } )
                == std::hash< geode::MeshElement >{}( element ),
            "[Test] std::hash of MeshVertex should match MeshElement" );
    }
}

void test_invalid_delete_vertices()
{
    geode::OpenGeodeVertexSet vertex_set;
    auto builder = geode::VertexSetBuilder::create( vertex_set );
    builder->create_vertices( 3 );
    for( const auto& to_delete :
        { std::vector< bool >{ true }, std::vector< bool >{ false },
            std::vector< bool >{ true, false, false, false },
            std::vector< bool >{} } )
    {
        bool thrown{ false };
        try
        {
            geode_unused( builder->delete_vertices( to_delete ) );
        }
        catch( const geode::OpenGeodeException& exception )
        {
            thrown = true;
            geode::OpenGeodeMeshException::test(
                exception.type() == geode::OpenGeodeException::TYPE::data,
                "[Test] A wrong size of to_delete should be a data error" );
        }
        geode::OpenGeodeMeshException::test(
            thrown, "[Test] A wrong size of to_delete should throw" );
        geode::OpenGeodeMeshException::test( vertex_set.nb_vertices() == 3,
            "[Test] The vertex set should be unchanged" );
    }
    geode::OpenGeodeMeshException::test(
        builder->delete_vertices( { false, true, false } )
            == std::vector< geode::index_t >{ 0, geode::NO_ID, 1 },
        "[Test] Wrong mapping of a valid deletion" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_invalid_delete_vertices();
    geode::OpenGeodeVertexSet vertex_set;
    test_default_vertex_set( vertex_set );
    auto builder = geode::VertexSetBuilder::create( vertex_set );
    test_create_vertices( vertex_set, *builder );
    test_io(
        vertex_set, absl::StrCat( "test.", vertex_set.native_extension() ) );
    test_io_failures( vertex_set );
    test_delete_vertex( vertex_set, *builder );
    test_clone( vertex_set );
    test_move( vertex_set );
    test_mesh_element_hash();
}

OPENGEODE_TEST( "vertex-set" )
