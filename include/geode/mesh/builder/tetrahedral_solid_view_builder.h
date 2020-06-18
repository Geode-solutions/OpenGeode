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

#include <vector>

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolidView );
} // namespace geode

namespace geode
{
    /*!
     * Implementation class for TetrahedralSolidBuilder using
     * TetrahedralSolidView data structure
     */
    template < index_t dimension >
    class TetrahedralSolidViewBuilder
        : public TetrahedralSolidBuilder< dimension >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        static std::unique_ptr< TetrahedralSolidViewBuilder< dimension > >
            create( TetrahedralSolidView< dimension >& mesh );

        void add_viewed_vertex( index_t vertex_id );

        void add_viewed_tetrahedron( index_t tetrahedron_id );

    private:
        void do_set_mesh( VertexSet& mesh ) final;

        void do_set_point(
            index_t vertex_id, const Point< dimension >& point ) final;

        void do_create_vertex() final;

        void do_create_vertices( index_t nb ) final;

        void do_delete_solid_vertices(
            const std::vector< bool >& to_delete ) final;

        void do_set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex,
            index_t vertex_id ) final;

        void do_delete_polyhedra( const std::vector< bool >& to_delete ) final;

        void do_set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet,
            index_t adjacent_id ) final;

        void do_unset_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) final;

        void do_create_facets( const std::array< index_t, 4 >& vertices ) final;

        void do_create_edges( const std::array< index_t, 4 >& vertices ) final;

        void do_create_tetrahedron(
            const std::array< index_t, 4 >& vertices ) final;

        void do_create_tetrahedra( index_t nb ) final;

    private:
        TetrahedralSolidView< dimension >* tetrahedral_solid_view_;
    };
    ALIAS_3D( TetrahedralSolidViewBuilder );
} // namespace geode
