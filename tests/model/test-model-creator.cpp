/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/simplicial_brep_creator.h>
#include <geode/model/helpers/simplicial_section_creator.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

#include <geode/tests/common.h>

void test_create_brep_with_dangling_components()
{
    std::vector< geode::Point3D > points{
        { { 0, 0, 0 } }, // 0
        { { 1, 0, 0 } }, // 1
        { { 1, 1, 0 } }, // 2
        { { 0, 1, 0 } }, // 3
        { { 0, 0, 1 } }, // 4
        { { 1, 0, 1 } }, // 5
        { { 1, 1, 1 } }, // 6
        { { 0, 1, 1 } }, // 7
        { { 0, 0, 2 } }, // 8
        { { 1, 0, 2 } }, // 9
        { { 1, 1, 2 } }, // 10
        { { 0, 1, 2 } }, // 11
        { { 1, 0.5, 1.5 } }, // 12
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
        { { 0, 1, 2, 3 }, { 0, 1, 2, 0, 3, 2 }, { 0, 1, 2, 3 }, {}, {} },
        { { 0, 1, 5, 4 }, { 0, 1, 2, 0, 3, 2 }, { 0, 4, 8, 9 }, {}, {} },
        { { 1, 2, 6, 5 }, { 0, 1, 2, 0, 3, 2 }, { 1, 5, 9, 10 }, {}, {} },
        { { 2, 3, 7, 6 }, { 0, 1, 2, 0, 3, 2 }, { 2, 6, 10, 11 }, {}, {} },
        { { 0, 3, 7, 4 }, { 0, 1, 2, 0, 3, 2 }, { 3, 7, 11, 8 }, {}, {} },
        { { 4, 5, 6, 7 }, { 0, 1, 2, 0, 3, 2 }, {}, {}, {} },
        { { 5, 6, 10, 9, 12 }, { 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4 },
            { 5, 12, 13, 14 }, {}, { 12 } },
    };
    const auto surface_uuids =
        creator.create_surfaces( corner_uuids, line_uuids, surfaces );

    std::vector< geode::BlockDefinition > blocks{
        { {}, {}, { 0, 1, 2, 3, 4, 5 }, {}, {}, {} },
    };
    creator.create_blocks( corner_uuids, line_uuids, surface_uuids, blocks );
    OPENGEODE_EXCEPTION(
        brep.nb_blocks() == blocks.size(), "[Test] Wrong number of blocks" );
    geode::index_t nb_embedding_blocks{ 0 };
    geode::index_t nb_embedding_surfaces{ 0 };
    for( const auto& corner : brep.corners() )
    {
        nb_embedding_blocks += brep.nb_embedding_blocks( corner );
        nb_embedding_surfaces += brep.nb_embedding_surfaces( corner );
    }
    OPENGEODE_EXCEPTION( nb_embedding_blocks == 0,
        "[Test] Wrong number of corner embedding blocks" );
    OPENGEODE_EXCEPTION( nb_embedding_surfaces == 1,
        "[Test] Wrong number of corner embedding surfaces" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    std::vector< geode::Point2D > points{
        { { 0, 0 } },
        { { 1, 0 } },
        { { 1, 1 } },
        { { 0, 1 } },
        { { 0.5, 0.2 } },
        { { 0.5, 0.8 } },
        { { 0, 0.5 } },
        { { 0.5, 0 } },
        { { 1, 0.5 } },
        { { 0.5, 1 } },
        { { 0.5, 0.5 } },
    };
    const auto nb_vertices = points.size();
    geode::Section section;
    geode::SimplicialSectionCreator creator{ section, std::move( points ) };
    OPENGEODE_EXCEPTION( section.nb_unique_vertices() == nb_vertices,
        "[Test] Wrong number of unique vertices" );

    std::vector< geode::CornerDefinition > corner_definitions{
        { 0 },
        { 1 },
        { 2 },
        { 3 },
        { 4 },
        { 5 },
    };
    const auto corners = creator.create_corners( corner_definitions );
    OPENGEODE_EXCEPTION( corners.size() == corner_definitions.size(),
        "[Test] Wrong number of corners" );

    std::vector< geode::LineDefinition > line_definitions{
        { { 0, 7, 1 } },
        { { 1, 8, 2 } },
        { { 2, 9, 3 } },
        { { 3, 6, 0 } },
        { { 4, 10, 5 } },
    };
    const auto lines = creator.create_lines( corners, line_definitions );
    OPENGEODE_EXCEPTION( lines.size() == line_definitions.size(),
        "[Test] Wrong number of lines" );
    for( const auto l : geode::Indices{ lines } )
    {
        const auto& line = section.line( lines[l] );
        const auto& mesh = line.mesh();
        OPENGEODE_EXCEPTION(
            mesh.nb_vertices() == line_definitions[l].vertices.size(),
            "[Test] Wrong number of Line vertices" );
        OPENGEODE_EXCEPTION(
            mesh.nb_edges() == line_definitions[l].vertices.size() - 1,
            "[Test] Wrong number of Line edges" );
        for( const auto& corner : section.boundaries( line ) )
        {
            if( corner.id() != corners[line_definitions[l].vertices.front()]
                && corner.id() != corners[line_definitions[l].vertices.back()] )
            {
                OPENGEODE_EXCEPTION(
                    false, "[Test] Missing Corner in Line boundary" );
            }
        }
    }

    std::vector< geode::SurfaceDefinition > surface_definitions{
        { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10 },
            { 0, 4, 7, 7, 4, 1, 1, 4, 8, 4, 10, 8, 8, 10, 5, 8, 5, 2, 2, 5, 9,
                9, 5, 3, 3, 5, 6, 6, 5, 11, 6, 11, 4, 6, 4, 0 },
            { 0, 1, 2, 3 }, { 4 }, {} },
    };
    const auto surfaces = creator.create_surfaces( lines, surface_definitions );
    OPENGEODE_EXCEPTION( surfaces.size() == surface_definitions.size(),
        "[Test] Wrong number of surfaces" );
    for( const auto s : geode::Indices{ surfaces } )
    {
        const auto& surface = section.surface( surfaces[s] );
        const auto& mesh = surface.mesh();
        OPENGEODE_EXCEPTION(
            mesh.nb_vertices() == surface_definitions[s].vertices.size(),
            "[Test] Wrong number of Surface vertices" );
        OPENGEODE_EXCEPTION(
            mesh.nb_polygons() == surface_definitions[s].triangles.size() / 3,
            "[Test] Wrong number of Surface triangles" );
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
            OPENGEODE_EXCEPTION(
                found, "[Test] Missing Line in Surface boundary" );
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
            OPENGEODE_EXCEPTION(
                found, "[Test] Missing Line in Surface internal lines" );
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
    OPENGEODE_EXCEPTION( boundaries.size() == boundary_definitions.size(),
        "[Test] Wrong number of boundaries" );
    for( const auto b : geode::Indices{ boundaries } )
    {
        const auto& boundary = section.model_boundary( boundaries[b] );
        for( const auto& line : section.items( boundary.id() ) )
        {
            bool found{ false };
            for( const auto b2 : boundary_definitions[b].boundaries )
            {
                if( line.id() == lines[b2] )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] Missing Line in ModelBoundary" );
        }
    }

    test_create_brep_with_dangling_components();
}

OPENGEODE_TEST( "model-creator" )