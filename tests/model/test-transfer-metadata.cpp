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

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/detail/transfer_metadata.hpp>
#include <geode/model/representation/core/mapping.hpp>

#include <geode/tests/common.hpp>

geode::uuid add_named_surface(
    const geode::BRep& brep, geode::BRepBuilder& builder, std::string_view name )
{
    const auto& surface = brep.surface( builder.add_surface() );
    builder.set_surface_name( surface, name );
    return surface.id();
}

void test_merged_surface_names()
{
    geode::BRep old_brep;
    geode::BRepBuilder old_builder{ old_brep };
    const auto fault1 =
        add_named_surface( old_brep, old_builder, "fault1_surface" );
    const auto fault2 =
        add_named_surface( old_brep, old_builder, "fault2_surface" );
    const auto merged_faults = add_named_surface(
        old_brep, old_builder, "fault2_surface__fault1_surface" );
    const auto horizon = add_named_surface( old_brep, old_builder, "horizon" );
    const auto unnamed = old_builder.add_surface();
    const auto unnamed_block = old_builder.add_block();

    geode::BRep new_brep;
    geode::BRepBuilder new_builder{ new_brep };
    const auto new_faults = new_builder.add_surface();
    const auto new_horizon = new_builder.add_surface();
    const auto new_unnamed = new_builder.add_surface();
    const auto new_block = new_builder.add_block();

    geode::ModelGenericMapping mapping;
    geode::GenericMapping< geode::uuid > surface_mapping;
    surface_mapping.map( fault1, new_faults );
    surface_mapping.map( fault2, new_faults );
    surface_mapping.map( merged_faults, new_faults );
    surface_mapping.map( horizon, new_horizon );
    surface_mapping.map( unnamed, new_horizon );
    surface_mapping.map( unnamed, new_unnamed );
    mapping.emplace(
        geode::Surface3D::component_type_static(), std::move( surface_mapping ) );
    geode::GenericMapping< geode::uuid > block_mapping;
    block_mapping.map( unnamed_block, new_block );
    mapping.emplace(
        geode::Block3D::component_type_static(), std::move( block_mapping ) );

    geode::detail::transfer_brep_metadata(
        old_brep, new_brep, new_builder, mapping );

    const auto& faults_name = new_brep.surface( new_faults ).name();
    geode::OpenGeodeModelException::test(
        faults_name.value() == "fault1_surface__fault2_surface",
        "Wrong merged surface name: ", faults_name.value() );
    const auto& horizon_name = new_brep.surface( new_horizon ).name();
    geode::OpenGeodeModelException::test( horizon_name.value() == "horizon",
        "Wrong surface name: ", horizon_name.value() );
    geode::OpenGeodeModelException::test(
        !new_brep.surface( new_unnamed ).name().has_value(),
        "Surface mapped from an unnamed surface should not be named" );
    geode::OpenGeodeModelException::test(
        !new_brep.block( new_block ).name().has_value(),
        "Block mapped from an unnamed block should not be named" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_merged_surface_names();
}

OPENGEODE_TEST( "transfer-metadata" )
