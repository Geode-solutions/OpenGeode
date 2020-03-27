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

#include <geode/mesh/core/polyhedral_solid.h>

#include <absl/container/flat_hash_set.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/polyhedral_solid_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/detail/facet_storage.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>

namespace
{
    template < geode::index_t dimension >
    void check_polyhedron_id( const geode::PolyhedralSolid< dimension >& solid,
        const geode::index_t polyhedron_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        OPENGEODE_ASSERT( polyhedron_id < solid.nb_polyhedra(),
            "[check_polyhedron_id] Trying to access an invalid polyhedron" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_vertex_id(
        const geode::PolyhedralSolid< dimension >& solid,
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
    void check_polyhedron_facet_id(
        const geode::PolyhedralSolid< dimension >& solid,
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
        const geode::PolyhedralSolid< dimension >& solid,
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
    geode::index_t get_vertex_id_in_polyhedron_facet(
        const geode::PolyhedralSolid< dimension >& solid,
        const geode::PolyhedronFacet& facet,
        geode::index_t vertex_id )
    {
        for( const auto v :
            geode::Range{ solid.nb_polyhedron_facet_vertices( facet ) } )
        {
            if( solid.polyhedron_facet_vertex( { facet, v } ) == vertex_id )
            {
                return v;
            }
        }
        return geode::NO_ID;
    }

    template < geode::index_t dimension >
    bool is_edge_in_polyhedron_facet(
        const geode::PolyhedralSolid< dimension >& solid,
        const geode::PolyhedronFacet& facet,
        const std::array< geode::index_t, 2 >& edge_vertices )
    {
        const auto nb_facet_vertices =
            solid.nb_polyhedron_facet_vertices( facet );
        const auto local_id =
            get_vertex_id_in_polyhedron_facet( solid, facet, edge_vertices[0] );
        if( local_id == geode::NO_ID )
        {
            return false;
        }
        const auto previous = solid.polyhedron_facet_vertex( { facet,
            ( local_id + nb_facet_vertices - 1 ) % nb_facet_vertices } );
        if( previous == edge_vertices[1] )
        {
            return true;
        }
        const auto next = solid.polyhedron_facet_vertex(
            { facet, ( local_id + 1 ) % nb_facet_vertices } );
        if( next == edge_vertices[1] )
        {
            return true;
        }
        return false;
    }

} // namespace

namespace geode
{
    template < index_t dimension >
    class PolyhedralSolid< dimension >::Impl
        : public detail::FacetStorage< PolyhedronFacetVertices >,
          public detail::FacetStorage< std::array< index_t, 2 > >
    {
        using Facets = detail::FacetStorage< PolyhedronFacetVertices >;
        using Edges = detail::FacetStorage< std::array< index_t, 2 > >;
        friend class bitsery::Access;

    public:
        explicit Impl( PolyhedralSolid& solid )
            : polyhedron_around_vertex_(
                solid.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        PolyhedronVertex >(
                        "polyhedron_around_vertex", PolyhedronVertex{} ) )
        {
        }

        const PolyhedronVertex& polyhedron_around_vertex(
            const index_t vertex_id ) const
        {
            return polyhedron_around_vertex_->value( vertex_id );
        }

        void associate_polyhedron_vertex_to_vertex(
            const PolyhedronVertex& polyhedron_vertex, const index_t vertex_id )
        {
            polyhedron_around_vertex_->set_value(
                vertex_id, polyhedron_vertex );
        }

        index_t find_facet(
            const PolyhedronFacetVertices& facet_vertices ) const
        {
            return Facets::find_facet( facet_vertices );
        }

        index_t find_edge( const std::array< index_t, 2 >& edge_vertices ) const
        {
            return Edges::find_facet( edge_vertices );
        }

        index_t find_or_create_facet( PolyhedronFacetVertices facet_vertices )
        {
            return Facets::add_facet( std::move( facet_vertices ) );
        }

        index_t find_or_create_edge( std::array< index_t, 2 > edge_vertices )
        {
            // if( edge_vertices[0] < edge_vertices[1] )
            // {
            return Edges::add_facet( std::move( edge_vertices ) );
            // }
            // return find_edge( edge_vertices );
        }

        const PolyhedronFacetVertices& get_facet_vertices(
            const index_t facet_id ) const
        {
            return Facets::get_facet_vertices( facet_id );
        }

        const std::array< index_t, 2 >& get_edge_vertices(
            const index_t edge_id ) const
        {
            return Edges::get_facet_vertices( edge_id );
        }

        void update_facet_vertex( PolyhedronFacetVertices facet_vertices,
            const index_t facet_vertex_id,
            const index_t new_vertex_id )
        {
            auto updated_facet_vertices = facet_vertices;
            updated_facet_vertices[facet_vertex_id] = new_vertex_id;
            Facets::add_facet( std::move( updated_facet_vertices ) );
            Facets::remove_facet( std::move( facet_vertices ) );
        }

        void update_edge_vertex( std::array< index_t, 2 > edge_vertices,
            const index_t edge_vertex_id,
            const index_t new_vertex_id )
        {
            auto updated_edge_vertices = edge_vertices;
            updated_edge_vertices[edge_vertex_id] = new_vertex_id;
            if( edge_vertices[0] < edge_vertices[1] )
            {
                Edges::add_facet( std::move( updated_edge_vertices ) );
                Edges::remove_facet( std::move( edge_vertices ) );
            }
        }

        void update_facet_vertices( absl::Span< const index_t > old2new )
        {
            Facets::update_facet_vertices( old2new );
        }

        void update_edge_vertices( absl::Span< const index_t > old2new )
        {
            Edges::update_facet_vertices( old2new );
        }

        void remove_facet( PolyhedronFacetVertices facet_vertices )
        {
            Facets::remove_facet( std::move( facet_vertices ) );
        }

        void remove_edge( std::array< index_t, 2 > edge_vertices )
        {
            if( edge_vertices[0] < edge_vertices[1] )
            {
                Edges::remove_facet( std::move( edge_vertices ) );
            }
        }

        std::vector< index_t > delete_facets(
            const std::vector< bool >& to_delete )
        {
            return Facets::delete_facets( to_delete );
        }

        std::vector< index_t > delete_edges(
            const std::vector< bool >& to_delete )
        {
            return Edges::delete_facets( to_delete );
        }

        std::vector< index_t > remove_isolated_facets()
        {
            return Facets::clean_facets();
        }

        std::vector< index_t > remove_isolated_edges()
        {
            return Edges::clean_facets();
        }

        bool isolated_vertex( index_t vertex_id ) const
        {
            return polyhedron_around_vertex( vertex_id ) == PolyhedronVertex{};
        }

        bool isolated_edge( index_t edge_id ) const
        {
            return Edges::get_counter( edge_id ) == 0;
        }

        bool isolated_facet( index_t facet_id ) const
        {
            return Facets::get_counter( facet_id ) == 0;
        }

        AttributeManager& polyhedron_attribute_manager() const
        {
            return polyhedron_attribute_manager_;
        }

        AttributeManager& facet_attribute_manager() const
        {
            return Facets::facet_attribute_manager();
        }

        AttributeManager& edge_attribute_manager() const
        {
            return Edges::facet_attribute_manager();
        }

    private:
        void convert_attribute_to_abseil()
        {
            const auto attribute_name = Facets::attribute_name();
            auto old_vertices =
                facet_attribute_manager()
                    .template find_attribute< std::vector< index_t > >(
                        attribute_name );
            facet_attribute_manager().delete_attribute( attribute_name );
            auto new_vertices =
                facet_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        PolyhedronFacetVertices >(
                        attribute_name, PolyhedronFacetVertices{} );

            for( const auto f :
                Range{ facet_attribute_manager().nb_elements() } )
            {
                const auto& old_values = old_vertices->value( f );
                new_vertices->modify_value(
                    f, [&old_values]( PolyhedronFacetVertices& value ) {
                        for( const auto& old_value : old_values )
                        {
                            value.push_back( old_value );
                        }
                    } );
            }
            Facets::update_attribute();
        }

        void initialize_edges_from_facets()
        {
            for( const auto f :
                Range{ facet_attribute_manager().nb_elements() } )
            {
                const auto& facet_vertices = get_facet_vertices( f );
                const auto nb_facet_vertices = facet_vertices.size();
                for( const auto v : Range{ nb_facet_vertices } )
                {
                    std::array< index_t, 2 > edge_vertices{ facet_vertices[v],
                        facet_vertices[( v + 1 ) % nb_facet_vertices] };
                    Edges::add_facet( std::move( edge_vertices ) );
                }
            }
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                         archive.ext( impl,
                             bitsery::ext::BaseClass< detail::FacetStorage<
                                 PolyhedronFacetVertices > >{} ); // should be
                                                                  // std::vector
                         archive.object( impl.polyhedron_attribute_manager_ );
                         archive.ext( impl.polyhedron_around_vertex_,
                             bitsery::ext::StdSmartPtr{} );
                     },
                        []( Archive& archive, Impl& impl ) {
                            archive.ext( impl,
                                bitsery::ext::BaseClass< detail::FacetStorage<
                                    PolyhedronFacetVertices > >{} ); // should
                                                                     // be
                                                                     // std::vector
                            archive.object(
                                impl.polyhedron_attribute_manager_ );
                            archive.ext( impl.polyhedron_around_vertex_,
                                bitsery::ext::StdSmartPtr{} );
                            archive.ext( impl,
                                bitsery::ext::BaseClass< detail::FacetStorage<
                                    std::array< index_t, 2 > > >{} );
                        },
                        []( Archive& archive, Impl& impl ) {
                            archive.ext( impl,
                                bitsery::ext::BaseClass< detail::FacetStorage<
                                    PolyhedronFacetVertices > >{} );
                            archive.object(
                                impl.polyhedron_attribute_manager_ );
                            archive.ext( impl.polyhedron_around_vertex_,
                                bitsery::ext::StdSmartPtr{} );
                            archive.ext( impl,
                                bitsery::ext::BaseClass< detail::FacetStorage<
                                    std::array< index_t, 2 > > >{} );
                        } },
                    { []( Impl& impl ) {
                         impl.convert_attribute_to_abseil();
                         impl.initialize_edges_from_facets();
                     },
                        []( Impl& impl ) {
                            impl.convert_attribute_to_abseil();
                        } } } );
        }

    private:
        mutable AttributeManager polyhedron_attribute_manager_;
        std::shared_ptr< VariableAttribute< PolyhedronVertex > >
            polyhedron_around_vertex_;
    };

    template < index_t dimension >
    PolyhedralSolid< dimension >::PolyhedralSolid() : impl_( *this )
    {
    }

    template < index_t dimension >
    PolyhedralSolid< dimension >::~PolyhedralSolid() // NOLINT
    {
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > >
        PolyhedralSolid< dimension >::create()
    {
        return std::unique_ptr< PolyhedralSolid< dimension > >{
            new OpenGeodePolyhedralSolid< dimension >
        };
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > >
        PolyhedralSolid< dimension >::create( const MeshType& type )
    {
        try
        {
            return PolyhedralSolidFactory< dimension >::create( type );
        }
        catch( const OpenGeodeException& )
        {
            try
            {
                return TetrahedralSolid< dimension >::create( type );
            }
            catch( const OpenGeodeException& e )
            {
                Logger::error( e.what() );
                throw OpenGeodeException{
                    "Could not create PolyhedralSolid data structure: ",
                    type.get()
                };
            }
        }
    }

    template < index_t dimension >
    double PolyhedralSolid< dimension >::polyhedron_volume(
        index_t /*unused*/ ) const
    {
        throw OpenGeodeException{ "polyhedron_volume not implemented yet" };
        return 0;
    }

    template < index_t dimension >
    double PolyhedralSolid< dimension >::facet_area( index_t /*unused*/ ) const
    {
        throw OpenGeodeException{ "facet_area not implemented yet" };
        return 0;
    }

    template < index_t dimension >
    double PolyhedralSolid< dimension >::edge_length( index_t edge_id ) const
    {
        const auto vertices = edge_vertices( edge_id );
        return Vector< dimension >{ this->point( vertices[0] ),
            this->point( vertices[1] ) }
            .length();
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        check_polyhedron_id( *this, polyhedron_vertex.polyhedron_id );
        check_polyhedron_vertex_id( *this, polyhedron_vertex.polyhedron_id,
            polyhedron_vertex.vertex_id );
        return get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::polyhedron_facet(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        PolyhedronFacetVertices facet_vertices(
            nb_polyhedron_facet_vertices( polyhedron_facet ) );
        for( const auto v : Range{ facet_vertices.size() } )
        {
            facet_vertices[v] =
                polyhedron_facet_vertex( { polyhedron_facet, v } );
        }
        return impl_->find_facet( facet_vertices );
    }

    template < index_t dimension >
    Point< dimension > PolyhedralSolid< dimension >::polyhedron_barycenter(
        index_t polyhedron_id ) const
    {
        Point< dimension > barycenter;
        for( const auto v : Range{ nb_polyhedron_vertices( polyhedron_id ) } )
        {
            barycenter =
                barycenter
                + this->point( polyhedron_vertex( { polyhedron_id, v } ) );
        }
        return barycenter / nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    Point< dimension > PolyhedralSolid< dimension >::facet_barycenter(
        index_t facet_id ) const
    {
        Point< dimension > barycenter;
        const auto vertices = facet_vertices( facet_id );
        OPENGEODE_ASSERT( !vertices.empty(),
            "[PolyhedralSolid::facet_barycenter] Facet "
            "vertices should not be empty" );
        for( const auto v : vertices )
        {
            barycenter = barycenter + this->point( v );
        }
        return barycenter / static_cast< double >( vertices.size() );
    }

    template < index_t dimension >
    Point< dimension > PolyhedralSolid< dimension >::edge_barycenter(
        index_t edge_id ) const
    {
        const auto vertices = edge_vertices( edge_id );
        return ( this->point( vertices[0] ) + this->point( vertices[1] ) ) / 2.;
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedra() const
    {
        return polyhedron_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    bool PolyhedralSolid< dimension >::isolated_vertex(
        index_t vertex_id ) const
    {
        return impl_->isolated_vertex( vertex_id );
    }

    template < index_t dimension >
    bool PolyhedralSolid< dimension >::isolated_edge( index_t edge_id ) const
    {
        return impl_->isolated_edge( edge_id );
    }

    template < index_t dimension >
    bool PolyhedralSolid< dimension >::isolated_facet( index_t facet_id ) const
    {
        return impl_->isolated_facet( facet_id );
    }

    template < index_t dimension >
    PolyhedraAroundVertex PolyhedralSolid< dimension >::polyhedra_around_vertex(
        index_t vertex_id ) const
    {
        OPENGEODE_ASSERT( vertex_id < this->nb_vertices(),
            "[PolyhedralSolid::polyhedra_around_vertex] Accessing an "
            "invalid vertex" );
        PolyhedraAroundVertex polyhedra;
        const auto& first_polyhedron =
            impl_->polyhedron_around_vertex( vertex_id );
        if( first_polyhedron.polyhedron_id == NO_ID )
        {
            return polyhedra;
        }
        OPENGEODE_ASSERT( polyhedron_vertex( first_polyhedron ) == vertex_id,
            "[PolyhedralSolid::polyhedra_around_vertex] Wrong "
            "polyhedron around vertex" );
        absl::flat_hash_set< index_t > polyhedra_visited;
        std::stack< PolyhedronVertex > S;
        S.push( first_polyhedron );
        polyhedra_visited.insert( first_polyhedron.polyhedron_id );
        while( !S.empty() )
        {
            const auto polyhedron_vertex_id = S.top();
            S.pop();
            const auto p = polyhedron_vertex_id.polyhedron_id;
            polyhedra.push_back( polyhedron_vertex_id );

            for( const auto f : Range{ nb_polyhedron_facets( p ) } )
            {
                const PolyhedronFacet polyhedron_facet{ p, f };
                for( const auto v :
                    Range{ nb_polyhedron_facet_vertices( polyhedron_facet ) } )
                {
                    if( polyhedron_facet_vertex( { polyhedron_facet, v } )
                        != vertex_id )
                    {
                        continue;
                    }
                    if( !is_polyhedron_facet_on_border( polyhedron_facet ) )
                    {
                        const auto adj_polyhedron =
                            polyhedron_adjacent( polyhedron_facet );
                        if( !polyhedra_visited.insert( adj_polyhedron ).second )
                        {
                            continue;
                        }
                        PolyhedronVertex adj_vertex{ adj_polyhedron, NO_ID };
                        for( const auto v_adj :
                            Range{ nb_polyhedron_vertices( adj_polyhedron ) } )
                        {
                            if( polyhedron_vertex( { adj_polyhedron, v_adj } )
                                == vertex_id )
                            {
                                adj_vertex.vertex_id = v_adj;
                                break;
                            }
                        }
                        OPENGEODE_ASSERT( adj_vertex.vertex_id != NO_ID,
                            "[PolyhedralSolid::polyhedra_around_vertex] "
                            "Adjacency issue detected" );
                        S.emplace( adj_vertex );
                    }
                    break;
                }
            }
        }
        return polyhedra;
    }

    template < index_t dimension >
    PolyhedraAroundEdge PolyhedralSolid< dimension >::polyhedra_around_edge(
        index_t edge_id ) const
    {
        OPENGEODE_ASSERT( edge_id < this->nb_edges(),
            "[PolyhedralSolid::polyhedra_around_edge] Accessing an "
            "invalid edge" );
        PolyhedraAroundEdge result;
        const auto& vertices = edge_vertices( edge_id );
        const auto& polyhedron_vertices =
            polyhedra_around_vertex( vertices[0] );
        for( const auto& polyhedron_vertex : polyhedron_vertices )
        {
            for( const auto f : Range{
                     nb_polyhedron_facets( polyhedron_vertex.polyhedron_id ) } )
            {
                const PolyhedronFacet facet{ polyhedron_vertex.polyhedron_id,
                    f };
                if( is_edge_in_polyhedron_facet( *this, facet, vertices ) )
                {
                    result.push_back( polyhedron_vertex.polyhedron_id );
                    break;
                }
            }
        }
        return result;
    }

    template < index_t dimension >
    PolyhedraAroundFacet PolyhedralSolid< dimension >::polyhedra_from_facet(
        index_t facet_id ) const
    {
        OPENGEODE_ASSERT( facet_id < this->nb_facets(),
            "[PolyhedralSolid::polyhedron_from_facet] Accessing an "
            "invalid facet" );
        const auto& vertices = facet_vertices( facet_id );
        const auto& polyhedron_vertices =
            polyhedra_around_vertex( vertices[0] );
        for( const auto& polyhedron_vertex : polyhedron_vertices )
        {
            for( const auto f : Range{
                     nb_polyhedron_facets( polyhedron_vertex.polyhedron_id ) } )
            {
                const PolyhedronFacet facet{ polyhedron_vertex.polyhedron_id,
                    f };
                if( polyhedron_facet( facet ) == facet_id )
                {
                    const auto adjacent = polyhedron_adjacent( facet );
                    if( adjacent != NO_ID )
                    {
                        return { polyhedron_vertex.polyhedron_id, adjacent };
                    }
                    else
                    {
                        return { polyhedron_vertex.polyhedron_id };
                    }
                }
            }
        }
        return {};
    }

    template < index_t dimension >
    const PolyhedronVertex&
        PolyhedralSolid< dimension >::polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        OPENGEODE_ASSERT( vertex_id < this->nb_vertices(),
            "[PolyhedralSolid::polyhedron_around_vertex] Accessing an "
            "invalid vertex" );
        return impl_->polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::find_or_create_facet(
        PolyhedronFacetVertices facet_vertices )
    {
        return impl_->find_or_create_facet( std::move( facet_vertices ) );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::find_or_create_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        return impl_->find_or_create_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    const PolyhedronFacetVertices& PolyhedralSolid< dimension >::facet_vertices(
        index_t facet_id ) const
    {
        return impl_->get_facet_vertices( facet_id );
    }

    template < index_t dimension >
    const std::array< index_t, 2 >& PolyhedralSolid< dimension >::edge_vertices(
        index_t edge_id ) const
    {
        return impl_->get_edge_vertices( edge_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::facet_from_vertices(
        const PolyhedronFacetVertices& vertices ) const
    {
        return impl_->find_facet( vertices );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::edge_from_vertices(
        const std::array< index_t, 2 >& vertices ) const
    {
        return impl_->find_edge( vertices );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::update_facet_vertices(
        absl::Span< const index_t > old2new, PolyhedralSolidKey )
    {
        impl_->update_facet_vertices( old2new );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new, PolyhedralSolidKey )
    {
        impl_->update_edge_vertices( old2new );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::update_facet_vertex(
        PolyhedronFacetVertices facet_vertices,
        index_t facet_vertex_id,
        index_t new_vertex_id,
        PolyhedralSolidKey )
    {
        impl_->update_facet_vertex(
            std::move( facet_vertices ), facet_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::update_edge_vertex(
        std::array< index_t, 2 > edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id,
        PolyhedralSolidKey )
    {
        impl_->update_edge_vertex(
            std::move( edge_vertices ), edge_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::remove_facet(
        PolyhedronFacetVertices facet_vertices, PolyhedralSolidKey )
    {
        impl_->remove_facet( std::move( facet_vertices ) );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::remove_edge(
        std::array< index_t, 2 > edge_vertices, PolyhedralSolidKey )
    {
        impl_->remove_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    std::vector< index_t > PolyhedralSolid< dimension >::remove_isolated_facets(
        PolyhedralSolidKey )
    {
        return impl_->remove_isolated_facets();
    }

    template < index_t dimension >
    std::vector< index_t > PolyhedralSolid< dimension >::remove_isolated_edges(
        PolyhedralSolidKey )
    {
        return impl_->remove_isolated_edges();
    }

    template < index_t dimension >
    std::vector< index_t > PolyhedralSolid< dimension >::delete_facets(
        const std::vector< bool >& to_delete, PolyhedralSolidKey )
    {
        return impl_->delete_facets( to_delete );
    }

    template < index_t dimension >
    std::vector< index_t > PolyhedralSolid< dimension >::delete_edges(
        const std::vector< bool >& to_delete, PolyhedralSolidKey )
    {
        return impl_->delete_edges( to_delete );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::associate_polyhedron_vertex_to_vertex(
        const PolyhedronVertex& polyhedron_vertex,
        index_t vertex_id,
        PolyhedralSolidKey )
    {
        impl_->associate_polyhedron_vertex_to_vertex(
            polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_facets() const
    {
        return facet_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_edges() const
    {
        return edge_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedron_vertices(
        index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        return get_nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedron_facets(
        index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        return get_nb_polyhedron_facets( polyhedron_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::polyhedron_facet_vertex(
        const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        check_polyhedron_id(
            *this, polyhedron_facet_vertex.polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id( *this,
            polyhedron_facet_vertex.polyhedron_facet.polyhedron_id,
            polyhedron_facet_vertex.polyhedron_facet.facet_id );
        check_polyhedron_facet_vertex_id( *this,
            polyhedron_facet_vertex.polyhedron_facet.polyhedron_id,
            polyhedron_facet_vertex.polyhedron_facet.facet_id,
            polyhedron_facet_vertex.vertex_id );
        return get_polyhedron_facet_vertex( polyhedron_facet_vertex );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::polyhedron_facet_edge(
        const PolyhedronFacetEdge& polyhedron_facet_edge ) const
    {
        check_polyhedron_id(
            *this, polyhedron_facet_edge.polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id( *this,
            polyhedron_facet_edge.polyhedron_facet.polyhedron_id,
            polyhedron_facet_edge.polyhedron_facet.facet_id );
        check_polyhedron_facet_vertex_id( *this,
            polyhedron_facet_edge.polyhedron_facet.polyhedron_id,
            polyhedron_facet_edge.polyhedron_facet.facet_id,
            polyhedron_facet_edge.edge_id );
        const auto v0 =
            polyhedron_facet_vertex( { polyhedron_facet_edge.polyhedron_facet,
                polyhedron_facet_edge.edge_id } );
        const auto v1 =
            polyhedron_facet_vertex( { polyhedron_facet_edge.polyhedron_facet,
                ( polyhedron_facet_edge.edge_id + 1 )
                    % nb_polyhedron_facet_vertices(
                        polyhedron_facet_edge.polyhedron_facet ) } );
        return edge_from_vertices( { v0, v1 } );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedron_facet_vertices(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        return get_nb_polyhedron_facet_vertices( polyhedron_facet );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        return get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    PolyhedronFacet PolyhedralSolid< dimension >::polyhedron_adjacent_facet(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        if( !is_polyhedron_facet_on_border( polyhedron_facet ) )
        {
            absl::FixedArray< index_t > vertices(
                nb_polyhedron_facet_vertices( polyhedron_facet ) );
            for( const auto v : Range{ vertices.size() } )
            {
                vertices[v] =
                    polyhedron_facet_vertex( { polyhedron_facet, v } );
            }
            const auto polyhedron_adj = polyhedron_adjacent( polyhedron_facet );
            for( const auto f :
                Range{ nb_polyhedron_facets( polyhedron_adj ) } )
            {
                const auto polyhedron =
                    polyhedron_adjacent( { polyhedron_adj, f } );
                if( polyhedron == polyhedron_facet.polyhedron_id )
                {
                    bool all_contained{ true };
                    for( const auto v : Range{ nb_polyhedron_facet_vertices(
                             { polyhedron_adj, f } ) } )
                    {
                        if( absl::c_find(
                                vertices, polyhedron_facet_vertex(
                                              { { polyhedron_adj, f }, v } ) )
                            == vertices.end() )
                        {
                            all_contained = false;
                            break;
                        }
                    }
                    if( all_contained )
                    {
                        return { polyhedron_adj, f };
                    }
                }
            }
            throw OpenGeodeException{ "[PolyhedralSolid::polyhedron_adjacent_"
                                      "facet] Wrong adjacency with polyhedra: ",
                polyhedron_facet.polyhedron_id, " and ", polyhedron_adj };
        }
        return {};
    }

    template < index_t dimension >
    bool PolyhedralSolid< dimension >::is_polyhedron_facet_on_border(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        return polyhedron_adjacent( polyhedron_facet ) == NO_ID;
    }

    template < index_t dimension >
    PolyhedronFacetsOnBorder
        PolyhedralSolid< dimension >::polyhedron_facets_on_border(
            index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        PolyhedronFacetsOnBorder borders;
        for( const auto f : Range{ nb_polyhedron_facets( polyhedron_id ) } )
        {
            const PolyhedronFacet facet{ polyhedron_id, f };
            if( is_polyhedron_facet_on_border( facet ) )
            {
                borders.emplace_back( facet );
            }
        }
        return borders;
    }

    template < index_t dimension >
    AttributeManager&
        PolyhedralSolid< dimension >::facet_attribute_manager() const
    {
        return impl_->facet_attribute_manager();
    }

    template < index_t dimension >
    AttributeManager&
        PolyhedralSolid< dimension >::edge_attribute_manager() const
    {
        return impl_->edge_attribute_manager();
    }

    template < index_t dimension >
    AttributeManager&
        PolyhedralSolid< dimension >::polyhedron_attribute_manager() const
    {
        return impl_->polyhedron_attribute_manager();
    }

    template < index_t dimension >
    const Point< dimension >& PolyhedralSolid< dimension >::point(
        index_t vertex_id ) const
    {
        OPENGEODE_ASSERT( vertex_id < nb_vertices(),
            "[PolyhedralSolid::point] Trying to access an invalid "
            "point" );
        return get_point( vertex_id );
    }

    template < index_t dimension >
    template < typename Archive >
    void PolyhedralSolid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, PolyhedralSolid >{},
            []( Archive& archive, PolyhedralSolid& solid ) {
                archive.ext( solid, bitsery::ext::BaseClass< VertexSet >{} );
                archive.object( solid.impl_ );
            } );
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > >
        PolyhedralSolid< dimension >::clone() const
    {
        auto clone = create( this->type_name() );
        auto builder = PolyhedralSolidBuilder< dimension >::create( *clone );
        builder->copy( *this, {} );
        return clone;
    }

    template < index_t dimension >
    BoundingBox< dimension > PolyhedralSolid< dimension >::bounding_box() const
    {
        BoundingBox< dimension > box;
        for( const auto p : Range{ nb_vertices() } )
        {
            box.add_point( point( p ) );
        }
        return box;
    }

    template class opengeode_mesh_api PolyhedralSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolyhedralSolid< 3 > );
} // namespace geode
