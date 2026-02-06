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

#include <geode/mesh/helpers/detail/solid_mesh_validity_fix.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        GenericMapping< index_t > repair_non_manifold_vertices(
            const SolidMesh< dimension >& mesh,
            SolidMeshBuilder< dimension >& builder )
        {
            absl::FixedArray< PolyhedraAroundVertex > polyhedra_around_vertices(
                mesh.nb_vertices() );
            for( const auto p : Range{ mesh.nb_polyhedra() } )
            {
                for( const auto v : LRange{ mesh.nb_polyhedron_vertices( p ) } )
                {
                    polyhedra_around_vertices[mesh.polyhedron_vertex(
                                                  { p, v } )]
                        .emplace_back( p, v );
                }
            }
            GenericMapping< index_t > vertex_mappings;
            for( const auto vertex_id : Range{ mesh.nb_vertices() } )
            {
                auto polyhedra_around =
                    mesh.polyhedra_around_vertex( vertex_id );
                const auto& polyhedron_vertices =
                    polyhedra_around_vertices[vertex_id];
                auto nb_polyhedra_around = polyhedra_around.size();
                if( nb_polyhedra_around == polyhedron_vertices.size() )
                {
                    continue;
                }
                PolyhedraAroundVertex total_polyhedra;
                while( nb_polyhedra_around != polyhedron_vertices.size() )
                {
                    for( auto&& polyhedron : polyhedra_around )
                    {
                        total_polyhedra.emplace_back( std::move( polyhedron ) );
                    }
                    const auto new_vertex_id =
                        builder.create_point( mesh.point( vertex_id ) );
                    mesh.vertex_attribute_manager().copy_attribute_value(
                        vertex_id, new_vertex_id );
                    builder.replace_vertex( vertex_id, new_vertex_id );
                    for( const auto& polyhedron_vertex : polyhedron_vertices )
                    {
                        if( absl::c_find( total_polyhedra, polyhedron_vertex )
                            == total_polyhedra.end() )
                        {
                            builder.associate_polyhedron_vertex_to_vertex(
                                polyhedron_vertex, vertex_id );
                            break;
                        }
                    }
                    vertex_mappings.map( vertex_id, new_vertex_id );
                    polyhedra_around =
                        mesh.polyhedra_around_vertex( vertex_id );
                    nb_polyhedra_around += polyhedra_around.size();
                }
            }
            return vertex_mappings;
        }

        template GenericMapping< index_t >
            opengeode_mesh_api repair_non_manifold_vertices(
                const SolidMesh< 3 >&, SolidMeshBuilder< 3 >& );
    } // namespace detail
} // namespace geode