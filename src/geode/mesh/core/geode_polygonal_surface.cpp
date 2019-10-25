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

#include <geode/mesh/core/geode_polygonal_surface.h>

#include <array>
#include <fstream>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/point.h>

#include <geode/mesh/core/detail/points_impl.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodePolygonalSurface< dimension >::Impl
        : public detail::PointsImpl< dimension >
    {
    public:
        explicit Impl( OpenGeodePolygonalSurface< dimension >& mesh )
            : detail::PointsImpl< dimension >( mesh )
        {
        }

        index_t get_polygon_vertex( const PolygonVertex& polygon_vertex ) const
        {
            return polygon_vertices_[starting_index( polygon_vertex.polygon_id )
                                     + polygon_vertex.vertex_id];
        }

        index_t get_nb_polygon_vertices( index_t polygon_id ) const
        {
            return starting_index( polygon_id + 1 )
                   - starting_index( polygon_id );
        }

        index_t get_polygon_adjacent( const PolygonEdge& polygon_edge ) const
        {
            return polygon_adjacents_[starting_index( polygon_edge.polygon_id )
                                      + polygon_edge.edge_id];
        }

        void set_polygon_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id )
        {
            polygon_vertices_[starting_index( polygon_vertex.polygon_id )
                              + polygon_vertex.vertex_id] = vertex_id;
        }

        void set_polygon_adjacent(
            const PolygonEdge& polygon_edge, index_t adjacent_id )
        {
            polygon_adjacents_[starting_index( polygon_edge.polygon_id )
                               + polygon_edge.edge_id] = adjacent_id;
        }

        void add_polygon( const std::vector< index_t >& vertices )
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
            for( auto p : Range{ to_delete.size() } )
            {
                if( to_delete[p] )
                {
                    offset++;
                }
                else
                {
                    auto nb_vertices = get_nb_polygon_vertices( p );
                    for( auto v : Range{ nb_vertices } )
                    {
                        polygon_vertices_[index] =
                            get_polygon_vertex( { p, v } );
                        polygon_adjacents_[index] =
                            get_polygon_adjacent( { p, v } );
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

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.container4b( impl.polygon_vertices_,
                        impl.polygon_vertices_.max_size() );
                    archive.container4b( impl.polygon_adjacents_,
                        impl.polygon_adjacents_.max_size() );
                    archive.container4b(
                        impl.polygon_ptr_, impl.polygon_ptr_.max_size() );
                    archive.ext(
                        impl, bitsery::ext::BaseClass<
                                  detail::PointsImpl< dimension > >{} );
                } );
        }

        index_t starting_index( index_t polygon ) const
        {
            return polygon_ptr_[polygon];
        }

    private:
        std::vector< index_t > polygon_vertices_;
        std::vector< index_t > polygon_adjacents_;
        std::vector< index_t > polygon_ptr_{ 0 };
    };

    template < index_t dimension >
    OpenGeodePolygonalSurface< dimension >::OpenGeodePolygonalSurface()
        : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodePolygonalSurface<
        dimension >::~OpenGeodePolygonalSurface() // NOLINT
    {
    }

    template < index_t dimension >
    const Point< dimension >& OpenGeodePolygonalSurface< dimension >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::set_vertex(
        index_t vertex_id, const Point< dimension >& point )
    {
        impl_->set_point( vertex_id, point );
    }

    template < index_t dimension >
    index_t OpenGeodePolygonalSurface< dimension >::get_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        return impl_->get_polygon_vertex( polygon_vertex );
    }

    template < index_t dimension >
    index_t OpenGeodePolygonalSurface< dimension >::get_nb_polygon_vertices(
        index_t polygon_id ) const
    {
        return impl_->get_nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    index_t OpenGeodePolygonalSurface< dimension >::get_polygon_adjacent(
        const PolygonEdge& polygon_edge ) const
    {
        return impl_->get_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodePolygonalSurface< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            DefaultGrowable< Archive, OpenGeodePolygonalSurface >{},
            []( Archive& archive, OpenGeodePolygonalSurface& surface ) {
                archive.ext( surface, bitsery::ext::BaseClass<
                                          PolygonalSurface< dimension > >{} );
                archive.object( surface.impl_ );
            } );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::set_polygon_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        impl_->set_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::add_polygon(
        const std::vector< index_t >& vertices )
    {
        impl_->add_polygon( vertices );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::remove_polygons(
        const std::vector< bool >& to_delete )
    {
        impl_->remove_polygons( to_delete );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurface< dimension >::set_polygon_adjacent(
        const PolygonEdge& polygon_edge, index_t adjacent_id )
    {
        impl_->set_polygon_adjacent( polygon_edge, adjacent_id );
    }

    template class opengeode_mesh_api OpenGeodePolygonalSurface< 2 >;
    template class opengeode_mesh_api OpenGeodePolygonalSurface< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodePolygonalSurface< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodePolygonalSurface< 3 > );
} // namespace geode
