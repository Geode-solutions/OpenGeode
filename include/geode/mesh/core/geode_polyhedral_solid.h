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

#pragma once

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/polyhedral_solid.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodePolyhedralSolidBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class OpenGeodePolyhedralSolid : public PolyhedralSolid< dimension >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( OpenGeodePolyhedralSolid );
        friend class OpenGeodePolyhedralSolidBuilder< dimension >;

    public:
        OpenGeodePolyhedralSolid();
        ~OpenGeodePolyhedralSolid();

        static MeshType type_name_static()
        {
            return MeshType{ "OpenGeodePolyhedralSolid"
                             + std::to_string( dimension ) + "D" };
        }

        MeshType type_name() const override
        {
            return type_name_static();
        }

        static std::string native_extension_static()
        {
            return "og_pso" + std::to_string( dimension ) + "d";
        }

        std::string native_extension() const override
        {
            return native_extension_static();
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        const Point< dimension >& get_point( index_t vertex_id ) const override;

        void set_vertex( index_t vertex_id, const Point< dimension >& point );

        index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const override;

        void set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id );

        index_t get_nb_polyhedron_vertices(
            index_t polyhedron_id ) const override;

        index_t get_nb_polyhedron_facets(
            index_t polyhedron_id ) const override;

        index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const override;

        index_t get_polyhedron_facet_vertex(
            const PolyhedronFacetVertex& polyhedron_facet_vertex )
            const override;

        PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex )
            const override;

        index_t get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const override;

        void add_polyhedron( const std::vector< index_t >& vertices,
            const std::vector< std::vector< index_t > >& facets );

        void remove_polyhedra( const std::vector< bool >& to_delete );

        void set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, index_t adjacent_id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( OpenGeodePolyhedralSolid );
} // namespace geode
