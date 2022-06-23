/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/model/helpers/component_mesh_edges.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/section.h>

geode::Section build_section()
{
    geode::Section section;
    geode::SectionBuilder builder{ section };
    std::vector< geode::Point2D > points{ { { 0, 0 } }, { { 2, 0 } },
        { { 4, 0 } }, { { 1, 1 } }, { { 3, 1 } }, { { 2, 2 } }, { { 1, -1 } },
        { { 3, -1 } }, { { 2, -2 } } };
    builder.create_unique_vertices( points.size() );

    {
        const auto& line_id = builder.add_line();
        auto line_builder = builder.line_mesh_builder( line_id );
        line_builder->create_point( points[0] );
        line_builder->create_point( points[1] );
        line_builder->create_point( points[2] );
        line_builder->create_point( points[4] );
        line_builder->create_point( points[5] );
        line_builder->create_point( points[3] );
        line_builder->create_edge( 0, 1 );
        line_builder->create_edge( 1, 2 );
        line_builder->create_edge( 2, 3 );
        line_builder->create_edge( 3, 4 );
        line_builder->create_edge( 4, 5 );
        line_builder->create_edge( 5, 0 );
        const auto& line = section.line( line_id );
        builder.set_unique_vertex( { line.component_id(), 0 }, 0 );
        builder.set_unique_vertex( { line.component_id(), 1 }, 1 );
        builder.set_unique_vertex( { line.component_id(), 2 }, 2 );
        builder.set_unique_vertex( { line.component_id(), 3 }, 3 );
        builder.set_unique_vertex( { line.component_id(), 4 }, 4 );
        builder.set_unique_vertex( { line.component_id(), 5 }, 5 );
    }

    {
        const auto& surface_id =
            builder.add_surface( geode::MeshFactory::default_impl(
                geode::TriangulatedSurface2D::type_name_static() ) );
        auto surface_builder =
            builder.surface_mesh_builder< geode::TriangulatedSurface2D >(
                surface_id );
        surface_builder->create_point( points[0] );
        surface_builder->create_point( points[1] );
        surface_builder->create_point( points[2] );
        surface_builder->create_point( points[4] );
        surface_builder->create_point( points[5] );
        surface_builder->create_point( points[3] );
        surface_builder->create_triangle( { 0, 1, 3 } );
        surface_builder->create_triangle( { 1, 2, 4 } );
        surface_builder->create_triangle( { 1, 4, 3 } );
        surface_builder->create_triangle( { 3, 4, 5 } );
        surface_builder->compute_polygon_adjacencies();
        const auto& surface = section.surface( surface_id );
        builder.set_unique_vertex( { surface.component_id(), 0 }, 0 );
        builder.set_unique_vertex( { surface.component_id(), 1 }, 1 );
        builder.set_unique_vertex( { surface.component_id(), 2 }, 2 );
        builder.set_unique_vertex( { surface.component_id(), 3 }, 3 );
        builder.set_unique_vertex( { surface.component_id(), 4 }, 4 );
        builder.set_unique_vertex( { surface.component_id(), 5 }, 5 );
    }

    {
        const auto& surface_id =
            builder.add_surface( geode::MeshFactory::default_impl(
                geode::TriangulatedSurface2D::type_name_static() ) );
        auto surface_builder =
            builder.surface_mesh_builder< geode::TriangulatedSurface2D >(
                surface_id );
        surface_builder->create_point( points[0] );
        surface_builder->create_point( points[1] );
        surface_builder->create_point( points[2] );
        surface_builder->create_point( points[6] );
        surface_builder->create_point( points[7] );
        surface_builder->create_point( points[8] );
        surface_builder->create_triangle( { 0, 1, 3 } );
        surface_builder->create_triangle( { 1, 2, 4 } );
        surface_builder->create_triangle( { 1, 4, 3 } );
        surface_builder->create_triangle( { 3, 4, 5 } );
        surface_builder->compute_polygon_adjacencies();
        const auto& surface = section.surface( surface_id );
        builder.set_unique_vertex( { surface.component_id(), 0 }, 0 );
        builder.set_unique_vertex( { surface.component_id(), 1 }, 1 );
        builder.set_unique_vertex( { surface.component_id(), 2 }, 2 );
        builder.set_unique_vertex( { surface.component_id(), 3 }, 6 );
        builder.set_unique_vertex( { surface.component_id(), 4 }, 7 );
        builder.set_unique_vertex( { surface.component_id(), 5 }, 8 );
    }

    return section;
}

int main()
{
    try
    {
        const auto section = build_section();
        const auto surface_edges = geode::component_mesh_edges(
            section, *section.surfaces().begin(), { 1, 0 } );
        OPENGEODE_EXCEPTION( surface_edges.line_edges.size() == 1,
            "[Test] Wrong number of Lines" );
        for( const auto& lines : surface_edges.line_edges )
        {
            for( const auto& line : lines.second )
            {
                OPENGEODE_EXCEPTION( line == 1, "[Test] Wrong Line edge id" );
            }
        }
        OPENGEODE_EXCEPTION( surface_edges.surface_edges.size() == 2,
            "[Test] Wrong number of Surfaces" );
        for( const auto& surfaces : surface_edges.surface_edges )
        {
            for( const auto& surface : surfaces.second )
            {
                const geode::PolygonEdge edge{ 1, 0 };
                OPENGEODE_EXCEPTION(
                    surface == edge, "[Test] Wrong Surface PolygonEdge" );
            }
        }

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
