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

#include <geode/tests/common.hpp>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/core/point_set.hpp>

#include <geode/model/helpers/compute_unique_vertices.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>

void test_brep()
{
    auto brep = geode::load_brep( absl::StrCat(
        geode::DATA_PATH, "rectangular_cuboid_without_uv.og_brep" ) );
    geode::BRepBuilder brep_builder{ brep };
    geode::compute_model_unique_vertices( brep, brep_builder );
    geode::OpenGeodeModelException::test(
        brep.nb_unique_vertices() == 8, "Wrong number of unique vertices" );
}

geode::uuid add_corner( geode::BRep& brep,
    geode::BRepBuilder& builder,
    const geode::Point3D& point )
{
    const auto corner_id = builder.add_corner();
    builder.corner_mesh_builder( brep.corner( corner_id ) )
        ->create_point( point );
    return corner_id;
}

geode::index_t corner_unique_vertex(
    const geode::BRep& brep, const geode::uuid& corner_id )
{
    return brep.unique_vertex( { brep.corner( corner_id ).component_id(), 0 } );
}

void test_colocated_initial_unique_vertices()
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    const geode::Point3D origin{ { 0, 0, 0 } };
    // Three pre-existing unique vertices at the same location
    builder.create_unique_vertices( 3 );
    for( const auto uv : geode::Range{ 3 } )
    {
        const auto corner_id = add_corner( brep, builder, origin );
        builder.set_unique_vertex(
            { brep.corner( corner_id ).component_id(), 0 }, uv );
    }
    const auto new_colocated = add_corner( brep, builder, origin );
    const auto new_corner =
        add_corner( brep, builder, geode::Point3D{ { 1, 0, 0 } } );
    geode::compute_model_unique_vertices( brep, builder );
    geode::OpenGeodeModelException::test( brep.nb_unique_vertices() == 4,
        "Wrong number of unique vertices with colocated initial unique "
        "vertices" );
    geode::OpenGeodeModelException::test(
        corner_unique_vertex( brep, new_colocated ) == 0,
        "New vertex colocated with initial unique vertices should use the "
        "first one" );
    geode::OpenGeodeModelException::test(
        corner_unique_vertex( brep, new_corner ) == 3,
        "Wrong unique vertex for new vertex" );
}

void test_isolated_initial_unique_vertex()
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    builder.create_unique_vertices( 1 );
    const auto corner =
        add_corner( brep, builder, geode::Point3D{ { 1, 0, 0 } } );
    geode::compute_model_unique_vertices( brep, builder );
    geode::OpenGeodeModelException::test( brep.nb_unique_vertices() == 2,
        "Wrong number of unique vertices with isolated unique vertex" );
    geode::OpenGeodeModelException::test(
        corner_unique_vertex( brep, corner ) == 1,
        "Wrong unique vertex with isolated unique vertex" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_brep();
    test_colocated_initial_unique_vertices();
    test_isolated_initial_unique_vertex();
}

OPENGEODE_TEST( "convert-brep" )