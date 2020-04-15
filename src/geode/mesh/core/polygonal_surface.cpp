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

#include <geode/mesh/core/polygonal_surface.h>

#include <algorithm>
#include <stack>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/polygonal_surface_builder.h>
#include <geode/mesh/core/detail/facet_storage.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
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
    double triangle_area( const geode::Point< dimension >& p0,
        const geode::Point< dimension >& p1,
        const geode::Point< dimension >& p2 )
    {
        // Heron's formula
        const auto l0 = geode::Vector< dimension >{ p0, p1 }.length();
        const auto l1 = geode::Vector< dimension >{ p1, p2 }.length();
        const auto l2 = geode::Vector< dimension >{ p2, p0 }.length();
        const auto p = ( l0 + l1 + l2 ) / 2;
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
        : public detail::FacetStorage< std::array< index_t, 2 > >
    {
        friend class bitsery::Access;

    public:
        explicit Impl( PolygonalSurfaceBase& surface )
            : polygon_around_vertex_(
                surface.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        PolygonVertex >(
                        "polygon_around_vertex", PolygonVertex{} ) )
        {
        }

        const PolygonVertex& polygon_around_vertex(
            const index_t vertex_id ) const
        {
            return polygon_around_vertex_->value( vertex_id );
        }

        void associate_polygon_vertex_to_vertex(
            const PolygonVertex& polygon_vertex, const index_t vertex_id )
        {
            polygon_around_vertex_->set_value( vertex_id, polygon_vertex );
        }

        index_t find_edge( std::array< index_t, 2 > edge_vertices ) const
        {
            return this->find_facet( std::move( edge_vertices ) );
        }

        index_t find_or_create_edge( std::array< index_t, 2 > edge_vertices )
        {
            return this->add_facet( std::move( edge_vertices ) );
        }

        const std::array< index_t, 2 >& get_edge_vertices(
            const index_t edge_id ) const
        {
            return this->get_facet_vertices( edge_id );
        }

        void update_edge_vertex( std::array< index_t, 2 > edge_vertices,
            const index_t edge_vertex_id,
            const index_t new_vertex_id )
        {
            auto updated_edge_vertices = edge_vertices;
            updated_edge_vertices[edge_vertex_id] = new_vertex_id;
            this->add_facet( std::move( updated_edge_vertices ) );
            this->remove_facet( std::move( edge_vertices ) );
        }

        void update_edge_vertices( absl::Span< const index_t > old2new )
        {
            this->update_facet_vertices( old2new );
        }

        void remove_edge( std::array< index_t, 2 > edge_vertices )
        {
            this->remove_facet( std::move( edge_vertices ) );
        }

        std::vector< index_t > delete_edges(
            const std::vector< bool >& to_delete )
        {
            return this->delete_facets( to_delete );
        }

        std::vector< index_t > remove_isolated_edges()
        {
            return this->clean_facets();
        }

        bool isolated_vertex( index_t vertex_id ) const
        {
            return polygon_around_vertex( vertex_id ) == PolygonVertex{};
        }

        bool isolated_edge( index_t edge_id ) const
        {
            return this->get_counter( edge_id ) == 0;
        }

        AttributeManager& polygon_attribute_manager() const
        {
            return polygon_attribute_manager_;
        }

        AttributeManager& edge_attribute_manager() const
        {
            return facet_attribute_manager();
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.ext(
                        impl, bitsery::ext::BaseClass< detail::FacetStorage<
                                  std::array< index_t, 2 > > >{} );
                    archive.object( impl.polygon_attribute_manager_ );
                    archive.ext( impl.polygon_around_vertex_,
                        bitsery::ext::StdSmartPtr{} );
                } );
        }

    private:
        mutable AttributeManager polygon_attribute_manager_;
        std::shared_ptr< VariableAttribute< PolygonVertex > >
            polygon_around_vertex_;
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
        return impl_->find_edge( { polygon_vertex( polygon_edge ),
            polygon_edge_vertex( polygon_edge, 1 ) } );
    }

    template < index_t dimension >
    const PolygonVertex&
        PolygonalSurfaceBase< dimension >::polygon_around_vertex(
            index_t vertex_id, PolygonalSurfaceKey ) const
    {
        OPENGEODE_ASSERT( vertex_id < this->nb_vertices(),
            "[PolygonalSurfaceBase::polygon_around_vertex] Accessing an "
            "invalid vertex" );
        return impl_->polygon_around_vertex( vertex_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::find_or_create_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        return impl_->find_or_create_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    const std::array< index_t, 2 >&
        PolygonalSurfaceBase< dimension >::edge_vertices(
            index_t edge_id ) const
    {
        return impl_->get_edge_vertices( edge_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBase< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new, PolygonalSurfaceKey )
    {
        impl_->update_edge_vertices( old2new );
    }

    template < index_t dimension >
    void PolygonalSurfaceBase< dimension >::update_edge_vertex(
        std::array< index_t, 2 > edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id,
        PolygonalSurfaceKey )
    {
        impl_->update_edge_vertex(
            std::move( edge_vertices ), edge_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void PolygonalSurfaceBase< dimension >::remove_edge(
        std::array< index_t, 2 > edge_vertices, PolygonalSurfaceKey )
    {
        impl_->remove_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    std::vector< index_t > PolygonalSurfaceBase< dimension >::delete_edges(
        const std::vector< bool >& to_delete, PolygonalSurfaceKey )
    {
        return impl_->delete_edges( to_delete );
    }

    template < index_t dimension >
    std::vector< index_t >
        PolygonalSurfaceBase< dimension >::remove_isolated_edges(
            PolygonalSurfaceKey )
    {
        return impl_->remove_isolated_edges();
    }

    template < index_t dimension >
    void PolygonalSurfaceBase< dimension >::associate_polygon_vertex_to_vertex(
        const PolygonVertex& polygon_vertex,
        index_t vertex_id,
        PolygonalSurfaceKey )
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
    bool PolygonalSurfaceBase< dimension >::isolated_vertex(
        index_t vertex_id ) const
    {
        return impl_->isolated_vertex( vertex_id );
    }

    template < index_t dimension >
    bool PolygonalSurfaceBase< dimension >::isolated_edge(
        index_t edge_id ) const
    {
        return impl_->isolated_edge( edge_id );
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
        const auto vertex = polygon_vertex.vertex_id;
        const auto polygon = polygon_vertex.polygon_id;
        const auto nb_vertices = nb_polygon_vertices( polygon );
        return { polygon, ( vertex + 1 ) % nb_vertices };
    }

    template < index_t dimension >
    PolygonVertex PolygonalSurfaceBase< dimension >::previous_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        const auto vertex = polygon_vertex.vertex_id;
        const auto polygon = polygon_vertex.polygon_id;
        const auto nb_vertices = nb_polygon_vertices( polygon );
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
            const auto v0 = polygon_edge_vertex( polygon_edge, 0 );
            const auto v1 = polygon_edge_vertex( polygon_edge, 1 );
            const auto polygon_adj = polygon_adjacent( polygon_edge );
            for( const auto e : Range{ nb_polygon_edges( polygon_adj ) } )
            {
                const auto polygon = polygon_adjacent( { polygon_adj, e } );
                if( polygon == polygon_edge.polygon_id
                    && polygon_edge_vertex( { polygon_adj, e }, 0 ) == v1
                    && polygon_edge_vertex( { polygon_adj, e }, 1 ) == v0 )
                {
                    return { polygon_adj, e };
                }
            }
            throw OpenGeodeException{ "[PolygonalSurfaceBase::polygon_adjacent_"
                                      "edge] Wrong adjacency with polygons: ",
                polygon_edge.polygon_id, " and ", polygon_adj };
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
    PolygonEdgesOnBorder
        PolygonalSurfaceBase< dimension >::polygon_edges_on_border(
            index_t polygon_id ) const
    {
        PolygonEdgesOnBorder borders;
        for( const auto e : Range{ nb_polygon_edges( polygon_id ) } )
        {
            PolygonEdge edge{ polygon_id, e };
            if( is_edge_on_border( edge ) )
            {
                borders.emplace_back( std::move( edge ) );
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
    double PolygonalSurfaceBase< dimension >::edge_length(
        index_t edge_id ) const
    {
        const auto vertices = edge_vertices( edge_id );
        return Vector< dimension >{ this->point( vertices[0] ),
            this->point( vertices[1] ) }
            .length();
    }

    template < index_t dimension >
    Point< dimension > PolygonalSurfaceBase< dimension >::edge_barycenter(
        index_t edge_id ) const
    {
        const auto vertices = edge_vertices( edge_id );
        return ( this->point( vertices[0] ) + this->point( vertices[1] ) ) / 2.;
    }

    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::polygon_edge_vertex(
        const PolygonEdge& polygon_edge, index_t vertex_id ) const
    {
        OPENGEODE_ASSERT( vertex_id < 2, "[PolygonalSurfaceBase::polygon_"
                                         "edge_vertex] vertex_id should be "
                                         "0 or 1" );
        const auto vertex = polygon_edge.edge_id;
        const auto polygon = polygon_edge.polygon_id;
        const auto nb_vertices = nb_polygon_vertices( polygon );
        return polygon_vertex(
            { polygon, ( vertex + vertex_id ) % nb_vertices } );
    }

    template < index_t dimension >
    Point< dimension > PolygonalSurfaceBase< dimension >::polygon_barycenter(
        index_t polygon_id ) const
    {
        Point< dimension > barycenter;
        for( const auto v : Range{ nb_polygon_vertices( polygon_id ) } )
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
        for( const auto i : Range{ 1, nb_polygon_vertices( polygon_id ) - 1 } )
        {
            const auto& p2 = this->point( polygon_vertex( { polygon_id, i } ) );
            const auto& p3 =
                this->point( polygon_vertex( { polygon_id, i + 1 } ) );
            area += triangle_area( p1, p2, p3 );
        }
        return area;
    }

    template < index_t dimension >
    PolygonsAroundVertex
        PolygonalSurfaceBase< dimension >::polygons_around_vertex(
            index_t vertex_id ) const
    {
        OPENGEODE_ASSERT( vertex_id < this->nb_vertices(),
            "[PolygonalSurfaceBase::polygons_around_vertex] Accessing an "
            "invalid vertex" );
        PolygonsAroundVertex polygons;
        const auto& first_polygon = impl_->polygon_around_vertex( vertex_id );
        if( first_polygon.polygon_id == NO_ID )
        {
            return polygons;
        }
        OPENGEODE_ASSERT( polygon_vertex( first_polygon ) == vertex_id,
            "[PolygonalSurfaceBase::polygons_around_vertex] Wrong polygon "
            "around vertex" );
        absl::InlinedVector< index_t, 10 > polygons_visited;
        std::stack< PolygonVertex > S;
        S.push( first_polygon );
        while( !S.empty() )
        {
            const auto polygon_vertex = S.top();
            S.pop();
            if( absl::c_find( polygons_visited, polygon_vertex.polygon_id )
                != polygons_visited.end() )
            {
                continue;
            }
            polygons_visited.push_back( polygon_vertex.polygon_id );
            polygons.push_back( polygon_vertex );

            const PolygonEdge polygon_edge{ polygon_vertex };
            if( !is_edge_on_border( polygon_edge ) )
            {
                const auto adj_edge = polygon_adjacent_edge( polygon_edge );
                S.emplace( next_polygon_edge( adj_edge ) );
            }
            const auto prev_edge = previous_polygon_edge( polygon_vertex );
            if( !is_edge_on_border( prev_edge ) )
            {
                const auto adj_edge = polygon_adjacent_edge( prev_edge );
                S.emplace( adj_edge );
            }
        }
        return polygons;
    }

    template < index_t dimension >
    absl::optional< PolygonEdge >
        PolygonalSurfaceBase< dimension >::polygon_edge_from_vertices(
            index_t from_vertex_id, index_t to_vertex_id ) const
    {
        for( auto&& polygon_vertex : polygons_around_vertex( from_vertex_id ) )
        {
            const auto next_vertex = next_polygon_vertex( polygon_vertex );
            if( this->polygon_vertex( next_vertex ) == to_vertex_id )
            {
                return polygon_vertex;
            }
        }
        return absl::nullopt;
    }
    template < index_t dimension >
    index_t PolygonalSurfaceBase< dimension >::edge_from_vertices(
        const std::array< index_t, 2 >& vertices ) const
    {
        return impl_->find_edge( vertices );
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
        OPENGEODE_ASSERT( vertex_id < nb_vertices(),
            "[PolygonalSurfaceBase::point] Trying to access an invalid point" );
        return get_point( vertex_id );
    }

    template < index_t dimension >
    template < typename Archive >
    void PolygonalSurfaceBase< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, PolygonalSurfaceBase >{},
            []( Archive& archive, PolygonalSurfaceBase& surface ) {
                archive.ext( surface, bitsery::ext::BaseClass< VertexSet >{} );
                archive.object( surface.impl_ );
            } );
    }

    template < index_t dimension >
    BoundingBox< dimension >
        PolygonalSurfaceBase< dimension >::bounding_box() const
    {
        BoundingBox< dimension > box;
        for( const auto p : Range{ nb_vertices() } )
        {
            box.add_point( point( p ) );
        }
        return box;
    }

    Vector3D PolygonalSurface< 3 >::polygon_normal( index_t polygon_id ) const
    {
        check_polygon_id( *this, polygon_id );
        const auto barycenter = polygon_barycenter( polygon_id );
        Vector3D normal;
        for( const auto v : Range{ 1, nb_polygon_vertices( polygon_id ) } )
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
        catch( const OpenGeodeException& )
        {
            try
            {
                return TriangulatedSurface< dimension >::create( type );
            }
            catch( const OpenGeodeException& e )
            {
                Logger::error( e.what() );
                throw OpenGeodeException{
                    "Could not create PolygonalSurface data structure: ",
                    type.get()
                };
            }
        }
    }

    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > >
        PolygonalSurface< dimension >::clone() const
    {
        auto clone = create( this->type_name() );
        auto builder = PolygonalSurfaceBuilder< dimension >::create( *clone );
        builder->copy( *this, {} );
        return clone;
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
        catch( const OpenGeodeException& )
        {
            try
            {
                return TriangulatedSurfaceFactory< 3 >::create( type );
            }
            catch( const OpenGeodeException& e )
            {
                Logger::error( e.what() );
                throw OpenGeodeException{
                    "Could not create PolygonalSurface data structure: ",
                    type.get()
                };
            }
        }
    }

    std::unique_ptr< PolygonalSurface< 3 > >
        PolygonalSurface< 3 >::clone() const
    {
        auto clone = create( type_name() );
        auto builder = PolygonalSurfaceBuilder< 3 >::create( *clone );
        builder->copy( *this, {} );
        return clone;
    }

    template class opengeode_mesh_api PolygonalSurfaceBase< 2 >;
    template class opengeode_mesh_api PolygonalSurfaceBase< 3 >;

    template class opengeode_mesh_api PolygonalSurface< 2 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolygonalSurfaceBase< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolygonalSurfaceBase< 3 > );
} // namespace geode
