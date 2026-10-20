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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/helpers/model_component_filter.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>

#include <geode/tests/common.hpp>

void check_brep_prism( const geode::BRep& brep )
{
    geode::OpenGeodeModelException::test( brep.nb_corners() == 7,
        "Filtered prism model has ", brep.nb_corners(),
        " Corners, should have 7 Corners" );
    geode::OpenGeodeModelException::test( brep.nb_lines() == 10,
        "Filtered prism model has ", brep.nb_lines(),
        " Lines, should have 10 Lines" );
    geode::OpenGeodeModelException::test( brep.nb_surfaces() == 5,
        "Filtered prism model has ", brep.nb_surfaces(),
        " Surfaces, should have 5 Surfaces" );
    geode::OpenGeodeModelException::test( brep.nb_blocks() == 1,
        "Filtered prism model has ", brep.nb_blocks(),
        " Blocks, should have 1 Block" );
}

void check_brep_dangling( const geode::BRep& brep )
{
    geode::OpenGeodeModelException::test( brep.nb_corners() == 8,
        "Filtered dangling model has ", brep.nb_corners(),
        " Corners, should have 8 Corners" );
    geode::OpenGeodeModelException::test( brep.nb_lines() == 12,
        "Filtered dangling model has ", brep.nb_lines(),
        " Lines, should have 12 Lines" );
    geode::OpenGeodeModelException::test( brep.nb_surfaces() == 6,
        "Filtered dangling model has ", brep.nb_surfaces(),
        " Surfaces, should have 6 Surfaces" );
    geode::OpenGeodeModelException::test( brep.nb_blocks() == 1,
        "Filtered dangling model has ", brep.nb_blocks(),
        " Blocks, should have 1 Block" );
}

void check_brep_dangling_with_inactive_surface( const geode::BRep& brep )
{
    geode::OpenGeodeModelException::test( brep.nb_corners() == 8,
        "Filtered dangling model has ", brep.nb_corners(),
        " Corners, should have 8 Corners" );
    geode::OpenGeodeModelException::test( brep.nb_lines() == 12,
        "Filtered dangling model has ", brep.nb_lines(),
        " Lines, should have 12 Lines" );
    geode::OpenGeodeModelException::test( brep.nb_surfaces() == 7,
        "Filtered dangling model with inactive surface has ",
        brep.nb_surfaces(), " Surfaces, should have 7 Surfaces" );
    geode::OpenGeodeModelException::test( brep.nb_active_surfaces() == 6,
        "Filtered dangling model with inactive surface has ",
        brep.nb_active_surfaces(), " Surfaces, should have 6 Surfaces" );
    geode::OpenGeodeModelException::test( brep.nb_blocks() == 1,
        "Filtered dangling model has ", brep.nb_blocks(),
        " Blocks, should have 1 Block" );
}

void add_inactive_surface( geode::BRep& brep )
{
    geode::BRepBuilder builder{ brep };
    const auto new_surface_id = builder.add_surface();
    builder.set_surface_active( brep.surface( new_surface_id ), false );
}

void check_same_brep( const geode::BRep& brep, const geode::BRep& other )
{
    geode::OpenGeodeModelException::test(
        brep.nb_corners() == other.nb_corners()
            && brep.nb_lines() == other.nb_lines()
            && brep.nb_surfaces() == other.nb_surfaces()
            && brep.nb_blocks() == other.nb_blocks(),
        "[Batch removal] Different number of components" );
    geode::OpenGeodeModelException::test(
        brep.nb_components_with_relations()
            == other.nb_components_with_relations(),
        "[Batch removal] Different number of components with relations" );
    for( const auto& [type, ids] : brep.mesh_components() )
    {
        auto sorted_ids = ids;
        auto other_ids = other.mesh_components().at( type );
        absl::c_sort( sorted_ids );
        absl::c_sort( other_ids );
        geode::OpenGeodeModelException::test( sorted_ids == other_ids,
            "[Batch removal] Different mesh component registry" );
        for( const auto& id : ids )
        {
            geode::OpenGeodeModelException::test(
                brep.nb_relations( id ) == other.nb_relations( id ),
                "[Batch removal] Different number of relations" );
        }
    }
    geode::OpenGeodeModelException::test(
        brep.nb_unique_vertices() == other.nb_unique_vertices(),
        "[Batch removal] Different number of unique vertices" );
    for( const auto uv : geode::Range{ brep.nb_unique_vertices() } )
    {
        auto cmvs = brep.component_mesh_vertices( uv );
        auto other_cmvs = other.component_mesh_vertices( uv );
        absl::c_sort( cmvs );
        absl::c_sort( other_cmvs );
        geode::OpenGeodeModelException::test( cmvs == other_cmvs,
            "[Batch removal] Different unique vertex ", uv );
    }
}

void test_batch_removal()
{
    auto one_by_one = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "dangling.og_brep" ) );
    auto batch = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "dangling.og_brep" ) );
    std::vector< geode::uuid > surfaces;
    for( const auto& surface : one_by_one.surfaces() )
    {
        if( surfaces.size() < 3 )
        {
            surfaces.push_back( surface.id() );
        }
    }
    std::vector< geode::uuid > lines;
    for( const auto& line : one_by_one.lines() )
    {
        if( lines.size() < 4 )
        {
            lines.push_back( line.id() );
        }
    }
    geode::BRepBuilder one_by_one_builder{ one_by_one };
    for( const auto& surface : surfaces )
    {
        one_by_one_builder.remove_surface( one_by_one.surface( surface ) );
    }
    for( const auto& line : lines )
    {
        one_by_one_builder.remove_line( one_by_one.line( line ) );
    }
    geode::BRepBuilder batch_builder{ batch };
    batch_builder.remove_surfaces( surfaces );
    lines.push_back( lines.front() );
    batch_builder.remove_lines( lines );
    batch_builder.remove_corners( {} );
    check_same_brep( one_by_one, batch );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    auto brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "prism_curve.og_brep" ) );
    geode::filter_brep_components_with_regards_to_blocks( brep );
    check_brep_prism( brep );

    auto brep2 = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "dangling.og_brep" ) );
    geode::filter_brep_components_with_regards_to_blocks( brep2 );
    check_brep_dangling( brep2 );
    add_inactive_surface( brep2 );
    geode::filter_brep_components_with_regards_to_blocks( brep2 );
    check_brep_dangling_with_inactive_surface( brep2 );
    test_batch_removal();
}

OPENGEODE_TEST( "model-filter" )