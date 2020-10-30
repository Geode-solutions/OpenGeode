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

#include <geode/mesh/builder/tetrahedral_solid_view_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/tetrahedral_solid_view.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TetrahedralSolidViewBuilder< dimension > >
        TetrahedralSolidViewBuilder< dimension >::create(
            TetrahedralSolidView< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            TetrahedralSolidViewBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_set_mesh(
        VertexSet& mesh )
    {
        tetrahedral_solid_view_ =
            &dynamic_cast< TetrahedralSolidView< dimension >& >( mesh );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_set_point(
        index_t /*unused*/, Point< dimension > /*unused*/ )
    {
        // tetrahedral_solid_view_->set_vertex( vertex_id, point, {} );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_create_vertex()
    {
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_delete_solid_vertices(
        const std::vector< bool >& /*unused*/ )
    {
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_set_polyhedron_vertex(
        const PolyhedronVertex& /*unused*/, index_t /*unused*/ )
    {
        // tetrahedral_solid_view_->set_polyhedron_vertex(
        //     polyhedron_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_set_polyhedron_adjacent(
        const PolyhedronFacet& /*unused*/, index_t /*unused*/ )
    {
        // tetrahedral_solid_view_->set_polyhedron_adjacent(
        //     polyhedron_facet, adjacent_id, {} );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_unset_polyhedron_adjacent(
        const PolyhedronFacet& /*unused*/ )
    {
        // tetrahedral_solid_view_->set_polyhedron_adjacent(
        //     polyhedron_facet, NO_ID, {} );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_delete_polyhedra(
        const std::vector< bool >& /*unused*/ )
    {
        // tetrahedral_solid_view_->remove_polyhedra( to_delete, {} );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::add_viewed_vertex(
        index_t vertex_id )
    {
        tetrahedral_solid_view_->add_viewed_vertex( vertex_id, {} );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::add_viewed_tetrahedron(
        index_t tetrahedron_id )
    {
        tetrahedral_solid_view_->add_viewed_tetrahedron( tetrahedron_id, {} );
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_create_tetrahedron(
        const std::array< index_t, 4 >& /*unused*/ )
    {
    }

    template < index_t dimension >
    void TetrahedralSolidViewBuilder< dimension >::do_create_tetrahedra(
        index_t /*unused*/ )
    {
    }

    template class opengeode_mesh_api TetrahedralSolidViewBuilder< 3 >;
} // namespace geode
