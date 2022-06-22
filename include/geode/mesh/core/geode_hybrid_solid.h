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

#include <array>

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/hybrid_solid.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeHybridSolidBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class OpenGeodeHybridSolid : public HybridSolid< dimension >
    {
        OPENGEODE_DISABLE_COPY( OpenGeodeHybridSolid );
        PASSKEY( OpenGeodeHybridSolidBuilder< dimension >, OGHybridSolidKey );

    public:
        OpenGeodeHybridSolid();
        OpenGeodeHybridSolid( OpenGeodeHybridSolid&& other );
        ~OpenGeodeHybridSolid();

        static MeshImpl impl_name_static()
        {
            return MeshImpl{ absl::StrCat(
                "OpenGeodeHybridSolid", dimension, "D" ) };
        }

        MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        MeshType type_name() const override
        {
            return HybridSolid< dimension >::type_name_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_hso", dimension, "d" );
            return extension;
        }

        absl::string_view native_extension() const override
        {
            return native_extension_static();
        }

        void set_vertex(
            index_t vertex_id, Point< dimension > point, OGHybridSolidKey );

        void set_polyhedron_vertex( const PolyhedronVertex& polyhedron_vertex,
            index_t vertex_id,
            OGHybridSolidKey );

        void set_polyhedron_adjacent( const PolyhedronFacet& polyhedron_facet,
            index_t adjacent_id,
            OGHybridSolidKey );

        void add_tetrahedron(
            const std::array< index_t, 4 >& vertices, OGHybridSolidKey );

        void add_hexahedron(
            const std::array< index_t, 8 >& vertices, OGHybridSolidKey );

        void add_prism(
            const std::array< index_t, 6 >& vertices, OGHybridSolidKey );

        void add_pyramid(
            const std::array< index_t, 5 >& vertices, OGHybridSolidKey );

        void remove_polyhedra(
            const std::vector< bool >& to_delete, OGHybridSolidKey );

        void permute_polyhedra(
            absl::Span< const index_t > permutation, OGHybridSolidKey );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

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

        std::vector< std::array< index_t, 2 > > polyhedron_edges_vertices(
            index_t polyhedron ) const final;

        std::vector< PolyhedronFacetVertices > polyhedron_facets_vertices(
            index_t polyhedron ) const final;

        typename HybridSolid< dimension >::Type polyhedron_type(
            index_t polyhedron_id ) const final;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( OpenGeodeHybridSolid );
} // namespace geode
