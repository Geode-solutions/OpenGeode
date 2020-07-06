/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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
#include <geode/mesh/core/tetrahedral_solid.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolidViewBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    /*!
     * A mesh view of a TetrahedralSolid meaning a subset of a TetrahedralSolid
     * without copying any geometry or microtopology, it only "views" the
     * selected subeset and acts like it is a complete mesh with all the API
     * from vertices to tetrahedra, attributes included.
     * @warning Modification on the TetrahedralSolid will not be reproduced on

     * the corresponding view.
     */
    template < index_t dimension >
    class TetrahedralSolidView : public TetrahedralSolid< dimension >
    {
        OPENGEODE_DISABLE_COPY( TetrahedralSolidView );
        PASSKEY(
            TetrahedralSolidViewBuilder< dimension >, TetrahedralSolidViewKey );

    public:
        TetrahedralSolidView( const TetrahedralSolid< dimension >& solid );
        TetrahedralSolidView( TetrahedralSolidView&& other );
        ~TetrahedralSolidView();

        static MeshImpl impl_name_static()
        {
            return MeshImpl{ absl::StrCat(
                "TetrahedralSolidView", dimension, "D" ) };
        }

        MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        MeshType type_name() const override
        {
            return TetrahedralSolidView< dimension >::type_name_static();
        }

        absl::string_view native_extension() const override
        {
            throw OpenGeodeException( "TetrahedralSolidView cannot be saved" );
            return "";
        }

        index_t viewed_vertex( index_t vertex_id ) const;

        index_t add_viewed_vertex( index_t vertex_id, TetrahedralSolidViewKey );

        index_t viewed_tetrahedron( index_t tetrahedron_id ) const;

        index_t add_viewed_tetrahedron(
            index_t tetrahedron_id, TetrahedralSolidViewKey );

    private:
        const Point< dimension >& get_point( index_t vertex_id ) const override;

        index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const override;

        PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex )
            const override;

        absl::optional< index_t > get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const override;

        index_t get_polyhedron_facet(
            const PolyhedronFacet& polyhedron_facet ) const override;

        bool get_isolated_edge( index_t edge_id ) const override;

        bool get_isolated_facet( index_t facet_id ) const override;

        absl::optional< PolyhedronVertex > get_polyhedron_around_vertex(
            index_t vertex_id ) const override;

        const PolyhedronFacetVertices& get_facet_vertices(
            index_t facet_id ) const override;

        const std::array< index_t, 2 >& get_edge_vertices(
            index_t edge_id ) const override;

        absl::optional< index_t > get_facet_from_vertices(
            const PolyhedronFacetVertices& vertices ) const override;

        absl::optional< index_t > get_edge_from_vertices(
            const std::array< index_t, 2 >& vertices ) const override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( TetrahedralSolidView );
} // namespace geode
