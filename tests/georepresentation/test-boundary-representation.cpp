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

#include <geode/georepresentation/builder/boundary_representation_builder.h>
#include <geode/georepresentation/core/boundary_representation.h>
#include <geode/georepresentation/io/boundary_representation_input.h>
#include <geode/georepresentation/io/boundary_representation_output.h>

#include <geode/basic/attribute_manager.h>
#include <geode/georepresentation/core/block.h>
#include <geode/georepresentation/core/boundaries.h>
#include <geode/georepresentation/core/boundary.h>
#include <geode/georepresentation/core/corner.h>
#include <geode/georepresentation/core/line.h>
#include <geode/georepresentation/core/surface.h>
#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/point_set.h>

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
    }
    const auto& temp_corner = model.corner(
        builder.add_corner( geode::OpenGeodePointSet3D::type_name_static() ) );
    builder.remove_corner( temp_corner );
    auto message = "BRep should have " + std::to_string( nb ) + " corners";
    OPENGEODE_EXCEPTION( model.nb_corners() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.corners() ) == nb, message );
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
    }
    const auto& temp_line = model.line(
        builder.add_line( geode::OpenGeodeEdgedCurve3D::type_name_static() ) );
    builder.remove_line( temp_line );
    auto message = "BRep should have " + std::to_string( nb ) + " lines";
    OPENGEODE_EXCEPTION( model.nb_lines() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.lines() ) == nb, message );
    return uuids;
}

std::vector< geode::uuid > add_surfaces(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    geode::index_t nb{ 5 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_surface() );
    }
    const auto& temp_surface = model.surface( builder.add_surface(
        geode::OpenGeodePolygonalSurface3D::type_name_static() ) );
    builder.remove_surface( temp_surface );
    auto message = "BRep should have " + std::to_string( nb ) + " surfaces";
    OPENGEODE_EXCEPTION( model.nb_surfaces() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.surfaces() ) == nb, message );
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
    }
    const auto& temp_block = model.block( builder.add_block(
        geode::OpenGeodePolyhedralSolid3D::type_name_static() ) );
    builder.remove_block( temp_block );
    auto message = "BRep should have " + std::to_string( nb ) + " blocks";
    OPENGEODE_EXCEPTION( model.nb_blocks() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.blocks() ) == nb, message );
    return uuids;
}

std::vector< geode::uuid > add_boundaries(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    geode::index_t nb{ 3 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_boundary() );
    }
    const auto& temp_boundary = model.boundary( builder.add_boundary() );
    builder.remove_boundary( temp_boundary );
    auto message = "BRep should have " + std::to_string( nb ) + " boundaries";
    OPENGEODE_EXCEPTION( model.nb_boundaries() == nb, message );
    OPENGEODE_EXCEPTION(
        count_components( model.boundaries() ) == nb, message );
    return uuids;
}

void add_corner_line_relation( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& corner_uuids,
    const std::vector< geode::uuid >& line_uuids )
{
    builder.add_boundary_relation(
        model.corner( corner_uuids[0] ), model.line( line_uuids[0] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[0] ), model.line( line_uuids[5] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[0] ), model.line( line_uuids[2] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[1] ), model.line( line_uuids[0] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[1] ), model.line( line_uuids[1] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[1] ), model.line( line_uuids[3] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[2] ), model.line( line_uuids[1] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[2] ), model.line( line_uuids[2] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[2] ), model.line( line_uuids[4] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[3] ), model.line( line_uuids[5] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[3] ), model.line( line_uuids[6] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[3] ), model.line( line_uuids[8] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[4] ), model.line( line_uuids[3] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[4] ), model.line( line_uuids[6] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[4] ), model.line( line_uuids[7] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[5] ), model.line( line_uuids[4] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[5] ), model.line( line_uuids[7] ) );
    builder.add_boundary_relation(
        model.corner( corner_uuids[5] ), model.line( line_uuids[8] ) );

    for( const auto& corner_id : corner_uuids )
    {
        for( const auto& incidence :
            model.relationships().incidences( corner_id ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( line_uuids, incidence ),
                "All Corners incidences should be Lines" );
        }
        OPENGEODE_EXCEPTION(
            model.relationships().nb_incidences( corner_id ) == 3,
            "All Corners should be connected to 3 Lines" );
    }

    for( const auto& line_id : line_uuids )
    {
        for( const auto& boundary :
            model.relationships().boundaries( line_id ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( corner_uuids, boundary ),
                "All Lines incidences should be Corners" );
        }
        OPENGEODE_EXCEPTION(
            model.relationships().nb_boundaries( line_id ) == 2,
            "All Lines should be connected to 2 Corners" );
    }
}

void add_line_surface_relation( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids )
{
    builder.add_boundary_relation(
        model.line( line_uuids[0] ), model.surface( surface_uuids[0] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[0] ), model.surface( surface_uuids[1] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[1] ), model.surface( surface_uuids[0] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[1] ), model.surface( surface_uuids[2] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[2] ), model.surface( surface_uuids[0] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[2] ), model.surface( surface_uuids[3] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[3] ), model.surface( surface_uuids[1] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[3] ), model.surface( surface_uuids[2] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[4] ), model.surface( surface_uuids[2] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[4] ), model.surface( surface_uuids[3] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[5] ), model.surface( surface_uuids[1] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[5] ), model.surface( surface_uuids[3] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[6] ), model.surface( surface_uuids[1] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[6] ), model.surface( surface_uuids[4] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[7] ), model.surface( surface_uuids[2] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[7] ), model.surface( surface_uuids[4] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[8] ), model.surface( surface_uuids[3] ) );
    builder.add_boundary_relation(
        model.line( line_uuids[8] ), model.surface( surface_uuids[4] ) );

    for( const auto& line_id : line_uuids )
    {
        for( const auto& incidence :
            model.relationships().incidences( line_id ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( surface_uuids, incidence ),
                "All Lines incidences should be Surfaces" );
        }
        OPENGEODE_EXCEPTION(
            model.relationships().nb_incidences( line_id ) == 2,
            "All Lines should be connected to 2 Surfaces" );
    }
}

void add_surface_block_relation( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& block_uuids )
{
    for( const auto& surface_id : surface_uuids )
    {
        builder.add_boundary_relation(
            model.surface( surface_id ), model.block( block_uuids.front() ) );
    }

    for( const auto& surface_id : surface_uuids )
    {
        for( const auto& incidence :
            model.relationships().incidences( surface_id ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( block_uuids, incidence ),
                "All Surfaces incidences should be Blocks" );
        }
        OPENGEODE_EXCEPTION(
            model.relationships().nb_incidences( surface_id ) == 1,
            "All Surfaces should be connected to 1 Block" );
    }

    OPENGEODE_EXCEPTION(
        model.relationships().nb_boundaries( block_uuids.front() )
            == surface_uuids.size(),
        "The Block should be connected to all Surfaces" );
}

void add_surfaces_in_boundaries( const geode::BRep& model,
    geode::BRepBuilder& builder,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& boundary_uuids )
{
    builder.add_surface_in_boundary( model.surface( surface_uuids[0] ),
        model.boundary( boundary_uuids[0] ) );
    for( auto i : geode::Range( 1, 4 ) )
    {
        builder.add_surface_in_boundary( model.surface( surface_uuids[i] ),
            model.boundary( boundary_uuids[1] ) );
    }
    builder.add_surface_in_boundary( model.surface( surface_uuids[4] ),
        model.boundary( boundary_uuids[2] ) );

    for( const auto& surface_id : surface_uuids )
    {
        OPENGEODE_EXCEPTION(
            model.relationships().nb_collections( surface_id ) == 1,
            "All Surfaces should be in 1 collection (of type Boundary)" );
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
            "LineBoundaryRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( line_boundary_count == 2,
        "LineBoundaryRange should iterates on 2 Corners" );

    const auto& surface_boundaries =
        model.boundaries( model.surface( surface_uuids[0] ) );
    geode::index_t surface_boundary_count{ 0 };
    for( const auto& surface_boundary : surface_boundaries )
    {
        surface_boundary_count++;
        OPENGEODE_EXCEPTION( surface_boundary.id() == line_uuids[0]
                                 || surface_boundary.id() == line_uuids[1]
                                 || surface_boundary.id() == line_uuids[2],
            "SurfaceBoundaryRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( surface_boundary_count == 3,
        "SurfaceBoundaryRange should iterates on 3 Lines" );

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
            "BlockBoundaryRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( block_boundary_count == 5,
        "BlockBoundaryRange should iterates on 5 Surfaces" );
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
            "CornerIncidenceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( corner_incidence_count == 3,
        "CornerIncidenceRange should iterates on 3 Lines" );

    const auto& line_incidences =
        model.incidences( model.line( line_uuids[0] ) );
    geode::index_t line_incidence_count{ 0 };
    for( const auto& line_incidence : line_incidences )
    {
        line_incidence_count++;
        OPENGEODE_EXCEPTION( line_incidence.id() == surface_uuids[0]
                                 || line_incidence.id() == surface_uuids[1],
            "LineIncidenceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( line_incidence_count == 2,
        "LineIncidenceRange should iterates on 2 Surface" );

    const auto& surface_incidences =
        model.incidences( model.surface( surface_uuids[0] ) );
    geode::index_t surface_incidence_count{ 0 };
    for( const auto& surface_incidence : surface_incidences )
    {
        surface_incidence_count++;
        OPENGEODE_EXCEPTION( surface_incidence.id() == block_uuids[0],
            "SurfaceIncidenceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( surface_incidence_count == 1,
        "SurfaceIncidenceRange should iterates on 1 Block" );
}

void test_item_ranges( const geode::BRep& model,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& boundary_uuids )
{
    const auto& boundary_items =
        model.items( model.boundary( boundary_uuids[1] ) );
    geode::index_t boundary_item_count{ 0 };
    for( const auto& boundary_item : boundary_items )
    {
        boundary_item_count++;
        OPENGEODE_EXCEPTION( boundary_item.id() == surface_uuids[1]
                                 || boundary_item.id() == surface_uuids[2]
                                 || boundary_item.id() == surface_uuids[3],
            "BoundaryItemRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( boundary_item_count == 3,
        "CornerIncidenceRange should iterates on 3 Surfaces (Boundary 1)" );
}

void test_reloaded_brep( const geode::BRep& model )
{
    OPENGEODE_EXCEPTION( model.nb_corners() == 6,
        "Number of Corners in reloaded BRep should be 6" );
    OPENGEODE_EXCEPTION(
        model.nb_lines() == 9, "Number of Lines in reloaded BRep should be 9" );
    OPENGEODE_EXCEPTION( model.nb_surfaces() == 5,
        "Number of Surfaces in reloaded BRep should be 5" );
    OPENGEODE_EXCEPTION( model.nb_blocks() == 1,
        "Number of Blocks in reloaded BRep should be 1" );
    OPENGEODE_EXCEPTION( model.nb_boundaries() == 3,
        "Number of Boundaries in reloaded BRep should be 3" );
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
        auto boundary_uuids = add_boundaries( model, builder );

        add_corner_line_relation( model, builder, corner_uuids, line_uuids );
        add_line_surface_relation( model, builder, line_uuids, surface_uuids );
        add_surface_block_relation(
            model, builder, surface_uuids, block_uuids );
        add_surfaces_in_boundaries(
            model, builder, surface_uuids, boundary_uuids );
        test_boundary_ranges(
            model, corner_uuids, line_uuids, surface_uuids, block_uuids );
        test_incidence_ranges(
            model, corner_uuids, line_uuids, surface_uuids, block_uuids );
        test_item_ranges( model, surface_uuids, boundary_uuids );

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
