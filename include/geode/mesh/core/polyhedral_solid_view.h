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

#pragma once

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/polyhedral_solid.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolyhedralSolidViewBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    /*!
     * A mesh view of a PolyhedralSolid meaning a subset of a PolyhedralSolid
     * without copying any geometry or microtopology, it only "views" the
     * selected subeset and acts like it is a complete mesh with all the API
     * from vertices to polyhedra, attributes included.
     * @warning Modification on the PolyhedralSolid will not be reproduced on

     * the corresponding view.
     */
    template < index_t dimension >
    class PolyhedralSolidView : public PolyhedralSolid< dimension >
    {
        OPENGEODE_DISABLE_COPY( PolyhedralSolidView );
        PASSKEY(
            PolyhedralSolidViewBuilder< dimension >, PolyhedralSolidViewKey );

    public:
        PolyhedralSolidView( const PolyhedralSolid< dimension >& solid );
        PolyhedralSolidView( PolyhedralSolidView&& other );
        ~PolyhedralSolidView();

        static MeshImpl impl_name_static()
        {
            return MeshImpl{ absl::StrCat(
                "PolyhedralSolidView", dimension, "D" ) };
        }

        MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        MeshType type_name() const override
        {
            return PolyhedralSolidView< dimension >::type_name_static();
        }

        absl::string_view native_extension() const override
        {
            throw OpenGeodeException( "PolyhedralSolidView cannot be saved" );
            return "";
        }

        index_t viewed_vertex( index_t vertex_id ) const;

        index_t add_viewed_vertex( index_t vertex_id, PolyhedralSolidViewKey );

        index_t viewed_polyhedron( index_t polyhedron_id ) const;

        index_t add_viewed_polyhedron(
            index_t polyhedron_id, PolyhedralSolidViewKey );

    private:
        const Point< dimension >& get_point( index_t vertex_id ) const override;

        index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const override;

        local_index_t get_nb_polyhedron_vertices(
            index_t polyhedron_id ) const override;

        local_index_t get_nb_polyhedron_facets(
            index_t polyhedron_id ) const override;

        local_index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const override;

        PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex )
            const override;

        absl::optional< index_t > get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const override;

        absl::optional< PolyhedronVertex > get_polyhedron_around_vertex(
            index_t vertex_id ) const override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( PolyhedralSolidView );
} // namespace geode
