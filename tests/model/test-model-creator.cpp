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

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/simplicial_brep_creator.hpp>
#include <geode/model/helpers/simplicial_section_creator.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/brep_output.hpp>
#include <geode/model/representation/io/section_output.hpp>

#include <geode/tests/common.hpp>

void test_create_brep_from_bbox()
{
    geode::BoundingBox3D box;
    box.add_point( geode::Point3D{ { 0, 0, 0 } } );
    box.add_point( geode::Point3D{ { 1, 1, 1 } } );
    const auto brep = geode::create_model_from_bounding_box( box );
    geode::save_brep( brep, "brep_from_bbox.og_brep" );
}

void test_create_brep_with_dangling_components()
{
    std::vector< geode::Point3D > points{
        geode::Point3D{ { 0, 0, 0 } }, // 0
        geode::Point3D{ { 1, 0, 0 } }, // 1
        geode::Point3D{ { 1, 1, 0 } }, // 2
        geode::Point3D{ { 0, 1, 0 } }, // 3
        geode::Point3D{ { 0, 0, 1 } }, // 4
        geode::Point3D{ { 1, 0, 1 } }, // 5
        geode::Point3D{ { 1, 1, 1 } }, // 6
        geode::Point3D{ { 0, 1, 1 } }, // 7
        geode::Point3D{ { 0, 0, 2 } }, // 8
        geode::Point3D{ { 1, 0, 2 } }, // 9
        geode::Point3D{ { 1, 1, 2 } }, // 10
        geode::Point3D{ { 0, 1, 2 } }, // 11
        geode::Point3D{ { 1, 0.5, 1.5 } }, // 12
    };
    geode::BRep brep;
    geode::SimplicialBRepCreator creator{ brep, points };
    std::vector< geode::CornerDefinition > corners{
        { 0 },
        { 1 },
        { 2 },
        { 3 },
        { 4 },
        { 5 },
        { 6 },
        { 7 },
        { 8 },
        { 9 },
        { 10 },
        { 11 },
        { 12 },
    };
    const auto corner_uuids = creator.create_corners( corners );

    std::vector< geode::LineDefinition > lines{
        { { 0, 1 } },
        { { 1, 2 } },
        { { 2, 3 } },
        { { 3, 0 } },
        { { 4, 5 } },
        { { 5, 6 } },
        { { 6, 7 } },
        { { 7, 4 } },
        { { 0, 4 } },
        { { 1, 5 } },
        { { 2, 6 } },
        { { 3, 7 } },
        { { 5, 9 } },
        { { 9, 10 } },
        { { 10, 6 } },
        { { 4, 8 } },
        { { 8, 11 } },
    };
    const auto line_uuids = creator.create_lines( corner_uuids, lines );

    std::vector< geode::SurfaceDefinition > surfaces{
        { { 0, 1, 2, 3 }, { 0, 1, 2, 0, 2, 3 }, { 0, 1, 2, 3 }, {}, {} },
        { { 0, 1, 5, 4 }, { 0, 1, 2, 0, 2, 3 }, { 0, 4, 8, 9 }, {}, {} },
        { { 1, 2, 6, 5 }, { 0, 1, 2, 0, 2, 3 }, { 1, 5, 9, 10 }, {}, {} },
        { { 2, 3, 7, 6 }, { 0, 1, 2, 0, 2, 3 }, { 2, 6, 10, 11 }, {}, {} },
        { { 0, 3, 7, 4 }, { 0, 1, 2, 0, 2, 3 }, { 3, 7, 11, 8 }, {}, {} },
        { { 4, 5, 6, 7 }, { 0, 1, 2, 0, 2, 3 }, {}, {}, {} },
        { { 5, 6, 10, 9, 12 }, { 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4 },
            { 5, 12, 13, 14 }, {}, { 12 } },
    };
    const auto surface_uuids =
        creator.create_surfaces( corner_uuids, line_uuids, surfaces );

    std::vector< geode::BlockDefinition > blocks{
        { {}, {}, { 0, 1, 2, 3, 4, 5 }, {}, {}, {} },
    };
    creator.create_blocks( corner_uuids, line_uuids, surface_uuids, blocks );
    geode::OpenGeodeModelException::test(
        brep.nb_blocks() == blocks.size(), "Wrong number of blocks" );
    geode::index_t nb_embedding_blocks{ 0 };
    geode::index_t nb_embedding_surfaces{ 0 };
    for( const auto& corner : brep.corners() )
    {
        nb_embedding_blocks += brep.nb_embedding_blocks( corner );
        nb_embedding_surfaces += brep.nb_embedding_surfaces( corner );
    }
    geode::OpenGeodeModelException::test(
        nb_embedding_blocks == 0, "Wrong number of corner embedding blocks" );
    geode::OpenGeodeModelException::test( nb_embedding_surfaces == 1,
        "Wrong number of corner embedding surfaces" );
    geode::save_brep( brep, "test.og_brep" );
}

void test_section()
{
    std::vector< geode::Point2D > points{
        geode::Point2D{ { 0, 0 } },
        geode::Point2D{ { 1, 0 } },
        geode::Point2D{ { 1, 1 } },
        geode::Point2D{ { 0, 1 } },
        geode::Point2D{ { 0.5, 0.2 } },
        geode::Point2D{ { 0.5, 0.8 } },
        geode::Point2D{ { 0, 0.5 } },
        geode::Point2D{ { 0.5, 0 } },
        geode::Point2D{ { 1, 0.5 } },
        geode::Point2D{ { 0.5, 1 } },
        geode::Point2D{ { 0.5, 0.5 } },
    };
    const auto nb_vertices = points.size();
    geode::Section section;
    geode::SimplicialSectionCreator creator{ section, std::move( points ) };
    geode::OpenGeodeModelException::test(
        section.nb_unique_vertices() == nb_vertices,
        "Wrong number of unique vertices" );

    std::vector< geode::CornerDefinition > corner_definitions{
        { 0 },
        { 1 },
        { 2 },
        { 3 },
        { 4 },
        { 5 },
    };
    const auto corners = creator.create_corners( corner_definitions );
    geode::OpenGeodeModelException::test(
        corners.size() == corner_definitions.size(),
        "Wrong number of corners" );

    std::vector< geode::LineDefinition > line_definitions{
        { { 0, 7, 1 } },
        { { 1, 8, 2 } },
        { { 2, 9, 3 } },
        { { 3, 6, 0 } },
        { { 4, 10, 5 } },
    };
    const auto lines = creator.create_lines( corners, line_definitions );
    geode::OpenGeodeModelException::test(
        lines.size() == line_definitions.size(), "Wrong number of lines" );
    for( const auto l : geode::Indices{ lines } )
    {
        const auto& line = section.line( lines[l] );
        const auto& mesh = line.mesh();
        geode::OpenGeodeModelException::test(
            mesh.nb_vertices() == line_definitions[l].vertices.size(),
            "Wrong number of Line vertices" );
        geode::OpenGeodeModelException::test(
            mesh.nb_edges() == line_definitions[l].vertices.size() - 1,
            "Wrong number of Line edges" );
        for( const auto& corner : section.boundaries( line ) )
        {
            if( corner.id() != corners[line_definitions[l].vertices.front()]
                && corner.id() != corners[line_definitions[l].vertices.back()] )
            {
                geode::OpenGeodeModelException::test(
                    false, "Missing Corner in Line boundary" );
            }
        }
    }

    std::vector< geode::SurfaceDefinition > surface_definitions{
        { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10 },
            { 0, 7, 4, 7, 1, 4, 1, 8, 4, 4, 8, 10, 8, 5, 10, 8, 2, 5, 2, 9, 5,
                9, 3, 5, 3, 6, 5, 6, 11, 5, 6, 4, 11, 6, 0, 4 },
            { 0, 1, 2, 3 }, { 4 }, {} },
    };
    const auto surfaces = creator.create_surfaces( lines, surface_definitions );
    geode::OpenGeodeModelException::test(
        surfaces.size() == surface_definitions.size(),
        "Wrong number of surfaces" );
    for( const auto s : geode::Indices{ surfaces } )
    {
        const auto& surface = section.surface( surfaces[s] );
        const auto& mesh = surface.mesh();
        geode::OpenGeodeModelException::test(
            mesh.nb_vertices() == surface_definitions[s].vertices.size(),
            "Wrong number of Surface vertices" );
        geode::OpenGeodeModelException::test(
            mesh.nb_polygons() == surface_definitions[s].triangles.size() / 3,
            "Wrong number of Surface triangles" );
        for( const auto& line : section.boundaries( surface ) )
        {
            bool found{ false };
            for( const auto b : surface_definitions[s].boundaries )
            {
                if( line.id() == lines[b] )
                {
                    found = true;
                    break;
                }
            }
            geode::OpenGeodeModelException::test(
                found, "Missing Line in Surface boundary" );
        }
        for( const auto& line : section.internal_lines( surface ) )
        {
            bool found{ false };
            for( const auto i : surface_definitions[s].internals )
            {
                if( line.id() == lines[i] )
                {
                    found = true;
                    break;
                }
            }
            geode::OpenGeodeModelException::test(
                found, "Missing Line in Surface internal lines" );
        }
    }

    std::vector< geode::BoundaryDefinition > boundary_definitions{
        { { 0 } },
        { { 1 } },
        { { 2 } },
        { { 3 } },
    };
    const auto boundaries =
        creator.create_model_boundaries( lines, boundary_definitions );
    geode::OpenGeodeModelException::test(
        boundaries.size() == boundary_definitions.size(),
        "Wrong number of boundaries" );
    for( const auto b : geode::Indices{ boundaries } )
    {
        const auto& boundary = section.model_boundary( boundaries[b] );
        for( const auto& line : section.items( boundary.id() ) )
        {
            bool found{ false };
            for( const auto b2 : boundary_definitions[b].boundaries )
            {
                if( line.id == lines[b2] )
                {
                    found = true;
                    break;
                }
            }
            geode::OpenGeodeModelException::test(
                found, "Missing Line in ModelBoundary" );
        }
    }
    geode::save_section( section, "test_section.og_sctn" );
}

void test_create_in_non_empty_section()
{
    geode::Section section;
    {
        geode::SimplicialSectionCreator creator{ section,
            { geode::Point2D{ { 0, 0 } }, geode::Point2D{ { 1, 0 } } } };
        creator.create_corners(
            std::vector< geode::CornerDefinition >{ { 0 }, { 1 } } );
    }
    const auto nb_existing_vertices = section.nb_unique_vertices();
    geode::SimplicialSectionCreator creator{ section,
        { geode::Point2D{ { 2, 0 } }, geode::Point2D{ { 3, 0 } },
            geode::Point2D{ { 4, 0 } } } };
    const std::vector< geode::CornerDefinition > corner_definitions{ { 0 },
        { 1 }, { 2 } };
    const auto corners = creator.create_corners( corner_definitions );
    for( const auto c : geode::Indices{ corners } )
    {
        const auto& corner = section.corner( corners[c] );
        geode::OpenGeodeModelException::test(
            section.unique_vertex( { corner.component_id(), 0 } )
                == nb_existing_vertices + corner_definitions[c].vertex,
            "[non empty model] Wrong Corner unique vertex" );
    }
    const std::vector< geode::LineDefinition > line_definitions{
        { { 0, 1, 2 } }
    };
    const auto lines = creator.create_lines( corners, line_definitions );
    const auto& line = section.line( lines.front() );
    for( const auto v : geode::Range{ line.mesh().nb_vertices() } )
    {
        const auto unique_vertex =
            section.unique_vertex( { line.component_id(), v } );
        geode::OpenGeodeModelException::test(
            unique_vertex
                == nb_existing_vertices + line_definitions.front().vertices[v],
            "[non empty model] Wrong Line unique vertex" );
    }
    for( const auto uv : geode::Range{ nb_existing_vertices } )
    {
        geode::OpenGeodeModelException::test(
            section.component_mesh_vertices( uv ).size() == 1,
            "[non empty model] Existing unique vertices should not be "
            "modified" );
    }
}

template < typename Function >
void expect_throw( Function&& function, std::string_view message )
{
    bool has_thrown{ false };
    try
    {
        function();
    }
    catch( const geode::OpenGeodeException& /*unused*/ )
    {
        has_thrown = true;
    }
    geode::OpenGeodeModelException::test(
        has_thrown, "[invalid definitions] Should throw with ", message );
}

void test_invalid_section_definitions()
{
    geode::Section section;
    geode::SimplicialSectionCreator creator{ section,
        { geode::Point2D{ { 0, 0 } }, geode::Point2D{ { 1, 0 } },
            geode::Point2D{ { 1, 1 } } } };
    expect_throw(
        [&creator] {
            creator.create_corners(
                std::vector< geode::CornerDefinition >{ { 3 } } );
        },
        "Corner vertex out of range" );
    const auto corners = creator.create_corners(
        std::vector< geode::CornerDefinition >{ { 0 }, { 1 }, { 2 } } );
    expect_throw(
        [&creator, &corners] {
            creator.create_lines(
                corners, std::vector< geode::LineDefinition >{ { {} } } );
        },
        "empty Line" );
    expect_throw(
        [&creator, &corners] {
            creator.create_lines(
                corners, std::vector< geode::LineDefinition >{ { { 0 } } } );
        },
        "Line with one vertex" );
    expect_throw(
        [&creator, &corners] {
            creator.create_lines(
                corners, std::vector< geode::LineDefinition >{ { { 0, 5 } } } );
        },
        "Line vertex out of range" );
    geode::OpenGeodeModelException::test( section.nb_lines() == 0,
        "[invalid definitions] No Line should be created" );
    const auto lines = creator.create_lines(
        corners, std::vector< geode::LineDefinition >{
                     { { 0, 1 } }, { { 1, 2 } }, { { 2, 0 } } } );
    expect_throw(
        [&creator, &lines] {
            creator.create_surfaces( lines,
                std::vector< geode::SurfaceDefinition >{
                    { { 0, 1, 2 }, { 0, 1, 2, 0 }, { 0, 1, 2 }, {}, {} } } );
        },
        "triangles not multiple of 3" );
    expect_throw(
        [&creator, &lines] {
            creator.create_surfaces( lines,
                std::vector< geode::SurfaceDefinition >{
                    { { 0, 1, 2 }, { 0, 1, 3 }, { 0, 1, 2 }, {}, {} } } );
        },
        "triangle vertex out of range" );
    expect_throw(
        [&creator, &lines] {
            creator.create_surfaces( lines,
                std::vector< geode::SurfaceDefinition >{
                    { { 0, 1, 4 }, { 0, 1, 2 }, { 0, 1, 2 }, {}, {} } } );
        },
        "Surface vertex out of range" );
    expect_throw(
        [&creator, &lines] {
            creator.create_surfaces( lines,
                std::vector< geode::SurfaceDefinition >{
                    { { 0, 1, 2 }, { 0, 1, 2 }, { 0, 1, 3 }, {}, {} } } );
        },
        "Surface boundary out of range" );
    geode::OpenGeodeModelException::test( section.nb_surfaces() == 0,
        "[invalid definitions] No Surface should be created" );
}

void test_invalid_brep_definitions()
{
    geode::BRep brep;
    geode::SimplicialBRepCreator creator{ brep,
        { geode::Point3D{ { 0, 0, 0 } }, geode::Point3D{ { 1, 0, 0 } },
            geode::Point3D{ { 0, 1, 0 } }, geode::Point3D{ { 0, 0, 1 } } } };
    expect_throw(
        [&creator] {
            creator.create_blocks(
                {}, std::vector< geode::BlockDefinition >{
                        { { 0, 1, 2, 3 }, { 0, 1, 2 }, {}, {}, {}, {} } } );
        },
        "tetrahedra not multiple of 4" );
    expect_throw(
        [&creator] {
            creator.create_blocks(
                {}, std::vector< geode::BlockDefinition >{
                        { { 0, 1, 2, 3 }, { 0, 1, 2, 4 }, {}, {}, {}, {} } } );
        },
        "tetrahedron vertex out of range" );
    expect_throw(
        [&creator] {
            creator.create_blocks( {},
                std::vector< geode::BlockDefinition >{
                    { { 0, 1, 2, 3 }, { 0, 1, 2, 3 }, { 0 }, {}, {}, {} } } );
        },
        "Block boundary out of range" );
    geode::OpenGeodeModelException::test( brep.nb_blocks() == 0,
        "[invalid definitions] No Block should be created" );
}

void test_line_corners_from_unique_points()
{
    geode::Section section;
    geode::SimplicialSectionCreator creator{ section,
        { geode::Point2D{ { 0, 0 } }, geode::Point2D{ { 1, 0 } },
            geode::Point2D{ { 1, 1 } }, geode::Point2D{ { 0, 1 } } } };
    const auto corners = creator.create_corners(
        std::vector< geode::CornerDefinition >{ { 3 }, { 1 } } );
    const auto lines = creator.create_lines(
        corners, std::vector< geode::LineDefinition >{ { { 3, 0, 1 } } } );
    const auto& line = section.line( lines.front() );
    geode::OpenGeodeModelException::test(
        section.nb_boundaries( line.id() ) == 2,
        "[Line corners] Line should have 2 boundaries" );
    geode::OpenGeodeModelException::test(
        section.is_boundary( section.corner( corners[0] ), line )
            && section.is_boundary( section.corner( corners[1] ), line ),
        "[Line corners] Wrong Line boundaries" );
    expect_throw(
        [&creator, &corners] {
            creator.create_lines(
                corners, std::vector< geode::LineDefinition >{ { { 1, 2 } } } );
        },
        "Line extremity without Corner" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_create_brep_from_bbox();
    test_create_brep_with_dangling_components();
    test_section();
    test_create_in_non_empty_section();
    test_invalid_section_definitions();
    test_invalid_brep_definitions();
    test_line_corners_from_unique_points();
}

OPENGEODE_TEST( "model-creator" )