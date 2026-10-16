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

#include <geode/mesh/core/solid_edges.hpp>

#include <absl/container/flat_hash_set.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/core/internal/facet_edges_impl.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>

namespace
{
    template < geode::index_t dimension >
    void check_edge_id( const geode::SolidEdges< dimension >& solid,
        const geode::index_t edge_id )
    {
        geode_unused( solid );
        geode_unused( edge_id );
        geode::OpenGeodeMeshException::check_assertion(
            edge_id < solid.nb_edges(),
            "[check_edge_id] Trying to access an invalid edge" );
    }

    template < geode::index_t dimension >
    geode::index_t estimate_nb_edges(
        const geode::SolidMesh< dimension >& solid )
    {
        // Euler characteristic of a ball: V - E + F - P = 1
        // Each inner facet is shared by 2 polyhedra, margin for border facets
        geode::index_t nb_facets{ 0 };
        if( solid.are_facets_enabled() )
        {
            nb_facets = solid.facets().nb_facets();
        }
        else
        {
            geode::index_t nb_polyhedron_facets{ 0 };
            for( const auto p : geode::Range{ solid.nb_polyhedra() } )
            {
                nb_polyhedron_facets += solid.nb_polyhedron_facets( p );
            }
            nb_facets =
                ( nb_polyhedron_facets + nb_polyhedron_facets / 16 ) / 2;
        }
        const auto nb_vertices_and_facets = solid.nb_vertices() + nb_facets;
        if( nb_vertices_and_facets <= solid.nb_polyhedra() )
        {
            return 0;
        }
        return nb_vertices_and_facets - solid.nb_polyhedra();
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidEdges< dimension >::Impl
        : public internal::FacetEdgesImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        Impl() : internal::FacetEdgesImpl< dimension >{ BITSERY::constructor }
        {
        }

        Impl( const SolidMesh< dimension >& solid )
        {
            this->reserve( estimate_nb_edges( solid ) );
            for( const auto p : Range{ solid.nb_polyhedra() } )
            {
                for( auto&& e : solid.polyhedron_edges_vertices( p ) )
                {
                    this->find_or_create_edge( std::move( e ) );
                }
            }
        }

    private:
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                        archive.ext( impl,
                            bitsery::ext::BaseClass<
                                internal::FacetEdgesImpl< dimension > >{} );
                    } } } );
        }
    };

    template < index_t dimension >
    SolidEdges< dimension >::SolidEdges( const SolidMesh< dimension >& solid )
        : impl_{ solid }
    {
    }

    template < index_t dimension >
    SolidEdges< dimension >::SolidEdges() = default;

    template < index_t dimension >
    SolidEdges< dimension >::~SolidEdges() = default;

    template < index_t dimension >
    bool SolidEdges< dimension >::is_edge_isolated( index_t edge_id ) const
    {
        check_edge_id( *this, edge_id );
        return impl_->is_edge_isolated( edge_id );
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
    std::optional< index_t > SolidEdges< dimension >::edge_from_vertices(
        const std::array< index_t, 2 >& vertices ) const
    {
        return impl_->find_edge( vertices );
    }

    template < index_t dimension >
    bool SolidEdges< dimension >::is_opposite(
        index_t edge_id, const std::array< index_t, 2 >& vertices ) const
    {
        check_edge_id( *this, edge_id );
        return impl_->is_opposite( edge_id, vertices );
    }

    template < index_t dimension >
    void SolidEdges< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new, SolidEdgesKey /*key*/ )
    {
        impl_->update_edge_vertices( old2new );
    }

    template < index_t dimension >
    BijectiveMapping< index_t > SolidEdges< dimension >::update_edge_vertex(
        std::array< index_t, 2 > edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id,
        SolidEdgesKey /*key*/ )
    {
        return impl_->update_edge_vertex(
            std::move( edge_vertices ), edge_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void SolidEdges< dimension >::remove_edge(
        std::array< index_t, 2 > edge_vertices, SolidEdgesKey /*key*/ )
    {
        impl_->remove_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    std::vector< index_t > SolidEdges< dimension >::remove_isolated_edges(
        SolidEdgesKey /*key*/ )
    {
        return impl_->remove_isolated_edges();
    }

    template < index_t dimension >
    std::vector< index_t > SolidEdges< dimension >::delete_edges(
        const std::vector< bool >& to_delete, SolidEdgesKey /*key*/ )
    {
        return impl_->delete_edges( to_delete );
    }

    template < index_t dimension >
    void SolidEdges< dimension >::overwrite_edges(
        const SolidEdges< dimension >& from, SolidEdgesKey /*key*/ )
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
    void SolidEdges< dimension >::serialize( Archive& serializer )
    {
        serializer.ext( *this, Growable< Archive, SolidEdges >{
                                   { []( Archive& archive, SolidEdges& solid ) {
                                       archive.object( solid.impl_ );
                                   } } } );
    }

    template class opengeode_mesh_api SolidEdges< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SolidEdges< 3 > );
} // namespace geode
