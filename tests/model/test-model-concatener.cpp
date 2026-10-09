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

#include <geode/model/helpers/model_concatener.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/brep_output.hpp>

#include <geode/tests/common.hpp>

void check_concatenation(
    const geode::BRep& brep, absl::Span< const geode::index_t > nb_components )
{
    geode::OpenGeodeModelException::test( brep.nb_corners() == nb_components[0],
        "Concatenated model has ", brep.nb_corners(), " Corners, should have ",
        nb_components[0], " Corners" );
    geode::OpenGeodeModelException::test( brep.nb_lines() == nb_components[1],
        "Concatenated model has ", brep.nb_lines(), " Lines, should have ",
        nb_components[1], " Lines" );
    geode::OpenGeodeModelException::test(
        brep.nb_surfaces() == nb_components[2], "Concatenated model has ",
        brep.nb_surfaces(), " Surfaces, should have ", nb_components[2],
        " Surfaces" );
    geode::OpenGeodeModelException::test( brep.nb_blocks() == nb_components[3],
        "Concatenated model has ", brep.nb_blocks(), " Blocks, should have ",
        nb_components[3], " Blocks" );
    geode::OpenGeodeModelException::test(
        brep.nb_model_boundaries() == nb_components[4],
        "Concatenated model has ", brep.nb_model_boundaries(),
        " ModelBoundaries, should have ", nb_components[4],
        " ModelBoundaries" );
}

void add_collections( geode::BRep& brep )
{
    geode::BRepBuilder builder{ brep };
    const auto& corner_collection =
        brep.corner_collection( builder.add_corner_collection() );
    for( const auto& corner : brep.corners() )
    {
        builder.add_corner_in_corner_collection( corner, corner_collection );
    }
    const auto& line_collection =
        brep.line_collection( builder.add_line_collection() );
    for( const auto& line : brep.lines() )
    {
        builder.add_line_in_line_collection( line, line_collection );
    }
    const auto& surface_collection =
        brep.surface_collection( builder.add_surface_collection() );
    for( const auto& surface : brep.surfaces() )
    {
        builder.add_surface_in_surface_collection(
            surface, surface_collection );
    }
    const auto& block_collection =
        brep.block_collection( builder.add_block_collection() );
    for( const auto& block : brep.blocks() )
    {
        builder.add_block_in_block_collection( block, block_collection );
    }
}

template < typename CollectionRange >
void check_collection_items( const geode::BRep& brep,
    const geode::BRep& brep2,
    CollectionRange&& collections,
    const geode::ModelCopyMapping& mapping )
{
    for( const auto& collection : collections )
    {
        const auto& copy_id =
            mapping.at( collection.component_type() ).in2out( collection.id() );
        geode::OpenGeodeModelException::test(
            brep.nb_items( copy_id ) == brep2.nb_items( collection.id() ),
            "Wrong number of items in concatenated ",
            collection.component_type().get() );
    }
}

void check_collections( const geode::BRep& brep,
    const geode::BRep& brep2,
    const geode::ModelCopyMapping& mapping )
{
    check_collection_items( brep, brep2, brep2.corner_collections(), mapping );
    check_collection_items( brep, brep2, brep2.line_collections(), mapping );
    check_collection_items( brep, brep2, brep2.surface_collections(), mapping );
    check_collection_items( brep, brep2, brep2.block_collections(), mapping );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    auto brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "prism_curve.og_brep" ) );
    auto brep2 = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "dangling.og_brep" ) );
    add_collections( brep2 );
    std::array< geode::index_t, 5 > nb_components{ brep.nb_corners()
                                                       + brep2.nb_corners(),
        brep.nb_lines() + brep2.nb_lines(),
        brep.nb_surfaces() + brep2.nb_surfaces(),
        brep.nb_blocks() + brep2.nb_blocks(),
        brep.nb_model_boundaries() + brep2.nb_model_boundaries() };
    geode::BRepConcatener concatener{ brep };
    const auto mapping = concatener.concatenate( brep2 );
    check_concatenation( brep, nb_components );
    check_collections( brep, brep2, mapping );
    geode::save_brep( brep, "concatenated_brep.og_brep" );
}

OPENGEODE_TEST( "model-concatener" )