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

#include <geode/mesh/core/geode/geode_triangulated_surface.hpp>

#include <array>
#include <fstream>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/permutation.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/detail/legacy_attributes.hpp>
#include <geode/mesh/core/internal/points_impl.hpp>
#include <geode/mesh/helpers/detail/initialize_crs.hpp>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeTriangulatedSurface< dimension >::Impl
    {
        friend class bitsery::Access;
        using Triangle = std::array< index_t, 3 >;
        static constexpr Triangle NO_TRIANGLE{ NO_ID, NO_ID, NO_ID };

    public:
        explicit Impl( OpenGeodeTriangulatedSurface< dimension >& mesh )
        {
            detail::template initialize_crs<
                OpenGeodeTriangulatedSurface< dimension > >( mesh );
        }

        Impl() = default;

        index_t get_polygon_vertex( const PolygonVertex& polygon_vertex ) const
        {
            return triangle_vertices_[polygon_vertex.polygon_id]
                                     [polygon_vertex.vertex_id];
        }

        std::optional< index_t > get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const
        {
            const auto adj = triangle_adjacents_[polygon_edge.polygon_id]
                                                [polygon_edge.edge_id];
            if( adj == NO_ID )
            {
                return std::nullopt;
            }
            return adj;
        }

        void set_polygon_vertex(
            const PolygonVertex& polygon_vertex, const index_t vertex_id )
        {
            triangle_vertices_[polygon_vertex.polygon_id]
                              [polygon_vertex.vertex_id] = vertex_id;
        }

        void set_polygon_adjacent(
            const PolygonEdge& polygon_edge, const index_t adjacent_id )
        {
            triangle_adjacents_[polygon_edge.polygon_id][polygon_edge.edge_id] =
                adjacent_id;
        }

        void add_triangle( const Triangle& vertices )
        {
            triangle_vertices_.push_back( vertices );
            triangle_adjacents_.push_back( NO_TRIANGLE );
        }

        void add_triangles( const index_t nb )
        {
            const auto new_size = triangle_vertices_.size() + nb;
            triangle_vertices_.resize( new_size, NO_TRIANGLE );
            triangle_adjacents_.resize( new_size, NO_TRIANGLE );
        }

        void remove_polygons( const std::vector< bool >& to_delete )
        {
            delete_vector_elements( to_delete, triangle_vertices_ );
            delete_vector_elements( to_delete, triangle_adjacents_ );
        }

        void permute_polygons( absl::Span< const index_t > permutation )
        {
            permute( triangle_vertices_, permutation );
            permute( triangle_adjacents_, permutation );
        }

        void copy_polygons(
            const OpenGeodeTriangulatedSurface< dimension >::Impl& impl )
        {
            triangle_vertices_ = impl.triangle_vertices_;
            triangle_adjacents_ = impl.triangle_adjacents_;
        }

        void remove_legacy_attributes( AttributeManager& manager )
        {
            legacy_attributes_.remove_from( manager );
        }

    public:
        void check_storage( index_t nb_polygons ) const
        {
            OpenGeodeMeshException::check_exception(
                triangle_vertices_.size() == nb_polygons
                    && triangle_adjacents_.size() == nb_polygons,
                nullptr, OpenGeodeException::TYPE::data,
                "[TriangulatedSurface] Wrong triangle storage size "
                "after deserialization: file may be corrupted" );
        }

    private:
        template < typename Archive >
        void deserialize_legacy_storage( Archive& archive )
        {
            internal::PointsImpl< dimension > temp;
            archive.object( temp );
            legacy_attributes_.deserialize( archive, triangle_vertices_ );
            legacy_attributes_.deserialize( archive, triangle_adjacents_ );
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
                            const auto serialize_triangle =
                                []( Archive& archive2, Triangle& triangle ) {
                                    archive2.container4b( triangle );
                                };
                            archive.container( impl.triangle_vertices_,
                                impl.triangle_vertices_.max_size(),
                                serialize_triangle );
                            archive.container( impl.triangle_adjacents_,
                                impl.triangle_adjacents_.max_size(),
                                serialize_triangle );
                        } } } );
        }

    private:
        std::vector< Triangle > triangle_vertices_;
        std::vector< Triangle > triangle_adjacents_;
        detail::LegacyAttributes legacy_attributes_;
    };

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >::OpenGeodeTriangulatedSurface()
        : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >::OpenGeodeTriangulatedSurface(
        BITSERY bitsery )
        : TriangulatedSurface< dimension >{ bitsery }
    {
    }

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >::OpenGeodeTriangulatedSurface(
        OpenGeodeTriangulatedSurface&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >&
        OpenGeodeTriangulatedSurface< dimension >::operator=(
            OpenGeodeTriangulatedSurface&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >::~OpenGeodeTriangulatedSurface() =
        default;

    template < index_t dimension >
    index_t OpenGeodeTriangulatedSurface< dimension >::get_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        return impl_->get_polygon_vertex( polygon_vertex );
    }

    template < index_t dimension >
    std::optional< index_t >
        OpenGeodeTriangulatedSurface< dimension >::get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const
    {
        return impl_->get_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodeTriangulatedSurface< dimension >::serialize(
        Archive& serializer )
    {
        serializer.ext( *this,
            Growable< Archive, OpenGeodeTriangulatedSurface >{
                { []( Archive& archive,
                      OpenGeodeTriangulatedSurface& surface ) {
                     archive.ext(
                         surface, bitsery::ext::BaseClass<
                                      TriangulatedSurface< dimension > >{} );
                     archive.object( surface.impl_ );
                     surface.impl_->check_storage( surface.nb_polygons() );
                     surface.impl_->remove_legacy_attributes(
                         surface.polygon_attribute_manager() );
                     const auto new_point_attribute_id =
                         surface.vertex_attribute_manager()
                             .attribute_ids_matching_name( internal::PointsImpl<
                                 dimension >::POINTS_NAME )
                             .value()
                             .at( 0 );
                     detail::template initialize_crs<
                         OpenGeodeTriangulatedSurface< dimension > >(
                         surface, new_point_attribute_id );
                 },
                    []( Archive& archive,
                        OpenGeodeTriangulatedSurface& surface ) {
                        archive.ext(
                            surface, bitsery::ext::BaseClass<
                                         TriangulatedSurface< dimension > >{} );
                        archive.object( surface.impl_ );
                        surface.impl_->check_storage( surface.nb_polygons() );
                        surface.impl_->remove_legacy_attributes(
                            surface.polygon_attribute_manager() );
                        const auto new_point_attribute_id =
                            surface.vertex_attribute_manager()
                                .attribute_ids_matching_name(
                                    internal::PointsImpl<
                                        dimension >::POINTS_NAME )
                                .value()
                                .at( 0 );
                        detail::template initialize_crs<
                            OpenGeodeTriangulatedSurface< dimension > >(
                            surface, new_point_attribute_id );
                    },
                    []( Archive& archive,
                        OpenGeodeTriangulatedSurface& surface ) {
                        archive.ext(
                            surface, bitsery::ext::BaseClass<
                                         TriangulatedSurface< dimension > >{} );
                        archive.object( surface.impl_ );
                        surface.impl_->check_storage( surface.nb_polygons() );
                        surface.impl_->remove_legacy_attributes(
                            surface.polygon_attribute_manager() );
                    } } } );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::set_polygon_vertex(
        const PolygonVertex& polygon_vertex,
        index_t vertex_id,
        OGTriangulatedSurfaceKey /*key*/ )
    {
        impl_->set_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::add_triangle(
        const std::array< index_t, 3 >& vertices,
        OGTriangulatedSurfaceKey /*key*/ )
    {
        impl_->add_triangle( vertices );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::set_polygon_adjacent(
        const PolygonEdge& polygon_edge,
        index_t adjacent_id,
        OGTriangulatedSurfaceKey /*key*/ )
    {
        impl_->set_polygon_adjacent( polygon_edge, adjacent_id );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::add_triangles(
        index_t nb, OGTriangulatedSurfaceKey /*key*/ )
    {
        impl_->add_triangles( nb );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::remove_polygons(
        const std::vector< bool >& to_delete, OGTriangulatedSurfaceKey /*key*/ )
    {
        impl_->remove_polygons( to_delete );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::permute_polygons(
        absl::Span< const index_t > permutation,
        OGTriangulatedSurfaceKey /*key*/ )
    {
        impl_->permute_polygons( permutation );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::copy_polygons(
        const OpenGeodeTriangulatedSurface< dimension >& surface_mesh,
        OGTriangulatedSurfaceKey /*key*/ )
    {
        impl_->copy_polygons( *surface_mesh.impl_ );
    }

    template class opengeode_mesh_api OpenGeodeTriangulatedSurface< 2 >;
    template class opengeode_mesh_api OpenGeodeTriangulatedSurface< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodeTriangulatedSurface< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodeTriangulatedSurface< 3 > );
} // namespace geode
