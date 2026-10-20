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
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/geometry/point.hpp>

#include <geode/model/helpers/convert_to_mesh.hpp>
#include <geode/model/helpers/simplicial_brep_creator.hpp>
#include <geode/model/helpers/simplicial_section_creator.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/section_input.hpp>

#include <geode/tests/common.hpp>

void run_test_brep()
{
    auto model = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
    const auto curve = std::get< 0 >( geode::convert_brep_into_curve( model ) );
    geode::OpenGeodeModelException::test(
        curve->nb_vertices() == 154, "BRep - Wrong number of curve vertices" );
    geode::OpenGeodeModelException::test(
        curve->nb_edges() == 162, "BRep - Wrong number of curve edges" );
    const auto surface =
        std::get< 0 >( geode::convert_brep_into_surface( model ) );
    geode::OpenGeodeModelException::test( surface->nb_vertices() == 840,
        "BRep - Wrong number of surface vertices" );
    geode::OpenGeodeModelException::test( surface->nb_polygons() == 1716,
        "BRep - Wrong number of surface polygons" );
    const auto solid = std::get< 0 >( geode::convert_brep_into_solid( model ) );
    geode::OpenGeodeModelException::test(
        solid->nb_vertices() == 1317, "BRep - Wrong number of solid vertices" );
    geode::OpenGeodeModelException::test( solid->nb_polyhedra() == 5709,
        "BRep - Wrong number of solid polyhedra" );
}

void run_test_section()
{
    const auto model =
        geode::load_section( absl::StrCat( geode::DATA_PATH, "quad.og_sctn" ) );

    const auto curve =
        std::get< 0 >( geode::convert_section_into_curve( model ) );
    geode::OpenGeodeModelException::test(
        curve->nb_vertices() == 0, "Section - Wrong number of curve vertices" );
    geode::OpenGeodeModelException::test(
        curve->nb_edges() == 0, "Section - Wrong number of curve edges" );
    const auto surface =
        std::get< 0 >( geode::convert_section_into_surface( model ) );
    geode::OpenGeodeModelException::test( surface->nb_vertices() == 4,
        "Section - Wrong number of surface vertices" );
    geode::OpenGeodeModelException::test( surface->nb_polygons() == 1,
        "Section - Wrong number of surface polygons" );
}

geode::Section create_section_with_dangling_components()
{
    geode::Section section;
    geode::SimplicialSectionCreator creator{ section,
        { geode::Point2D{ { 0, 0 } }, geode::Point2D{ { 1, 0 } },
            geode::Point2D{ { 1, 1 } }, geode::Point2D{ { 0, 1 } },
            geode::Point2D{ { 2, 0 } }, geode::Point2D{ { 3, 0 } },
            geode::Point2D{ { 5, 5 } } } };
    const auto corners = creator.create_corners(
        std::vector< geode::CornerDefinition >{
            { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 } } );
    const auto lines = creator.create_lines( corners,
        std::vector< geode::LineDefinition >{ { { 0, 1 } }, { { 1, 2 } },
            { { 2, 3 } }, { { 3, 0 } }, { { 4, 5 } }, { { 0, 2 } } } );
    creator.create_surfaces(
        lines, std::vector< geode::SurfaceDefinition >{ { { 0, 1, 2, 3 },
                   { 0, 1, 3, 1, 2, 3 }, { 0, 1, 2, 3 }, {}, {} } } );
    geode::SectionBuilder builder{ section };
    builder.add_corner();
    return section;
}

void run_test_section_dangling_components()
{
    const auto section = create_section_with_dangling_components();
    const auto [curve, curve_mappings] =
        geode::convert_section_into_curve( section );
    geode::OpenGeodeModelException::test( curve->nb_edges() == 6,
        "Dangling Section - Wrong number of curve edges" );
    geode::OpenGeodeModelException::test(
        curve_mappings.corner_vertices_mapping.size() == 6,
        "Dangling Section - Corners not on Lines should not be mapped in "
        "curve" );
    geode::OpenGeodeModelException::test(
        curve_mappings.line_edges_mapping.size() == 6,
        "Dangling Section - Wrong number of mapped curve edges" );

    const auto [surface, surface_mappings] =
        geode::convert_section_into_surface( section );
    geode::OpenGeodeModelException::test( surface->nb_polygons() == 2,
        "Dangling Section - Wrong number of surface polygons" );
    geode::OpenGeodeModelException::test(
        surface_mappings.corner_vertices_mapping.size() == 4,
        "Dangling Section - Corners not on Surfaces should not be mapped "
        "in surface" );
    geode::OpenGeodeModelException::test(
        surface_mappings.line_edges_mapping.size() == 4,
        "Dangling Section - Lines not on Surface edges should not be "
        "mapped in surface" );
}

void run_test_brep_dangling_components()
{
    geode::BRep brep;
    geode::SimplicialBRepCreator creator{ brep,
        { geode::Point3D{ { 0, 0, 0 } }, geode::Point3D{ { 1, 0, 0 } },
            geode::Point3D{ { 0, 1, 0 } }, geode::Point3D{ { 0, 0, 1 } },
            geode::Point3D{ { 0, 0, -1 } } } };
    const auto corners = creator.create_corners(
        std::vector< geode::CornerDefinition >{ { 3 }, { 4 } } );
    const auto lines = creator.create_lines(
        corners, std::vector< geode::LineDefinition >{ { { 3, 4 } } } );
    const auto surfaces = creator.create_surfaces( lines,
        std::vector< geode::SurfaceDefinition >{
            { { 0, 3, 4 }, { 0, 1, 2 }, {}, {}, {} } } );
    creator.create_blocks(
        surfaces, std::vector< geode::BlockDefinition >{
                      { { 0, 1, 2, 3, 4 }, { 0, 1, 2, 3, 0, 2, 1, 4 }, {}, {},
                          {}, {} } } );
    const auto [solid, mappings] = geode::convert_brep_into_solid( brep );
    geode::OpenGeodeModelException::test( solid->nb_polyhedra() == 2,
        "Dangling BRep - Wrong number of solid polyhedra" );
    geode::OpenGeodeModelException::test(
        mappings.corner_vertices_mapping.size() == 2,
        "Dangling BRep - Wrong number of mapped corners" );
    geode::OpenGeodeModelException::test(
        mappings.line_edges_mapping.size() == 0,
        "Dangling BRep - Line not on solid edges should not be mapped" );
    geode::OpenGeodeModelException::test(
        mappings.surface_polygons_mapping.size() == 0,
        "Dangling BRep - Surface not on solid facets should not be mapped" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    run_test_brep();
    run_test_section();
    run_test_section_dangling_components();
    run_test_brep_dangling_components();
}

OPENGEODE_TEST( "convert-to-mesh" )