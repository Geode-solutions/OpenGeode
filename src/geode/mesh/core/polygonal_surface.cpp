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

#include <geode/mesh/core/polygonal_surface.h>

#include <algorithm>
#include <stack>

#include <geode/basic/algorithm.h>
#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/vector.h>

#include <geode/mesh/builder/detail/mapping_after_deletion.h>
#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/geode_polygonal_surface.h>

namespace
{
    template < geode::index_t dimension >
    void check_polygon_id(
        const geode::PolygonalSurfaceBase< dimension >& surface,
        geode::index_t polygon_id )
    {
        OPENGEODE_EXCEPTION( polygon_id < surface.nb_polygons(),
            "[check_polygon_id] Trying to access an invalid polygon" );
    }

    template < geode::index_t dimension >
    void check_polygon_vertex_id(
        const geode::PolygonalSurfaceBase< dimension >& surface,
        geode::index_t polygon_id,
        geode::index_t vertex_id )
    {
        OPENGEODE_EXCEPTION(
            vertex_id < surface.nb_polygon_vertices( polygon_id ),
            "[check_polygon_vertex_id] Trying to access an invalid polygon "
            "local vertex" );
    }

    template < geode::index_t dimension >
    void check_polygon_edge_id(
        const geode::PolygonalSurfaceBase< dimension >& surface,
        geode::index_t polygon_id,
        geode::index_t edge_id )
    {
        OPENGEODE_EXCEPTION( edge_id < surface.nb_polygon_edges( polygon_id ),
            "[check_polygon_edge_id] Trying to access an invalid polygon local "
            "edge" );
    }

    template < geode::index_t dimension >
    double triangle_area( const geode::Point< dimension >& p0,
        const geode::Point< dimension >& p1,
        const geode::Point< dimension >& p2 )
    {
        // Heron's formula
        auto l0 = geode::Vector< dimension >{ p0, p1 }.length();
        auto l1 = geode::Vector< dimension >{ p1, p2 }.length();
        auto l2 = geode::Vector< dimension >{ p2, p0 }.length();
        auto p = ( l0 + l1 + l2 ) / 2;
        return std::sqrt( p * ( p - l0 ) * ( p - l1 ) * ( p - l2 ) );
    }
} // namespace

namespace geode
{
    PolygonVertex::PolygonVertex( PolygonEdge polygon_edge )
        : polygon_id( polygon_edge.polygon_id ),
          vertex_id( polygon_edge.edge_id )
    {
    }

    PolygonEdge::PolygonEdge( PolygonVertex polygon_vertex )
        : polygon_id( polygon_vertex.polygon_id ),
          edge_id( polygon_vertex.vertex_id )
    {
    }

    template < index_t dimension >
    class PolygonalSurfaceBase< dimension >::Impl
    {
    public:
        explicit Impl( PolygonalSurfaceBase& surface )
            : polygon_around_vertex_(
                surface.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        PolygonVertex >(
                        "polygon_around_vertex", PolygonVertex{} ) )
        {
        }

        const PolygonVertex& polygon_around_vertex( index_t vertex_id ) const
        {
            return polygon_around_vertex_->value( vertex_id );
        }

        void associate_polygon_vertex_to_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id )
        {
            polygon_around_vertex_->value( vertex_id ) = polygon_vertex;
        }

        index_t find_edge( const std::vector< index_t >& edge_vertices ) const
        {
            auto itr = edge_indices_.find( edge_vertices );
            if( itr != edge_indices_.end() )
            {
                return edge_indices_.at( edge_vertices );
            }
            return NO_ID;
        }

        index_t find_or_create_edge(
            const std::array< index_t, 2 >& edge_vertices )
        {
            auto size = edge_indices_.size();
            std::vector< index_t > vertices{ edge_vertices[0],
                edge_vertices[1] };
            auto id = find_edge( vertices );
            if( id != NO_ID )
            {
                return id;
            }
            edge_indices_[vertices] = size;
            edge_attribute_manager_.resize( size + 1 );
            return size;
        }

        void update_edge_vertices( const std::vector< index_t >& old2new )
        {
            auto old_edge_indices = edge_indices_;
            edge_indices_.clear();
            edge_indices_.reserve( old_edge_indices.size() );
            for( const auto& cycle : old_edge_indices )
            {
                auto updated_vertices = cycle.first.vertices();
                for( auto& v : updated_vertices )
                {
                    v = old2new[v];
                }
                detail::VertexCycle updated_cycle{ updated_vertices };
                edge_indices_[updated_cycle] = cycle.second;
            }
        }

        void delete_edges( const std::vector< bool >& to_delete )
        {
            auto old2new = mapping_after_deletion( to_delete );
            std::vector< detail::VertexCycle > key_to_erase;
            key_to_erase.reserve( old2new.size() );
            for( const auto& cycle : edge_indices_ )
            {
                if( old2new[cycle.second] == NO_ID )
                {
                    key_to_erase.emplace_back( cycle.first );
                }
            }
            for( const auto& key : key_to_erase )
            {
                edge_indices_.erase( key );
            }
            for( auto& cycle : edge_indices_ )
            {
                cycle.second = old2new[cycle.second];
            }
        }

        AttributeManager& polygon_attribute_manager() const
        {
            return polygon_attribute_manager_;
        }

        AttributeManager& edge_attribute_manager() const
        {
            return edge_attribute_manager_;
        }

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.object( polygon_attribute_manager_ );
            archive.ext( polygon_around_vertex_, bitsery::ext::StdSmartPtr{} );
        }

    private:
        mutable AttributeManager polygon_attribute_manager_;
        mutable AttributeManager edge_attribute_manager_;
        std::shared_ptr< VariableAttribute< PolygonVertex > >
            polygon_around_vertex_;
        std::unordered_map< detail::VertexCycle, index_t > edge_indices_;
    };

    template < index_t dimension >
    PolygonalSurfaceBase< dimension >::PolygonalSurfaceBase() : impl_( *this )
    {
    }

    template < index_t dimension >
    PolygonalSurfaceBase< dimension >::~PolygonalSurfaceBase() // NOLINT
    {
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        check_polygon_id( *this, polygon_vertex.polygon_id );
        check_polygon_vertex_id(
            *this, polygon_vertex.polygon_id, polygon_vertex.vertex_id );
        return get_polygon_vertex( polygon_vertex );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::polygon_edge(
        const PolygonEdge& polygon_edge ) const
    {
        check_polygon_id( *this, polygon_edge.polygon_id );
        check_polygon_edge_id(
            *this, polygon_edge.polygon_id, polygon_edge.edge_id );
        return impl_->find_edge( { polygon_edge_vertex( polygon_edge, 0 ),
            polygon_edge_vertex( polygon_edge, 1 ) } );
    }

    template < index_t dimension >
    const PolygonVertex&
        PolygonalSurfaceBase< dimension >::polygon_around_vertex(
            index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < this->nb_vertices(),
            "[PolygonalSurfaceBase::polygon_around_vertex] Accessing an "
            "invalid vertex" );
        return impl_->polygon_around_vertex( vertex_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::find_or_create_edge(
        const std::array< index_t, 2 >& edge_vertices )
    {
        return impl_->find_or_create_edge( edge_vertices );
    }

    template < index_t dimension >
    void PolygonalSurfaceBase< dimension >::update_edge_vertices(
        const std::vector< index_t >& old2new )
    {
        impl_->update_edge_vertices( old2new );
    }

    template < index_t dimension >
    void PolygonalSurfaceBase< dimension >::delete_edges(
        const std::vector< bool >& to_delete )
    {
        impl_->delete_edges( to_delete );
    }

    template < index_t dimension >
    void PolygonalSurfaceBase< dimension >::associate_polygon_vertex_to_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        impl_->associate_polygon_vertex_to_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::nb_edges() const
    {
        return edge_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::nb_polygons() const
    {
        return polygon_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::nb_polygon_vertices(
        index_t polygon_id ) const
    {
        check_polygon_id( *this, polygon_id );
        return get_nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::nb_polygon_edges(
        index_t polygon_id ) const
    {
        return nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    PolygonVertex PolygonalSurfaceBase< dimension >::next_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        auto vertex = polygon_vertex.vertex_id;
        auto polygon = polygon_vertex.polygon_id;
        auto nb_vertices = nb_polygon_vertices( polygon );
        return { polygon, ( vertex + 1 ) % nb_vertices };
    }

    template < index_t dimension >
    PolygonVertex PolygonalSurfaceBase< dimension >::previous_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        auto vertex = polygon_vertex.vertex_id;
        auto polygon = polygon_vertex.polygon_id;
        auto nb_vertices = nb_polygon_vertices( polygon );
        return { polygon, ( vertex + nb_vertices - 1 ) % nb_vertices };
    }

    template < index_t dimension >
    PolygonEdge PolygonalSurfaceBase< dimension >::next_polygon_edge(
        const PolygonEdge& polygon_edge ) const
    {
        return next_polygon_vertex( polygon_edge );
    }

    template < index_t dimension >
    PolygonEdge PolygonalSurfaceBase< dimension >::previous_polygon_edge(
        const PolygonEdge& polygon_edge ) const
    {
        return previous_polygon_vertex( polygon_edge );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::polygon_adjacent(
        const PolygonEdge& polygon_edge ) const
    {
        check_polygon_id( *this, polygon_edge.polygon_id );
        check_polygon_edge_id(
            *this, polygon_edge.polygon_id, polygon_edge.edge_id );
        return get_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    PolygonEdge PolygonalSurfaceBase< dimension >::polygon_adjacent_edge(
        const PolygonEdge& polygon_edge ) const
    {
        if( !is_edge_on_border( polygon_edge ) )
        {
            auto v0 = polygon_edge_vertex( polygon_edge, 0 );
            auto v1 = polygon_edge_vertex( polygon_edge, 1 );
            auto polygon_adj = polygon_adjacent( polygon_edge );
            for( auto e : Range{ nb_polygon_edges( polygon_adj ) } )
            {
                auto polygon = polygon_adjacent( { polygon_adj, e } );
                if( polygon == polygon_edge.polygon_id
                    && polygon_edge_vertex( { polygon_adj, e }, 0 ) == v1
                    && polygon_edge_vertex( { polygon_adj, e }, 1 ) == v0 )
                {
                    return { polygon_adj, e };
                }
            }
            throw OpenGeodeException( "[PolygonalSurfaceBase::polygon_adjacent_"
                                      "edge] Wrong adjacency with polygons: ",
                polygon_edge.polygon_id, " and ", polygon_adj );
        }
        return {};
    }

    template < index_t dimension >
    bool PolygonalSurfaceBase< dimension >::is_edge_on_border(
        const PolygonEdge& polygon_edge ) const
    {
        return polygon_adjacent( polygon_edge ) == NO_ID;
    }

    template < index_t dimension >
    std::vector< PolygonEdge >
        PolygonalSurfaceBase< dimension >::polygon_edges_on_border(
            index_t polygon_id ) const
    {
        std::vector< PolygonEdge > borders;
        for( auto e : Range{ nb_polygon_edges( polygon_id ) } )
        {
            PolygonEdge edge{ polygon_id, e };
            if( is_edge_on_border( edge ) )
            {
                borders.emplace_back( edge );
            }
        }
        return borders;
    }

    template < index_t dimension >
    PolygonEdge PolygonalSurfaceBase< dimension >::next_on_border(
        const PolygonEdge& polygon_edge ) const
    {
        OPENGEODE_EXCEPTION( is_edge_on_border( polygon_edge ),
            "[PolygonalSurfaceBase::next_on_border] Polygon edge should be on "
            "border" );
        auto next_border = next_polygon_edge( polygon_edge );
        while( !is_edge_on_border( next_border ) )
        {
            next_border = polygon_adjacent_edge( next_border );
            next_border = next_polygon_edge( next_border );
        }
        return next_border;
    }

    template < index_t dimension >
    PolygonEdge PolygonalSurfaceBase< dimension >::previous_on_border(
        const PolygonEdge& polygon_edge ) const
    {
        OPENGEODE_EXCEPTION( is_edge_on_border( polygon_edge ),
            "[PolygonalSurfaceBase::previous_on_border] Polygon edge should be "
            "on border" );
        auto previous_border = previous_polygon_edge( polygon_edge );
        while( !is_edge_on_border( previous_border ) )
        {
            previous_border = polygon_adjacent_edge( previous_border );
            previous_border = previous_polygon_edge( previous_border );
        }
        return previous_border;
    }

    template < index_t dimension >
    double PolygonalSurfaceBase< dimension >::polygon_edge_length(
        const PolygonEdge& polygon_edge ) const
    {
        auto v0 = polygon_edge_vertex( polygon_edge, 0 );
        auto v1 = polygon_edge_vertex( polygon_edge, 1 );
        return Vector< dimension >{ this->point( v0 ), this->point( v1 ) }
            .length();
    }

    template < index_t dimension >
    Point< dimension >
        PolygonalSurfaceBase< dimension >::polygon_edge_barycenter(
            const PolygonEdge& polygon_edge ) const
    {
        auto v0 = polygon_edge_vertex( polygon_edge, 0 );
        auto v1 = polygon_edge_vertex( polygon_edge, 1 );
        return ( this->point( v0 ) + this->point( v1 ) ) / 2;
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::polygon_edge_vertex(
        const PolygonEdge& polygon_edge, index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < 2, "[PolygonalSurfaceBase::polygon_"
                                            "edge_vertex] vertex_id should be "
                                            "0 or 1" );
        if( vertex_id == 0 )
        {
            return polygon_vertex( polygon_edge );
        }
        return polygon_vertex( next_polygon_edge( polygon_edge ) );
    }

    template < index_t dimension >
    Point< dimension > PolygonalSurfaceBase< dimension >::polygon_barycenter(
        index_t polygon_id ) const
    {
        Point< dimension > barycenter;
        for( auto v : Range{ nb_polygon_vertices( polygon_id ) } )
        {
            barycenter =
                barycenter + this->point( polygon_vertex( { polygon_id, v } ) );
        }
        return barycenter / nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    double PolygonalSurfaceBase< dimension >::polygon_area(
        index_t polygon_id ) const
    {
        double area{ 0 };
        if( nb_polygon_vertices( polygon_id ) == 0 )
        {
            return area;
        }
        const auto& p1 = this->point( polygon_vertex( { polygon_id, 0 } ) );
        for( auto i : Range{ 1, nb_polygon_vertices( polygon_id ) - 1 } )
        {
            const auto& p2 = this->point( polygon_vertex( { polygon_id, i } ) );
            const auto& p3 =
                this->point( polygon_vertex( { polygon_id, i + 1 } ) );
            area += triangle_area( p1, p2, p3 );
        }
        return area;
    }

    template < index_t dimension >
    std::vector< PolygonVertex >
        PolygonalSurfaceBase< dimension >::polygons_around_vertex(
            index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < this->nb_vertices(),
            "[PolygonalSurfaceBase::polygons_around_vertex] Accessing an "
            "invalid vertex" );
        std::vector< PolygonVertex > polygons;
        const auto& first_polygon = impl_->polygon_around_vertex( vertex_id );
        if( first_polygon.polygon_id == NO_ID )
        {
            return polygons;
        }
        OPENGEODE_ASSERT( polygon_vertex( first_polygon ) == vertex_id,
            "[PolygonalSurfaceBase::polygons_around_vertex] Wrong polygon "
            "around vertex" );
        std::vector< index_t > polygons_visited;
        polygons_visited.reserve( 10 );
        std::stack< PolygonVertex > S;
        S.push( first_polygon );
        while( !S.empty() )
        {
            auto polygon_vertex = S.top();
            S.pop();
            if( contain( polygons_visited, polygon_vertex.polygon_id ) )
            {
                continue;
            }
            polygons_visited.push_back( polygon_vertex.polygon_id );
            polygons.push_back( polygon_vertex );

            PolygonEdge polygon_edge{ polygon_vertex };
            if( !is_edge_on_border( polygon_edge ) )
            {
                auto adj_edge = polygon_adjacent_edge( polygon_edge );
                S.emplace( next_polygon_edge( adj_edge ) );
            }
            auto prev_edge = previous_polygon_edge( polygon_vertex );
            if( !is_edge_on_border( prev_edge ) )
            {
                auto adj_edge = polygon_adjacent_edge( prev_edge );
                S.emplace( adj_edge );
            }
        }
        return polygons;
    }

    template < index_t dimension >
    std::tuple< bool, PolygonEdge >
        PolygonalSurfaceBase< dimension >::polygon_edge_from_vertices(
            index_t from_vertex_id, index_t to_vertex_id ) const
    {
        for( auto& polygon_vertex : polygons_around_vertex( from_vertex_id ) )
        {
            auto next_vertex = next_polygon_vertex( polygon_vertex );
            if( this->polygon_vertex( next_vertex ) == to_vertex_id )
            {
                return std::make_tuple( true, std::move( polygon_vertex ) );
            }
        }
        return std::make_tuple( false, PolygonEdge{} );
    }

    template < index_t dimension >
    AttributeManager&
        PolygonalSurfaceBase< dimension >::edge_attribute_manager() const
    {
        return impl_->edge_attribute_manager();
    }

    template < index_t dimension >
    AttributeManager&
        PolygonalSurfaceBase< dimension >::polygon_attribute_manager() const
    {
        return impl_->polygon_attribute_manager();
    }

    template < index_t dimension >
    const Point< dimension >& PolygonalSurfaceBase< dimension >::point(
        index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < nb_vertices(),
            "[PolygonalSurfaceBase::point] Trying to access an invalid point" );
        return get_point( vertex_id );
    }

    template < index_t dimension >
    template < typename Archive >
    void PolygonalSurfaceBase< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, bitsery::ext::BaseClass< VertexSet >{} );
        archive.object( impl_ );
    }

    Vector3D PolygonalSurface< 3 >::polygon_normal( index_t polygon_id ) const
    {
        check_polygon_id( *this, polygon_id );
        auto barycenter = polygon_barycenter( polygon_id );
        Vector3D normal;
        for( index_t v = 1; v < nb_polygon_vertices( polygon_id ); v++ )
        {
            const auto& p1 =
                this->point( polygon_vertex( { polygon_id, v - 1 } ) );
            const auto& p2 = this->point( polygon_vertex( { polygon_id, v } ) );
            normal =
                normal + Vector3D{ p1, barycenter }.cross( { p2, barycenter } );
        }
        const auto& p1 = this->point( polygon_vertex(
            { polygon_id, nb_polygon_vertices( polygon_id ) - 1 } ) );
        const auto& p2 = this->point( polygon_vertex( { polygon_id, 0 } ) );
        normal =
            normal + Vector3D{ p1, barycenter }.cross( { p2, barycenter } );
        return normal.normalize();
    }

    Vector3D PolygonalSurface< 3 >::polygon_vertex_normal(
        index_t vertex_id ) const
    {
        Vector3D normal;
        for( const auto& polygon : polygons_around_vertex( vertex_id ) )
        {
            normal = normal + polygon_normal( polygon.polygon_id );
        }
        return normal.normalize();
    }

    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > >
        PolygonalSurface< dimension >::create()
    {
        return std::unique_ptr< PolygonalSurface< dimension > >{
            new OpenGeodePolygonalSurface< dimension >
        };
    }

    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > >
        PolygonalSurface< dimension >::create( const MeshType& type )
    {
        try
        {
            return PolygonalSurfaceFactory< dimension >::create( type );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException(
                "Could not create PolygonalSurface data structure: ",
                type.get() );
        }
    }

    std::unique_ptr< PolygonalSurface< 3 > > PolygonalSurface< 3 >::create()
    {
        return std::unique_ptr< PolygonalSurface< 3 > >{
            new OpenGeodePolygonalSurface< 3 >
        };
    }

    std::unique_ptr< PolygonalSurface< 3 > > PolygonalSurface< 3 >::create(
        const MeshType& type )
    {
        try
        {
            return PolygonalSurfaceFactory< 3 >::create( type );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException(
                "Could not create PolygonalSurface data structure: ",
                type.get() );
        }
    }

    template class opengeode_mesh_api PolygonalSurfaceBase< 2 >;
    template class opengeode_mesh_api PolygonalSurfaceBase< 3 >;

    template class opengeode_mesh_api PolygonalSurface< 2 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolygonalSurfaceBase< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolygonalSurfaceBase< 3 > );
} // namespace geode
