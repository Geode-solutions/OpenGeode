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

#pragma once

#include <array>

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>

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
        using Builder = OpenGeodeHybridSolidBuilder< dimension >;
        static constexpr auto dim = dimension;

        OpenGeodeHybridSolid();
        OpenGeodeHybridSolid( OpenGeodeHybridSolid&& other ) noexcept;
        OpenGeodeHybridSolid& operator=(
            OpenGeodeHybridSolid&& other ) noexcept;
        ~OpenGeodeHybridSolid();

        [[nodiscard]] static MeshImpl impl_name_static()
        {
            return MeshImpl{ absl::StrCat(
                "OpenGeodeHybridSolid", dimension, "D" ) };
        }

        [[nodiscard]] MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        [[nodiscard]] MeshType type_name() const override
        {
            return HybridSolid< dimension >::type_name_static();
        }

        [[nodiscard]] static std::string_view native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_hso", dimension, "d" );
            return extension;
        }

        [[nodiscard]] std::string_view native_extension() const override
        {
            return native_extension_static();
        }

    public:
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

        void copy_polyhedra(
            const OpenGeodeHybridSolid< dimension >& solid_mesh,
            OGHybridSolidKey );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        [[nodiscard]] index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const override;

        [[nodiscard]] local_index_t get_nb_polyhedron_vertices(
            index_t polyhedron_id ) const override;

        [[nodiscard]] local_index_t get_nb_polyhedron_facets(
            index_t polyhedron_id ) const override;

        [[nodiscard]] local_index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const override;

        [[nodiscard]] PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex )
            const override;

        [[nodiscard]] std::optional< index_t > get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const override;

        [[nodiscard]] PolyhedronEdgesVertices polyhedron_edges_vertices(
            index_t polyhedron ) const final;

        [[nodiscard]] PolyhedronFacetsVertices polyhedron_facets_vertices(
            index_t polyhedron ) const final;

        [[nodiscard]] typename HybridSolid< dimension >::Type polyhedron_type(
            index_t polyhedron_id ) const final;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( OpenGeodeHybridSolid );
} // namespace geode
