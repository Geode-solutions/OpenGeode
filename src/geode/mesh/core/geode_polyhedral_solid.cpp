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

#include <geode/mesh/core/geode_polyhedral_solid.h>

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
    class OpenGeodePolyhedralSolid< dimension >::Impl
        : public detail::PointsImpl< dimension >
    {
    public:
        explicit Impl( OpenGeodePolyhedralSolid< dimension >& mesh )
            : detail::PointsImpl< dimension >( mesh )
        {
        }

        index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const
        {
            return polyhedron_vertices_[starting_vertex_index(
                                            polyhedron_vertex.polyhedron_id )
                                        + polyhedron_vertex.vertex_id];
        }

        index_t get_nb_polyhedron_vertices( index_t polyhedron_id ) const
        {
            return starting_vertex_index( polyhedron_id + 1 )
                   - starting_vertex_index( polyhedron_id );
        }

        index_t get_nb_polyhedron_facets( index_t polyhedron_id ) const
        {
            return starting_adjacent_index( polyhedron_id + 1 )
                   - starting_adjacent_index( polyhedron_id );
        }

        index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            auto facet_id = get_facet_id( polyhedron_facet );
            return starting_facet_index( facet_id + 1 )
                   - starting_facet_index( facet_id );
        }

        index_t get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            return polyhedron_adjacents_[starting_adjacent_index(
                                             polyhedron_facet.polyhedron_id )
                                         + polyhedron_facet.facet_id];
        }

        void set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
        {
            polyhedron_vertices_[starting_vertex_index(
                                     polyhedron_vertex.polyhedron_id )
                                 + polyhedron_vertex.vertex_id] = vertex_id;
        }

        void set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
        {
            polyhedron_adjacents_[starting_adjacent_index(
                                      polyhedron_facet.polyhedron_id )
                                  + polyhedron_facet.facet_id] = adjacent_id;
        }

        index_t get_polyhedron_facet_vertex(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
        {
            return get_polyhedron_vertex(
                get_polyhedron_facet_vertex_id( polyhedron_facet_vertex ) );
        }

        void add_polyhedron( const std::vector< index_t >& vertices,
            const std::vector< std::vector< index_t > >& facets )
        {
            polyhedron_vertices_.insert(
                polyhedron_vertices_.end(), vertices.begin(), vertices.end() );
            polyhedron_vertex_ptr_.push_back(
                polyhedron_vertex_ptr_.back() + vertices.size() );

            for( const auto& facet : facets )
            {
                polyhedron_facets_.insert(
                    polyhedron_facets_.end(), facet.begin(), facet.end() );
                polyhedron_facet_ptr_.push_back(
                    polyhedron_facet_ptr_.back() + facet.size() );
            }

            polyhedron_adjacent_ptr_.push_back(
                polyhedron_adjacent_ptr_.back() + facets.size() );
            polyhedron_adjacents_.resize(
                polyhedron_adjacent_ptr_.back(), NO_ID );
        }

        void remove_polyhedra( const std::vector< bool >& to_delete )
        {
            index_t offset{ 0 };
            index_t vertex_index{ 0 };
            index_t facet_index{ 0 };
            index_t adjacent_index{ 0 };
            for( auto p : Range{ to_delete.size() } )
            {
                if( to_delete[p] )
                {
                    offset++;
                }
                else
                {
                    auto nb_vertices = get_nb_polyhedron_vertices( p );
                    for( auto v : Range{ nb_vertices } )
                    {
                        polyhedron_vertices_[vertex_index] =
                            get_polyhedron_vertex( { p, v } );
                        vertex_index++;
                    }
                    polyhedron_vertex_ptr_[p - offset + 1] =
                        polyhedron_vertex_ptr_[p - offset] + nb_vertices;

                    auto nb_facets = get_nb_polyhedron_facets( p );
                    for( auto f : Range{ nb_facets } )
                    {
                        PolyhedronFacet facet{ p, f };
                        auto nb_facet_vertices =
                            get_nb_polyhedron_facet_vertices( facet );
                        for( auto v : Range{ nb_facet_vertices } )
                        {
                            polyhedron_facets_[facet_index] =
                                get_polyhedron_facet_vertex_id( { facet, v } )
                                    .vertex_id;
                            facet_index++;
                        }
                        polyhedron_facet_ptr_[adjacent_index + 1] =
                            polyhedron_facet_ptr_[adjacent_index]
                            + nb_facet_vertices;

                        polyhedron_adjacents_[adjacent_index] =
                            get_polyhedron_adjacent( facet );
                        adjacent_index++;
                    }
                    polyhedron_adjacent_ptr_[p - offset + 1] =
                        polyhedron_adjacent_ptr_[p - offset] + nb_facets;
                }
            }
            polyhedron_vertex_ptr_.erase( polyhedron_vertex_ptr_.end() - offset,
                polyhedron_vertex_ptr_.end() );
            polyhedron_vertices_.erase(
                polyhedron_vertices_.begin() + vertex_index,
                polyhedron_vertices_.end() );

            polyhedron_adjacent_ptr_.erase(
                polyhedron_adjacent_ptr_.end() - offset,
                polyhedron_adjacent_ptr_.end() );
            polyhedron_adjacents_.erase(
                polyhedron_adjacents_.begin() + adjacent_index,
                polyhedron_adjacents_.end() );

            polyhedron_facet_ptr_.erase(
                polyhedron_facet_ptr_.begin() + adjacent_index + 1,
                polyhedron_facet_ptr_.end() );
            polyhedron_facets_.erase( polyhedron_facets_.begin() + facet_index,
                polyhedron_facets_.end() );
        }

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, geode::Growable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.container4b( impl.polyhedron_vertices_,
                        impl.polyhedron_vertices_.max_size() );
                    archive.container4b( impl.polyhedron_vertex_ptr_,
                        impl.polyhedron_vertex_ptr_.max_size() );
                    archive.container4b( impl.polyhedron_facets_,
                        impl.polyhedron_facets_.max_size() );
                    archive.container4b( impl.polyhedron_facet_ptr_,
                        impl.polyhedron_facet_ptr_.max_size() );
                    archive.container4b( impl.polyhedron_adjacents_,
                        impl.polyhedron_adjacents_.max_size() );
                    archive.container4b( impl.polyhedron_adjacent_ptr_,
                        impl.polyhedron_adjacent_ptr_.max_size() );
                    archive.ext(
                        impl, bitsery::ext::BaseClass<
                                  detail::PointsImpl< dimension > >{} );
                } );
        }

        index_t starting_vertex_index( index_t polyhedron ) const
        {
            return polyhedron_vertex_ptr_[polyhedron];
        }

        index_t starting_facet_index( index_t polyhedron ) const
        {
            return polyhedron_facet_ptr_[polyhedron];
        }

        index_t starting_adjacent_index( index_t polyhedron ) const
        {
            return polyhedron_adjacent_ptr_[polyhedron];
        }

        index_t get_facet_id( const PolyhedronFacet& polyhedron_facet ) const
        {
            return starting_adjacent_index( polyhedron_facet.polyhedron_id )
                   + polyhedron_facet.facet_id;
        }

        PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
        {
            auto facet_id =
                get_facet_id( polyhedron_facet_vertex.polyhedron_facet );
            auto polyhedron_id =
                polyhedron_facet_vertex.polyhedron_facet.polyhedron_id;
            auto vertex_id =
                polyhedron_facets_[starting_facet_index( facet_id )
                                   + polyhedron_facet_vertex.vertex_id];
            return { polyhedron_id, vertex_id };
        }

    private:
        std::vector< index_t > polyhedron_vertices_;
        std::vector< index_t > polyhedron_vertex_ptr_{ 0 };

        std::vector< index_t > polyhedron_facets_;
        std::vector< index_t > polyhedron_facet_ptr_{ 0 };

        std::vector< index_t > polyhedron_adjacents_;
        std::vector< index_t > polyhedron_adjacent_ptr_{ 0 };
    };

    template < index_t dimension >
    OpenGeodePolyhedralSolid< dimension >::OpenGeodePolyhedralSolid()
        : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodePolyhedralSolid< dimension >::~OpenGeodePolyhedralSolid() // NOLINT
    {
    }

    template < index_t dimension >
    const Point< dimension >& OpenGeodePolyhedralSolid< dimension >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolid< dimension >::set_vertex(
        index_t vertex_id, const Point< dimension >& point )
    {
        impl_->set_point( vertex_id, point );
    }

    template < index_t dimension >
    index_t OpenGeodePolyhedralSolid< dimension >::get_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        return impl_->get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    index_t OpenGeodePolyhedralSolid< dimension >::get_nb_polyhedron_vertices(
        index_t polyhedron_id ) const
    {
        return impl_->get_nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    index_t OpenGeodePolyhedralSolid< dimension >::get_nb_polyhedron_facets(
        index_t polyhedron_id ) const
    {
        return impl_->get_nb_polyhedron_facets( polyhedron_id );
    }

    template < index_t dimension >
    index_t
        OpenGeodePolyhedralSolid< dimension >::get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_nb_polyhedron_facet_vertices( polyhedron_facet );
    }

    template < index_t dimension >
    index_t OpenGeodePolyhedralSolid< dimension >::get_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    index_t OpenGeodePolyhedralSolid< dimension >::get_polyhedron_facet_vertex(
        const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return impl_->get_polyhedron_facet_vertex( polyhedron_facet_vertex );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolid< dimension >::set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        impl_->set_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolid< dimension >::set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
    {
        impl_->set_polyhedron_adjacent( polyhedron_facet, adjacent_id );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolid< dimension >::remove_polyhedra(
        const std::vector< bool >& to_delete )
    {
        impl_->remove_polyhedra( to_delete );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolid< dimension >::add_polyhedron(
        const std::vector< index_t >& vertices,
        const std::vector< std::vector< index_t > >& facets )
    {
        impl_->add_polyhedron( vertices, facets );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodePolyhedralSolid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            geode::Growable< Archive, OpenGeodePolyhedralSolid >{},
            []( Archive& archive, OpenGeodePolyhedralSolid& solid ) {
                archive.ext( solid,
                    bitsery::ext::BaseClass< PolyhedralSolid< dimension > >{} );
                archive.object( solid.impl_ );
            } );
    }

    template class opengeode_mesh_api OpenGeodePolyhedralSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodePolyhedralSolid< 3 > );
} // namespace geode
