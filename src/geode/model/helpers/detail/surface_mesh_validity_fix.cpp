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

#include <absl/container/flat_hash_map.h>

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
        template < typename Model >
        void repair_non_manifold_vertices(
            Model& model, const Surface< Model::dim >& surface )
        {
            const auto& mesh = surface.mesh();
            typename Model::Builder model_builder{ model };
            auto mesh_builder =
                model_builder.surface_mesh_builder( surface.id() );
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
            absl::flat_hash_map< index_t, std::vector< index_t > > mappings;
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
                        mesh_builder->create_point( mesh.point( v ) );
                    mesh_builder->replace_vertex( v, new_vertex_id );
                    for( const auto& polygon_vertex : polygon_vertices )
                    {
                        if( absl::c_find( total_polygons, polygon_vertex )
                            == total_polygons.end() )
                        {
                            mesh_builder->associate_polygon_vertex_to_vertex(
                                polygon_vertex, v );
                            break;
                        }
                    }
                    new_values.push_back( new_vertex_id );
                    polygons_around = mesh.polygons_around_vertex( v );
                    nb_polygons_around += polygons_around.size();
                }
                mappings.try_emplace( v, std::move( new_values ) );
            }
            for( const auto& [old_vertex, new_vertices] : mappings )
            {
                const auto unique_vertex = model.unique_vertex(
                    { surface.component_id(), old_vertex } );
                for( const auto new_vertex : new_vertices )
                {
                    model_builder.set_unique_vertex(
                        { surface.component_id(), new_vertex }, unique_vertex );
                }
            }
        }

        template void opengeode_model_api
            repair_non_manifold_vertices< Section >(
                Section&, const Surface2D& );
        template void opengeode_model_api repair_non_manifold_vertices< BRep >(
            BRep&, const Surface3D& );
    } // namespace detail
} // namespace geode