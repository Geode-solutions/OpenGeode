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

#include <geode/mesh/core/geode/geode_tetrahedral_solid.hpp>

#include <array>
#include <fstream>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/permutation.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/detail/geode_elements.hpp>
#include <geode/mesh/core/detail/legacy_attributes.hpp>
#include <geode/mesh/core/internal/points_impl.hpp>
#include <geode/mesh/helpers/detail/initialize_crs.hpp>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeTetrahedralSolid< dimension >::Impl
    {
        friend class bitsery::Access;
        using Tetrahedron = std::array< index_t, 4 >;
        static constexpr Tetrahedron NO_TETRAHEDRON{ NO_ID, NO_ID, NO_ID,
            NO_ID };

    public:
        explicit Impl( OpenGeodeTetrahedralSolid< dimension >& mesh )
        {
            detail::template initialize_crs<
                OpenGeodeTetrahedralSolid< dimension > >( mesh );
        }

        Impl() = default;

        index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const
        {
            return tetrahedron_vertices_[polyhedron_vertex.polyhedron_id]
                                        [polyhedron_vertex.vertex_id];
        }

        PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
        {
            const auto vertex_id = detail::TETRAHEDRON_FACET_VERTICES
                [polyhedron_facet_vertex.polyhedron_facet.facet_id]
                [polyhedron_facet_vertex.vertex_id];
            return { polyhedron_facet_vertex.polyhedron_facet.polyhedron_id,
                vertex_id };
        }

        std::optional< index_t > get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            const auto adj =
                tetrahedron_adjacents_[polyhedron_facet.polyhedron_id]
                                      [polyhedron_facet.facet_id];
            if( adj == NO_ID )
            {
                return std::nullopt;
            }
            return adj;
        }

        void set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, const index_t vertex_id )
        {
            tetrahedron_vertices_[polyhedron_vertex.polyhedron_id]
                                 [polyhedron_vertex.vertex_id] = vertex_id;
        }

        void set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, const index_t adjacent_id )
        {
            tetrahedron_adjacents_[polyhedron_facet.polyhedron_id]
                                  [polyhedron_facet.facet_id] = adjacent_id;
        }

        void add_tetrahedron( const Tetrahedron& vertices )
        {
            tetrahedron_vertices_.push_back( vertices );
            tetrahedron_adjacents_.push_back( NO_TETRAHEDRON );
        }

        void add_tetrahedra( const index_t nb )
        {
            const auto new_size = tetrahedron_vertices_.size() + nb;
            tetrahedron_vertices_.resize( new_size, NO_TETRAHEDRON );
            tetrahedron_adjacents_.resize( new_size, NO_TETRAHEDRON );
        }

        void remove_polyhedra( const std::vector< bool >& to_delete )
        {
            delete_vector_elements( to_delete, tetrahedron_vertices_ );
            delete_vector_elements( to_delete, tetrahedron_adjacents_ );
        }

        void permute_polyhedra( absl::Span< const index_t > permutation )
        {
            permute( tetrahedron_vertices_, permutation );
            permute( tetrahedron_adjacents_, permutation );
        }

        void copy_polyhedra(
            const OpenGeodeTetrahedralSolid< dimension >::Impl& impl )
        {
            tetrahedron_vertices_ = impl.tetrahedron_vertices_;
            tetrahedron_adjacents_ = impl.tetrahedron_adjacents_;
        }

        void remove_legacy_attributes( AttributeManager& manager )
        {
            legacy_attributes_.remove_from( manager );
        }

    private:
        template < typename Archive >
        void deserialize_legacy_storage( Archive& archive )
        {
            internal::PointsImpl< dimension > temp;
            archive.object( temp );
            legacy_attributes_.deserialize( archive, tetrahedron_vertices_ );
            legacy_attributes_.deserialize( archive, tetrahedron_adjacents_ );
        }

        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                         impl.deserialize_legacy_storage( archive );
                     },
                        []( Archive& archive, Impl& impl ) {
                            impl.deserialize_legacy_storage( archive );
                        },
                        []( Archive& archive, Impl& impl ) {
                            const auto serialize_tetrahedron =
                                []( Archive& archive2,
                                    Tetrahedron& tetrahedron ) {
                                    archive2.container4b( tetrahedron );
                                };
                            archive.container( impl.tetrahedron_vertices_,
                                impl.tetrahedron_vertices_.max_size(),
                                serialize_tetrahedron );
                            archive.container( impl.tetrahedron_adjacents_,
                                impl.tetrahedron_adjacents_.max_size(),
                                serialize_tetrahedron );
                        } } } );
        }

    private:
        std::vector< Tetrahedron > tetrahedron_vertices_;
        std::vector< Tetrahedron > tetrahedron_adjacents_;
        detail::LegacyAttributes legacy_attributes_;
    };

    template < index_t dimension >
    OpenGeodeTetrahedralSolid< dimension >::OpenGeodeTetrahedralSolid()
        : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodeTetrahedralSolid< dimension >::OpenGeodeTetrahedralSolid(
        BITSERY bitsery )
        : TetrahedralSolid< dimension >{ bitsery }
    {
    }

    template < index_t dimension >
    OpenGeodeTetrahedralSolid< dimension >::OpenGeodeTetrahedralSolid(
        OpenGeodeTetrahedralSolid&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeTetrahedralSolid< dimension >&
        OpenGeodeTetrahedralSolid< dimension >::operator=(
            OpenGeodeTetrahedralSolid&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeTetrahedralSolid< dimension >::~OpenGeodeTetrahedralSolid() =
        default;

    template < index_t dimension >
    index_t OpenGeodeTetrahedralSolid< dimension >::get_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        return impl_->get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    PolyhedronVertex
        OpenGeodeTetrahedralSolid< dimension >::get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return impl_->get_polyhedron_facet_vertex_id( polyhedron_facet_vertex );
    }

    template < index_t dimension >
    std::optional< index_t >
        OpenGeodeTetrahedralSolid< dimension >::get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodeTetrahedralSolid< dimension >::serialize(
        Archive& serializer )
    {
        serializer.ext( *this,
            Growable< Archive, OpenGeodeTetrahedralSolid >{
                { []( Archive& archive, OpenGeodeTetrahedralSolid& solid ) {
                     archive.ext(
                         solid, bitsery::ext::BaseClass<
                                    TetrahedralSolid< dimension > >{} );
                     archive.object( solid.impl_ );
                     solid.impl_->remove_legacy_attributes(
                         solid.polyhedron_attribute_manager() );
                     const auto new_point_attribute_id =
                         solid.vertex_attribute_manager()
                             .attribute_ids_matching_name( internal::PointsImpl<
                                 dimension >::POINTS_NAME )
                             .value()
                             .at( 0 );
                     detail::template initialize_crs<
                         OpenGeodeTetrahedralSolid< dimension > >(
                         solid, new_point_attribute_id );
                 },
                    []( Archive& archive, OpenGeodeTetrahedralSolid& solid ) {
                        archive.ext(
                            solid, bitsery::ext::BaseClass<
                                       TetrahedralSolid< dimension > >{} );
                        archive.object( solid.impl_ );
                        solid.impl_->remove_legacy_attributes(
                            solid.polyhedron_attribute_manager() );
                        const auto new_point_attribute_id =
                            solid.vertex_attribute_manager()
                                .attribute_ids_matching_name(
                                    internal::PointsImpl<
                                        dimension >::POINTS_NAME )
                                .value()
                                .at( 0 );
                        detail::template initialize_crs<
                            OpenGeodeTetrahedralSolid< dimension > >(
                            solid, new_point_attribute_id );
                    },
                    []( Archive& archive, OpenGeodeTetrahedralSolid& solid ) {
                        archive.ext(
                            solid, bitsery::ext::BaseClass<
                                       TetrahedralSolid< dimension > >{} );
                        archive.object( solid.impl_ );
                        solid.impl_->remove_legacy_attributes(
                            solid.polyhedron_attribute_manager() );
                    } } } );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex,
        index_t vertex_id,
        OGTetrahedralSolidKey /*key*/ )
    {
        impl_->set_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::add_tetrahedron(
        const std::array< index_t, 4 >& vertices,
        OGTetrahedralSolidKey /*key*/ )
    {
        impl_->add_tetrahedron( vertices );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet,
        index_t adjacent_id,
        OGTetrahedralSolidKey /*key*/ )
    {
        impl_->set_polyhedron_adjacent( polyhedron_facet, adjacent_id );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::add_tetrahedra(
        index_t nb, OGTetrahedralSolidKey /*key*/ )
    {
        impl_->add_tetrahedra( nb );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::remove_polyhedra(
        const std::vector< bool >& to_delete, OGTetrahedralSolidKey /*key*/ )
    {
        impl_->remove_polyhedra( to_delete );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::permute_polyhedra(
        absl::Span< const index_t > permutation, OGTetrahedralSolidKey /*key*/ )
    {
        impl_->permute_polyhedra( permutation );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::copy_polyhedra(
        const OpenGeodeTetrahedralSolid< dimension >& solid_mesh,
        OGTetrahedralSolidKey /*key*/ )
    {
        impl_->copy_polyhedra( *solid_mesh.impl_ );
    }

    template class opengeode_mesh_api OpenGeodeTetrahedralSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodeTetrahedralSolid< 3 > );
} // namespace geode
