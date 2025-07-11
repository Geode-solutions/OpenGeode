/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/model/helpers/detail/surface_mesh_validity_fix.hpp>

#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        GenericMapping< index_t > repair_non_manifold_vertices(
            const SurfaceMesh< dimension >& mesh,
            SurfaceMeshBuilder< dimension >& builder )
        {
            absl::FixedArray< PolygonsAroundVertex > polygons_around_vertices(
                mesh.nb_vertices() );
            for( const auto p : Range{ mesh.nb_polygons() } )
            {
                for( const auto v : LRange{ mesh.nb_polygon_vertices( p ) } )
                {
                    polygons_around_vertices[mesh.polygon_vertex( { p, v } )]
                        .emplace_back( p, v );
                }
            }
            GenericMapping< index_t > vertex_mappings;
            for( const auto v : Range{ mesh.nb_vertices() } )
            {
                auto polygons_around = mesh.polygons_around_vertex( v );
                const auto& polygon_vertices = polygons_around_vertices[v];
                auto nb_polygons_around = polygons_around.size();
                if( nb_polygons_around == polygon_vertices.size() )
                {
                    continue;
                }
                std::vector< index_t > new_values;
                PolygonsAroundVertex total_polygons;
                while( nb_polygons_around != polygon_vertices.size() )
                {
                    for( auto&& polygon : polygons_around )
                    {
                        total_polygons.emplace_back( std::move( polygon ) );
                    }
                    const auto new_vertex_id =
                        builder.create_point( mesh.point( v ) );
                    builder.replace_vertex( v, new_vertex_id );
                    for( const auto& polygon_vertex : polygon_vertices )
                    {
                        if( absl::c_find( total_polygons, polygon_vertex )
                            == total_polygons.end() )
                        {
                            builder.associate_polygon_vertex_to_vertex(
                                polygon_vertex, v );
                            break;
                        }
                    }
                    vertex_mappings.map( v, new_vertex_id );
                    polygons_around = mesh.polygons_around_vertex( v );
                    nb_polygons_around += polygons_around.size();
                }
            }
            return vertex_mappings;
        }

        template GenericMapping< index_t >
            opengeode_mesh_api repair_non_manifold_vertices(
                const SurfaceMesh< 2 >&, SurfaceMeshBuilder< 2 >& );
        template GenericMapping< index_t >
            opengeode_mesh_api repair_non_manifold_vertices(
                const SurfaceMesh< 3 >&, SurfaceMeshBuilder< 3 >& );
    } // namespace detail
} // namespace geode