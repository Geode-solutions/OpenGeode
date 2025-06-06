/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/mesh/core/surface_edges.hpp>

#include <algorithm>
#include <stack>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/detail/mapping_after_deletion.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/internal/facet_edges_impl.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>

namespace
{
    template < geode::index_t dimension >
    void check_edge_id( const geode::SurfaceEdges< dimension >& edges,
        const geode::index_t edge_id )
    {
        geode_unused( edges );
        geode_unused( edge_id );
        OPENGEODE_ASSERT( edge_id < edges.nb_edges(),
            "[check_edge_id] Trying to access an invalid edge" );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SurfaceEdges< dimension >::Impl
        : public internal::FacetEdgesImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        Impl() = default;
        Impl( const SurfaceMesh< dimension >& surface )
        {
            for( const auto p : Range{ surface.nb_polygons() } )
            {
                for( const auto e : LRange{ surface.nb_polygon_edges( p ) } )
                {
                    this->find_or_create_edge(
                        surface.polygon_edge_vertices( { p, e } ) );
                }
            }
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext(
                        impl, bitsery::ext::BaseClass<
                                  internal::FacetEdgesImpl< dimension > >{} );
                } } } );
        }
    };

    template < index_t dimension >
    SurfaceEdges< dimension >::SurfaceEdges() = default;

    template < index_t dimension >
    SurfaceEdges< dimension >::SurfaceEdges(
        const SurfaceMesh< dimension >& surface )
        : impl_{ surface }
    {
    }

    template < index_t dimension >
    SurfaceEdges< dimension >::~SurfaceEdges() = default;

    template < index_t dimension >
    index_t SurfaceEdges< dimension >::find_or_create_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        return impl_->find_or_create_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    const std::array< index_t, 2 >& SurfaceEdges< dimension >::edge_vertices(
        index_t edge_id ) const
    {
        check_edge_id( *this, edge_id );
        return impl_->edge_vertices( edge_id );
    }

    template < index_t dimension >
    void SurfaceEdges< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new, SurfaceEdgesKey )
    {
        impl_->update_edge_vertices( old2new );
    }

    template < index_t dimension >
    void SurfaceEdges< dimension >::update_edge_vertex(
        std::array< index_t, 2 > edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id,
        SurfaceEdgesKey )
    {
        impl_->update_edge_vertex(
            std::move( edge_vertices ), edge_vertex_id, new_vertex_id );
    }

    template < index_t dimension >
    void SurfaceEdges< dimension >::remove_edge(
        std::array< index_t, 2 > edge_vertices, SurfaceEdgesKey )
    {
        impl_->remove_edge( std::move( edge_vertices ) );
    }

    template < index_t dimension >
    std::vector< index_t > SurfaceEdges< dimension >::delete_edges(
        const std::vector< bool >& to_delete, SurfaceEdgesKey )
    {
        return impl_->delete_edges( to_delete );
    }

    template < index_t dimension >
    std::vector< index_t > SurfaceEdges< dimension >::remove_isolated_edges(
        SurfaceEdgesKey )
    {
        return impl_->remove_isolated_edges();
    }

    template < index_t dimension >
    void SurfaceEdges< dimension >::overwrite_edges(
        const SurfaceEdges< dimension >& from, SurfaceEdgesKey )
    {
        impl_->overwrite_edges( *from.impl_ );
    }

    template < index_t dimension >
    index_t SurfaceEdges< dimension >::nb_edges() const
    {
        return edge_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    bool SurfaceEdges< dimension >::is_edge_isolated( index_t edge_id ) const
    {
        check_edge_id( *this, edge_id );
        return impl_->is_edge_isolated( edge_id );
    }

    template < index_t dimension >
    std::optional< index_t > SurfaceEdges< dimension >::edge_from_vertices(
        const std::array< index_t, 2 >& vertices ) const
    {
        return impl_->find_edge( vertices );
    }

    template < index_t dimension >
    bool SurfaceEdges< dimension >::is_opposite(
        index_t edge_id, const std::array< index_t, 2 >& vertices ) const
    {
        check_edge_id( *this, edge_id );
        return impl_->is_opposite( edge_id, vertices );
    }

    template < index_t dimension >
    AttributeManager& SurfaceEdges< dimension >::edge_attribute_manager() const
    {
        return impl_->edge_attribute_manager();
    }

    template < index_t dimension >
    template < typename Archive >
    void SurfaceEdges< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, Growable< Archive, SurfaceEdges >{
                                { []( Archive& a, SurfaceEdges& edges ) {
                                    a.object( edges.impl_ );
                                } } } );
    }

    template class opengeode_mesh_api SurfaceEdges< 2 >;
    template class opengeode_mesh_api SurfaceEdges< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SurfaceEdges< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SurfaceEdges< 3 > );
} // namespace geode
