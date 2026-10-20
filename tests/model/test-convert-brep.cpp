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
#include <geode/basic/range.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/geode/geode_polygonal_surface.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/helpers/convert_brep_section.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/section_input.hpp>

void test_convert_brep_section()
{
    auto brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "random_dfn.og_brep" ) );
    const auto section =
        std::get< 0 >( geode::convert_brep_into_section( brep, 2 ) );
    geode::OpenGeodeModelException::test(
        section.nb_corners() == 172, "Section should have 172 corners" );
    geode::OpenGeodeModelException::test(
        section.nb_lines() == 288, "Section should have 288 lines" );
    geode::OpenGeodeModelException::test(
        section.nb_surfaces() == 117, "Section should have 117 surfaces" );
    const auto brep2 =
        std::get< 0 >( geode::convert_section_into_brep( section, 2, 10. ) );

    geode::OpenGeodeModelException::test(
        brep2.nb_corners() == 172, "BRep should have 172 corners" );
    geode::OpenGeodeModelException::test(
        brep2.nb_lines() == 288, "BRep should have 288 lines" );
    geode::OpenGeodeModelException::test(
        brep2.nb_surfaces() == 117, "BRep should have 117 surfaces" );
}

void test_convert_brep_with_blocks_into_section()
{
    const auto brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
    geode::OpenGeodeModelException::test(
        brep.nb_blocks() > 0, "BRep should have Blocks" );
    geode::index_t nb_non_block_unique_vertices{ 0 };
    for( const auto uv : geode::Range{ brep.nb_unique_vertices() } )
    {
        for( const auto& cmv : brep.component_mesh_vertices( uv ) )
        {
            if( cmv.component_id.type
                != geode::Block3D::component_type_static() )
            {
                nb_non_block_unique_vertices++;
                break;
            }
        }
    }
    const auto section =
        std::get< 0 >( geode::convert_brep_into_section( brep, 2 ) );
    geode::OpenGeodeModelException::test(
        section.nb_corners() == brep.nb_corners(),
        "Section from BRep with Blocks - wrong number of corners" );
    geode::OpenGeodeModelException::test(
        section.nb_lines() == brep.nb_lines(),
        "Section from BRep with Blocks - wrong number of lines" );
    geode::OpenGeodeModelException::test(
        section.nb_surfaces() == brep.nb_surfaces(),
        "Section from BRep with Blocks - wrong number of surfaces" );
    geode::OpenGeodeModelException::test(
        section.nb_unique_vertices() == nb_non_block_unique_vertices,
        "Section from BRep with Blocks - wrong number of unique vertices" );
    for( const auto uv : geode::Range{ section.nb_unique_vertices() } )
    {
        geode::OpenGeodeModelException::test(
            !section.component_mesh_vertices( uv ).empty(),
            "Section from BRep with Blocks - isolated unique vertex" );
    }
}

void test_extrusion_section_to_brep()
{
    const auto section = geode::load_section(
        absl::StrCat( geode::DATA_PATH, "fractures.og_sctn" ) );
    geode::SectionExtruderOptions options;
    options.axis_to_extrude = 2;
    options.min_coordinate = 0.;
    options.max_coordinate = 10.;
    const auto brep = geode::extrude_section_to_brep( section, options );
    geode::OpenGeodeModelException::test(
        brep.nb_corners() == 2 * section.nb_corners(),
        "Extruded BRep - wrong number of corners." );
    geode::OpenGeodeModelException::test(
        brep.nb_lines() == 2 * section.nb_lines() + section.nb_corners(),
        "Extruded BRep - wrong number of lines." );
    geode::OpenGeodeModelException::test(
        brep.nb_surfaces() == 2 * section.nb_surfaces() + section.nb_lines(),
        "Extruded BRep - wrong number of surfaces." );
    geode::OpenGeodeModelException::test(
        brep.nb_blocks() == section.nb_surfaces(),
        "Extruded BRep - wrong number of blocks." );
}

void test_extrusion_quad_section_to_brep()
{
    const auto section =
        geode::load_section( absl::StrCat( geode::DATA_PATH, "quad.og_sctn" ) );
    geode::SectionExtruderOptions options;
    options.axis_to_extrude = 2;
    options.min_coordinate = 0.;
    options.max_coordinate = 10.;
    const auto brep = geode::extrude_section_to_brep( section, options );
    geode::OpenGeodeModelException::test( brep.nb_blocks() == 1,
        "Extruded quad BRep - wrong number of blocks." );
    for( const auto& block : brep.blocks() )
    {
        const auto& mesh = block.mesh();
        geode::OpenGeodeModelException::test( mesh.nb_polyhedra() == 1,
            "Extruded quad BRep - wrong number of polyhedra." );
        geode::OpenGeodeModelException::test(
            mesh.nb_polyhedron_vertices( 0 ) == 8,
            "Extruded quad BRep - polyhedron should be an hexahedron." );
        geode::OpenGeodeModelException::test( mesh.nb_vertices() == 8,
            "Extruded quad BRep - wrong number of block vertices." );
    }
}

void test_extrusion_pentagon_section_to_brep()
{
    geode::Section section;
    geode::SectionBuilder builder{ section };
    const auto& surface = section.surface( builder.add_surface(
        geode::OpenGeodePolygonalSurface2D::impl_name_static() ) );
    auto mesh_builder = builder.surface_mesh_builder( surface );
    mesh_builder->create_point( geode::Point2D{ { 0, 0 } } );
    mesh_builder->create_point( geode::Point2D{ { 1, 0 } } );
    mesh_builder->create_point( geode::Point2D{ { 2, 1 } } );
    mesh_builder->create_point( geode::Point2D{ { 1, 2 } } );
    mesh_builder->create_point( geode::Point2D{ { 0, 1 } } );
    mesh_builder->create_polygon( { 0, 1, 2, 3, 4 } );
    builder.create_unique_vertices( 5 );
    for( const auto v : geode::Range{ 5 } )
    {
        builder.set_unique_vertex( { surface.component_id(), v }, v );
    }
    geode::SectionExtruderOptions options;
    options.axis_to_extrude = 2;
    options.min_coordinate = 0.;
    options.max_coordinate = 10.;
    bool has_thrown{ false };
    try
    {
        const auto brep = geode::extrude_section_to_brep( section, options );
    }
    catch( const geode::OpenGeodeException& /*unused*/ )
    {
        has_thrown = true;
    }
    geode::OpenGeodeModelException::test( has_thrown,
        "Extruding a pentagon should throw an exception." );
}

void test_extrusion_section_with_empty_corner_to_brep()
{
    auto section = geode::load_section(
        absl::StrCat( geode::DATA_PATH, "fractures.og_sctn" ) );
    const auto nb_corners = section.nb_corners();
    geode::SectionBuilder builder{ section };
    builder.add_corner();
    geode::SectionExtruderOptions options;
    options.axis_to_extrude = 2;
    options.min_coordinate = 0.;
    options.max_coordinate = 10.;
    const auto brep = geode::extrude_section_to_brep( section, options );
    geode::OpenGeodeModelException::test(
        brep.nb_lines() == 2 * section.nb_lines() + nb_corners,
        "Extruded BRep with empty Corner - wrong number of lines." );
    for( const auto& line : brep.lines() )
    {
        geode::OpenGeodeModelException::test( line.mesh().nb_edges() > 0,
            "Extruded BRep with empty Corner - empty Line." );
    }
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_convert_brep_section();
    test_convert_brep_with_blocks_into_section();
    test_extrusion_section_to_brep();
    test_extrusion_quad_section_to_brep();
    test_extrusion_pentagon_section_to_brep();
    test_extrusion_section_with_empty_corner_to_brep();
}

OPENGEODE_TEST( "convert-brep" )