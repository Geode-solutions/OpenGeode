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

#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/section_input.h>
#include <geode/model/representation/io/section_output.h>

#include <geode/basic/attribute_manager.h>
#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/point_set.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>

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
    const geode::Section& model, geode::SectionBuilder& builder )
{
    geode::index_t nb{ 5 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_corner() );
    }
    const auto& temp_corner = model.corner(
        builder.add_corner( geode::OpenGeodePointSet2D::type_name_static() ) );
    builder.remove_corner( temp_corner );
    auto message =
        "[Test] Section should have " + std::to_string( nb ) + " corners";
    OPENGEODE_EXCEPTION( model.nb_corners() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.corners() ) == nb, message );
    return uuids;
}

std::vector< geode::uuid > add_lines(
    const geode::Section& model, geode::SectionBuilder& builder )
{
    geode::index_t nb{ 6 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_line() );
    }
    const auto& temp_line = model.line(
        builder.add_line( geode::OpenGeodeEdgedCurve2D::type_name_static() ) );
    builder.remove_line( temp_line );
    auto message =
        "[Test] Section should have " + std::to_string( nb ) + " lines";
    OPENGEODE_EXCEPTION( model.nb_lines() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.lines() ) == nb, message );
    return uuids;
}

std::vector< geode::uuid > add_surfaces(
    const geode::Section& model, geode::SectionBuilder& builder )
{
    geode::index_t nb{ 2 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_surface() );
    }
    const auto& temp_surface = model.surface( builder.add_surface(
        geode::OpenGeodePolygonalSurface2D::type_name_static() ) );
    builder.remove_surface( temp_surface );
    auto message =
        "[Test] Section should have " + std::to_string( nb ) + " surfaces";
    OPENGEODE_EXCEPTION( model.nb_surfaces() == nb, message );
    OPENGEODE_EXCEPTION( count_components( model.surfaces() ) == nb, message );
    return uuids;
}

std::vector< geode::uuid > add_model_boundaries(
    const geode::Section& section, geode::SectionBuilder& builder )
{
    geode::index_t nb{ 2 };
    std::vector< geode::uuid > uuids;
    for( auto unused : geode::Range{ nb } )
    {
        geode_unused( unused );
        uuids.push_back( builder.add_model_boundary() );
        builder.set_model_boundary_name(
            uuids.back(), "boundary" + std::to_string( uuids.size() ) );
    }
    const auto& temp_boundary =
        section.model_boundary( builder.add_model_boundary() );
    builder.remove_model_boundary( temp_boundary );
    auto message = "[Test] Section should have " + std::to_string( nb )
                   + " model boundaries";
    OPENGEODE_EXCEPTION( section.nb_model_boundaries() == nb, message );
    OPENGEODE_EXCEPTION(
        count_components( section.model_boundaries() ) == nb, message );
    OPENGEODE_EXCEPTION(
        section.model_boundary( uuids[0] ).name() == "boundary1",
        "[Test] Wrong ModelBoundary name" );
    return uuids;
}

void add_corner_line_boundary_relation( const geode::Section& model,
    geode::SectionBuilder& builder,
    const std::vector< geode::uuid >& corner_uuids,
    const std::vector< geode::uuid >& line_uuids )
{
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[0] ), model.line( line_uuids[0] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[0] ), model.line( line_uuids[1] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[1] ), model.line( line_uuids[0] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[1] ), model.line( line_uuids[2] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[1] ), model.line( line_uuids[3] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[2] ), model.line( line_uuids[1] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[2] ), model.line( line_uuids[2] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[2] ), model.line( line_uuids[4] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[3] ), model.line( line_uuids[3] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[3] ), model.line( line_uuids[5] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[4] ), model.line( line_uuids[4] ) );
    builder.add_corner_line_boundary_relationship(
        model.corner( corner_uuids[4] ), model.line( line_uuids[5] ) );

    for( const auto& corner_id : corner_uuids )
    {
        for( const auto& incidence :
            model.incidences( model.corner( corner_id ) ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( line_uuids, incidence.id() ),
                "[Test] All Corners incidences should be Lines" );
        }
    }

    for( const auto& line_id : line_uuids )
    {
        for( const auto& boundary : model.boundaries( model.line( line_id ) ) )
        {
            OPENGEODE_EXCEPTION( geode::contain( corner_uuids, boundary.id() ),
                "[Test] All Lines incidences should be Corners" );
        }
    }
}

void add_line_surface_boundary_relation( const geode::Section& model,
    geode::SectionBuilder& builder,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids )
{
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[0] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[1] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[2] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[2] ), model.surface( surface_uuids[1] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[3] ), model.surface( surface_uuids[1] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[4] ), model.surface( surface_uuids[1] ) );
    builder.add_line_surface_boundary_relationship(
        model.line( line_uuids[5] ), model.surface( surface_uuids[1] ) );

    for( const auto& line_id : line_uuids )
    {
        for( const auto& incidence : model.incidences( model.line( line_id ) ) )
        {
            OPENGEODE_EXCEPTION(
                geode::contain( surface_uuids, incidence.id() ),
                "[Test] All Lines incidences should be Surfaces" );
        }
    }
    OPENGEODE_EXCEPTION( model.nb_boundaries( surface_uuids[0] ) == 3,
        "[Test] Surface 0 should have 3 Lines as boundaries" );
    OPENGEODE_EXCEPTION( model.nb_boundaries( surface_uuids[1] ) == 4,
        "[Test] Surface 1 should have 4 Lines as boundaries" );
}

void add_lines_in_model_boundaries( const geode::Section& model,
    geode::SectionBuilder& builder,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& boundary_uuids )
{
    builder.add_line_in_model_boundary( model.line( line_uuids[0] ),
        model.model_boundary( boundary_uuids[0] ) );
    for( auto i : geode::Range{ 1, 3 } )
    {
        builder.add_line_in_model_boundary( model.line( line_uuids[i] ),
            model.model_boundary( boundary_uuids[1] ) );
    }

    for( auto i : geode::Range{ 3 } )
    {
        OPENGEODE_EXCEPTION( model.nb_collections( line_uuids[i] ) == 1,
            "[Test] This Line should be in 1 collection (of type Boundary)" );
    }
    OPENGEODE_EXCEPTION( model.nb_collections( line_uuids[4] ) == 0,
        "[Test] Last Line should be in no collection (of type Boundary)" );
}

void add_internal_relations( const geode::Section& model,
    geode::SectionBuilder& builder,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids )
{
    for( const auto& line_id : line_uuids )
    {
        builder.add_line_surface_internal_relationship(
            model.line( line_id ), model.surface( surface_uuids.front() ) );
    }

    for( const auto& line_id : line_uuids )
    {
        for( const auto& embedding : model.embeddings( model.line( line_id ) ) )
        {
            OPENGEODE_EXCEPTION(
                geode::contain( surface_uuids, embedding.id() ),
                "[Test] All Lines embeddings should be Surfaces" );
        }
        OPENGEODE_EXCEPTION( model.nb_embeddings( line_id ) == 1,
            "[Test] All Lines should be embedded to 1 Surface" );
    }

    OPENGEODE_EXCEPTION(
        model.nb_internals( surface_uuids.front() ) == line_uuids.size(),
        "[Test] The Surface should embed all Lines (that are internal to the "
        "Block)" );
}

void test_boundary_ranges( const geode::Section& model,
    const std::vector< geode::uuid >& corner_uuids,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids )
{
    const auto& line_boundaries =
        model.boundaries( model.line( line_uuids[0] ) );
    geode::index_t line_boundary_count{ 0 };
    for( const auto& line_boundary : line_boundaries )
    {
        line_boundary_count++;
        OPENGEODE_EXCEPTION( line_boundary.id() == corner_uuids[0]
                                 || line_boundary.id() == corner_uuids[1],
            "[Test] LineBoundaryRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( line_boundary_count == 2,
        "[Test] LineBoundaryRange should iterates on 2 Corners" );

    const auto& surface_boundaries =
        model.boundaries( model.surface( surface_uuids[0] ) );
    geode::index_t surface_boundary_count{ 0 };
    for( const auto& surface_boundary : surface_boundaries )
    {
        surface_boundary_count++;
        OPENGEODE_EXCEPTION( surface_boundary.id() == line_uuids[0]
                                 || surface_boundary.id() == line_uuids[1]
                                 || surface_boundary.id() == line_uuids[2],
            "[Test] SurfaceBoundaryRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( surface_boundary_count == 3,
        "[Test] SurfaceBoundaryRange should iterates on 3 Lines" );
}

void test_incidence_ranges( const geode::Section& model,
    const std::vector< geode::uuid >& corner_uuids,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& surface_uuids )
{
    const auto& corner_incidences =
        model.incidences( model.corner( corner_uuids[0] ) );
    geode::index_t corner_incidence_count{ 0 };
    for( const auto& corner_incidence : corner_incidences )
    {
        corner_incidence_count++;
        OPENGEODE_EXCEPTION( corner_incidence.id() == line_uuids[0]
                                 || corner_incidence.id() == line_uuids[1],
            "[Test] CornerIncidenceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( corner_incidence_count == 2,
        "[Test] CornerIncidenceRange should iterates on 2 Lines" );

    const auto& line_incidences =
        model.incidences( model.line( line_uuids[0] ) );
    geode::index_t line_incidence_count{ 0 };
    for( const auto& line_incidence : line_incidences )
    {
        line_incidence_count++;
        OPENGEODE_EXCEPTION( line_incidence.id() == surface_uuids[0],
            "[Test] LineIncidenceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( line_incidence_count == 1,
        "[Test] LineIncidenceRange should iterates on 1 Surface" );
}

void test_item_ranges( const geode::Section& model,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& boundary_uuids )
{
    const auto& boundary_items =
        model.items( model.model_boundary( boundary_uuids[1] ) );
    geode::index_t boundary_item_count{ 0 };
    for( const auto& boundary_item : boundary_items )
    {
        boundary_item_count++;
        OPENGEODE_EXCEPTION( boundary_item.id() == line_uuids[1]
                                 || boundary_item.id() == line_uuids[2],
            "[Test] ModelBoundaryItemRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( boundary_item_count == 2,
        "[Test] CornerIncidenceRange should iterates on 2 Lines (Boundary 1)" );
}

void test_clone( const geode::Section& section )
{
    geode::Section section2;
    geode::SectionBuilder builder{ section2 };
    builder.copy( section );
    OPENGEODE_EXCEPTION(
        section2.nb_corners() == 5, "[Test] Section should have 5 corners" );
    OPENGEODE_EXCEPTION(
        section2.nb_lines() == 6, "[Test] Section should have 6 lines" );
    OPENGEODE_EXCEPTION(
        section2.nb_surfaces() == 2, "[Test] Section should have 2 surfaces" );
    OPENGEODE_EXCEPTION( section2.nb_model_boundaries() == 2,
        "[Test] Section should have 2 model boundaries" );

    auto mapping = builder.copy_components( section );
    builder.copy_component_relationships( mapping, section );
    OPENGEODE_EXCEPTION(
        section2.nb_corners() == 10, "[Test] Section should have 10 corners" );
    OPENGEODE_EXCEPTION(
        section2.nb_lines() == 12, "[Test] Section should have 12 lines" );
    OPENGEODE_EXCEPTION(
        section2.nb_surfaces() == 4, "[Test] Section should have 4 surfaces" );
    OPENGEODE_EXCEPTION( section2.nb_model_boundaries() == 4,
        "[Test] Section should have 4 model boundaries" );

    for( const auto& corner : section.corners() )
    {
        const auto& new_corner =
            section2.corner( mapping.corners.at( corner.id() ) );
        for( const auto& line : section.incidences( corner ) )
        {
            bool found = { false };
            for( const auto& new_line : section2.incidences( new_corner ) )
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
    for( const auto& line : section.lines() )
    {
        const auto& new_line = section2.line( mapping.lines.at( line.id() ) );
        for( const auto& surface : section.incidences( line ) )
        {
            bool found = { false };
            for( const auto& new_surface : section2.incidences( new_line ) )
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
    for( const auto& model_boundary : section.model_boundaries() )
    {
        const auto& new_model_boundary = section2.model_boundary(
            mapping.model_boundaries.at( model_boundary.id() ) );
        for( const auto& line : section.items( model_boundary ) )
        {
            bool found = { false };
            for( const auto& new_line : section2.items( new_model_boundary ) )
            {
                if( mapping.lines.at( line.id() ) == new_line.id() )
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
        Section model;
        SectionBuilder builder( model );

        // This Section represents a house (with one triangle and one square as
        // in children sketches)
        auto corner_uuids = add_corners( model, builder );
        auto line_uuids = add_lines( model, builder );
        auto surface_uuids = add_surfaces( model, builder );
        auto model_boundary_uuids = add_model_boundaries( model, builder );

        add_corner_line_boundary_relation( model, builder, corner_uuids, line_uuids );
        add_line_surface_boundary_relation( model, builder, line_uuids, surface_uuids );
        add_lines_in_model_boundaries(
            model, builder, line_uuids, model_boundary_uuids );
        add_internal_relations( model, builder, line_uuids, surface_uuids );
        test_boundary_ranges( model, corner_uuids, line_uuids, surface_uuids );
        test_incidence_ranges( model, corner_uuids, line_uuids, surface_uuids );
        test_item_ranges( model, line_uuids, model_boundary_uuids );

        std::string file_io{ "test." + model.native_extension() };
        save_section( model, file_io );

        Section model2;
        load_section( model2, file_io );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
