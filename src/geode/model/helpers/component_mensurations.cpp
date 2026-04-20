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

#include <geode/model/helpers/component_mensurations.hpp>

#include <queue>

#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/component_mesh_edges.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace
{
    double surface_volume(
        const geode::SurfaceMesh3D& mesh, const geode::Point3D& anchor )
    {
        double volume{ 0 };
        for( const auto p : geode::Range{ mesh.nb_polygons() } )
        {
            const auto vertices = mesh.polygon_vertices( p );
            const auto& point0 = mesh.point( vertices[0] );
            for( const auto v :
                geode::LRange{ 2, mesh.nb_polygon_vertices( p ) } )
            {
                volume += geode::tetrahedron_signed_volume(
                    { point0, mesh.point( vertices[v - 1] ),
                        mesh.point( vertices[v] ), anchor } );
            }
        }
        return volume;
    }
    std::optional< bool > find_surface_side( const geode::BRep& brep,
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
                const geode::PolygonVertex polygon_vertex{
                    surfaces_cmes.at( surface_uuid ).front()
                };
                const geode::PolygonVertex other_polygon_vertex{
                    surfaces_cmes.at( other_surface.id() ).front()
                };
                const auto different_orientation =
                    brep.unique_vertex( { surface.component_id(),
                        surface.mesh().polygon_vertex( polygon_vertex ) } )
                    == brep.unique_vertex( { other_surface.component_id(),
                        other_surface.mesh().polygon_vertex(
                            other_polygon_vertex ) } );
                return other_surface_itr->second != different_orientation;
            }
        }
        return std::nullopt;
    }

    struct SidedSurface
    {
        SidedSurface() = default;
        geode::uuid surface_id;
        bool side{ false };
    };

    using GroupedSidedSurfaces = std::vector< SidedSurface >;

    std::queue< geode::uuid > initialize_to_process_boundaries(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        std::queue< geode::uuid > to_process;
        for( const auto& surface : brep.boundaries( block ) )
        {
            to_process.emplace( surface.id() );
        }
        return to_process;
    }

    std::vector< SidedSurface > compute_grouped_sided_surfaces(
        const geode::BRep& brep, std::queue< geode::uuid >& to_process )
    {
        std::vector< SidedSurface > cur_surfaces;
        absl::flat_hash_map< geode::uuid, bool > processed;
        const auto first_surface_uuid = to_process.front();
        to_process.pop();
        auto& itr = cur_surfaces.emplace_back();
        itr.surface_id = first_surface_uuid;
        itr.side = true;
        processed.emplace( first_surface_uuid, true );
        geode::index_t skip_counter{ 0 };
        while( !to_process.empty() )
        {
            if( skip_counter == to_process.size() )
            {
                return cur_surfaces;
            }
            const auto surface_uuid = to_process.front();
            to_process.pop();
            if( processed.contains( surface_uuid ) )
            {
                continue;
            }
            const auto surface_side =
                find_surface_side( brep, surface_uuid, processed );
            if( surface_side.has_value() )
            {
                auto& new_itr = cur_surfaces.emplace_back();
                new_itr.surface_id = surface_uuid;
                new_itr.side = surface_side.value();
                processed.emplace( surface_uuid, surface_side.value() );
            }
            else
            {
                to_process.emplace( surface_uuid );
                skip_counter++;
            }
        }
        return cur_surfaces;
    }

    std::vector< GroupedSidedSurfaces > sided_surfaces(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        std::vector< GroupedSidedSurfaces > grouped_sided_surfaces;
        auto to_process = initialize_to_process_boundaries( brep, block );
        while( !to_process.empty() )
        {
            grouped_sided_surfaces.emplace_back(
                compute_grouped_sided_surfaces( brep, to_process ) );
        }
        if( grouped_sided_surfaces.size() > 1 )
        {
            geode::Logger::warn( block.component_id().string(),
                " has unconnected boundaries. This is either not valid or "
                "the block boundaries are valid and the block shows one or "
                "more holes inside it." );
        }
        return grouped_sided_surfaces;
    }

    double block_mesh_volume( const geode::SolidMesh3D& mesh )
    {
        double total_volume{ 0 };
        for( const auto p : geode::Range{ mesh.nb_polyhedra() } )
        {
            total_volume += mesh.polyhedron_volume( p );
        }
        return total_volume;
    }

    double compute_group_volume(
        const geode::BRep& brep, const std::vector< SidedSurface >& surfaces )
    {
        double group_volume{ 0 };
        const auto& anchor =
            brep.surface( surfaces.front().surface_id ).mesh().point( 0 );
        for( const auto& surface : surfaces )
        {
            const auto volume = surface_volume(
                brep.surface( surface.surface_id ).mesh(), anchor );
            if( surface.side )
            {
                group_volume += volume;
            }
            else
            {
                group_volume -= volume;
            }
        }
        return std::fabs( group_volume );
    }

    double compute_total_volume(
        const std::vector< double >& volumes, geode::index_t biggest_volume_id )
    {
        double total_volume{ volumes[biggest_volume_id] };
        for( const auto volume_id : geode::Indices{ volumes } )
        {
            if( volume_id != biggest_volume_id )
            {
                total_volume -= volumes[volume_id];
            }
        }
        return total_volume;
    }
} // namespace

namespace geode
{
    double block_volume( const BRep& brep, const Block3D& block )
    {
        if( block.mesh().nb_polyhedra() > 0 )
        {
            return block_mesh_volume( block.mesh() );
        }
        const auto& grouped_surfaces = sided_surfaces( brep, block );
        std::vector< double > volumes( grouped_surfaces.size() );
        index_t biggest_volume_id{ 0 };
        double biggest_volume{ 0 };
        for( const auto group_id : geode::Indices{ grouped_surfaces } )
        {
            volumes[group_id] =
                compute_group_volume( brep, grouped_surfaces[group_id] );
            if( volumes[group_id] > biggest_volume )
            {
                biggest_volume = volumes[group_id];
                biggest_volume_id = group_id;
            }
        }
        return compute_total_volume( volumes, biggest_volume_id );
    }

    BoundingBox3D block_bounding_box( const BRep& brep, const Block3D& block )
    {
        if( block.mesh().nb_vertices() != 0 )
        {
            return block.mesh().bounding_box();
        }
        BoundingBox3D result;
        for( const auto& surface : brep.boundaries( block ) )
        {
            result.add_box( surface.mesh().bounding_box() );
        }
        return result;
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
