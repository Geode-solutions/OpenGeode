/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/model/helpers/component_mensurations.h>

#include <queue>

#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/mensuration.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/component_mesh_edges.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    double surface_volume(
        const geode::SurfaceMesh3D& mesh, const geode::Point3D& anchor )
    {
        double volume{ 0 };
        for( const auto p : geode::Range{ mesh.nb_polygons() } )
        {
            const auto& p0 = mesh.point( mesh.polygon_vertex( { p, 0 } ) );
            for( const auto v :
                geode::LRange{ 1, mesh.nb_polygon_vertices( p ) - 1 } )
            {
                volume += geode::tetrahedron_signed_volume( { p0,
                    mesh.point( mesh.polygon_vertex( { p, v } ) ),
                    mesh.point( mesh.polygon_vertex(
                        { p, static_cast< geode::local_index_t >( v + 1 ) } ) ),
                    anchor } );
            }
        }
        return volume;
    }
    absl::optional< bool > find_surface_side( const geode::BRep& brep,
        const geode::uuid& surface_uuid,
        const absl::flat_hash_map< geode::uuid, bool >& processed )
    {
        const auto& surface = brep.surface( surface_uuid );
        for( const auto& line : brep.boundaries( surface ) )
        {
            for( const auto& other_surface : brep.incidences( line ) )
            {
                const auto& other_surface_itr =
                    processed.find( other_surface.id() );
                if( other_surface_itr == processed.end() )
                {
                    continue;
                }
                const auto unique_vertices =
                    geode::edge_unique_vertices( brep, line, 0 );
                const auto& surfaces_cmes =
                    geode::detail::surface_component_mesh_edges(
                        brep, unique_vertices );
                const auto polygon_edge =
                    surfaces_cmes.at( surface_uuid ).front();
                const auto other_polygon_edge =
                    surfaces_cmes.at( other_surface.id() ).front();
                const auto different_orientation =
                    surface.mesh().polygon_vertex( polygon_edge )
                    == other_surface.mesh().polygon_vertex(
                        other_polygon_edge );
                return other_surface_itr->second != different_orientation;
            }
        }
        return absl::nullopt;
    }

    std::vector< std::pair< geode::uuid, bool > > sided_surfaces(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        std::vector< std::pair< geode::uuid, bool > > surfaces;
        surfaces.reserve( brep.nb_boundaries( block.id() ) );
        std::queue< geode::uuid > to_process;
        for( const auto& surface : brep.boundaries( block ) )
        {
            to_process.emplace( surface.id() );
        }
        absl::flat_hash_map< geode::uuid, bool > processed;
        const auto first_surface_uuid = to_process.front();
        to_process.pop();
        surfaces.emplace_back( first_surface_uuid, true );
        processed.emplace( first_surface_uuid, true );
        while( !to_process.empty() )
        {
            const auto surface_uuid = to_process.front();
            to_process.pop();
            if( processed.contains( surface_uuid ) )
            {
                continue;
            }
            const auto surface_side =
                find_surface_side( brep, surface_uuid, processed );
            if( surface_side )
            {
                surfaces.emplace_back( surface_uuid, surface_side );
                processed.emplace( surface_uuid, surface_side );
            }
            else
            {
                to_process.emplace( surface_uuid );
            }
        }
        return surfaces;
    }
} // namespace

namespace geode
{
    double block_volume( const BRep& brep, const Block3D& block )
    {
        double total_volume{ 0 };
        const auto& surfaces = sided_surfaces( brep, block );
        const auto& anchor =
            brep.surface( surfaces.front().first ).mesh().point( 0 );
        for( const auto& surface : surfaces )
        {
            const auto volume =
                surface_volume( brep.surface( surface.first ).mesh(), anchor );
            if( surface.second )
            {
                total_volume += volume;
            }
            else
            {
                total_volume -= volume;
            }
        }
        return std::fabs( total_volume );
    }

    template < index_t dimension >
    double surface_area( const Surface< dimension >& surface )
    {
        double total_area{ 0 };
        for( const auto p : Range{ surface.mesh().nb_polygons() } )
        {
            total_area += surface.mesh().polygon_area( p );
        }
        return total_area;
    }

    template double opengeode_model_api surface_area( const Surface3D& );
    template double opengeode_model_api surface_area( const Surface2D& );
} // namespace geode
