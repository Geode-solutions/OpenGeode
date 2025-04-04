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

#include <geode/mesh/core/geode/geode_hybrid_solid.hpp>

#include <array>
#include <fstream>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/detail/geode_elements.hpp>
#include <geode/mesh/core/internal/points_impl.hpp>

namespace
{
    static constexpr std::array< geode::HybridSolid3D::Type, 9 > TYPES{
        geode::HybridSolid3D::Type::UNKNOWN,
        geode::HybridSolid3D::Type::UNKNOWN,
        geode::HybridSolid3D::Type::UNKNOWN,
        geode::HybridSolid3D::Type::UNKNOWN,
        geode::HybridSolid3D::Type::TETRAHEDRON,
        geode::HybridSolid3D::Type::PYRAMID,
        geode::HybridSolid3D::Type::PRISM,
        geode::HybridSolid3D::Type::UNKNOWN,
        geode::HybridSolid3D::Type::HEXAHEDRON,
    };

    template < typename Data >
    geode::PolyhedronEdgesVertices polyhedron_edges_vertices(
        const geode::HybridSolid3D& mesh,
        geode::index_t polyhedron,
        const Data& data )
    {
        geode::PolyhedronEdgesVertices result;
        result.reserve( data.size() );
        for( const auto& edge : data )
        {
            result.emplace_back( std::array< geode::index_t, 2 >{
                mesh.polyhedron_vertex( { polyhedron, edge[0] } ),
                mesh.polyhedron_vertex( { polyhedron, edge[1] } ) } );
        }
        return result;
    }

    template < typename Data >
    geode::PolyhedronFacetsVertices polyhedron_facets_vertices(
        const geode::HybridSolid3D& mesh,
        geode::index_t polyhedron,
        const Data& data )
    {
        geode::PolyhedronFacetsVertices result;
        result.reserve( data.size() );
        for( const auto& facet : data )
        {
            geode::PolyhedronFacetVertices facet_vertices;
            facet_vertices.reserve( facet.size() );
            for( const auto v : facet )
            {
                facet_vertices.push_back(
                    mesh.polyhedron_vertex( { polyhedron, v } ) );
            }
            result.emplace_back( std::move( facet_vertices ) );
        }
        return result;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class OpenGeodeHybridSolid< dimension >::Impl
        : public internal::PointsImpl< dimension >
    {
        friend class bitsery::Access;
        using Type = HybridSolid3D::Type;

    public:
        explicit Impl( OpenGeodeHybridSolid< dimension >& mesh )
            : internal::PointsImpl< dimension >( mesh )
        {
            polyhedron_vertex_ptr_.emplace_back( 0 );
            polyhedron_adjacent_ptr_.emplace_back( 0 );
        }

        index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const
        {
            return polyhedron_vertices_[starting_vertex_index(
                                            polyhedron_vertex.polyhedron_id )
                                        + polyhedron_vertex.vertex_id];
        }

        local_index_t get_nb_polyhedron_vertices(
            const index_t polyhedron_id ) const
        {
            return starting_vertex_index( polyhedron_id + 1 )
                   - starting_vertex_index( polyhedron_id );
        }

        local_index_t get_nb_polyhedron_facets(
            const index_t polyhedron_id ) const
        {
            return starting_adjacent_index( polyhedron_id + 1 )
                   - starting_adjacent_index( polyhedron_id );
        }

        local_index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            switch( polyhedron_type( polyhedron_facet.polyhedron_id ) )
            {
                case Type::TETRAHEDRON:
                    return 3;
                case Type::HEXAHEDRON:
                    return 4;
                case Type::PRISM:
                    return detail::PRISM_FACET_VERTICES[polyhedron_facet
                                                            .facet_id]
                        .size();
                case Type::PYRAMID:
                    return detail::PYRAMID_FACET_VERTICES[polyhedron_facet
                                                              .facet_id]
                        .size();
                case Type::UNKNOWN:
                    throw OpenGeodeException(
                        "[HybridSolid] Unknown polyhedron type" );
            }
            return 0;
        }

        std::optional< index_t > get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            const auto adj = get_polyhedron_adjacent_impl( polyhedron_facet );
            if( adj == NO_ID )
            {
                return std::nullopt;
            }
            return adj;
        }

        void set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, const index_t vertex_id )
        {
            polyhedron_vertices_[starting_vertex_index(
                                     polyhedron_vertex.polyhedron_id )
                                 + polyhedron_vertex.vertex_id] = vertex_id;
        }

        void set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, const index_t adjacent_id )
        {
            polyhedron_adjacents_[starting_adjacent_index(
                                      polyhedron_facet.polyhedron_id )
                                  + polyhedron_facet.facet_id] = adjacent_id;
        }

        PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
        {
            const auto polyhedron_id =
                polyhedron_facet_vertex.polyhedron_facet.polyhedron_id;
            const auto facet_id =
                polyhedron_facet_vertex.polyhedron_facet.facet_id;
            const auto vertex_id = polyhedron_facet_vertex.vertex_id;
            switch( polyhedron_type( polyhedron_id ) )
            {
                case Type::TETRAHEDRON:
                    return {
                        polyhedron_id,
                        detail::TETRAHEDRON_FACET_VERTICES[facet_id][vertex_id]
                    };
                case Type::HEXAHEDRON:
                    return {
                        polyhedron_id,
                        detail::HEXAHEDRON_FACET_VERTICES[facet_id][vertex_id]
                    };
                case Type::PRISM:
                    return { polyhedron_id,
                        detail::PRISM_FACET_VERTICES[facet_id][vertex_id] };
                case Type::PYRAMID:
                    return { polyhedron_id,
                        detail::PYRAMID_FACET_VERTICES[facet_id][vertex_id] };
                case Type::UNKNOWN:
                    throw OpenGeodeException(
                        "[HybridSolid] Unknown polyhedron type" );
            }
            return {};
        }

        void add_tetrahedron( const std::array< index_t, 4 >& vertices )
        {
            add_vertices( vertices );
            polyhedron_adjacent_ptr_.push_back(
                polyhedron_adjacent_ptr_.back() + 4 );
            polyhedron_adjacents_.resize(
                polyhedron_adjacent_ptr_.back(), NO_ID );
        }

        void add_hexahedron( const std::array< index_t, 8 >& vertices )
        {
            add_vertices( vertices );
            polyhedron_adjacent_ptr_.push_back(
                polyhedron_adjacent_ptr_.back() + 6 );
            polyhedron_adjacents_.resize(
                polyhedron_adjacent_ptr_.back(), NO_ID );
        }

        void add_prism( const std::array< index_t, 6 >& vertices )
        {
            add_vertices( vertices );
            polyhedron_adjacent_ptr_.push_back(
                polyhedron_adjacent_ptr_.back() + 5 );
            polyhedron_adjacents_.resize(
                polyhedron_adjacent_ptr_.back(), NO_ID );
        }

        void add_pyramid( const std::array< index_t, 5 >& vertices )
        {
            add_vertices( vertices );
            polyhedron_adjacent_ptr_.push_back(
                polyhedron_adjacent_ptr_.back() + 5 );
            polyhedron_adjacents_.resize(
                polyhedron_adjacent_ptr_.back(), NO_ID );
        }

        void remove_polyhedra( const std::vector< bool >& to_delete )
        {
            index_t offset{ 0 };
            index_t vertex_index{ 0 };
            index_t adjacent_index{ 0 };
            for( const auto p : Indices{ to_delete } )
            {
                if( to_delete[p] )
                {
                    offset++;
                    continue;
                }
                const auto nb_vertices = get_nb_polyhedron_vertices( p );
                for( const auto v : LRange{ nb_vertices } )
                {
                    polyhedron_vertices_[vertex_index++] =
                        get_polyhedron_vertex( { p, v } );
                }
                polyhedron_vertex_ptr_[p - offset + 1] =
                    polyhedron_vertex_ptr_[p - offset] + nb_vertices;

                const auto nb_facets = get_nb_polyhedron_facets( p );
                for( const auto f : LRange{ nb_facets } )
                {
                    polyhedron_adjacents_[adjacent_index++] =
                        get_polyhedron_adjacent_impl( { p, f } );
                }
                polyhedron_adjacent_ptr_[p - offset + 1] =
                    polyhedron_adjacent_ptr_[p - offset] + nb_facets;
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
        }

        PolyhedronEdgesVertices polyhedron_edges_vertices(
            const HybridSolid< dimension >& solid, index_t polyhedron ) const
        {
            switch( polyhedron_type( polyhedron ) )
            {
                case Type::TETRAHEDRON:
                    return ::polyhedron_edges_vertices(
                        solid, polyhedron, detail::TETRAHEDRON_EDGE_VERTICES );
                case Type::HEXAHEDRON:
                    return ::polyhedron_edges_vertices(
                        solid, polyhedron, detail::HEXAHEDRON_EDGE_VERTICES );
                case Type::PRISM:
                    return ::polyhedron_edges_vertices(
                        solid, polyhedron, detail::PRISM_EDGE_VERTICES );
                case Type::PYRAMID:
                    return ::polyhedron_edges_vertices(
                        solid, polyhedron, detail::PYRAMID_EDGE_VERTICES );
                case Type::UNKNOWN:
                    throw OpenGeodeException(
                        "[HybridSolid] Unknown polyhedron type" );
            }
            return {};
        }

        PolyhedronFacetsVertices polyhedron_facets_vertices(
            const HybridSolid< dimension >& solid, index_t polyhedron ) const
        {
            switch( polyhedron_type( polyhedron ) )
            {
                case Type::TETRAHEDRON:
                    return ::polyhedron_facets_vertices(
                        solid, polyhedron, detail::TETRAHEDRON_FACET_VERTICES );
                case Type::HEXAHEDRON:
                    return ::polyhedron_facets_vertices(
                        solid, polyhedron, detail::HEXAHEDRON_FACET_VERTICES );
                case Type::PRISM:
                    return ::polyhedron_facets_vertices(
                        solid, polyhedron, detail::PRISM_FACET_VERTICES );
                case Type::PYRAMID:
                    return ::polyhedron_facets_vertices(
                        solid, polyhedron, detail::PYRAMID_FACET_VERTICES );
                case Type::UNKNOWN:
                    throw OpenGeodeException(
                        "[HybridSolid] Unknown polyhedron type" );
            }
            return {};
        }

        Type polyhedron_type( index_t polyhedron_id ) const
        {
            return TYPES[get_nb_polyhedron_vertices( polyhedron_id )];
        }

        void permute_polyhedra( absl::Span< const index_t > permutation )
        {
            std::vector< index_t > new_polyhedron_vertices;
            new_polyhedron_vertices.reserve( polyhedron_vertices_.size() );
            for( const auto p : Indices{ permutation } )
            {
                const auto old_p = permutation[p];
                const auto nb_vertices = get_nb_polyhedron_vertices( old_p );
                for( const auto v : LRange{ nb_vertices } )
                {
                    new_polyhedron_vertices.push_back(
                        get_polyhedron_vertex( { old_p, v } ) );
                }
            }
            polyhedron_vertices_ = std::move( new_polyhedron_vertices );

            std::vector< index_t > new_polyhedron_vertex_ptr;
            new_polyhedron_vertex_ptr.reserve( polyhedron_vertex_ptr_.size() );
            new_polyhedron_vertex_ptr.push_back( 0 );
            for( const auto p : Indices{ permutation } )
            {
                const auto old_p = permutation[p];
                const auto nb_vertices = get_nb_polyhedron_vertices( old_p );
                new_polyhedron_vertex_ptr.push_back(
                    new_polyhedron_vertex_ptr.back() + nb_vertices );
            }
            polyhedron_vertex_ptr_ = std::move( new_polyhedron_vertex_ptr );

            std::vector< index_t > new_polyhedron_adjacents;
            new_polyhedron_adjacents.reserve( polyhedron_adjacents_.size() );
            for( const auto p : Indices{ permutation } )
            {
                const auto old_p = permutation[p];
                const auto nb_facets = get_nb_polyhedron_facets( old_p );
                for( const auto f : LRange{ nb_facets } )
                {
                    new_polyhedron_adjacents.push_back(
                        get_polyhedron_adjacent_impl( { old_p, f } ) );
                }
            }
            polyhedron_adjacents_ = std::move( new_polyhedron_adjacents );

            std::vector< index_t > new_polyhedron_adjacent_ptr;
            new_polyhedron_adjacent_ptr.reserve(
                polyhedron_adjacent_ptr_.size() );
            new_polyhedron_adjacent_ptr.push_back( 0 );
            for( const auto p : Indices{ permutation } )
            {
                const auto old_p = permutation[p];
                const auto nb_facets = get_nb_polyhedron_facets( old_p );
                new_polyhedron_adjacent_ptr.push_back(
                    new_polyhedron_adjacent_ptr.back() + nb_facets );
            }
            polyhedron_adjacent_ptr_ = std::move( new_polyhedron_adjacent_ptr );
        }

        void copy_polyhedra(
            const OpenGeodeHybridSolid< dimension >::Impl& impl )
        {
            polyhedron_vertices_ = impl.polyhedron_vertices_;
            polyhedron_vertex_ptr_ = impl.polyhedron_vertex_ptr_;
            polyhedron_adjacents_ = impl.polyhedron_adjacents_;
            polyhedron_adjacent_ptr_ = impl.polyhedron_adjacent_ptr_;
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.container4b( impl.polyhedron_vertices_,
                        impl.polyhedron_vertices_.max_size() );
                    a.container4b( impl.polyhedron_vertex_ptr_,
                        impl.polyhedron_vertex_ptr_.max_size() );
                    a.container4b( impl.polyhedron_adjacents_,
                        impl.polyhedron_adjacents_.max_size() );
                    a.container4b( impl.polyhedron_adjacent_ptr_,
                        impl.polyhedron_adjacent_ptr_.max_size() );
                    a.ext( impl, bitsery::ext::BaseClass<
                                     internal::PointsImpl< dimension > >{} );
                } } } );
        }

        void add_vertices( absl::Span< const index_t > vertices )
        {
            polyhedron_vertices_.insert(
                polyhedron_vertices_.end(), vertices.begin(), vertices.end() );
            polyhedron_vertex_ptr_.push_back(
                polyhedron_vertex_ptr_.back() + vertices.size() );
        }

        index_t get_polyhedron_adjacent_impl(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            return polyhedron_adjacents_[starting_adjacent_index(
                                             polyhedron_facet.polyhedron_id )
                                         + polyhedron_facet.facet_id];
        }

        index_t starting_vertex_index( const index_t polyhedron ) const
        {
            return polyhedron_vertex_ptr_[polyhedron];
        }

        index_t starting_adjacent_index( const index_t polyhedron ) const
        {
            return polyhedron_adjacent_ptr_[polyhedron];
        }

    private:
        std::vector< index_t > polyhedron_vertices_;
        std::vector< index_t > polyhedron_vertex_ptr_;

        std::vector< index_t > polyhedron_adjacents_;
        std::vector< index_t > polyhedron_adjacent_ptr_;
    }; // namespace geode

    template < index_t dimension >
    OpenGeodeHybridSolid< dimension >::OpenGeodeHybridSolid() : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodeHybridSolid< dimension >::OpenGeodeHybridSolid(
        OpenGeodeHybridSolid&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeHybridSolid< dimension >&
        OpenGeodeHybridSolid< dimension >::operator=(
            OpenGeodeHybridSolid&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeHybridSolid< dimension >::~OpenGeodeHybridSolid() = default;

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::set_vertex(
        index_t vertex_id, Point< dimension > point, OGHybridSolidKey )
    {
        impl_->set_point( vertex_id, std::move( point ) );
    }

    template < index_t dimension >
    index_t OpenGeodeHybridSolid< dimension >::get_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        return impl_->get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    local_index_t OpenGeodeHybridSolid< dimension >::get_nb_polyhedron_vertices(
        index_t polyhedron_id ) const
    {
        return impl_->get_nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    local_index_t OpenGeodeHybridSolid< dimension >::get_nb_polyhedron_facets(
        index_t polyhedron_id ) const
    {
        return impl_->get_nb_polyhedron_facets( polyhedron_id );
    }

    template < index_t dimension >
    local_index_t
        OpenGeodeHybridSolid< dimension >::get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_nb_polyhedron_facet_vertices( polyhedron_facet );
    }

    template < index_t dimension >
    std::optional< index_t >
        OpenGeodeHybridSolid< dimension >::get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    PolyhedronVertex
        OpenGeodeHybridSolid< dimension >::get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return impl_->get_polyhedron_facet_vertex_id( polyhedron_facet_vertex );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex,
        index_t vertex_id,
        OGHybridSolidKey )
    {
        impl_->set_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet,
        index_t adjacent_id,
        OGHybridSolidKey )
    {
        impl_->set_polyhedron_adjacent( polyhedron_facet, adjacent_id );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::remove_polyhedra(
        const std::vector< bool >& to_delete, OGHybridSolidKey )
    {
        impl_->remove_polyhedra( to_delete );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::permute_polyhedra(
        absl::Span< const index_t > permutation, OGHybridSolidKey )
    {
        impl_->permute_polyhedra( permutation );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::add_tetrahedron(
        const std::array< index_t, 4 >& vertices, OGHybridSolidKey )
    {
        impl_->add_tetrahedron( vertices );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::add_hexahedron(
        const std::array< index_t, 8 >& vertices, OGHybridSolidKey )
    {
        impl_->add_hexahedron( vertices );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::add_prism(
        const std::array< index_t, 6 >& vertices, OGHybridSolidKey )
    {
        impl_->add_prism( vertices );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::add_pyramid(
        const std::array< index_t, 5 >& vertices, OGHybridSolidKey )
    {
        impl_->add_pyramid( vertices );
    }

    template < index_t dimension >
    PolyhedronEdgesVertices
        OpenGeodeHybridSolid< dimension >::polyhedron_edges_vertices(
            index_t polyhedron ) const
    {
        return impl_->polyhedron_edges_vertices( *this, polyhedron );
    }

    template < index_t dimension >
    PolyhedronFacetsVertices
        OpenGeodeHybridSolid< dimension >::polyhedron_facets_vertices(
            index_t polyhedron ) const
    {
        return impl_->polyhedron_facets_vertices( *this, polyhedron );
    }

    template < index_t dimension >
    typename HybridSolid< dimension >::Type
        OpenGeodeHybridSolid< dimension >::polyhedron_type(
            index_t polyhedron_id ) const
    {
        return impl_->polyhedron_type( polyhedron_id );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolid< dimension >::copy_polyhedra(
        const OpenGeodeHybridSolid< dimension >& solid_mesh, OGHybridSolidKey )
    {
        impl_->copy_polyhedra( *solid_mesh.impl_ );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodeHybridSolid< dimension >::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, OpenGeodeHybridSolid >{
                       { []( Archive& a, OpenGeodeHybridSolid& solid ) {
                            a.ext( solid, bitsery::ext::BaseClass<
                                              HybridSolid< dimension > >{} );
                            a.object( solid.impl_ );
                            solid.impl_->initialize_crs( solid );
                        },
                           []( Archive& a, OpenGeodeHybridSolid& solid ) {
                               a.ext( solid, bitsery::ext::BaseClass<
                                                 HybridSolid< dimension > >{} );
                               a.object( solid.impl_ );
                           } } } );
    }

    template class opengeode_mesh_api OpenGeodeHybridSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeHybridSolid< 3 > );
} // namespace geode
