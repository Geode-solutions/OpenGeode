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

#include <geode/mesh/builder/polygonal_surface_builder.h>

#include <numeric>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/detail/mapping_after_deletion.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/triangulated_surface.h>

namespace
{
    using geode::index_t;

    template < index_t dimension >
    void update_polygon_adjacencies(
        const geode::PolygonalSurface< dimension >& surface,
        geode::PolygonalSurfaceBuilder< dimension >& builder,
        const std::vector< index_t >& old2new )
    {
        for( auto p : geode::Range{ surface.nb_polygons() } )
        {
            for( auto e : geode::Range{ surface.nb_polygon_edges( p ) } )
            {
                geode::PolygonEdge id{ p, e };
                if( surface.is_edge_on_border( id ) )
                {
                    continue;
                }
                auto new_adjacent = old2new[surface.polygon_adjacent( id )];
                builder.set_polygon_adjacent( id, new_adjacent );
            }
        }
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
        catch( const OpenGeodeException& e )
        {
            try
            {
                return TriangulatedSurfaceBuilder< dimension >::create(
                    dynamic_cast< TriangulatedSurface< dimension >& >( mesh ) );
            }
            catch( const std::bad_cast& e )
            {
                Logger::error( e.what() );
                throw OpenGeodeException( "Could not cast PolygonalSurface "
                                          "to TriangulatedSurface" );
            }
            catch( const OpenGeodeException& e )
            {
                Logger::error( e.what() );
                throw OpenGeodeException( "Could not create PolygonalSurface "
                                          "builder of data structure: ",
                    mesh.type_name().get() );
            }
        }
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBuilder< dimension >::create_polygon(
        const std::vector< index_t >& vertices )
    {
        auto added_polygon = polygonal_surface_.nb_polygons();
        polygonal_surface_.polygon_attribute_manager().resize(
            added_polygon + 1 );
        for( auto v : Range{ vertices.size() } )
        {
            associate_polygon_vertex_to_vertex(
                { added_polygon, v }, vertices[v] );
        }
        for( auto e : Range{ vertices.size() - 1 } )
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
            polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBuilder< dimension >::find_or_create_edge(
        const std::array< index_t, 2 >& edge_vertices )
    {
        return polygonal_surface_.find_or_create_edge( edge_vertices );
    }

    template < index_t dimension >
    const PolygonVertex&
        PolygonalSurfaceBuilder< dimension >::polygon_around_vertex(
            index_t vertex_id ) const
    {
        return polygonal_surface_.polygon_around_vertex( vertex_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::set_polygon_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        auto polygon_vertex_id =
            polygonal_surface_.polygon_vertex( polygon_vertex );
        auto previous_id = polygonal_surface_.polygon_vertex(
            polygonal_surface_.previous_polygon_vertex( polygon_vertex ) );
        auto next_id = polygonal_surface_.polygon_vertex(
            polygonal_surface_.next_polygon_edge( polygon_vertex ) );

        auto polygon_around =
            polygonal_surface_.polygon_around_vertex( polygon_vertex_id );
        if( polygon_around == polygon_vertex )
        {
            auto polygon_arounds =
                polygonal_surface_.polygons_around_vertex( polygon_vertex_id );
            if( polygon_arounds.size() < 2 )
            {
                associate_polygon_vertex_to_vertex(
                    PolygonVertex{}, polygon_vertex_id );
            }
            else
            {
                associate_polygon_vertex_to_vertex(
                    polygon_arounds[1], polygon_vertex_id );
            }
        }

        polygonal_surface_.update_edge_vertex(
            { polygon_vertex_id, next_id }, 0, vertex_id );
        polygonal_surface_.update_edge_vertex(
            { previous_id, polygon_vertex_id }, 1, vertex_id );
        polygonal_surface_.remove_isolated_edges();
        update_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::update_polygon_vertices(
        const std::vector< index_t >& old2new )
    {
        for( auto v : geode::Range{ polygonal_surface_.nb_vertices() } )
        {
            if( old2new[v] != NO_ID )
            {
                auto new_polygon_around =
                    polygonal_surface_.polygon_around_vertex( old2new[v] );
                associate_polygon_vertex_to_vertex( new_polygon_around, v );
            }
            else
            {
                associate_polygon_vertex_to_vertex( PolygonVertex{}, v );
            }
        }

        std::vector< bool > polygons_to_delete(
            polygonal_surface_.nb_polygons(), false );

        for( auto p : geode::Range{ polygonal_surface_.nb_polygons() } )
        {
            for( auto v :
                geode::Range{ polygonal_surface_.nb_polygon_vertices( p ) } )
            {
                geode::PolygonVertex id{ p, v };
                auto new_vertex =
                    old2new[polygonal_surface_.polygon_vertex( id )];
                if( new_vertex == geode::NO_ID )
                {
                    polygons_to_delete[p] = true;
                }
            }
        }
        delete_polygons( polygons_to_delete );

        for( auto p : geode::Range{ polygonal_surface_.nb_polygons() } )
        {
            for( auto v :
                geode::Range{ polygonal_surface_.nb_polygon_vertices( p ) } )
            {
                geode::PolygonVertex id{ p, v };
                auto new_vertex =
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
        OPENGEODE_EXCEPTION(
            polygon_vertex.polygon_id < polygonal_surface_.nb_polygons(),
            "[PolygonalSurfaceBuilder::update_polygon_vertex] Accessing a "
            "polygon "
            "that does not exist" );
        OPENGEODE_EXCEPTION(
            polygon_vertex.vertex_id < polygonal_surface_.nb_polygon_vertices(
                polygon_vertex.polygon_id ),
            "[PolygonalSurfaceBuilder::update_polygon_vertex] Accessing an "
            "invalid polygon vertex" );
        OPENGEODE_EXCEPTION( vertex_id < polygonal_surface_.nb_vertices(),
            "[PolygonalSurfaceBuilder::update_polygon_vertex] Accessing a "
            "vertex "
            "that does not exist" );
        associate_polygon_vertex_to_vertex( polygon_vertex, vertex_id );
        do_set_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::do_delete_vertices(
        const std::vector< bool >& to_delete )
    {
        auto old2new = mapping_after_deletion( to_delete );
        update_polygon_vertices( old2new );
        update_edge_vertices( old2new );
        do_delete_surface_vertices( to_delete );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::set_polygon_adjacent(
        const PolygonEdge& polygon_edge, index_t adjacent_id )
    {
        OPENGEODE_EXCEPTION(
            polygon_edge.polygon_id < polygonal_surface_.nb_polygons(),
            "[PolygonalSurfaceBuilder::set_polygon_adjacent] Accessing a "
            "polygon that does not exist" );
        OPENGEODE_EXCEPTION(
            polygon_edge.edge_id < polygonal_surface_.nb_polygon_edges(
                polygon_edge.polygon_id ),
            "[PolygonalSurfaceBuilder::set_polygon_adjacent] Accessing an "
            "invalid polygon vertex" );
        OPENGEODE_EXCEPTION( adjacent_id < polygonal_surface_.nb_polygons()
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
        std::iota( polygons_to_connect.begin(), polygons_to_connect.end(), 0 );
        compute_polygon_adjacencies( polygons_to_connect );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::compute_polygon_adjacencies(
        const std::vector< index_t >& polygons_to_connect )
    {
        std::vector< std::vector< PolygonVertex > > polygon_vertices(
            polygonal_surface_.nb_vertices() );
        for( auto polygon : polygons_to_connect )
        {
            for( auto v :
                Range{ polygonal_surface_.nb_polygon_vertices( polygon ) } )
            {
                PolygonVertex vertex_id{ polygon, v };
                auto vertex = polygonal_surface_.polygon_vertex( vertex_id );
                polygon_vertices[vertex].emplace_back( vertex_id );
            }
        }

        for( auto polygon : polygons_to_connect )
        {
            for( auto v :
                Range{ polygonal_surface_.nb_polygon_vertices( polygon ) } )
            {
                if( !polygonal_surface_.is_edge_on_border( { polygon, v } ) )
                {
                    continue;
                }
                PolygonVertex vertex_id{ polygon, v };
                auto vertex = polygonal_surface_.polygon_vertex( vertex_id );
                auto next_vertex = polygonal_surface_.polygon_vertex(
                    polygonal_surface_.next_polygon_vertex( vertex_id ) );
                for( const auto& vertex2 : polygon_vertices[vertex] )
                {
                    if( vertex2.polygon_id == polygon )
                    {
                        continue;
                    }
                    auto prev_vertex_id =
                        polygonal_surface_.previous_polygon_vertex( vertex2 );
                    auto prev_vertex =
                        polygonal_surface_.polygon_vertex( prev_vertex_id );
                    if( next_vertex == prev_vertex )
                    {
                        do_set_polygon_adjacent(
                            vertex_id, vertex2.polygon_id );
                        do_set_polygon_adjacent( prev_vertex_id, polygon );
                        break;
                    }
                }
            }
        }
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::delete_polygons(
        const std::vector< bool >& to_delete )
    {
        for( auto p : geode::Range{ polygonal_surface_.nb_polygons() } )
        {
            if( to_delete[p] )
            {
                for( auto e :
                    geode::Range{ polygonal_surface_.nb_polygon_edges( p ) } )
                {
                    polygonal_surface_.remove_edge(
                        { polygonal_surface_.polygon_edge_vertex( { p, e }, 0 ),
                            polygonal_surface_.polygon_edge_vertex(
                                { p, e }, 1 ) } );
                }
            }
        }
        polygonal_surface_.remove_isolated_edges();

        auto old2new = mapping_after_deletion( to_delete );

        for( auto v : geode::Range{ polygonal_surface_.nb_vertices() } )
        {
            const auto& polygon_vertex =
                polygonal_surface_.polygon_around_vertex( v );
            if( polygon_vertex.polygon_id == geode::NO_ID )
            {
                continue;
            }
            PolygonVertex new_polygon_vertex{ polygon_vertex };
            new_polygon_vertex.polygon_id = old2new[polygon_vertex.polygon_id];
            if( new_polygon_vertex.polygon_id == geode::NO_ID )
            {
                for( auto&& polygon :
                    polygonal_surface_.polygons_around_vertex( v ) )
                {
                    polygon.polygon_id = old2new[polygon.polygon_id];
                    if( polygon.polygon_id != geode::NO_ID )
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
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::update_edge_vertices(
        const std::vector< index_t >& old2new )
    {
        polygonal_surface_.update_edge_vertices( old2new );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::update_edge_vertex(
        const std::array< index_t, 2 >& edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id )
    {
        OPENGEODE_EXCEPTION( edge_vertices.size() == 2,
            "[PolygonalSurfaceBuilder::update_edge_vertex] "
            "Number of edge vertices should be 2" );
        OPENGEODE_EXCEPTION( edge_vertex_id < 2,
            "[PolygonalSurfaceBuilder::update_edge_vertex] "
            "Accessing an invalid vertex in edge" );
        polygonal_surface_.update_edge_vertex(
            edge_vertices, edge_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::delete_edges(
        const std::vector< bool >& to_delete )
    {
        polygonal_surface_.delete_edges( to_delete );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::set_point(
        index_t vertex_id, const Point< dimension >& point )
    {
        OPENGEODE_EXCEPTION( vertex_id < polygonal_surface_.nb_vertices(),
            "[PolygonalSurfaceBuilder::set_point] Accessing a vertex that does "
            "not exist" );
        do_set_point( vertex_id, point );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBuilder< dimension >::create_point(
        const Point< dimension >& point )
    {
        auto added_vertex = polygonal_surface_.nb_vertices();
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
            std::vector< index_t > vertices(
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
