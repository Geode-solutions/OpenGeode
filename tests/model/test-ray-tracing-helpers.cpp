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

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/bounding_box.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>

#include <geode/model/helpers/aabb_model_helpers.hpp>
#include <geode/model/helpers/ray_tracing.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/section_input.hpp>

#include <geode/tests/common.hpp>

void test_surface_without_polygon()
{
    auto brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "box_brep.og_brep" ) );
    const auto nb_surfaces = brep.nb_surfaces();
    geode::BRepBuilder builder{ brep };
    // A boundary surface with vertices but without any polygon
    const auto surface_id = builder.add_surface();
    const auto& surface = brep.surface( surface_id );
    builder.surface_mesh_builder( surface )->create_point(
        geode::Point3D{ { 100., 100., 100. } } );
    const auto& block = *brep.blocks().begin();
    builder.add_surface_block_boundary_relationship( surface, block );

    const auto trees = geode::create_surface_meshes_aabb_trees( brep );
    geode::OpenGeodeModelException::test(
        trees.mesh_trees_.size() == nb_surfaces,
        "Surface without polygon should not be in the AABB trees" );
    geode::OpenGeodeModelException::test(
        !trees.mesh_tree_ids_.contains( surface_id ),
        "Surface without polygon should not be in the AABB trees" );

    geode::BRepRayTracing brep_ray_tracing{ brep };
    const geode::Point3D center{ { 20., 20., 20. } };
    geode::OpenGeodeModelException::test(
        brep_ray_tracing.is_point_inside_block( center, block ), "Point [",
        center.string(),
        "] should be inside the block with a surface without polygon." );
    const auto intersections =
        brep_ray_tracing.find_intersections_with_boundaries(
            geode::InfiniteLine3D{ geode::Vector3D{ { 1., 0., 0. } }, center },
            block );
    geode::OpenGeodeModelException::test(
        intersections.at( surface_id ).empty(),
        "Surface without polygon should not have intersections" );

    const auto empty_tree = geode::create_aabb_tree( surface.mesh() );
    geode::OpenGeodeModelException::test(
        !geode::is_point_inside_closed_surface(
            center, surface.mesh(), empty_tree ),
        "Point should be outside a surface without polygon" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_surface_without_polygon();

    // load a 40x40x40 aligned cubic box brep
    auto brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "box_brep.og_brep" ) );
    geode::Point3D center{ { 20., 20., 20. } };

    //  get the block
    geode::BRepRayTracing brep_ray_tracing{ brep };
    const auto block_id = brep_ray_tracing.block_containing_point( center );
    geode::OpenGeodeModelException::test(
        block_id.has_value(), "Failed to recover block_containing_point." );

    // test point inside/outside the block
    geode::Point3D inside{ { 0.00001, 0.00001, 0.00001 } };
    geode::Point3D outside{ { -0.00001, 0.00001, 0.00001 } };
    geode::OpenGeodeModelException::test(
        brep_ray_tracing.is_point_inside_block(
            inside, brep.block( block_id.value() ) ),
        "Point [", inside.string(), "] should be inside the block." );

    geode::OpenGeodeModelException::test(
        !brep_ray_tracing.is_point_inside_block(
            outside, brep.block( block_id.value() ) ),
        "Point [", outside.string(), "] should be outside the block." );

    // load a section with various surfaces to test 2D
    auto section = geode::load_section(
        absl::StrCat( geode::DATA_PATH, "fractures.og_sctn" ) );
    geode::Point2D section_center{ { 230., 240. } };

    geode::SectionRayTracing section_ray_tracing{ section };
    const auto surface_id =
        section_ray_tracing.surface_containing_point( section_center );
    geode::OpenGeodeModelException::test(
        surface_id.has_value(), "Failed to recover surface_containing_point." );
    geode::OpenGeodeModelException::test(
        surface_id.value()
            == geode::uuid{ "00000000-9fa8-42a9-8000-00001948ab25" },
        "surface_containing_point recovered surface ", surface_id->string(),
        " instead of 00000000-9fa8-42a9-8000-00001948ab25" );

    geode::Point2D inside_1{ { 230., 220. } };
    geode::Point2D inside_2{ { 250., 230. } };
    const auto& surface_2 = section.surface(
        geode::uuid{ "00000000-af68-436b-8000-00002d626514" } );
    geode::OpenGeodeModelException::test(
        section_ray_tracing.is_point_inside_surface(
            inside_1, section.surface( surface_id.value() ) )
            && !section_ray_tracing.is_point_inside_surface(
                inside_1, surface_2 ),
        "Point [", inside_1.string(), "] should be inside surface ",
        surface_id->string(), " but not inside surface ",
        surface_2.id().string() );

    geode::OpenGeodeModelException::test(
        !section_ray_tracing.is_point_inside_surface(
            inside_2, section.surface( surface_id.value() ) )
            && section_ray_tracing.is_point_inside_surface(
                inside_2, surface_2 ),
        "Point [", inside_2.string(), "] should be inside surface ",
        surface_2.id().string(), " but not inside surface ",
        surface_id->string() );
}

OPENGEODE_TEST( "ray-tracing-helpers" )