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

#include <geode/mesh/core/geode_tetrahedral_solid.h>

#include <array>
#include <fstream>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/point.h>

#include <geode/mesh/core/detail/points_impl.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeTetrahedralSolid< dimension >::Impl
        : public detail::PointsImpl< dimension >
    {
    public:
        explicit Impl( OpenGeodeTetrahedralSolid< dimension >& mesh )
            : detail::PointsImpl< dimension >( mesh ),
              tetrahedron_vertices_(
                  mesh.polyhedron_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          std::array< index_t, 4 > >( "tetrahedron_vertices",
                          [] {
                              std::array< index_t, 4 > tetrahedron{};
                              tetrahedron.fill( NO_ID );
                              return tetrahedron;
                          }() ) ),
              tetrahedron_adjacents_(
                  mesh.polyhedron_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          std::array< index_t, 4 > >(
                          "tetrahedron_adjacents", [] {
                              std::array< index_t, 4 > tetrahedron{};
                              tetrahedron.fill( NO_ID );
                              return tetrahedron;
                          }() ) )
        {
        }

        index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const
        {
            return tetrahedron_vertices_
                ->value( polyhedron_vertex.polyhedron_id )
                .at( polyhedron_vertex.vertex_id );
        }

        index_t get_polyhedron_facet_vertex(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
        {
            static const std::array< std::array< index_t, 3 >, 4 >
                tetrahedron_facet_vertices{ { { 1, 3, 2 }, { 0, 2, 3 },
                    { 3, 1, 0 }, { 0, 1, 2 } } };
            auto vertex_id = tetrahedron_facet_vertices.at(
                polyhedron_facet_vertex.polyhedron_facet
                    .facet_id )[polyhedron_facet_vertex.vertex_id];
            return get_polyhedron_vertex(
                { polyhedron_facet_vertex.polyhedron_facet.polyhedron_id,
                    vertex_id } );
        }

        index_t get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            return tetrahedron_adjacents_
                ->value( polyhedron_facet.polyhedron_id )
                .at( polyhedron_facet.facet_id );
        }

        void set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
        {
            tetrahedron_vertices_->value( polyhedron_vertex.polyhedron_id )
                .at( polyhedron_vertex.vertex_id ) = vertex_id;
        }

        void set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
        {
            tetrahedron_adjacents_->value( polyhedron_facet.polyhedron_id )
                .at( polyhedron_facet.facet_id ) = adjacent_id;
        }

        void add_tetrahedron( const TetrahedralSolid< dimension >& surface,
            const std::array< index_t, 4 >& vertices )
        {
            tetrahedron_vertices_->value( surface.nb_polyhedra() - 1 ) =
                vertices;
        }

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
        archive.ext( *this, geode::Growable< Archive, Impl >{},
            []( Archive &archive, Impl &impl ) {
            archive.ext( impl,
                bitsery::ext::BaseClass< detail::PointsImpl< dimension > >{} );
            archive.ext( impl.tetrahedron_vertices_, bitsery::ext::StdSmartPtr{} );
            archive.ext( impl.tetrahedron_adjacents_, bitsery::ext::StdSmartPtr{} );
            });
        }

    private:
        std::shared_ptr< VariableAttribute< std::array< index_t, 4 > > >
            tetrahedron_vertices_;
        std::shared_ptr< VariableAttribute< std::array< index_t, 4 > > >
            tetrahedron_adjacents_;
    };

    template < index_t dimension >
    OpenGeodeTetrahedralSolid< dimension >::OpenGeodeTetrahedralSolid()
        : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodeTetrahedralSolid<
        dimension >::~OpenGeodeTetrahedralSolid() // NOLINT
    {
    }

    template < index_t dimension >
    const Point< dimension >& OpenGeodeTetrahedralSolid< dimension >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::set_vertex(
        index_t vertex_id, const Point< dimension >& point )
    {
        impl_->set_point( vertex_id, point );
    }

    template < index_t dimension >
    index_t OpenGeodeTetrahedralSolid< dimension >::get_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        return impl_->get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    index_t OpenGeodeTetrahedralSolid< dimension >::get_polyhedron_facet_vertex(
        const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return impl_->get_polyhedron_facet_vertex( polyhedron_facet_vertex );
    }

    template < index_t dimension >
    index_t OpenGeodeTetrahedralSolid< dimension >::get_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodeTetrahedralSolid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, geode::Growable< Archive, OpenGeodeTetrahedralSolid >{},
            []( Archive &archive, OpenGeodeTetrahedralSolid &solid ) {
        archive.ext(
            solid, bitsery::ext::BaseClass< TetrahedralSolid< dimension > >{} );
        archive.object( solid.impl_ );
            });
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        impl_->set_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::add_tetrahedron(
        const std::array< index_t, 4 >& vertices )
    {
        impl_->add_tetrahedron( *this, vertices );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
    {
        impl_->set_polyhedron_adjacent( polyhedron_facet, adjacent_id );
    }

    template class opengeode_mesh_api OpenGeodeTetrahedralSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodeTetrahedralSolid< 3 > );
} // namespace geode
