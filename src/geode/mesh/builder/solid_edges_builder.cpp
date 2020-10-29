/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/builder/solid_edges_builder.h>

#include <geode/basic/attribute_manager.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_mesh.h>

namespace
{
    template < geode::index_t dimension >
    void check_polyhedron_id( const geode::SolidEdges< dimension >& solid,
        const geode::index_t polyhedron_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        OPENGEODE_ASSERT( polyhedron_id < solid.nb_polyhedra(),
            "[check_polyhedron_id] Trying to access an invalid polyhedron" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_vertex_id(
        const geode::SolidEdges< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t vertex_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT(
            vertex_id < solid.nb_polyhedron_vertices( polyhedron_id ),
            "[check_polyhedron_vertex_id] Trying to access an invalid "
            "polyhedron vertex" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_facet_id( const geode::SolidEdges< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t facet_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( facet_id );
        OPENGEODE_ASSERT(
            facet_id < solid.nb_polyhedron_facets( polyhedron_id ),
            "[check_polyhedron_facet_id] Trying to access an invalid "
            "polyhedron facet" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_facet_vertex_id(
        const geode::SolidEdges< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t facet_id,
        const geode::index_t vertex_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( facet_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < solid.nb_polyhedron_facet_vertices(
                              { polyhedron_id, facet_id } ),
            "[check_polyhedron_facet_vertex_id] Trying to access an invalid "
            "polyhedron facet vertex" );
    }

    template < geode::index_t dimension >
    void update_polyhedron_adjacencies(
        const geode::SolidEdges< dimension >& solid,
        geode::SolidEdgesBuilder< dimension >& builder,
        absl::Span< const geode::index_t > old2new )
    {
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            for( const auto f :
                geode::Range{ solid.nb_polyhedron_facets( p ) } )
            {
                const geode::PolyhedronFacet id{ p, f };
                if( const auto adj = solid.polyhedron_adjacent( id ) )
                {
                    const auto new_adjacent = old2new[adj.value()];
                    if( new_adjacent == geode::NO_ID )
                    {
                        builder.unset_polyhedron_adjacent( id );
                    }
                    else
                    {
                        builder.set_polyhedron_adjacent( id, new_adjacent );
                    }
                }
            }
        }
    }

    template < geode::index_t dimension >
    void update_polyhedron_around_vertices(
        const geode::SolidEdges< dimension >& solid,
        geode::SolidEdgesBuilder< dimension >& builder,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        for( const auto v : geode::Range{ solid.nb_vertices() } )
        {
            if( vertices_old2new[v] != geode::NO_ID )
            {
                const auto new_polyhedron_around =
                    solid.polyhedron_around_vertex( vertices_old2new[v] );
                if( new_polyhedron_around )
                {
                    builder.associate_polyhedron_vertex_to_vertex(
                        new_polyhedron_around.value(), v );
                }
                else
                {
                    builder.disassociate_polyhedron_vertex_to_vertex( v );
                }
            }
            else
            {
                builder.disassociate_polyhedron_vertex_to_vertex( v );
            }
        }
    }

    template < geode::index_t dimension >
    std::vector< bool > find_polyhedra_to_delete(
        const geode::SolidEdges< dimension >& solid,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        std::vector< bool > polyhedra_to_delete( solid.nb_polyhedra(), false );
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            for( const auto v :
                geode::Range{ solid.nb_polyhedron_vertices( p ) } )
            {
                const geode::PolyhedronVertex id{ p, v };
                const auto new_vertex =
                    vertices_old2new[solid.polyhedron_vertex( id )];
                if( new_vertex == geode::NO_ID )
                {
                    polyhedra_to_delete[p] = true;
                }
            }
        }
        return polyhedra_to_delete;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    SolidEdgesBuilder< dimension >::SolidEdgesBuilder(
        SolidEdges< dimension >& edges )
        : edges_( &edges )
    {
    }

    template < index_t dimension >
    index_t SolidEdgesBuilder< dimension >::find_or_create_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        return edges_->find_or_create_edge( std::move( edge_vertices ), {} );
    }

    template < index_t dimension >
    void SolidEdgesBuilder< dimension >::remove_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        edges_->remove_edge( std::move( edge_vertices ), {} );
    }

    template < index_t dimension >
    std::vector< index_t >
        SolidEdgesBuilder< dimension >::delete_isolated_edges()
    {
        return edges_->remove_isolated_edges( {} );
    }

    template < index_t dimension >
    void SolidEdgesBuilder< dimension >::update_edge_vertex(
        std::array< index_t, 2 > edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id )
    {
        edges_->update_edge_vertex(
            std::move( edge_vertices ), edge_vertex_id, new_vertex_id, {} );
    }

    template < index_t dimension >
    void SolidEdgesBuilder< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new )
    {
        edges_->update_edge_vertices( old2new, {} );
    }

    template < index_t dimension >
    void SolidEdgesBuilder< dimension >::copy(
        const SolidEdges< dimension >& edges )
    {
        edges_->overwrite_edges( edges, {} );
    }

    template class opengeode_mesh_api SolidEdgesBuilder< 3 >;
} // namespace geode
