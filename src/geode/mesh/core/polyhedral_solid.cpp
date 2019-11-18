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

#include <geode/mesh/core/polyhedral_solid.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/vector.h>

#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/mesh/builder/polyhedral_solid_builder.h>
#include <geode/mesh/core/detail/facet_storage.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>

namespace
{
    template < geode::index_t dimension >
    void check_polyhedron_id( const geode::PolyhedralSolid< dimension >& solid,
        const geode::index_t polyhedron_id )
    {
        OPENGEODE_EXCEPTION( polyhedron_id < solid.nb_polyhedra(),
            "[check_polyhedron_id] Trying to access an invalid polyhedron" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_vertex_id(
        const geode::PolyhedralSolid< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t vertex_id )
    {
        OPENGEODE_EXCEPTION(
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
        OPENGEODE_EXCEPTION(
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
        OPENGEODE_EXCEPTION( vertex_id < solid.nb_polyhedron_facet_vertices(
                                 { polyhedron_id, facet_id } ),
            "[check_polyhedron_facet_vertex_id] Trying to access an invalid "
            "polyhedron facet vertex" );
    }

} // namespace

namespace geode
{
    template < index_t dimension >
    class PolyhedralSolid< dimension >::Impl
        : public detail::FacetStorage< std::vector< index_t > >
    {
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

        index_t find_facet( const std::vector< index_t >& facet_vertices ) const
        {
            return FacetStorage::find_facet( facet_vertices );
        }

        index_t find_or_create_facet(
            const std::vector< index_t >& facet_vertices )
        {
            return this->add_facet( facet_vertices );
        }

        const std::vector< index_t >& get_facet_vertices(
            const index_t facet_id ) const
        {
            return FacetStorage::get_facet_vertices( facet_id );
        }

        void update_facet_vertex( const std::vector< index_t >& facet_vertices,
            const index_t facet_vertex_id,
            const index_t new_vertex_id )
        {
            auto updated_facet_vertices = facet_vertices;
            updated_facet_vertices[facet_vertex_id] = new_vertex_id;
            this->add_facet( updated_facet_vertices );
            this->remove_facet( facet_vertices );
            this->clean_facets();
        }

        void update_facet_vertices( const std::vector< index_t >& old2new )
        {
            FacetStorage::update_facet_vertices( old2new );
        }

        void remove_facet( const std::vector< index_t >& facet_vertices )
        {
            FacetStorage::remove_facet( facet_vertices );
        }

        void delete_facets( const std::vector< bool >& to_delete )
        {
            FacetStorage::delete_facets( to_delete );
        }

        void remove_isolated_facets()
        {
            this->clean_facets();
        }

        AttributeManager& polyhedron_attribute_manager() const
        {
            return polyhedron_attribute_manager_;
        }

        AttributeManager& facet_attribute_manager() const
        {
            return FacetStorage::facet_attribute_manager();
        }

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.ext(
                        impl, bitsery::ext::BaseClass< detail::FacetStorage<
                                  std::vector< index_t > > >{} );
                    archive.object( impl.polyhedron_attribute_manager_ );
                    archive.ext( impl.polyhedron_around_vertex_,
                        bitsery::ext::StdSmartPtr{} );
                } );
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
        catch( const OpenGeodeException& e )
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
        std::vector< index_t > facet_vertices(
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
        return barycenter / vertices.size();
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedra() const
    {
        return polyhedron_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    std::vector< PolyhedronVertex >
        PolyhedralSolid< dimension >::polyhedra_around_vertex(
            index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < this->nb_vertices(),
            "[PolyhedralSolid::polyhedra_around_vertex] Accessing an "
            "invalid "
            "vertex" );
        std::vector< PolyhedronVertex > polyhedra;
        const auto& first_polyhedron =
            impl_->polyhedron_around_vertex( vertex_id );
        if( first_polyhedron.polyhedron_id == NO_ID )
        {
            return polyhedra;
        }
        OPENGEODE_ASSERT( polyhedron_vertex( first_polyhedron ) == vertex_id,
            "[PolyhedralSolid::polyhedra_around_vertex] Wrong "
            "polyhedron "
            "around vertex" );
        std::vector< index_t > polyhedra_visited;
        polyhedra_visited.reserve( 10 );
        std::stack< PolyhedronVertex > S;
        S.push( first_polyhedron );
        while( !S.empty() )
        {
            const auto polyhedron_vertex_id = S.top();
            S.pop();
            const auto p = polyhedron_vertex_id.polyhedron_id;
            if( contain( polyhedra_visited, p ) )
            {
                continue;
            }
            polyhedra_visited.push_back( p );
            polyhedra.push_back( polyhedron_vertex_id );

            for( const auto f : Range{ nb_polyhedron_facets( p ) } )
            {
                const PolyhedronFacet polyhedron_facet{ p, f };
                for( const auto v :
                    Range{ nb_polyhedron_facet_vertices( polyhedron_facet ) } )
                {
                    if( polyhedron_facet_vertex( { polyhedron_facet, v } )
                        == vertex_id )
                    {
                        if( !is_polyhedron_facet_on_border( polyhedron_facet ) )
                        {
                            const auto adj_polyhedron =
                                polyhedron_adjacent( polyhedron_facet );
                            PolyhedronVertex adj_vertex{ adj_polyhedron,
                                NO_ID };
                            for( const auto v_adj :
                                Range{
                                    nb_polyhedron_vertices( adj_polyhedron ) } )
                            {
                                if( polyhedron_vertex(
                                        { adj_polyhedron, v_adj } )
                                    == vertex_id )
                                {
                                    adj_vertex.vertex_id = v_adj;
                                    break;
                                }
                            }
                            S.emplace( adj_vertex );
                        }
                        break;
                    }
                }
            }
        }
        return polyhedra;
    }

    template < index_t dimension >
    const PolyhedronVertex&
        PolyhedralSolid< dimension >::polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < this->nb_vertices(),
            "[PolyhedralSolid::polyhedron_around_vertex] Accessing an "
            "invalid "
            "vertex" );
        return impl_->polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::find_or_create_facet(
        const std::vector< index_t >& facet_vertices )
    {
        return impl_->find_or_create_facet( facet_vertices );
    }

    template < index_t dimension >
    const std::vector< index_t >& PolyhedralSolid< dimension >::facet_vertices(
        index_t facet_id ) const
    {
        return impl_->get_facet_vertices( facet_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::facet_from_vertices(
        const std::vector< index_t >& vertices ) const
    {
        return impl_->find_facet( vertices );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::update_facet_vertices(
        const std::vector< index_t >& old2new )
    {
        impl_->update_facet_vertices( old2new );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::update_facet_vertex(
        const std::vector< index_t >& facet_vertices,
        index_t facet_vertex_id,
        index_t new_vertex_id )
    {
        impl_->update_facet_vertex(
            facet_vertices, facet_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::remove_facet(
        const std::vector< index_t >& facet_vertices )
    {
        impl_->remove_facet( facet_vertices );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::remove_isolated_facets()
    {
        impl_->remove_isolated_facets();
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::delete_facets(
        const std::vector< bool >& to_delete )
    {
        impl_->delete_facets( to_delete );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::associate_polyhedron_vertex_to_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
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
            std::vector< index_t > vertices(
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
                        if( !contain(
                                vertices, polyhedron_facet_vertex(
                                              { { polyhedron_adj, f }, v } ) ) )
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
    std::vector< PolyhedronFacet >
        PolyhedralSolid< dimension >::polyhedron_facets_on_border(
            index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        std::vector< PolyhedronFacet > borders;
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
        PolyhedralSolid< dimension >::polyhedron_attribute_manager() const
    {
        return impl_->polyhedron_attribute_manager();
    }

    template < index_t dimension >
    const Point< dimension >& PolyhedralSolid< dimension >::point(
        index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < nb_vertices(),
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
        builder->copy( *this );
        return clone;
    }

    template class opengeode_mesh_api PolyhedralSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolyhedralSolid< 3 > );
} // namespace geode
