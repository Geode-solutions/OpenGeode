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

#include <geode/mesh/builder/geode_polyhedral_solid_builder.h>

#include <geode/mesh/core/geode_polyhedral_solid.h>

namespace geode
{
    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_set_point(
        index_t vertex_id, const Point< dimension >& point )
    {
        geode_polyhedral_solid_.set_vertex( vertex_id, point );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_create_vertex()
    {
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_delete_solid_vertices(
        const std::vector< bool >& /*unused*/ )
    {
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        geode_polyhedral_solid_.set_polyhedron_vertex(
            polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_create_polyhedron(
        const std::vector< index_t >& vertices,
        const std::vector< std::vector< index_t > >& facets )
    {
        geode_polyhedral_solid_.add_polyhedron( vertices, facets );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::
        do_set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
    {
        geode_polyhedral_solid_.set_polyhedron_adjacent(
            polyhedron_facet, adjacent_id );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_delete_polyhedra(
        const std::vector< bool >& to_delete )
    {
        geode_polyhedral_solid_.remove_polyhedra( to_delete );
    }

    template class opengeode_mesh_api OpenGeodePolyhedralSolidBuilder< 3 >;
} // namespace geode
