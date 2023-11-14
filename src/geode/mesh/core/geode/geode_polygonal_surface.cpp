/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/core/geode/geode_polygonal_surface.h>

#include <array>
#include <fstream>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/private/points_impl.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodePolygonalSurface< dimension >::Impl
        : public detail::PointsImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        explicit Impl( OpenGeodePolygonalSurface< dimension >& mesh )
            : detail::PointsImpl< dimension >( mesh )
        {
            polygon_ptr_.emplace_back( 0 );
        }

        index_t get_polygon_vertex( const PolygonVertex& polygon_vertex ) const
        {
            return polygon_vertices_[starting_index( polygon_vertex.polygon_id )
                                     + polygon_vertex.vertex_id];
        }

        local_index_t get_nb_polygon_vertices( const index_t polygon_id ) const
        {
            return starting_index( polygon_id + 1 )
                   - starting_index( polygon_id );
        }

        absl::optional< index_t > get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const
        {
            const auto adj = get_polygon_adjacent_impl( polygon_edge );
            if( adj == NO_ID )
            {
                return absl::nullopt;
            }
            return adj;
        }

        void set_polygon_vertex(
            const PolygonVertex& polygon_vertex, const index_t vertex_id )
        {
            polygon_vertices_[starting_index( polygon_vertex.polygon_id )
                              + polygon_vertex.vertex_id] = vertex_id;
        }

        void set_polygon_adjacent(
            const PolygonEdge& polygon_edge, const index_t adjacent_id )
        {
            polygon_adjacents_[starting_index( polygon_edge.polygon_id )
                               + polygon_edge.edge_id] = adjacent_id;
        }

        void add_polygon( absl::Span< const index_t > vertices )
        {
            polygon_vertices_.insert(
                polygon_vertices_.end(), vertices.begin(), vertices.end() );
            polygon_ptr_.push_back( polygon_ptr_.back() + vertices.size() );
            polygon_adjacents_.resize( polygon_ptr_.back(), NO_ID );
        }

        void remove_polygons( const std::vector< bool >& to_delete )
        {
            index_t offset{ 0 };
            index_t index{ 0 };
            for( const auto p : Indices{ to_delete } )
            {
                if( to_delete[p] )
                {
                    offset++;
                }
                else
                {
                    const auto nb_vertices = get_nb_polygon_vertices( p );
                    for( const auto v : LRange{ nb_vertices } )
                    {
                        polygon_vertices_[index] =
                            get_polygon_vertex( { p, v } );
                        polygon_adjacents_[index] =
                            get_polygon_adjacent_impl( { p, v } );
                        index++;
                    }
                    polygon_ptr_[p - offset + 1] =
                        polygon_ptr_[p - offset] + nb_vertices;
                }
            }
            polygon_ptr_.erase(
                polygon_ptr_.end() - offset, polygon_ptr_.end() );
            polygon_vertices_.erase(
                polygon_vertices_.begin() + index, polygon_vertices_.end() );
            polygon_adjacents_.erase(
                polygon_adjacents_.begin() + index, polygon_adjacents_.end() );
        }

        void permute_polygons( absl::Span< const index_t > permutation )
        {
            std::vector< index_t > new_polygon_vertices;
            new_polygon_vertices.reserve( polygon_vertices_.size() );
            for( const auto p : Indices{ permutation } )
            {
                const auto old_p = permutation[p];
                const auto nb_vertices = get_nb_polygon_vertices( old_p );
                for( const auto v : LRange{ nb_vertices } )
                {
                    new_polygon_vertices.push_back(
                        get_polygon_vertex( { old_p, v } ) );
                }
            }
            polygon_vertices_ = std::move( new_polygon_vertices );

            std::vector< index_t > new_polygon_adjacents;
            new_polygon_adjacents.reserve( polygon_adjacents_.size() );
            for( const auto p : Indices{ permutation } )
            {
                const auto old_p = permutation[p];
                const auto nb_vertices = get_nb_polygon_vertices( old_p );
                for( const auto v : LRange{ nb_vertices } )
                {
                    new_polygon_adjacents.push_back(
                        get_polygon_adjacent_impl( { old_p, v } ) );
                }
            }
            polygon_adjacents_ = std::move( new_polygon_adjacents );

            std::vector< index_t > new_polygon_ptr;
            new_polygon_ptr.reserve( polygon_ptr_.size() );
            new_polygon_ptr.push_back( 0 );
            for( const auto p : Indices{ permutation } )
            {
                const auto old_p = permutation[p];
                const auto nb_vertices = get_nb_polygon_vertices( old_p );
                new_polygon_ptr.push_back(
                    new_polygon_ptr.back() + nb_vertices );
            }
            polygon_ptr_ = std::move( new_polygon_ptr );
        }

        void copy_polygons(
            const OpenGeodePolygonalSurface< dimension >::Impl& impl )
        {
            polygon_vertices_ = impl.polygon_vertices_;
            polygon_adjacents_ = impl.polygon_adjacents_;
            polygon_ptr_ = impl.polygon_ptr_;
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.container4b( impl.polygon_vertices_,
                        impl.polygon_vertices_.max_size() );
                    a.container4b( impl.polygon_adjacents_,
                        impl.polygon_adjacents_.max_size() );
                    a.container4b(
                        impl.polygon_ptr_, impl.polygon_ptr_.max_size() );
                    a.ext( impl, bitsery::ext::BaseClass<
                                     detail::PointsImpl< dimension > >{} );
                } } } );
        }

        index_t get_polygon_adjacent_impl(
            const PolygonEdge& polygon_edge ) const
        {
            return polygon_adjacents_[starting_index( polygon_edge.polygon_id )
                                      + polygon_edge.edge_id];
        }

        index_t starting_index( const index_t polygon ) const
        {
            return polygon_ptr_[polygon];
        }

    private:
        std::vector< index_t > polygon_vertices_;
        std::vector< index_t > polygon_adjacents_;
        std::vector< index_t > polygon_ptr_;
    }; // namespace geode

    template < index_t dimension >
    OpenGeodePolygonalSurface< dimension >::OpenGeodePolygonalSurface()
        : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodePolygonalSurface< dimension >::OpenGeodePolygonalSurface(
        OpenGeodePolygonalSurface&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodePolygonalSurface< dimension >&
        OpenGeodePolygonalSurface< dimension >::operator=(
            OpenGeodePolygonalSurface&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodePolygonalSurface< dimension >::~OpenGeodePolygonalSurface() =
        default;

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::set_vertex(
        index_t vertex_id, Point< dimension > point, OGPolygonalSurfaceKey )
    {
        impl_->set_point( vertex_id, std::move( point ) );
    }

    template < index_t dimension >
    index_t OpenGeodePolygonalSurface< dimension >::get_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        return impl_->get_polygon_vertex( polygon_vertex );
    }

    template < index_t dimension >
    local_index_t
        OpenGeodePolygonalSurface< dimension >::get_nb_polygon_vertices(
            index_t polygon_id ) const
    {
        return impl_->get_nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    absl::optional< index_t >
        OpenGeodePolygonalSurface< dimension >::get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const
    {
        return impl_->get_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodePolygonalSurface< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, OpenGeodePolygonalSurface >{
                { []( Archive& a, OpenGeodePolygonalSurface& surface ) {
                     a.ext( surface, bitsery::ext::BaseClass<
                                         PolygonalSurface< dimension > >{} );
                     a.object( surface.impl_ );
                     surface.impl_->initialize_crs( surface );
                 },
                    []( Archive& a, OpenGeodePolygonalSurface& surface ) {
                        a.ext( surface, bitsery::ext::BaseClass<
                                            PolygonalSurface< dimension > >{} );
                        a.object( surface.impl_ );
                    } } } );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::set_polygon_vertex(
        const PolygonVertex& polygon_vertex,
        index_t vertex_id,
        OGPolygonalSurfaceKey )
    {
        impl_->set_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::add_polygon(
        absl::Span< const index_t > vertices, OGPolygonalSurfaceKey )
    {
        impl_->add_polygon( vertices );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::remove_polygons(
        const std::vector< bool >& to_delete, OGPolygonalSurfaceKey )
    {
        impl_->remove_polygons( to_delete );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::permute_polygons(
        absl::Span< const index_t > permutation, OGPolygonalSurfaceKey )
    {
        impl_->permute_polygons( permutation );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::set_polygon_adjacent(
        const PolygonEdge& polygon_edge,
        index_t adjacent_id,
        OGPolygonalSurfaceKey )
    {
        impl_->set_polygon_adjacent( polygon_edge, adjacent_id );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::copy_polygons(
        const OpenGeodePolygonalSurface< dimension >& surface_mesh,
        OGPolygonalSurfaceKey )
    {
        impl_->copy_polygons( *surface_mesh.impl_ );
    }

    template class opengeode_mesh_api OpenGeodePolygonalSurface< 2 >;
    template class opengeode_mesh_api OpenGeodePolygonalSurface< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodePolygonalSurface< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodePolygonalSurface< 3 > );
} // namespace geode
