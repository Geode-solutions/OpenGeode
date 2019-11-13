/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/algorithm.h>
#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode_triangulated_surface.h>
#include <geode/mesh/core/point_set.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>
#include <geode/model/representation/io/brep_output.h>

template < typename Range >
geode::index_t count_components( Range range )
{
    geode::index_t count{ 0 };
    for( const auto& unused : range )
    {
        geode_unused( unused );
        count++;
    }
    return count;
}

std::vector< geode::uuid > add_corners(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    geode::index_t nb{ 6 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_corner() );
        builder.set_corner_name(
            uuids.back(), "corner" + std::to_string( uuids.size() ) );
    }
    const auto& temp_corner = model.corner(
        builder.add_corner( geode::OpenGeodePointSet3D::type_name_static() ) );
    builder.remove_corner( temp_corner );
    auto message =
        "[Test] BRep should have " + std::to_string( nb ) + " corners";
    OPENGEODE_EXCEPTION( model.nb_corners() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.corners() ) == nb, message );
    OPENGEODE_EXCEPTION( model.corner( uuids[3] ).name() == "corner4",
        "[Test] Wrong Corner name" );
    return uuids;
}

std::vector< geode::uuid > add_lines(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    geode::index_t nb{ 9 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_line() );
        builder.set_line_name(
            uuids.back(), "line" + std::to_string( uuids.size() ) );
    }
    const auto& temp_line = model.line(
        builder.add_line( geode::OpenGeodeEdgedCurve3D::type_name_static() ) );
    builder.remove_line( temp_line );
    auto message = "[Test] BRep should have " + std::to_string( nb ) + " lines";
    OPENGEODE_EXCEPTION( model.nb_lines() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.lines() ) == nb, message );
    OPENGEODE_EXCEPTION(
        model.line( uuids[3] ).name() == "line4", "[Test] Wrong Line name" );
    return uuids;
}

std::vector< geode::uuid > add_surfaces(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    geode::index_t nb{ 5 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ 2 } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_surface(
            geode::OpenGeodeTriangulatedSurface3D::type_name_static() ) );
        builder.set_surface_name(
            uuids.back(), "surface" + std::to_string( uuids.size() ) );
    }
    for( auto unused : geode::Range{ 2, nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_surface(
            geode::OpenGeodePolygonalSurface3D::type_name_static() ) );
    }
    const auto& temp_surface = model.surface( builder.add_surface() );
    builder.remove_surface( temp_surface );
    auto message =
        "[Test] BRep should have " + std::to_string( nb ) + " surfaces";
    OPENGEODE_EXCEPTION( model.nb_surfaces() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.surfaces() ) == nb, message );
    OPENGEODE_EXCEPTION( model.surface( uuids[1] ).name() == "surface2",
        "[Test] Wrong Surface name" );
    return uuids;
}

std::vector< geode::uuid > add_blocks(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    geode::index_t nb{ 1 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_block() );
        builder.set_block_name(
            uuids.back(), "block" + std::to_string( uuids.size() ) );
    }
    const auto& temp_block = model.block( builder.add_block(
        geode::OpenGeodePolyhedralSolid3D::type_name_static() ) );
    builder.remove_block( temp_block );
    auto message =
        "[Test] BRep should have " + std::to_string( nb ) + " blocks";
    OPENGEODE_EXCEPTION( model.nb_blocks() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.blocks() ) == nb, message );
    OPENGEODE_EXCEPTION(
        model.block( uuids[0] ).name() == "block1", "[Test] Wrong Block name" );
    return uuids;
}

std::vector< geode::uuid > add_model_boundaries(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    geode::index_t nb{ 3 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_model_boundary() );
        builder.set_model_boundary_name(
            uuids.back(), "boundary" + std::to_string( uuids.size() ) );
    }
    const auto& temp_boundary =
        model.model_boundary( builder.add_model_boundary() );
    builder.remove_model_boundary( temp_boundary );
    auto message =
        "[Test] BRep should have " + std::to_string( nb ) + " model boundaries";
    OPENGEODE_EXCEPTION( model.nb_model_boundaries() == nb, message );
    OPENGEODE_EXCEPTION(
        count_components( model.model_boundaries() ) == nb, message );
    OPENGEODE_EXCEPTION( model.model_boundary( uuids[0] ).name() == "boundary1",
        "[Test] Wrong ModelBoundary name" );
    return uuids;
}

void add_corner_line_boundary_relation( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& corner_uuids,
    const std::vector< geode::uuid >& line_uuids )
{
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[0] ), model.line( line_uuids[0] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[0] ), model.line( line_uuids[5] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[0] ), model.line( line_uuids[2] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[1] ), model.line( line_uuids[0] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[1] ), model.line( line_uuids[1] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[1] ), model.line( line_uuids[3] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[2] ), model.line( line_uuids[1] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[2] ), model.line( line_uuids[2] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[2] ), model.line( line_uuids[4] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[3] ), model.line( line_uuids[5] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[3] ), model.line( line_uuids[6] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[3] ), model.line( line_uuids[8] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[4] ), model.line( line_uuids[3] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[4] ), model.line( line_uuids[6] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[4] ), model.line( line_uuids[7] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[5] ), model.line( line_uuids[4] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[5] ), model.line( line_uuids[7] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[5] ), model.line( line_uuids[8] ) );

    for( const auto& corner_id : corner_uuids )
    {
        for( const auto& incidence :
            model.incidences( model.corner( corner_id ) ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( line_uuids, incidence.id() ),
                "[Test] All Corners incidences should be Lines" );
        }
        OPENGEODE_EXCEPTION( model.nb_incidences( corner_id ) == 3,
            "[Test] All Corners should be connected to 3 Lines" );
    }

    for( const auto& line_id : line_uuids )
    {
        for( const auto& boundary : model.boundaries( model.line( line_id ) ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( corner_uuids, boundary.id() ),
                "[Test] All Lines incidences should be Corners" );
        }
        OPENGEODE_EXCEPTION( model.nb_boundaries( line_id ) == 2,
            "[Test] All Lines should be connected to 2 Corners" );
    }
}

void add_line_surface_boundary_relation( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids )
{
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[0] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[0] ), model.surface( surface_uuids[1] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[1] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[1] ), model.surface( surface_uuids[2] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[2] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[2] ), model.surface( surface_uuids[3] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[3] ), model.surface( surface_uuids[1] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[3] ), model.surface( surface_uuids[2] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[4] ), model.surface( surface_uuids[2] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[4] ), model.surface( surface_uuids[3] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[5] ), model.surface( surface_uuids[1] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[5] ), model.surface( surface_uuids[3] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[6] ), model.surface( surface_uuids[1] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[6] ), model.surface( surface_uuids[4] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[7] ), model.surface( surface_uuids[2] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[7] ), model.surface( surface_uuids[4] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[8] ), model.surface( surface_uuids[3] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[8] ), model.surface( surface_uuids[4] ) );

    for( const auto& line_id : line_uuids )
    {
        for( const auto& incidence : model.incidences( model.line( line_id ) ) )
        {
            OPENGEODE_EXCEPTION(
                geode::contain( surface_uuids, incidence.id() ),
                "[Test] All Lines incidences should be Surfaces" );
        }
        OPENGEODE_EXCEPTION( model.nb_incidences( line_id ) == 2,
            "[Test] All Lines should be connected to 2 Surfaces" );
    }
}

void add_surface_block_relation( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& block_uuids )
{
    for( const auto& surface_id : surface_uuids )
    {
        builder.add_surface_block_boundary_relationship(
            model.surface( surface_id ), model.block( block_uuids.front() ) );
    }

    for( const auto& surface_id : surface_uuids )
    {
        for( const auto& incidence :
            model.incidences( model.surface( surface_id ) ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( block_uuids, incidence.id() ),
                "[Test] All Surfaces incidences should be Blocks" );
        }
        OPENGEODE_EXCEPTION( model.nb_incidences( surface_id ) == 1,
            "[Test] All Surfaces should be connected to 1 Block" );
    }

    OPENGEODE_EXCEPTION(
        model.nb_boundaries( block_uuids.front() ) == surface_uuids.size(),
        "[Test] The Block should be connected to all Surfaces" );
}

void add_surfaces_in_model_boundaries( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& boundary_uuids )
{
    builder.add_surface_in_model_boundary( model.surface( surface_uuids[0] ),
        model.model_boundary( boundary_uuids[0] ) );
    for( auto i : geode::Range{ 1, 4 } )
    {
        builder.add_surface_in_model_boundary(
            model.surface( surface_uuids[i] ),
            model.model_boundary( boundary_uuids[1] ) );
    }
    builder.add_surface_in_model_boundary( model.surface( surface_uuids[4] ),
        model.model_boundary( boundary_uuids[2] ) );

    for( const auto& surface_id : surface_uuids )
    {
        OPENGEODE_EXCEPTION( model.nb_collections( surface_id ) == 1,
            "[Test] All Surfaces should be in 1 collection (of type "
            "Boundary)" );
    }
}

void add_internal_corner_relations( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& corner_uuids,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& block_uuids )
{
    for( const auto& corner_id : corner_uuids )
    {
        builder.add_corner_surface_internal_relationship(
            model.corner( corner_id ), model.surface( surface_uuids.front() ) );
        builder.add_corner_block_internal_relationship(
            model.corner( corner_id ), model.block( block_uuids.front() ) );
    }

    for( const auto& corner_id : corner_uuids )
    {
        for( const auto& embedding :
            model.embedded_surfaces( model.corner( corner_id ) ) )
        {
            OPENGEODE_EXCEPTION(
                geode::contain( surface_uuids, embedding.id() ),
                "[Test] All Corners embedded surfaces should be Surfaces" );
        }
        for( const auto& embedding :
            model.embedded_blocks( model.corner( corner_id ) ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( block_uuids, embedding.id() ),
                "[Test] All Corners embedded blocks should be Blocks" );
        }
        OPENGEODE_EXCEPTION( model.nb_embeddings( corner_id ) == 2,
            "[Test] All Corners should be embedded to 1 Block and 1 Surface" );
        OPENGEODE_EXCEPTION(
            model.nb_embedded_surfaces( model.corner( corner_id ) ) == 1,
            "[Test] All Corners should be embedded to 1 Surface" );
        OPENGEODE_EXCEPTION(
            model.nb_embedded_blocks( model.corner( corner_id ) ) == 1,
            "[Test] All Corners should be embedded to 1 Block" );
    }
}

void add_internal_line_relations( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& block_uuids )
{
    for( const auto& line_id : line_uuids )
    {
        builder.add_line_surface_internal_relationship(
            model.line( line_id ), model.surface( surface_uuids.front() ) );
        builder.add_line_block_internal_relationship(
            model.line( line_id ), model.block( block_uuids.front() ) );
    }

    for( const auto& line_id : line_uuids )
    {
        for( const auto& embedding :
            model.embedded_surfaces( model.line( line_id ) ) )
        {
            OPENGEODE_EXCEPTION(
                geode::contain( surface_uuids, embedding.id() ),
                "[Test] All Line embedded surfaces should be Surfaces" );
        }
        for( const auto& embedding :
            model.embedded_blocks( model.line( line_id ) ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( block_uuids, embedding.id() ),
                "[Test] All Lines embedded blocks should be Blocks" );
        }
        OPENGEODE_EXCEPTION( model.nb_embeddings( line_id ) == 2,
            "[Test] All Surfaces should be embedded to 1 Block and 1 Surface" );
        OPENGEODE_EXCEPTION(
            model.nb_embedded_surfaces( model.line( line_id ) ) == 1,
            "[Test] All Surfaces should be embedded to 1 Surface" );
        OPENGEODE_EXCEPTION(
            model.nb_embedded_blocks( model.line( line_id ) ) == 1,
            "[Test] All Surfaces should be embedded to 1 Block" );
    }
}

void add_internal_surface_relations( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& block_uuids )
{
    for( const auto& surface_id : surface_uuids )
    {
        builder.add_surface_block_internal_relationship(
            model.surface( surface_id ), model.block( block_uuids.front() ) );
    }

    for( const auto& surface_id : surface_uuids )
    {
        for( const auto& embedding :
            model.embedded_blocks( model.surface( surface_id ) ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( block_uuids, embedding.id() ),
                "[Test] All Surfaces embeddings should be Blocks" );
        }
        OPENGEODE_EXCEPTION( model.nb_embeddings( surface_id ) == 1,
            "[Test] All Surfaces should be embedded to 1 Block" );
        OPENGEODE_EXCEPTION(
            model.nb_embedded_blocks( model.surface( surface_id ) ) == 1,
            "[Test] All Surfaces should be embedded to 1 Block" );
    }
}

void test_boundary_ranges( const geode::BRep& model,
    const std::vector< geode::uuid >& corner_uuids,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& block_uuids )
{
    const auto& line_boundaries =
        model.boundaries( model.line( line_uuids[0] ) );
    geode::index_t line_boundary_count{ 0 };
    for( const auto& line_boundary : line_boundaries )
    {
        line_boundary_count++;
        OPENGEODE_EXCEPTION( line_boundary.id() == corner_uuids[0]
                                 || line_boundary.id() == corner_uuids[1],
            "[Test] BoundaryCornerRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( line_boundary_count == 2,
        "[Test] BoundaryCornerRange should iterates on 2 Corners" );

    const auto& surface_boundaries =
        model.boundaries( model.surface( surface_uuids[0] ) );
    geode::index_t surface_boundary_count{ 0 };
    for( const auto& surface_boundary : surface_boundaries )
    {
        surface_boundary_count++;
        OPENGEODE_EXCEPTION( surface_boundary.id() == line_uuids[0]
                                 || surface_boundary.id() == line_uuids[1]
                                 || surface_boundary.id() == line_uuids[2],
            "[Test] BoundaryLineRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( surface_boundary_count == 3,
        "[Test] BoundaryLineRange should iterates on 3 Lines" );

    const auto& block_boundaries =
        model.boundaries( model.block( block_uuids[0] ) );
    geode::index_t block_boundary_count{ 0 };
    for( const auto& block_boundary : block_boundaries )
    {
        block_boundary_count++;
        OPENGEODE_EXCEPTION( block_boundary.id() == surface_uuids[0]
                                 || block_boundary.id() == surface_uuids[1]
                                 || block_boundary.id() == surface_uuids[2]
                                 || block_boundary.id() == surface_uuids[3]
                                 || block_boundary.id() == surface_uuids[4],
            "[Test] BoundarySurfaceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( block_boundary_count == 5,
        "[Test] BoundarySurfaceRange should iterates on 5 Surfaces" );
}

void test_incidence_ranges( const geode::BRep& model,
    const std::vector< geode::uuid >& corner_uuids,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& block_uuids )
{
    const auto& corner_incidences =
        model.incidences( model.corner( corner_uuids[0] ) );
    geode::index_t corner_incidence_count{ 0 };
    for( const auto& corner_incidence : corner_incidences )
    {
        corner_incidence_count++;
        OPENGEODE_EXCEPTION( corner_incidence.id() == line_uuids[0]
                                 || corner_incidence.id() == line_uuids[2]
                                 || corner_incidence.id() == line_uuids[5],
            "[Test] IncidentLineRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( corner_incidence_count == 3,
        "[Test] IncidentLineRange should iterates on 3 Lines" );

    const auto& line_incidences =
        model.incidences( model.line( line_uuids[0] ) );
    geode::index_t line_incidence_count{ 0 };
    for( const auto& line_incidence : line_incidences )
    {
        line_incidence_count++;
        OPENGEODE_EXCEPTION( line_incidence.id() == surface_uuids[0]
                                 || line_incidence.id() == surface_uuids[1],
            "[Test] IncidentSurfaceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( line_incidence_count == 2,
        "[Test] IncidentSurfaceRange should iterates on 2 Surfaces" );

    const auto& surface_incidences =
        model.incidences( model.surface( surface_uuids[0] ) );
    geode::index_t surface_incidence_count{ 0 };
    for( const auto& surface_incidence : surface_incidences )
    {
        surface_incidence_count++;
        OPENGEODE_EXCEPTION( surface_incidence.id() == block_uuids[0],
            "[Test] IncidentBlockRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( surface_incidence_count == 1,
        "[Test] IncidentBlockRange should iterates on 1 Block" );
}

void test_item_ranges( const geode::BRep& model,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& boundary_uuids )
{
    const auto& boundary_items =
        model.items( model.model_boundary( boundary_uuids[1] ) );
    geode::index_t boundary_item_count{ 0 };
    for( const auto& boundary_item : boundary_items )
    {
        boundary_item_count++;
        OPENGEODE_EXCEPTION( boundary_item.id() == surface_uuids[1]
                                 || boundary_item.id() == surface_uuids[2]
                                 || boundary_item.id() == surface_uuids[3],
            "[Test] ItemSurfaceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( boundary_item_count == 3,
        "[Test] IncidentLineRange should iterates "
        "on 3 Surfaces (Boundary 1)" );
}

void test_reloaded_brep( const geode::BRep& model )
{
    OPENGEODE_EXCEPTION( model.nb_corners() == 6,
        "[Test] Number of Corners in reloaded BRep should be 6" );
    OPENGEODE_EXCEPTION( model.nb_lines() == 9,
        "[Test] Number of Lines in reloaded BRep should be 9" );
    OPENGEODE_EXCEPTION( model.nb_surfaces() == 5,
        "[Test] Number of Surfaces in reloaded BRep should be 5" );
    OPENGEODE_EXCEPTION( model.nb_blocks() == 1,
        "[Test] Number of Blocks in reloaded BRep should be 1" );
    OPENGEODE_EXCEPTION( model.nb_model_boundaries() == 3,
        "[Test] Number of Boundaries in reloaded BRep should be 3" );
}

void test_clone( const geode::BRep& brep )
{
    geode::BRep brep2;
    geode::BRepBuilder builder{ brep2 };
    builder.copy( brep );
    OPENGEODE_EXCEPTION(
        brep2.nb_corners() == 6, "[Test] BRep should have 6 corners" );
    OPENGEODE_EXCEPTION(
        brep2.nb_lines() == 9, "[Test] BRep should have 9 lines" );
    OPENGEODE_EXCEPTION(
        brep2.nb_surfaces() == 5, "[Test] BRep should have 5 surfaces" );
    OPENGEODE_EXCEPTION(
        brep2.nb_blocks() == 1, "[Test] BRep should have 1 block" );
    OPENGEODE_EXCEPTION( brep2.nb_model_boundaries() == 3,
        "[Test] BRep should have 3 model boundaries" );

    auto mapping = builder.copy_components( brep );
    builder.copy_component_relationships( mapping, brep );
    OPENGEODE_EXCEPTION(
        brep2.nb_corners() == 12, "[Test] BRep should have 12 corners" );
    OPENGEODE_EXCEPTION(
        brep2.nb_lines() == 18, "[Test] BRep should have 18 lines" );
    OPENGEODE_EXCEPTION(
        brep2.nb_surfaces() == 10, "[Test] BRep should have 10 surfaces" );
    OPENGEODE_EXCEPTION(
        brep2.nb_blocks() == 2, "[Test] BRep should have 2 blocks" );
    OPENGEODE_EXCEPTION( brep2.nb_model_boundaries() == 6,
        "[Test] BRep should have 6 model boundaries" );

    for( const auto& corner : brep.corners() )
    {
        const auto& new_corner =
            brep2.corner( mapping.corners.at( corner.id() ) );
        for( const auto& line : brep.incidences( corner ) )
        {
            bool found{ false };
            for( const auto& new_line : brep2.incidences( new_corner ) )
            {
                if( mapping.lines.at( line.id() ) == new_line.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All Corners incidences are not correct" );
        }
    }
    for( const auto& line : brep.lines() )
    {
        const auto& new_line = brep2.line( mapping.lines.at( line.id() ) );
        for( const auto& surface : brep.incidences( line ) )
        {
            bool found = { false };
            for( const auto& new_surface : brep2.incidences( new_line ) )
            {
                if( mapping.surfaces.at( surface.id() ) == new_surface.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All Lines incidences are not correct" );
        }
    }
    for( const auto& surface : brep.surfaces() )
    {
        const auto& new_surface =
            brep2.surface( mapping.surfaces.at( surface.id() ) );
        for( const auto& block : brep.incidences( surface ) )
        {
            bool found = { false };
            for( const auto& new_block : brep2.incidences( new_surface ) )
            {
                if( mapping.blocks.at( block.id() ) == new_block.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All Surfaces incidences are not correct" );
        }
    }
    for( const auto& model_boundary : brep.model_boundaries() )
    {
        const auto& new_model_boundary = brep2.model_boundary(
            mapping.model_boundaries.at( model_boundary.id() ) );
        for( const auto& surface : brep.items( model_boundary ) )
        {
            bool found = { false };
            for( const auto& new_surface : brep2.items( new_model_boundary ) )
            {
                if( mapping.surfaces.at( surface.id() ) == new_surface.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION( found,
                "[Test] All ModelBoundaries incidences are not correct" );
        }
    }
}

int main()
{
    using namespace geode;

    try
    {
        BRep model;
        BRepBuilder builder( model );

        // This BRep represents a prism
        auto corner_uuids = add_corners( model, builder );
        auto line_uuids = add_lines( model, builder );
        auto surface_uuids = add_surfaces( model, builder );
        auto block_uuids = add_blocks( model, builder );
        auto model_boundary_uuids = add_model_boundaries( model, builder );

        add_corner_line_boundary_relation(
            model, builder, corner_uuids, line_uuids );
        add_line_surface_boundary_relation(
            model, builder, line_uuids, surface_uuids );
        add_surface_block_relation(
            model, builder, surface_uuids, block_uuids );
        add_surfaces_in_model_boundaries(
            model, builder, surface_uuids, model_boundary_uuids );
        add_internal_corner_relations(
            model, builder, corner_uuids, surface_uuids, block_uuids );
        add_internal_line_relations(
            model, builder, line_uuids, surface_uuids, block_uuids );
        add_internal_surface_relations(
            model, builder, surface_uuids, block_uuids );
        OPENGEODE_EXCEPTION( model.nb_internals( block_uuids.front() )
                                 == corner_uuids.size() + line_uuids.size()
                                        + surface_uuids.size(),
            "[Test] The Block should embed all Corners & Lines & Surfaces "
            "(that are internal to the "
            "Block)" );
        test_boundary_ranges(
            model, corner_uuids, line_uuids, surface_uuids, block_uuids );
        test_incidence_ranges(
            model, corner_uuids, line_uuids, surface_uuids, block_uuids );
        test_item_ranges( model, surface_uuids, model_boundary_uuids );
        test_clone( model );

        std::string file_io{ "test." + model.native_extension() };
        save_brep( model, file_io );

        BRep model2;
        load_brep( model2, file_io );
        test_reloaded_brep( model2 );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
