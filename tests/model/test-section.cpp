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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/geode/geode_edged_curve.hpp>
#include <geode/mesh/core/geode/geode_point_set.hpp>
#include <geode/mesh/core/geode/geode_polygonal_surface.hpp>
#include <geode/mesh/core/geode/geode_polyhedral_solid.hpp>
#include <geode/mesh/core/point_set.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/detail/count_relationships.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/section_input.hpp>
#include <geode/model/representation/io/section_output.hpp>

#include <geode/tests/common.hpp>

std::array< geode::uuid, 5 > add_corners(
    const geode::Section& model, geode::SectionBuilder& builder )
{
    std::array< geode::uuid, 5 > uuids;
    for( const auto c : geode::Range{ 5 } )
    {
        uuids[c] = builder.add_corner();
    }
    const auto& temp_corner = model.corner(
        builder.add_corner( geode::OpenGeodePointSet2D::impl_name_static() ) );
    builder.remove_corner( temp_corner );
    const auto message =
        absl::StrCat( "[Test] Section should have ", 5, " corners" );
    OPENGEODE_EXCEPTION( model.nb_corners() == 5, message );
    OPENGEODE_EXCEPTION(
        geode::detail::count_relationships( model.corners() ) == 5, message );
    return uuids;
}

std::array< geode::uuid, 6 > add_lines(
    const geode::Section& model, geode::SectionBuilder& builder )
{
    std::array< geode::uuid, 6 > uuids;
    for( auto l : geode::Range{ 6 } )
    {
        uuids[l] = builder.add_line();
    }
    const auto& temp_line = model.line(
        builder.add_line( geode::OpenGeodeEdgedCurve2D::impl_name_static() ) );
    builder.remove_line( temp_line );
    const auto message =
        absl::StrCat( "[Test] Section should have ", 6, " lines" );
    OPENGEODE_EXCEPTION( model.nb_lines() == 6, message );
    OPENGEODE_EXCEPTION(
        geode::detail::count_relationships( model.lines() ) == 6, message );
    return uuids;
}

std::array< geode::uuid, 2 > add_surfaces(
    const geode::Section& model, geode::SectionBuilder& builder )
{
    std::array< geode::uuid, 2 > uuids;
    for( const auto s : geode::Range{ 2 } )
    {
        uuids[s] = builder.add_surface();
    }
    const auto& temp_surface = model.surface( builder.add_surface(
        geode::OpenGeodePolygonalSurface2D::impl_name_static() ) );
    builder.remove_surface( temp_surface );
    const auto message =
        absl::StrCat( "[Test] Section should have ", 2, " surfaces" );
    OPENGEODE_EXCEPTION( model.nb_surfaces() == 2, message );
    OPENGEODE_EXCEPTION(
        geode::detail::count_relationships( model.surfaces() ) == 2, message );
    return uuids;
}

std::array< geode::uuid, 2 > add_model_boundaries(
    const geode::Section& section, geode::SectionBuilder& builder )
{
    std::array< geode::uuid, 2 > uuids;
    for( auto mb : geode::Range{ 2 } )
    {
        uuids[mb] = builder.add_model_boundary();
        builder.set_model_boundary_name(
            uuids[mb], absl::StrCat( "boundary", mb + 1 ) );
    }
    const auto& temp_boundary =
        section.model_boundary( builder.add_model_boundary() );
    builder.remove_model_boundary( temp_boundary );
    const auto message =
        absl::StrCat( "[Test] Section should have ", 2, " model boundaries" );
    OPENGEODE_EXCEPTION( section.nb_model_boundaries() == 2, message );
    OPENGEODE_EXCEPTION(
        geode::detail::count_relationships( section.model_boundaries() ) == 2,
        message );
    OPENGEODE_EXCEPTION(
        section.model_boundary( uuids[0] ).name() == "boundary1",
        "[Test] Wrong ModelBoundary name" );
    return uuids;
}

std::array< geode::uuid, 2 > add_corner_collections(
    const geode::Section& section, geode::SectionBuilder& builder )
{
    std::array< geode::uuid, 2 > uuids;
    for( auto mb : geode::Range{ 2 } )
    {
        uuids[mb] = builder.add_corner_collection();
        builder.set_corner_collection_name(
            uuids[mb], absl::StrCat( "collection", mb + 1 ) );
    }
    const auto& temp_collection =
        section.corner_collection( builder.add_corner_collection() );
    builder.remove_corner_collection( temp_collection );
    const auto message =
        absl::StrCat( "[Test] Section should have ", 2, " corner collections" );
    OPENGEODE_EXCEPTION( section.nb_corner_collections() == 2, message );
    OPENGEODE_EXCEPTION(
        geode::detail::count_relationships( section.corner_collections() ) == 2,
        message );
    OPENGEODE_EXCEPTION(
        section.corner_collection( uuids[0] ).name() == "collection1",
        "[Test] Wrong CornerCollection name" );
    return uuids;
}

std::array< geode::uuid, 2 > add_line_collections(
    const geode::Section& section, geode::SectionBuilder& builder )
{
    std::array< geode::uuid, 2 > uuids;
    for( auto mb : geode::Range{ 2 } )
    {
        uuids[mb] = builder.add_line_collection();
        builder.set_line_collection_name(
            uuids[mb], absl::StrCat( "collection", mb + 1 ) );
    }
    const auto& temp_collection =
        section.line_collection( builder.add_line_collection() );
    builder.remove_line_collection( temp_collection );
    const auto message =
        absl::StrCat( "[Test] Section should have ", 2, " line collections" );
    OPENGEODE_EXCEPTION( section.nb_line_collections() == 2, message );
    OPENGEODE_EXCEPTION(
        geode::detail::count_relationships( section.line_collections() ) == 2,
        message );
    OPENGEODE_EXCEPTION(
        section.line_collection( uuids[0] ).name() == "collection1",
        "[Test] Wrong LineCollection name" );
    return uuids;
}

std::array< geode::uuid, 2 > add_surface_collections(
    const geode::Section& section, geode::SectionBuilder& builder )
{
    std::array< geode::uuid, 2 > uuids;
    for( auto mb : geode::Range{ 2 } )
    {
        uuids[mb] = builder.add_surface_collection();
        builder.set_surface_collection_name(
            uuids[mb], absl::StrCat( "collection", mb + 1 ) );
    }
    const auto& temp_collection =
        section.surface_collection( builder.add_surface_collection() );
    builder.remove_surface_collection( temp_collection );
    const auto message = absl::StrCat(
        "[Test] Section should have ", 2, " surface collections" );
    OPENGEODE_EXCEPTION( section.nb_surface_collections() == 2, message );
    OPENGEODE_EXCEPTION(
        geode::detail::count_relationships( section.surface_collections() )
            == 2,
        message );
    OPENGEODE_EXCEPTION(
        section.surface_collection( uuids[0] ).name() == "collection1",
        "[Test] Wrong SurfaceCollection name" );
    return uuids;
}

void add_corner_line_boundary_relation( const geode::Section& model,
    geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > corner_uuids,
    absl::Span< const geode::uuid > line_uuids )
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
            OPENGEODE_EXCEPTION(
                absl::c_find( line_uuids, incidence.id() ) != line_uuids.end(),
                "[Test] All Corners incidences should be Lines" );
        }
    }

    for( const auto& line_id : line_uuids )
    {
        for( const auto& boundary : model.boundaries( model.line( line_id ) ) )
        {
            OPENGEODE_EXCEPTION( absl::c_find( corner_uuids, boundary.id() )
                                     != corner_uuids.end(),
                "[Test] All Lines boundaries should be Corners" );
        }
    }
}

void add_line_surface_boundary_relation( const geode::Section& model,
    geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > surface_uuids )
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
            OPENGEODE_EXCEPTION( surface_uuids[0] == incidence.id()
                                     || surface_uuids[1] == incidence.id(),
                "[Test] All Lines incidences should be Surfaces" );
        }
    }
    OPENGEODE_EXCEPTION( model.nb_boundaries( surface_uuids[0] ) == 3,
        "[Test] Surface 0 should have 3 Lines as boundaries" );
    OPENGEODE_EXCEPTION( model.nb_boundaries( surface_uuids[1] ) == 4,
        "[Test] Surface 1 should have 4 Lines as boundaries" );
}

void set_geometry( geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > corner_uuids,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > surface_uuids )
{
    std::array< geode::Point2D, 5 > points;
    points[0] = geode::Point2D{ { 1., 2. } };
    points[1] = geode::Point2D{ { 0., 1. } };
    points[2] = geode::Point2D{ { 2., 1. } };
    points[3] = geode::Point2D{ { 0., 2. } };
    points[4] = geode::Point2D{ { 2., 2. } };
    for( const auto i : geode::Range{ 5 } )
    {
        builder.corner_mesh_builder( corner_uuids[i] )
            ->create_point( points[i] );
    }
    builder.line_mesh_builder( line_uuids[0] )->create_point( points[0] );
    builder.line_mesh_builder( line_uuids[0] )->create_point( points[1] );
    builder.line_mesh_builder( line_uuids[1] )->create_point( points[0] );
    builder.line_mesh_builder( line_uuids[1] )->create_point( points[2] );
    builder.line_mesh_builder( line_uuids[2] )->create_point( points[1] );
    builder.line_mesh_builder( line_uuids[2] )->create_point( points[2] );
    builder.line_mesh_builder( line_uuids[3] )->create_point( points[1] );
    builder.line_mesh_builder( line_uuids[3] )->create_point( points[3] );
    builder.line_mesh_builder( line_uuids[4] )->create_point( points[2] );
    builder.line_mesh_builder( line_uuids[4] )->create_point( points[4] );
    builder.line_mesh_builder( line_uuids[5] )->create_point( points[3] );
    builder.line_mesh_builder( line_uuids[5] )->create_point( points[4] );
    for( const auto i : geode::Range{ 6 } )
    {
        builder.line_mesh_builder( line_uuids[i] )->create_edge( 0, 1 );
    }

    builder.surface_mesh_builder( surface_uuids[0] )->create_point( points[0] );
    builder.surface_mesh_builder( surface_uuids[0] )->create_point( points[1] );
    builder.surface_mesh_builder( surface_uuids[0] )->create_point( points[2] );
    builder.surface_mesh_builder( surface_uuids[0] )
        ->create_polygon( { 0, 1, 2 } );

    builder.surface_mesh_builder( surface_uuids[1] )->create_point( points[1] );
    builder.surface_mesh_builder( surface_uuids[1] )->create_point( points[2] );
    builder.surface_mesh_builder( surface_uuids[1] )->create_point( points[4] );
    builder.surface_mesh_builder( surface_uuids[1] )->create_point( points[3] );
    builder.surface_mesh_builder( surface_uuids[1] )
        ->create_polygon( { 0, 1, 2 } );
    builder.surface_mesh_builder( surface_uuids[1] )
        ->create_polygon( { 0, 2, 3 } );
}

void add_lines_in_model_boundaries( const geode::Section& model,
    geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > boundary_uuids )
{
    builder.add_line_in_model_boundary( model.line( line_uuids[0] ),
        model.model_boundary( boundary_uuids[0] ) );
    for( const auto i : geode::Range{ 1, 5 } )
    {
        builder.add_line_in_model_boundary( model.line( line_uuids[i] ),
            model.model_boundary( boundary_uuids[1] ) );
    }

    for( const auto i : geode::Range{ 5 } )
    {
        OPENGEODE_EXCEPTION( model.nb_collections( line_uuids[i] ) == 1,
            "[Test] This Line should be in 1 collection (of type Boundary)" );
    }
    OPENGEODE_EXCEPTION( model.nb_collections( line_uuids[5] ) == 0,
        "[Test] Last Line should be in no collection." );
}

void add_corners_in_corner_collections( const geode::Section& model,
    geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > corner_uuids,
    absl::Span< const geode::uuid > collection_uuids )
{
    for( const auto i : geode::Range{ 2 } )
    {
        builder.add_corner_in_corner_collection(
            model.corner( corner_uuids[i] ),
            model.corner_collection( collection_uuids[0] ) );
    }
    for( const auto i : geode::Range{ 2, 4 } )
    {
        builder.add_corner_in_corner_collection(
            model.corner( corner_uuids[i] ),
            model.corner_collection( collection_uuids[1] ) );
    }

    for( const auto i : geode::Range{ 4 } )
    {
        OPENGEODE_EXCEPTION( model.nb_collections( corner_uuids[i] ) == 1,
            "[Test] This Corner should be in 1 collection (of type "
            "CornerCollection)" );
    }
    OPENGEODE_EXCEPTION( model.nb_collections( corner_uuids[4] ) == 0,
        "[Test] Last Corner should be in no collection (of type Collection)" );
}

void add_lines_in_line_collections( const geode::Section& model,
    geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > collection_uuids )
{
    for( const auto i : geode::Range{ 2 } )
    {
        builder.add_line_in_line_collection( model.line( line_uuids[i] ),
            model.line_collection( collection_uuids[0] ) );
    }
    for( const auto i : geode::Range{ 2, 5 } )
    {
        builder.add_line_in_line_collection( model.line( line_uuids[i] ),
            model.line_collection( collection_uuids[1] ) );
    }

    for( const auto i : geode::Range{ 5 } )
    {
        OPENGEODE_EXCEPTION( model.nb_collections( line_uuids[i] ) == 2,
            "[Test] This Line should be in 2 collections (of type "
            "Boundary and LineCollection), not ",
            model.nb_collections( line_uuids[i] ) );
    }
    OPENGEODE_EXCEPTION( model.nb_collections( line_uuids[5] ) == 0,
        "[Test] Last Line should be in no collection." );
}

void add_surfaces_in_surface_collections( const geode::Section& model,
    geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > surface_uuids,
    absl::Span< const geode::uuid > collection_uuids )
{
    for( const auto i : geode::Range{ 2 } )
    {
        builder.add_surface_in_surface_collection(
            model.surface( surface_uuids[i] ),
            model.surface_collection( collection_uuids[i] ) );
    }

    for( const auto i : geode::Range{ 2 } )
    {
        OPENGEODE_EXCEPTION( model.nb_collections( surface_uuids[i] ) == 1,
            "[Test] This Surface should be in 1 collection (of type "
            "SurfaceCollection)" );
    }
}

void add_internal_corner_relations( const geode::Section& model,
    geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > corner_uuids,
    absl::Span< const geode::uuid > surface_uuids )
{
    for( const auto& corner_id : corner_uuids )
    {
        builder.add_corner_surface_internal_relationship(
            model.corner( corner_id ), model.surface( surface_uuids.front() ) );
    }

    for( const auto& corner_id : corner_uuids )
    {
        for( const auto& embedding :
            model.embedding_surfaces( model.corner( corner_id ) ) )
        {
            OPENGEODE_EXCEPTION( surface_uuids.front() == embedding.id(),
                "[Test] All Corners embeddings should be Surfaces" );
            OPENGEODE_EXCEPTION(
                model.nb_internal_corners( embedding ) == corner_uuids.size(),
                "[Test] Surface should embed all Corners" );
        }
        OPENGEODE_EXCEPTION( model.nb_embeddings( corner_id ) == 1,
            "[Test] All Corners should be embedded to 1 Surface" );
        OPENGEODE_EXCEPTION(
            model.nb_embedding_surfaces( model.corner( corner_id ) ) == 1,
            "[Test] All Corners should be embedded to 1 Surface" );
    }
}

void add_internal_line_relations( const geode::Section& model,
    geode::SectionBuilder& builder,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > surface_uuids )
{
    builder.add_line_surface_internal_relationship(
        model.line( line_uuids[3] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_internal_relationship(
        model.line( line_uuids[4] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_internal_relationship(
        model.line( line_uuids[5] ), model.surface( surface_uuids[0] ) );
    builder.add_line_surface_internal_relationship(
        model.line( line_uuids[0] ), model.surface( surface_uuids[1] ) );
    builder.add_line_surface_internal_relationship(
        model.line( line_uuids[1] ), model.surface( surface_uuids[1] ) );

    for( const auto& line_id : geode::LRange{ 0, 1 } )
    {
        for( const auto& embedding :
            model.embedding_surfaces( model.line( line_uuids[line_id] ) ) )
        {
            OPENGEODE_EXCEPTION( embedding.id() == surface_uuids[1],
                "[Test] Lines embeddings are wrong" );
        }
        OPENGEODE_EXCEPTION( model.nb_embeddings( line_uuids[line_id] ) == 1,
            "[Test] All Lines should be embedded to 1 Surface" );
        OPENGEODE_EXCEPTION(
            model.nb_embedding_surfaces( model.line( line_uuids[line_id] ) )
                == 1,
            "[Test] All Lines should be embedded to 1 Surface" );
    }
    OPENGEODE_EXCEPTION(
        model.nb_internal_lines( model.surface( surface_uuids[0] ) ) == 3,
        "[Test] Surface should embed 3 Lines" );
    OPENGEODE_EXCEPTION(
        model.nb_internal_lines( model.surface( surface_uuids[1] ) ) == 2,
        "[Test] Surface should embed 2 Lines" );
}

void test_boundary_ranges( const geode::Section& model,
    absl::Span< const geode::uuid > corner_uuids,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > surface_uuids )
{
    geode::index_t line_boundary_count{ 0 };
    for( const auto& line_boundary :
        model.boundaries( model.line( line_uuids[0] ) ) )
    {
        line_boundary_count++;
        OPENGEODE_EXCEPTION( line_boundary.id() == corner_uuids[0]
                                 || line_boundary.id() == corner_uuids[1],
            "[Test] BoundaryCornerRange iteration result is not correct" );
        OPENGEODE_EXCEPTION(
            model.is_boundary( line_boundary, model.line( line_uuids[0] ) ),
            "[Test] Corner should be boundary of Line" );
    }
    OPENGEODE_EXCEPTION( line_boundary_count == 2,
        "[Test] BoundaryCornerRange should iterates on 2 Corners" );

    geode::index_t surface_boundary_count{ 0 };
    for( const auto& surface_boundary :
        model.boundaries( model.surface( surface_uuids[0] ) ) )
    {
        surface_boundary_count++;
        OPENGEODE_EXCEPTION( surface_boundary.id() == line_uuids[0]
                                 || surface_boundary.id() == line_uuids[1]
                                 || surface_boundary.id() == line_uuids[2],
            "[Test] BoundaryLineRange iteration result is not correct" );
        OPENGEODE_EXCEPTION( model.is_boundary( surface_boundary,
                                 model.surface( surface_uuids[0] ) ),
            "[Test] Line should be boundary of Surface" );
    }
    OPENGEODE_EXCEPTION( surface_boundary_count == 3,
        "[Test] BoundaryLineRange should iterates on 3 Lines" );
}

void test_incidence_ranges( const geode::Section& model,
    absl::Span< const geode::uuid > corner_uuids,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > surface_uuids )
{
    geode::index_t corner_incidence_count{ 0 };
    for( const auto& corner_incidence :
        model.incidences( model.corner( corner_uuids[0] ) ) )
    {
        corner_incidence_count++;
        OPENGEODE_EXCEPTION( corner_incidence.id() == line_uuids[0]
                                 || corner_incidence.id() == line_uuids[1],
            "[Test] IncidentLineRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( corner_incidence_count == 2,
        "[Test] IncidentLineRange should iterates on 2 Lines" );

    geode::index_t line_incidence_count{ 0 };
    for( const auto& line_incidence :
        model.incidences( model.line( line_uuids[0] ) ) )
    {
        line_incidence_count++;
        OPENGEODE_EXCEPTION( line_incidence.id() == surface_uuids[0],
            "[Test] IncidentSurfaceRange iteration result is not correct" );
    }
    OPENGEODE_EXCEPTION( line_incidence_count == 1,
        "[Test] IncidentSurfaceRange should iterates on 1 Surface" );
}

void test_item_ranges( const geode::Section& model,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > boundary_uuids )
{
    geode::index_t boundary_item_count{ 0 };
    for( const auto& boundary_item : model.model_boundary_items(
             model.model_boundary( boundary_uuids[1] ) ) )
    {
        boundary_item_count++;
        OPENGEODE_EXCEPTION( boundary_item.id() == line_uuids[1]
                                 || boundary_item.id() == line_uuids[2]
                                 || boundary_item.id() == line_uuids[3]
                                 || boundary_item.id() == line_uuids[4],
            "[Test] LineItemRange iteration result is not correct" );
        OPENGEODE_EXCEPTION( model.is_model_boundary_item( boundary_item,
                                 model.model_boundary( boundary_uuids[1] ) ),
            "[Test] Line should be item of ModelBoundary" );
    }
    OPENGEODE_EXCEPTION( boundary_item_count == 4,
        "[Test] LineItemRange should iterates on 2 Lines (Boundary 1)" );
}

void test_corner_collections_ranges( const geode::Section& model,
    absl::Span< const geode::uuid > corner_uuids,
    absl::Span< const geode::uuid > collection_uuids )
{
    geode::index_t collection_item_count{ 0 };
    for( const auto& collection_item : model.corner_collection_items(
             model.corner_collection( collection_uuids[1] ) ) )
    {
        collection_item_count++;
        OPENGEODE_EXCEPTION( collection_item.id() == corner_uuids[2]
                                 || collection_item.id() == corner_uuids[3],
            "[Test] CornerItemRange iteration result is not correct" );
        OPENGEODE_EXCEPTION(
            model.is_corner_collection_item( collection_item,
                model.corner_collection( collection_uuids[1] ) ),
            "[Test] Corner should be item of ModelCollection" );
    }
    OPENGEODE_EXCEPTION( collection_item_count == 2,
        "[Test] CornerItemRange should iterates on 2 Corners (Collection 1)" );
}

void test_line_collections_ranges( const geode::Section& model,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > collection_uuids )
{
    geode::index_t collection_item_count{ 0 };
    for( const auto& collection_item : model.line_collection_items(
             model.line_collection( collection_uuids[1] ) ) )
    {
        collection_item_count++;
        OPENGEODE_EXCEPTION( collection_item.id() == line_uuids[2]
                                 || collection_item.id() == line_uuids[3]
                                 || collection_item.id() == line_uuids[4],
            "[Test] LineItemRange iteration result is not correct" );
        OPENGEODE_EXCEPTION( model.is_line_collection_item( collection_item,
                                 model.line_collection( collection_uuids[1] ) ),
            "[Test] Line should be item of ModelCollection" );
    }
    OPENGEODE_EXCEPTION( collection_item_count == 3,
        "[Test] LineItemRange should iterates on 2 Lines (Collection 1)" );
}

void test_surface_collections_ranges( const geode::Section& model,
    absl::Span< const geode::uuid > surface_uuids,
    absl::Span< const geode::uuid > collection_uuids )
{
    geode::index_t collection_item_count{ 0 };
    for( const auto& collection_item : model.surface_collection_items(
             model.surface_collection( collection_uuids[1] ) ) )
    {
        collection_item_count++;
        OPENGEODE_EXCEPTION( collection_item.id() == surface_uuids[1],
            "[Test] SurfaceItemRange iteration result is not correct" );
        OPENGEODE_EXCEPTION(
            model.is_surface_collection_item( collection_item,
                model.surface_collection( collection_uuids[1] ) ),
            "[Test] Surface should be item of ModelCollection" );
    }
    OPENGEODE_EXCEPTION( collection_item_count == 1,
        "[Test] SurfaceItemRange should iterates "
        "on 1 Surfaces (Collection 1)" );
}

void test_clone( const geode::Section& section )
{
    geode::Section section2;
    geode::SectionBuilder builder{ section2 };
    builder.copy( section );
    OPENGEODE_EXCEPTION(
        section2.nb_corners() == 5, "[Test] Section should have 5 corners" );
    OPENGEODE_EXCEPTION(
        section2.nb_lines() == 6, "[Test] Section should have 6 lines" );
    OPENGEODE_EXCEPTION(
        section2.nb_surfaces() == 2, "[Test] Section should have 2 surfaces" );
    OPENGEODE_EXCEPTION( section2.nb_model_boundaries() == 2,
        "[Test] Section should have 2 model boundaries" );

    const auto mappings = builder.copy_components( section );
    builder.copy_relationships( mappings, section );
    OPENGEODE_EXCEPTION(
        section2.nb_corners() == 10, "[Test] Section should have 10 corners" );
    OPENGEODE_EXCEPTION(
        section2.nb_lines() == 12, "[Test] Section should have 12 lines" );
    OPENGEODE_EXCEPTION(
        section2.nb_surfaces() == 4, "[Test] Section should have 4 surfaces" );
    OPENGEODE_EXCEPTION( section2.nb_model_boundaries() == 4,
        "[Test] Section should have 4 model boundaries" );

    for( const auto& corner : section.corners() )
    {
        const auto& new_corner = section2.corner(
            mappings.at( geode::Corner2D::component_type_static() )
                .in2out( corner.id() ) );
        for( const auto& line : section.incidences( corner ) )
        {
            bool found = { false };
            for( const auto& new_line : section2.incidences( new_corner ) )
            {
                if( mappings.at( geode::Line2D::component_type_static() )
                        .in2out( line.id() )
                    == new_line.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All Corners incidences are not correct" );
        }
    }
    for( const auto& line : section.lines() )
    {
        const auto& new_line =
            section2.line( mappings.at( geode::Line2D::component_type_static() )
                               .in2out( line.id() ) );
        for( const auto& surface : section.incidences( line ) )
        {
            bool found = { false };
            for( const auto& new_surface : section2.incidences( new_line ) )
            {
                if( mappings.at( geode::Surface2D::component_type_static() )
                        .in2out( surface.id() )
                    == new_surface.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All Lines incidences are not correct" );
        }
    }
    for( const auto& model_boundary : section.model_boundaries() )
    {
        const auto& new_model_boundary = section2.model_boundary(
            mappings.at( geode::ModelBoundary2D::component_type_static() )
                .in2out( model_boundary.id() ) );
        for( const auto& line : section.model_boundary_items( model_boundary ) )
        {
            bool found = { false };
            for( const auto& new_line :
                section2.model_boundary_items( new_model_boundary ) )
            {
                if( mappings.at( geode::Line2D::component_type_static() )
                        .in2out( line.id() )
                    == new_line.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All ModelBoundaries items are not correct" );
        }
    }
    for( const auto& collection : section.corner_collections() )
    {
        const auto& new_collection = section2.corner_collection(
            mappings.at( geode::CornerCollection2D::component_type_static() )
                .in2out( collection.id() ) );
        for( const auto& corner :
            section.corner_collection_items( collection ) )
        {
            bool found = { false };
            for( const auto& new_corner :
                section2.corner_collection_items( new_collection ) )
            {
                if( mappings.at( geode::Corner2D::component_type_static() )
                        .in2out( corner.id() )
                    == new_corner.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All CornerCollections items are not correct" );
        }
    }
    for( const auto& collection : section.line_collections() )
    {
        const auto& new_collection = section2.line_collection(
            mappings.at( geode::LineCollection2D::component_type_static() )
                .in2out( collection.id() ) );
        for( const auto& line : section.line_collection_items( collection ) )
        {
            bool found = { false };
            for( const auto& new_line :
                section2.line_collection_items( new_collection ) )
            {
                if( mappings.at( geode::Line2D::component_type_static() )
                        .in2out( line.id() )
                    == new_line.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All LineCollections items are not correct" );
        }
    }
    for( const auto& collection : section.surface_collections() )
    {
        const auto& new_collection = section2.surface_collection(
            mappings.at( geode::SurfaceCollection2D::component_type_static() )
                .in2out( collection.id() ) );
        for( const auto& surface :
            section.surface_collection_items( collection ) )
        {
            bool found = { false };
            for( const auto& new_surface :
                section2.surface_collection_items( new_collection ) )
            {
                if( mappings.at( geode::Surface2D::component_type_static() )
                        .in2out( surface.id() )
                    == new_surface.id() )
                {
                    found = true;
                    break;
                }
            }
            OPENGEODE_EXCEPTION(
                found, "[Test] All SurfaceCollections items are not correct" );
        }
    }
}

void test_compare_section(
    const geode::Section& model, const geode::Section& model2 )
{
    OPENGEODE_EXCEPTION( model.nb_corners() == model2.nb_corners(),
        "[Test] Number of Corners in reloaded Model should be ",
        model.nb_corners() );
    OPENGEODE_EXCEPTION( model.nb_lines() == model2.nb_lines(),
        "[Test] Number of Lines in reloaded Model should be ",
        model.nb_lines() );
    OPENGEODE_EXCEPTION( model.nb_surfaces() == model2.nb_surfaces(),
        "[Test] Number of Surfaces in reloaded Model should be ",
        model.nb_surfaces() );
    OPENGEODE_EXCEPTION(
        model.nb_model_boundaries() == model2.nb_model_boundaries(),
        "[Test] Number of Boundaries in reloaded Model should be ",
        model.nb_model_boundaries() );
    OPENGEODE_EXCEPTION(
        model.nb_corner_collections() == model2.nb_corner_collections(),
        "[Test] Number of CornerCollections in reloaded Model should be ",
        model.nb_corner_collections() );
    OPENGEODE_EXCEPTION(
        model.nb_line_collections() == model2.nb_line_collections(),
        "[Test] Number of LineCollections in reloaded Model should be ",
        model.nb_line_collections() );
    OPENGEODE_EXCEPTION(
        model.nb_surface_collections() == model2.nb_surface_collections(),
        "[Test] Number of SurfaceCollections in reloaded Model should be ",
        model.nb_surface_collections() );
    for( const auto& surface : model.surfaces() )
    {
        OPENGEODE_EXCEPTION( surface.id() == surface.mesh().id(),
            "[Backward_IO] Model surface should have the same uuid as its "
            "mesh." );
        const auto& mesh = surface.mesh();
        const auto& mesh2 = model2.surface( surface.id() ).mesh();
        for( const auto vertex_id : geode::Range{ mesh.nb_vertices() } )
        {
            OPENGEODE_EXCEPTION( mesh.point( vertex_id )
                                     .inexact_equal( mesh2.point( vertex_id ) ),
                "[Test] Wrong reloaded mesh point coordinates." );
        }
    }
    for( const auto& line : model.lines() )
    {
        OPENGEODE_EXCEPTION( line.id() == line.mesh().id(),
            "[Backward_IO] Model line should have the same uuid as its "
            "mesh." );
        const auto& mesh = line.mesh();
        const auto& mesh2 = model2.line( line.id() ).mesh();
        for( const auto vertex_id : geode::Range{ mesh.nb_vertices() } )
        {
            OPENGEODE_EXCEPTION( mesh.point( vertex_id )
                                     .inexact_equal( mesh2.point( vertex_id ) ),
                "[Test] Wrong reloaded mesh point coordinates." );
        }
    }
    for( const auto& corner : model.corners() )
    {
        OPENGEODE_EXCEPTION( corner.id() == corner.mesh().id(),
            "[Backward_IO] Model corner should have the same uuid as its "
            "mesh." );
        const auto& mesh = corner.mesh();
        const auto& mesh2 = model2.corner( corner.id() ).mesh();
        for( const auto vertex_id : geode::Range{ mesh.nb_vertices() } )
        {
            OPENGEODE_EXCEPTION( mesh.point( vertex_id )
                                     .inexact_equal( mesh2.point( vertex_id ) ),
                "[Test] Wrong reloaded mesh point coordinates." );
        }
    }
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    geode::Section model;
    geode::SectionBuilder builder( model );

    // This Section represents a house (with one triangle and one square as
    // in children sketches)
    const auto corner_uuids = add_corners( model, builder );
    const auto line_uuids = add_lines( model, builder );
    const auto surface_uuids = add_surfaces( model, builder );
    const auto model_boundary_uuids = add_model_boundaries( model, builder );
    const auto corner_collection_uuids =
        add_corner_collections( model, builder );
    const auto line_collection_uuids = add_line_collections( model, builder );
    const auto surface_collection_uuids =
        add_surface_collections( model, builder );

    set_geometry( builder, corner_uuids, line_uuids, surface_uuids );

    add_corner_line_boundary_relation(
        model, builder, corner_uuids, line_uuids );
    add_line_surface_boundary_relation(
        model, builder, line_uuids, surface_uuids );
    add_lines_in_model_boundaries(
        model, builder, line_uuids, model_boundary_uuids );
    add_corners_in_corner_collections(
        model, builder, corner_uuids, corner_collection_uuids );
    add_lines_in_line_collections(
        model, builder, line_uuids, line_collection_uuids );
    add_surfaces_in_surface_collections(
        model, builder, surface_uuids, surface_collection_uuids );
    add_internal_corner_relations(
        model, builder, corner_uuids, surface_uuids );
    add_internal_line_relations( model, builder, line_uuids, surface_uuids );
    OPENGEODE_EXCEPTION(
        model.nb_internals( surface_uuids[0] ) == corner_uuids.size() + 3,
        "[Test] The Surface should embed all Corners & Lines (that are "
        "internal to the Surface)" );
    test_boundary_ranges( model, corner_uuids, line_uuids, surface_uuids );
    test_incidence_ranges( model, corner_uuids, line_uuids, surface_uuids );
    test_item_ranges( model, line_uuids, model_boundary_uuids );
    test_corner_collections_ranges(
        model, corner_uuids, corner_collection_uuids );
    test_line_collections_ranges( model, line_uuids, line_collection_uuids );
    test_surface_collections_ranges(
        model, surface_uuids, surface_collection_uuids );
    test_clone( model );

    const auto file_io = absl::StrCat( "test.", model.native_extension() );
    geode::save_section( model, file_io );

    auto model2 = geode::load_section( file_io );
    test_compare_section( model, model2 );

    geode::Section model3{ std::move( model2 ) };
    test_compare_section( model, model3 );
}

OPENGEODE_TEST( "section" )