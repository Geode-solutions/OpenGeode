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

#include <geode/mesh/builder/polygonal_surface_builder.h>

#include <numeric>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/detail/mapping_after_deletion.h>

#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/triangulated_surface.h>

namespace
{
    template < geode::index_t dimension >
    void check_polygon_id(
        const geode::PolygonalSurfaceBase< dimension >& surface,
        const geode::index_t polygon_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        OPENGEODE_ASSERT( polygon_id < surface.nb_polygons(),
            "[check_polygon_id] Trying to access an invalid polygon" );
    }

    template < geode::index_t dimension >
    void check_polygon_vertex_id(
        const geode::PolygonalSurfaceBase< dimension >& surface,
        const geode::index_t polygon_id,
        const geode::index_t vertex_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < surface.nb_polygon_vertices( polygon_id ),
            "[check_polygon_vertex_id] Trying to access an invalid polygon "
            "local vertex" );
    }

    template < geode::index_t dimension >
    void check_polygon_edge_id(
        const geode::PolygonalSurfaceBase< dimension >& surface,
        const geode::index_t polygon_id,
        const geode::index_t edge_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        geode_unused( edge_id );
        OPENGEODE_ASSERT( edge_id < surface.nb_polygon_edges( polygon_id ),
            "[check_polygon_edge_id] Trying to access an invalid polygon local "
            "edge" );
    }

    template < geode::index_t dimension >
    void update_polygon_adjacencies(
        const geode::PolygonalSurface< dimension >& surface,
        geode::PolygonalSurfaceBuilder< dimension >& builder,
        absl::Span< const geode::index_t > old2new )
    {
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            for( const auto e : geode::Range{ surface.nb_polygon_edges( p ) } )
            {
                const geode::PolygonEdge id{ p, e };
                if( surface.is_edge_on_border( id ) )
                {
                    continue;
                }
                const auto new_adjacent =
                    old2new[surface.polygon_adjacent( id )];
                builder.set_polygon_adjacent( id, new_adjacent );
            }
        }
    }

    template < geode::index_t dimension >
    void update_polygon_around_vertices(
        const geode::PolygonalSurface< dimension >& surface,
        geode::PolygonalSurfaceBuilder< dimension >& builder,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        for( const auto v : geode::Range{ surface.nb_vertices() } )
        {
            if( vertices_old2new[v] != geode::NO_ID )
            {
                const auto new_polygon_around =
                    builder.polygon_around_vertex( vertices_old2new[v] );
                builder.associate_polygon_vertex_to_vertex(
                    new_polygon_around, v );
            }
            else
            {
                builder.associate_polygon_vertex_to_vertex(
                    geode::PolygonVertex{}, v );
            }
        }
    }

    template < geode::index_t dimension >
    std::vector< bool > find_polygons_to_delete(
        const geode::PolygonalSurface< dimension >& surface,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        std::vector< bool > polygons_to_delete( surface.nb_polygons(), false );
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            for( const auto v :
                geode::Range{ surface.nb_polygon_vertices( p ) } )
            {
                const geode::PolygonVertex id{ p, v };
                const auto new_vertex =
                    vertices_old2new[surface.polygon_vertex( id )];
                if( new_vertex == geode::NO_ID )
                {
                    polygons_to_delete[p] = true;
                }
            }
        }
        return polygons_to_delete;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< PolygonalSurfaceBuilder< dimension > >
        PolygonalSurfaceBuilder< dimension >::create(
            PolygonalSurface< dimension >& mesh )
    {
        try
        {
            return PolygonalSurfaceBuilderFactory< dimension >::create(
                mesh.type_name(), mesh );
        }
        catch( const OpenGeodeException& )
        {
            try
            {
                return TriangulatedSurfaceBuilder< dimension >::create(
                    dynamic_cast< TriangulatedSurface< dimension >& >( mesh ) );
            }
            catch( const std::bad_cast& e )
            {
                Logger::error( e.what() );
                throw OpenGeodeException{ "Could not cast PolygonalSurface "
                                          "to TriangulatedSurface" };
            }
            catch( const OpenGeodeException& e )
            {
                Logger::error( e.what() );
                throw OpenGeodeException{ "Could not create PolygonalSurface "
                                          "builder of data structure: ",
                    mesh.type_name().get() };
            }
        }
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBuilder< dimension >::create_polygon(
        absl::Span< const index_t > vertices )
    {
        const auto added_polygon = polygonal_surface_.nb_polygons();
        polygonal_surface_.polygon_attribute_manager().resize(
            added_polygon + 1 );
        for( const auto v : Range{ vertices.size() } )
        {
            associate_polygon_vertex_to_vertex(
                { added_polygon, v }, vertices[v] );
        }
        for( const auto e : Range{ vertices.size() - 1 } )
        {
            this->find_or_create_edge( { vertices[e], vertices[e + 1] } );
        }
        this->find_or_create_edge( { vertices.back(), vertices.front() } );
        do_create_polygon( vertices );
        return added_polygon;
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::
        associate_polygon_vertex_to_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        polygonal_surface_.associate_polygon_vertex_to_vertex(
            polygon_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBuilder< dimension >::find_or_create_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        return polygonal_surface_.find_or_create_edge(
            std::move( edge_vertices ), {} );
    }

    template < index_t dimension >
    const PolygonVertex&
        PolygonalSurfaceBuilder< dimension >::polygon_around_vertex(
            index_t vertex_id ) const
    {
        return polygonal_surface_.polygon_around_vertex( vertex_id, {} );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::replace_vertex(
        index_t old_vertex_id, index_t new_vertex_id )
    {
        const auto polygons_around =
            polygonal_surface_.polygons_around_vertex( old_vertex_id );
        associate_polygon_vertex_to_vertex( PolygonVertex{}, old_vertex_id );
        for( const auto& polygon_around : polygons_around )
        {
            const auto previous_id = polygonal_surface_.polygon_vertex(
                polygonal_surface_.previous_polygon_vertex( polygon_around ) );
            const auto next_id = polygonal_surface_.polygon_vertex(
                polygonal_surface_.next_polygon_edge( polygon_around ) );
            polygonal_surface_.update_edge_vertex(
                { old_vertex_id, next_id }, 0, new_vertex_id, {} );
            polygonal_surface_.update_edge_vertex(
                { previous_id, old_vertex_id }, 1, new_vertex_id, {} );
            update_polygon_vertex( polygon_around, new_vertex_id );
        }
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::set_polygon_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        const auto polygon_vertex_id =
            polygonal_surface_.polygon_vertex( polygon_vertex );
        const auto previous_id = polygonal_surface_.polygon_vertex(
            polygonal_surface_.previous_polygon_vertex( polygon_vertex ) );
        const auto next_id = polygonal_surface_.polygon_vertex(
            polygonal_surface_.next_polygon_edge( polygon_vertex ) );

        if( polygon_vertex_id != NO_ID )
        {
            const auto polygon_around =
                polygonal_surface_.polygon_around_vertex(
                    polygon_vertex_id, {} );
            if( polygon_around == polygon_vertex )
            {
                const auto polygons_around =
                    polygonal_surface_.polygons_around_vertex(
                        polygon_vertex_id );
                if( polygons_around.size() < 2 )
                {
                    associate_polygon_vertex_to_vertex(
                        PolygonVertex{}, polygon_vertex_id );
                }
                else
                {
                    associate_polygon_vertex_to_vertex(
                        polygons_around[1], polygon_vertex_id );
                }
            }
        }

        polygonal_surface_.update_edge_vertex(
            { polygon_vertex_id, next_id }, 0, vertex_id, {} );
        polygonal_surface_.update_edge_vertex(
            { previous_id, polygon_vertex_id }, 1, vertex_id, {} );
        update_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::update_polygon_vertices(
        absl::Span< const index_t > old2new )
    {
        update_polygon_around_vertices( polygonal_surface_, *this, old2new );
        const auto polygons_to_delete =
            find_polygons_to_delete( polygonal_surface_, old2new );
        delete_polygons( polygons_to_delete );

        for( const auto p : Range{ polygonal_surface_.nb_polygons() } )
        {
            for( const auto v :
                Range{ polygonal_surface_.nb_polygon_vertices( p ) } )
            {
                const PolygonVertex id{ p, v };
                const auto new_vertex =
                    old2new[polygonal_surface_.polygon_vertex( id )];
                OPENGEODE_ASSERT( new_vertex != NO_ID,
                    "[PolygonalSurfaceBuilder::update_polygon_vertices] No "
                    "more polygons with vertices to delete should remain at "
                    "this point" );
                update_polygon_vertex( id, new_vertex );
            }
        }
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::update_polygon_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        check_polygon_id( polygonal_surface_, polygon_vertex.polygon_id );
        check_polygon_vertex_id( polygonal_surface_, polygon_vertex.polygon_id,
            polygon_vertex.vertex_id );
        OPENGEODE_ASSERT( vertex_id < polygonal_surface_.nb_vertices(),
            "[PolygonalSurfaceBuilder::update_polygon_vertex] Accessing a "
            "vertex that does not exist" );
        associate_polygon_vertex_to_vertex( polygon_vertex, vertex_id );
        do_set_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::do_delete_vertices(
        const std::vector< bool >& to_delete )
    {
        const auto old2new = detail::mapping_after_deletion( to_delete );
        update_polygon_vertices( old2new );
        update_edge_vertices( old2new );
        do_delete_surface_vertices( to_delete );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::set_polygon_adjacent(
        const PolygonEdge& polygon_edge, index_t adjacent_id )
    {
        check_polygon_id( polygonal_surface_, polygon_edge.polygon_id );
        check_polygon_edge_id(
            polygonal_surface_, polygon_edge.polygon_id, polygon_edge.edge_id );
        OPENGEODE_ASSERT( adjacent_id < polygonal_surface_.nb_polygons()
                              || adjacent_id == NO_ID,
            "[PolygonalSurfaceBuilder::set_polygon_adjacent] Accessing a "
            "polygon that does not exist" );
        do_set_polygon_adjacent( polygon_edge, adjacent_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::compute_polygon_adjacencies()
    {
        std::vector< index_t > polygons_to_connect(
            polygonal_surface_.nb_polygons() );
        absl::c_iota( polygons_to_connect, 0 );
        compute_polygon_adjacencies( polygons_to_connect );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::compute_polygon_adjacencies(
        absl::Span< const index_t > polygons_to_connect )
    {
        absl::FixedArray< absl::InlinedVector< PolygonEdge, 3 > > polygon_edges(
            polygonal_surface_.nb_edges() );
        absl::FixedArray< absl::InlinedVector< index_t, 4 > > polygon_vertices(
            polygonal_surface_.nb_polygons() );

        for( const auto polygon : polygons_to_connect )
        {
            const auto nb_vertices =
                polygonal_surface_.nb_polygon_vertices( polygon );
            for( const auto v : Range{ nb_vertices } )
            {
                polygon_vertices[polygon].push_back(
                    polygonal_surface_.polygon_vertex( { polygon, v } ) );
            }
            for( const auto e : Range{ nb_vertices - 1 } )
            {
                const PolygonEdge edge{ polygon, e };
                const auto edge_id = polygonal_surface_.edge_from_vertices(
                    { polygon_vertices[polygon][e],
                        polygon_vertices[polygon][e + 1] } );
                polygon_edges[edge_id].emplace_back( edge );
            }
            const PolygonEdge edge{ polygon, nb_vertices - 1 };
            const auto edge_id = polygonal_surface_.edge_from_vertices(
                { polygon_vertices[polygon].back(),
                    polygon_vertices[polygon].front() } );
            polygon_edges[edge_id].emplace_back( edge );
        }

        for( const auto polygon : polygons_to_connect )
        {
            const auto nb_edges =
                polygonal_surface_.nb_polygon_vertices( polygon );
            for( const auto e : Range{ nb_edges - 1 } )
            {
                const PolygonEdge edge{ polygon, e };
                if( !polygonal_surface_.is_edge_on_border( edge ) )
                {
                    continue;
                }
                const auto edge_id = polygonal_surface_.edge_from_vertices(
                    { polygon_vertices[polygon][e],
                        polygon_vertices[polygon][e + 1] } );
                const auto& edges = polygon_edges[edge_id];
                if( polygon_edges[edge_id].size() != 2 )
                {
                    continue;
                }
                do_set_polygon_adjacent( edges[0], edges[1].polygon_id );
                do_set_polygon_adjacent( edges[1], edges[0].polygon_id );
            }
            const PolygonEdge edge{ polygon, nb_edges };
            if( !polygonal_surface_.is_edge_on_border( edge ) )
            {
                continue;
            }
            const auto edge_id = polygonal_surface_.edge_from_vertices(
                { polygon_vertices[polygon][nb_edges],
                    polygon_vertices[polygon][0] } );
            const auto& edges = polygon_edges[edge_id];
            if( polygon_edges[edge_id].size() != 2 )
            {
                continue;
            }
            do_set_polygon_adjacent( edges[0], edges[1].polygon_id );
            do_set_polygon_adjacent( edges[1], edges[0].polygon_id );
        }
    }

    template < index_t dimension >
    std::vector< index_t >
        PolygonalSurfaceBuilder< dimension >::delete_polygons(
            const std::vector< bool >& to_delete )
    {
        for( const auto p : Range{ polygonal_surface_.nb_polygons() } )
        {
            if( to_delete[p] )
            {
                for( const auto e :
                    Range{ polygonal_surface_.nb_polygon_edges( p ) } )
                {
                    polygonal_surface_.remove_edge(
                        { polygonal_surface_.polygon_edge_vertex( { p, e }, 0 ),
                            polygonal_surface_.polygon_edge_vertex(
                                { p, e }, 1 ) },
                        {} );
                }
            }
        }
        polygonal_surface_.remove_isolated_edges( {} );

        const auto old2new = detail::mapping_after_deletion( to_delete );
        for( const auto v : Range{ polygonal_surface_.nb_vertices() } )
        {
            const auto& polygon_vertex =
                polygonal_surface_.polygon_around_vertex( v, {} );
            if( polygon_vertex.polygon_id == NO_ID )
            {
                continue;
            }
            PolygonVertex new_polygon_vertex{ polygon_vertex };
            new_polygon_vertex.polygon_id = old2new[polygon_vertex.polygon_id];
            if( new_polygon_vertex.polygon_id == NO_ID )
            {
                for( auto&& polygon :
                    polygonal_surface_.polygons_around_vertex( v ) )
                {
                    polygon.polygon_id = old2new[polygon.polygon_id];
                    if( polygon.polygon_id != NO_ID )
                    {
                        new_polygon_vertex = std::move( polygon );
                        break;
                    }
                }
            }
            associate_polygon_vertex_to_vertex( new_polygon_vertex, v );
        }
        update_polygon_adjacencies( polygonal_surface_, *this, old2new );
        polygonal_surface_.polygon_attribute_manager().delete_elements(
            to_delete );
        do_delete_polygons( to_delete );
        return old2new;
    }

    template < index_t dimension >
    std::vector< index_t >
        PolygonalSurfaceBuilder< dimension >::delete_isolated_vertices()
    {
        std::vector< bool > to_delete(
            polygonal_surface_.nb_vertices(), false );
        for( const auto v : Range{ polygonal_surface_.nb_vertices() } )
        {
            const auto& polygon_vertex =
                polygonal_surface_.polygon_around_vertex( v, {} );
            if( polygon_vertex.polygon_id == NO_ID )
            {
                to_delete[v] = true;
            }
        }
        return delete_vertices( to_delete );
    }

    template < index_t dimension >
    std::vector< index_t >
        PolygonalSurfaceBuilder< dimension >::delete_isolated_edges()
    {
        return polygonal_surface_.remove_isolated_edges( {} );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new )
    {
        polygonal_surface_.update_edge_vertices( old2new, {} );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::update_edge_vertex(
        std::array< index_t, 2 > edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id )
    {
        OPENGEODE_ASSERT( edge_vertex_id < 2,
            "[PolygonalSurfaceBuilder::update_edge_vertex] "
            "Accessing an invalid vertex in edge" );
        polygonal_surface_.update_edge_vertex(
            std::move( edge_vertices ), edge_vertex_id, new_vertex_id, {} );
    }

    template < index_t dimension >
    std::vector< index_t > PolygonalSurfaceBuilder< dimension >::delete_edges(
        const std::vector< bool >& to_delete )
    {
        return polygonal_surface_.delete_edges( to_delete, {} );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::set_point(
        index_t vertex_id, const Point< dimension >& point )
    {
        OPENGEODE_ASSERT( vertex_id < polygonal_surface_.nb_vertices(),
            "[PolygonalSurfaceBuilder::set_point] Accessing a vertex that does "
            "not exist" );
        do_set_point( vertex_id, point );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBuilder< dimension >::create_point(
        const Point< dimension >& point )
    {
        const auto added_vertex = polygonal_surface_.nb_vertices();
        create_vertex();
        set_point( added_vertex, point );
        return added_vertex;
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::copy(
        const PolygonalSurface< dimension >& polygonal_surface )
    {
        VertexSetBuilder::copy( polygonal_surface );
        for( const auto p : Range{ polygonal_surface.nb_vertices() } )
        {
            set_point( p, polygonal_surface.point( p ) );
        }
        for( const auto p : Range{ polygonal_surface.nb_polygons() } )
        {
            absl::FixedArray< index_t > vertices(
                polygonal_surface.nb_polygon_vertices( p ) );
            for( const auto v :
                Range{ polygonal_surface.nb_polygon_vertices( p ) } )
            {
                vertices[v] = polygonal_surface.polygon_vertex( { p, v } );
            }
            create_polygon( vertices );
        }
        polygonal_surface_.polygon_attribute_manager().copy(
            polygonal_surface.polygon_attribute_manager() );
    }

    template class opengeode_mesh_api PolygonalSurfaceBuilder< 2 >;
    template class opengeode_mesh_api PolygonalSurfaceBuilder< 3 >;
} // namespace geode
