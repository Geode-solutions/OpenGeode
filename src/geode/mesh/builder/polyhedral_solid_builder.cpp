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

#include <geode/mesh/builder/polyhedral_solid_builder.h>

#include <numeric>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/detail/mapping_after_deletion.h>
#include <geode/mesh/core/polyhedral_solid.h>

namespace
{
    using geode::index_t;

    template < index_t dimension >
    void update_polyhedron_vertices(
        const geode::PolyhedralSolid< dimension >& solid,
        geode::PolyhedralSolidBuilder< dimension >& builder,
        const std::vector< index_t >& old2new )
    {
        std::vector< bool > polyhedra_to_delete( solid.nb_polyhedra(), false );
        for( auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            for( auto v : geode::Range{ solid.nb_polyhedron_vertices( p ) } )
            {
                geode::PolyhedronVertex id{ p, v };
                auto new_vertex = old2new[solid.polyhedron_vertex( id )];
                if( new_vertex == geode::NO_ID )
                {
                    polyhedra_to_delete[p] = true;
                }
                else
                {
                    builder.set_polyhedron_vertex( id, new_vertex );
                }
            }
        }
        builder.delete_polyhedra( polyhedra_to_delete );
    }

    template < index_t dimension >
    void update_polyhedron_adjacencies(
        const geode::PolyhedralSolid< dimension >& solid,
        geode::PolyhedralSolidBuilder< dimension >& builder,
        const std::vector< index_t >& old2new )
    {
        for( auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            for( auto f : geode::Range{ solid.nb_polyhedron_facets( p ) } )
            {
                geode::PolyhedronFacet id{ p, f };
                if( solid.is_polyhedron_facet_on_border( id ) )
                {
                    continue;
                }
                auto new_adjacent = old2new[solid.polyhedron_adjacent( id )];
                builder.set_polyhedron_adjacent( id, new_adjacent );
            }
        }
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< PolyhedralSolidBuilder< dimension > >
        PolyhedralSolidBuilder< dimension >::create(
            PolyhedralSolid< dimension >& mesh )
    {
        try
        {
            return PolyhedralSolidBuilderFactory< dimension >::create(
                mesh.type_name(), mesh );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException(
                "Could not create PolyhedralSolid builder of data structure: ",
                mesh.type_name().get() );
        }
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        OPENGEODE_EXCEPTION(
            polyhedron_vertex.polyhedron_id < polyhedral_solid_.nb_polyhedra(),
            "[PolyhedralSolidBuilder::set_polyhedron_vertex] Accessing a "
            "polyhedron that does not exist" );
        OPENGEODE_EXCEPTION( polyhedron_vertex.vertex_id
                                 < polyhedral_solid_.nb_polyhedron_vertices(
                                     polyhedron_vertex.polyhedron_id ),
            "[PolyhedralSolidBuilder::set_polyhedron_vertex] Accessing an "
            "invalid polyhedron vertex" );
        OPENGEODE_EXCEPTION( vertex_id < polyhedral_solid_.nb_vertices(),
            "[PolyhedralSolidBuilder::set_polyhedron_vertex] Accessing a "
            "vertex that does not exist" );
        associate_polyhedron_vertex_to_vertex( polyhedron_vertex, vertex_id );
        do_set_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolidBuilder< dimension >::create_polyhedron(
        const std::vector< index_t >& vertices,
        const std::vector< std::vector< index_t > >& facets )
    {
        auto first_added_polyhedron = polyhedral_solid_.nb_polyhedra();
        polyhedral_solid_.polyhedron_attribute_manager().resize(
            first_added_polyhedron + 1 );
        for( auto v : Range{ vertices.size() } )
        {
            associate_polyhedron_vertex_to_vertex(
                { first_added_polyhedron, v }, vertices[v] );
        }
        do_create_polyhedron( vertices, facets );
        return first_added_polyhedron;
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::
        associate_polyhedron_vertex_to_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        polyhedral_solid_.associate_polyhedron_vertex_to_vertex(
            polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::do_delete_vertices(
        const std::vector< bool >& to_delete )
    {
        auto old2new = mapping_after_deletion( to_delete );
        update_polyhedron_vertices( polyhedral_solid_, *this, old2new );
        do_delete_solid_vertices( to_delete );
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
    {
        OPENGEODE_EXCEPTION(
            polyhedron_facet.polyhedron_id < polyhedral_solid_.nb_polyhedra(),
            "[PolyhedralSolidBuilder::set_polyhedron_adjacent] Accessing a "
            "polyhedron that does not exist" );
        OPENGEODE_EXCEPTION(
            polyhedron_facet.facet_id < polyhedral_solid_.nb_polyhedron_facets(
                polyhedron_facet.polyhedron_id ),
            "[PolyhedralSolidBuilder::set_polyhedron_adjacent] Accessing an "
            "invalid polyhedron vertex" );
        OPENGEODE_EXCEPTION( adjacent_id < polyhedral_solid_.nb_polyhedra()
                                 || adjacent_id == NO_ID,
            "[PolyhedralSolidBuilder::set_polyhedron_adjacent] Accessing a "
            "polyhedron that does not exist" );
        do_set_polyhedron_adjacent( polyhedron_facet, adjacent_id );
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::compute_polyhedron_adjacencies()
    {
        std::vector< index_t > polyhedra_to_connect(
            polyhedral_solid_.nb_polyhedra() );
        std::iota(
            polyhedra_to_connect.begin(), polyhedra_to_connect.end(), 0 );
        compute_polyhedron_adjacencies( polyhedra_to_connect );
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::compute_polyhedron_adjacencies(
        const std::vector< index_t >& polyhedra_to_connect )
    {
        std::vector< std::vector< PolyhedronFacet > >
            polyhedron_facet_around_vertex( polyhedral_solid_.nb_vertices() );
        for( auto polyhedron : polyhedra_to_connect )
        {
            for( auto f :
                Range{ polyhedral_solid_.nb_polyhedron_facets( polyhedron ) } )
            {
                PolyhedronFacet facet{ polyhedron, f };
                for( auto v :
                    Range{ polyhedral_solid_.nb_polyhedron_facet_vertices(
                        facet ) } )
                {
                    auto vertex = polyhedral_solid_.polyhedron_facet_vertex(
                        { facet, v } );
                    polyhedron_facet_around_vertex[vertex].push_back( facet );
                }
            }
        }

        for( auto polyhedron : polyhedra_to_connect )
        {
            for( auto f :
                Range{ polyhedral_solid_.nb_polyhedron_facets( polyhedron ) } )
            {
                PolyhedronFacet facet{ polyhedron, f };
                if( !polyhedral_solid_.is_polyhedron_facet_on_border( facet ) )
                {
                    continue;
                }

                auto nb_vertices =
                    polyhedral_solid_.nb_polyhedron_facet_vertices( facet );
                auto first_vertex =
                    polyhedral_solid_.polyhedron_facet_vertex( { facet, 0 } );
                for( const auto& facet2 :
                    polyhedron_facet_around_vertex[first_vertex] )
                {
                    auto nb_vertices2 =
                        polyhedral_solid_.nb_polyhedron_facet_vertices(
                            facet2 );
                    if( nb_vertices != nb_vertices2 )
                    {
                        continue;
                    }
                    for( auto offset : Range{ nb_vertices } )
                    {
                        bool found_facet{ true };
                        for( auto v : Range{ nb_vertices } )
                        {
                            auto vertex =
                                polyhedral_solid_.polyhedron_facet_vertex(
                                    { facet, v } );
                            auto vertex2 =
                                polyhedral_solid_.polyhedron_facet_vertex(
                                    { facet2, ( nb_vertices - v + offset )
                                                  % nb_vertices } );
                            if( vertex != vertex2 )
                            {
                                found_facet = false;
                                break;
                            }
                        }
                        if( found_facet )
                        {
                            do_set_polyhedron_adjacent(
                                facet, facet2.polyhedron_id );
                            do_set_polyhedron_adjacent(
                                facet2, facet.polyhedron_id );
                            goto next_facet;
                        }
                    }
                }
            next_facet:;
            }
        }
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::delete_polyhedra(
        const std::vector< bool >& to_delete )
    {
        auto old2new = mapping_after_deletion( to_delete );

        for( auto v : geode::Range{ polyhedral_solid_.nb_vertices() } )
        {
            const auto& polyhedron_vertex =
                polyhedral_solid_.polyhedron_around_vertex( v );
            if( polyhedron_vertex.polyhedron_id == geode::NO_ID ){
                continue;
            }
            PolyhedronVertex new_polyhedron_vertex{ polyhedron_vertex };
            new_polyhedron_vertex.polyhedron_id = old2new[polyhedron_vertex.polyhedron_id];
            
            if( new_polyhedron_vertex.polyhedron_id == geode::NO_ID )
            {
                for( auto&& polyhedron :
                    polyhedral_solid_.polyhedra_around_vertex( v ) )
                {
                    polyhedron.polyhedron_id = old2new[polyhedron.polyhedron_id];
                    if( polyhedron.polyhedron_id != geode::NO_ID )
                    {
                        new_polyhedron_vertex = std::move( polyhedron );
                        break;
                    }
                }
            }
            associate_polyhedron_vertex_to_vertex( new_polyhedron_vertex, v );
        }

        update_polyhedron_adjacencies( polyhedral_solid_, *this, old2new );
        polyhedral_solid_.polyhedron_attribute_manager().delete_elements(
            to_delete );
        do_delete_polyhedra( to_delete );
    }

    template < index_t dimension >
    void PolyhedralSolidBuilder< dimension >::set_point(
        index_t vertex_id, const Point< dimension >& point )
    {
        OPENGEODE_EXCEPTION( vertex_id < polyhedral_solid_.nb_vertices(),
            "[PolyhedralSolidBuilder::set_point] Accessing a vertex that does "
            "not exist" );
        do_set_point( vertex_id, point );
    }

    template < index_t dimension >
    index_t PolyhedralSolidBuilder< dimension >::create_point(
        const Point< dimension >& point )
    {
        auto first_added_vertex = polyhedral_solid_.nb_vertices();
        create_vertex();
        set_point( first_added_vertex, point );
        return first_added_vertex;
    }

    template class opengeode_mesh_api PolyhedralSolidBuilder< 3 >;
} // namespace geode
