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

#include <geode/mesh/core/solid_edges.h>

#include <absl/container/flat_hash_set.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/detail/facet_edges_impl.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/polyhedral_solid.h>

namespace
{
    template < geode::index_t dimension >
    void check_vertex_id( const geode::SolidEdges< dimension >& solid,
        const geode::index_t vertex_id )
    {
        geode_unused( solid );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < solid.nb_vertices(),
            "[check_vertex_id] Trying to access an invalid vertex" );
    }

    template < geode::index_t dimension >
    void check_edge_id( const geode::SolidEdges< dimension >& solid,
        const geode::index_t edge_id )
    {
        geode_unused( solid );
        geode_unused( edge_id );
        OPENGEODE_ASSERT( edge_id < solid.nb_edges(),
            "[check_edge_id] Trying to access an invalid edge" );
    }

    template < geode::index_t dimension >
    void check_facet_id( const geode::SolidEdges< dimension >& solid,
        const geode::index_t facet_id )
    {
        geode_unused( solid );
        geode_unused( facet_id );
        OPENGEODE_ASSERT( facet_id < solid.nb_facets(),
            "[check_facet_id] Trying to access an invalid facet" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_id( const geode::SolidEdges< dimension >& solid,
        const geode::index_t polyhedron_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        OPENGEODE_ASSERT( polyhedron_id < solid.nb_polyhedra(),
            "[check_polyhedron_id] Trying to access an invalid polyhedron" );
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
    geode::index_t get_vertex_id_in_polyhedron_facet(
        const geode::SolidEdges< dimension >& solid,
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
        const geode::SolidEdges< dimension >& solid,
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
        return next == edge_vertices[1];
    }

} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidEdges< dimension >::Impl
        : public detail::FacetEdgesImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        Impl() = default;
        Impl( const SolidMesh< dimension >& solid )
        {
            for( const auto p : Range{ solid.nb_polyhedra() } )
            {
                for( auto&& e : solid.polyhedron_edges_vertices( p ) )
                {
                    if( e[0] < e[1] )
                    {
                        this->find_or_create_edge( std::move( e ) );
                    }
                }
            }
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.ext(
                        impl, bitsery::ext::BaseClass<
                                  detail::FacetEdgesImpl< dimension > >{} );
                } );
        }
    };

    template < index_t dimension >
    SolidEdges< dimension >::SolidEdges( const SolidMesh< dimension >& solid )
        : impl_{ solid }
    {
    }

    template < index_t dimension >
    SolidEdges< dimension >::SolidEdges()
    {
    }

    template < index_t dimension >
    SolidEdges< dimension >::~SolidEdges() // NOLINT
    {
    }

    template < index_t dimension >
    bool SolidEdges< dimension >::isolated_edge( index_t edge_id ) const
    {
        check_edge_id( *this, edge_id );
        return impl_->isolated_edge( edge_id );
    }

    template < index_t dimension >
    index_t SolidEdges< dimension >::find_or_create_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        return impl_->find_or_create_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    const std::array< index_t, 2 >& SolidEdges< dimension >::edge_vertices(
        index_t edge_id ) const
    {
        check_edge_id( *this, edge_id );
        return impl_->edge_vertices( edge_id );
    }

    template < index_t dimension >
    absl::optional< index_t > SolidEdges< dimension >::edge_from_vertices(
        const std::array< index_t, 2 >& vertices ) const
    {
        return impl_->find_edge( vertices );
    }

    template < index_t dimension >
    void SolidEdges< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new, SolidEdgesKey )
    {
        impl_->update_edge_vertices( old2new );
    }

    template < index_t dimension >
    void SolidEdges< dimension >::update_edge_vertex(
        std::array< index_t, 2 > edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id,
        SolidEdgesKey )
    {
        impl_->update_edge_vertex(
            std::move( edge_vertices ), edge_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void SolidEdges< dimension >::remove_edge(
        std::array< index_t, 2 > edge_vertices, SolidEdgesKey )
    {
        impl_->remove_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    std::vector< index_t > SolidEdges< dimension >::remove_isolated_edges(
        SolidEdgesKey )
    {
        return impl_->remove_isolated_edges();
    }

    template < index_t dimension >
    std::vector< index_t > SolidEdges< dimension >::delete_edges(
        const std::vector< bool >& to_delete, SolidEdgesKey )
    {
        return impl_->delete_edges( to_delete );
    }

    template < index_t dimension >
    void SolidEdges< dimension >::overwrite_edges(
        const SolidEdges< dimension >& from, SolidEdgesKey )
    {
        impl_->overwrite_edges( *from.impl_ );
    }

    template < index_t dimension >
    index_t SolidEdges< dimension >::nb_edges() const
    {
        return edge_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    AttributeManager& SolidEdges< dimension >::edge_attribute_manager() const
    {
        return impl_->edge_attribute_manager();
    }

    template < index_t dimension >
    template < typename Archive >
    void SolidEdges< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, SolidEdges >{},
            []( Archive& archive, SolidEdges& solid ) {
                archive.object( solid.impl_ );
            } );
    }

    template class opengeode_mesh_api SolidEdges< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SolidEdges< 3 > );
} // namespace geode
