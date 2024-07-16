/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/detail/geode_elements.hpp>
#include <geode/mesh/core/internal/points_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeTetrahedralSolid< dimension >::Impl
        : public internal::PointsImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        explicit Impl( OpenGeodeTetrahedralSolid< dimension >& mesh )
            : internal::PointsImpl< dimension >( mesh ),
              tetrahedron_vertices_(
                  mesh.polyhedron_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          std::array< index_t, 4 > >( "tetrahedron_vertices",
                          std::array< index_t, 4 >{
                              NO_ID, NO_ID, NO_ID, NO_ID },
                          { false, false } ) ),
              tetrahedron_adjacents_(
                  mesh.polyhedron_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          std::array< index_t, 4 > >( "tetrahedron_adjacents",
                          std::array< index_t, 4 >{
                              NO_ID, NO_ID, NO_ID, NO_ID },
                          { false, false } ) )
        {
        }

        index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const
        {
            return tetrahedron_vertices_->value(
                polyhedron_vertex.polyhedron_id )[polyhedron_vertex.vertex_id];
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
            const auto adj = tetrahedron_adjacents_->value(
                polyhedron_facet.polyhedron_id )[polyhedron_facet.facet_id];
            if( adj == NO_ID )
            {
                return std::nullopt;
            }
            return adj;
        }

        void set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, const index_t vertex_id )
        {
            tetrahedron_vertices_->modify_value(
                polyhedron_vertex.polyhedron_id,
                [&polyhedron_vertex, vertex_id](
                    std::array< index_t, 4 >& array ) {
                    array[polyhedron_vertex.vertex_id] = vertex_id;
                } );
        }

        void set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, const index_t adjacent_id )
        {
            tetrahedron_adjacents_->modify_value(
                polyhedron_facet.polyhedron_id,
                [&polyhedron_facet, adjacent_id](
                    std::array< index_t, 4 >& array ) {
                    array[polyhedron_facet.facet_id] = adjacent_id;
                } );
        }

        void add_tetrahedron( const TetrahedralSolid< dimension >& solid,
            const std::array< index_t, 4 >& vertices )
        {
            tetrahedron_vertices_->set_value(
                solid.nb_polyhedra() - 1, vertices );
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl, bitsery::ext::BaseClass<
                                     internal::PointsImpl< dimension > >{} );
                    a.ext( impl.tetrahedron_vertices_,
                        bitsery::ext::StdSmartPtr{} );
                    a.ext( impl.tetrahedron_adjacents_,
                        bitsery::ext::StdSmartPtr{} );
                } } } );
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
    void OpenGeodeTetrahedralSolid< dimension >::set_vertex(
        index_t vertex_id, Point< dimension > point, OGTetrahedralSolidKey )
    {
        impl_->set_point( vertex_id, std::move( point ) );
    }

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
    void OpenGeodeTetrahedralSolid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, OpenGeodeTetrahedralSolid >{
                { []( Archive& a, OpenGeodeTetrahedralSolid& solid ) {
                     a.ext( solid, bitsery::ext::BaseClass<
                                       TetrahedralSolid< dimension > >{} );
                     a.object( solid.impl_ );
                     solid.impl_->initialize_crs( solid );
                 },
                    []( Archive& a, OpenGeodeTetrahedralSolid& solid ) {
                        a.ext( solid, bitsery::ext::BaseClass<
                                          TetrahedralSolid< dimension > >{} );
                        a.object( solid.impl_ );
                    } } } );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex,
        index_t vertex_id,
        OGTetrahedralSolidKey )
    {
        impl_->set_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::add_tetrahedron(
        const std::array< index_t, 4 >& vertices, OGTetrahedralSolidKey )
    {
        impl_->add_tetrahedron( *this, vertices );
    }

    template < index_t dimension >
    void OpenGeodeTetrahedralSolid< dimension >::set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet,
        index_t adjacent_id,
        OGTetrahedralSolidKey )
    {
        impl_->set_polyhedron_adjacent( polyhedron_facet, adjacent_id );
    }

    template class opengeode_mesh_api OpenGeodeTetrahedralSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodeTetrahedralSolid< 3 > );
} // namespace geode
