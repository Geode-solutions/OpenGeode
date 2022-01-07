/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/core/solid_facets.h>

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
#include <geode/mesh/core/detail/facet_storage.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/solid_edges.h>

namespace
{
    template < geode::index_t dimension >
    void check_facet_id( const geode::SolidFacets< dimension >& solid,
        const geode::index_t facet_id )
    {
        geode_unused( solid );
        geode_unused( facet_id );
        OPENGEODE_ASSERT( facet_id < solid.nb_facets(),
            "[check_facet_id] Trying to access an invalid facet" );
    }

} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidFacets< dimension >::Impl
        : public detail::FacetStorage< PolyhedronFacetVertices >
    {
        friend class bitsery::Access;
        using Facets = detail::FacetStorage< PolyhedronFacetVertices >;

    public:
        Impl() = default;
        Impl( const SolidMesh< dimension >& solid )
        {
            for( const auto p : Range{ solid.nb_polyhedra() } )
            {
                for( auto&& f : solid.polyhedron_facets_vertices( p ) )
                {
                    this->find_or_create_facet( std::move( f ) );
                }
            }
        }

        absl::optional< index_t > find_facet(
            const PolyhedronFacetVertices& facet_vertices ) const
        {
            return Facets::find_facet( facet_vertices );
        }

        index_t find_or_create_facet( PolyhedronFacetVertices facet_vertices )
        {
            return this->add_facet( std::move( facet_vertices ) );
        }

        const PolyhedronFacetVertices& get_facet_vertices(
            const index_t facet_id ) const
        {
            return Facets::get_facet_vertices( facet_id );
        }

        void update_facet_vertex( PolyhedronFacetVertices facet_vertices,
            const index_t facet_vertex_id,
            const index_t new_vertex_id )
        {
            Facets::update_facet_vertex(
                std::move( facet_vertices ), facet_vertex_id, new_vertex_id );
        }

        std::vector< index_t > update_facet_vertices(
            absl::Span< const index_t > old2new )
        {
            return Facets::update_facet_vertices( old2new );
        }

        void remove_facet( PolyhedronFacetVertices facet_vertices )
        {
            Facets::remove_facet( std::move( facet_vertices ) );
        }

        std::vector< index_t > delete_facets(
            const std::vector< bool >& to_delete )
        {
            return Facets::delete_facets( to_delete );
        }

        std::vector< index_t > remove_isolated_facets()
        {
            return this->clean_facets();
        }

        bool isolated_facet( index_t facet_id ) const
        {
            return this->get_counter( facet_id ) == 0;
        }

        AttributeManager& facet_attribute_manager() const
        {
            return Facets::facet_attribute_manager();
        }

        void overwrite_facets(
            const detail::FacetStorage< PolyhedronFacetVertices >& from )
        {
            this->overwrite( from );
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.ext( impl, bitsery::ext::BaseClass< detail::FacetStorage<
                                     PolyhedronFacetVertices > >{} );
                } );
        }
    };

    template < index_t dimension >
    SolidFacets< dimension >::SolidFacets( const SolidMesh< dimension >& solid )
        : impl_{ solid }
    {
    }

    template < index_t dimension >
    SolidFacets< dimension >::SolidFacets()
    {
    }

    template < index_t dimension >
    SolidFacets< dimension >::~SolidFacets() // NOLINT
    {
    }

    template < index_t dimension >
    bool SolidFacets< dimension >::isolated_facet( index_t facet_id ) const
    {
        check_facet_id( *this, facet_id );
        return impl_->isolated_facet( facet_id );
    }

    template < index_t dimension >
    index_t SolidFacets< dimension >::find_or_create_facet(
        PolyhedronFacetVertices facet_vertices )
    {
        return impl_->find_or_create_facet( std::move( facet_vertices ) );
    }

    template < index_t dimension >
    const PolyhedronFacetVertices& SolidFacets< dimension >::facet_vertices(
        index_t facet_id ) const
    {
        check_facet_id( *this, facet_id );
        return impl_->get_facet_vertices( facet_id );
    }

    template < index_t dimension >
    absl::optional< index_t > SolidFacets< dimension >::facet_from_vertices(
        const PolyhedronFacetVertices& vertices ) const
    {
        return impl_->find_facet( vertices );
    }

    template < index_t dimension >
    std::vector< index_t > SolidFacets< dimension >::update_facet_vertices(
        absl::Span< const index_t > old2new, SolidFacetsKey )
    {
        return impl_->update_facet_vertices( old2new );
    }

    template < index_t dimension >
    void SolidFacets< dimension >::update_facet_vertex(
        PolyhedronFacetVertices facet_vertices,
        index_t facet_vertex_id,
        index_t new_vertex_id,
        SolidFacetsKey )
    {
        impl_->update_facet_vertex(
            std::move( facet_vertices ), facet_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void SolidFacets< dimension >::remove_facet(
        PolyhedronFacetVertices facet_vertices, SolidFacetsKey )
    {
        impl_->remove_facet( std::move( facet_vertices ) );
    }

    template < index_t dimension >
    std::vector< index_t > SolidFacets< dimension >::remove_isolated_facets(
        SolidFacetsKey )
    {
        return impl_->remove_isolated_facets();
    }

    template < index_t dimension >
    std::vector< index_t > SolidFacets< dimension >::delete_facets(
        const std::vector< bool >& to_delete, SolidFacetsKey )
    {
        return impl_->delete_facets( to_delete );
    }

    template < index_t dimension >
    void SolidFacets< dimension >::overwrite_facets(
        const SolidFacets< dimension >& from, SolidFacetsKey )
    {
        impl_->overwrite_facets( *from.impl_ );
    }

    template < index_t dimension >
    index_t SolidFacets< dimension >::nb_facets() const
    {
        return facet_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    AttributeManager& SolidFacets< dimension >::facet_attribute_manager() const
    {
        return impl_->facet_attribute_manager();
    }

    template < index_t dimension >
    template < typename Archive >
    void SolidFacets< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, SolidFacets >{},
            []( Archive& a, SolidFacets& solid ) { a.object( solid.impl_ ); } );
    }

    template class opengeode_mesh_api SolidFacets< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SolidFacets< 3 > );
} // namespace geode
