/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/mesh/builder/geode/geode_polyhedral_solid_builder.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/mesh_builder_factory.hpp>
#include <geode/mesh/core/geode/geode_polyhedral_solid.hpp>

namespace geode
{
    template < index_t dimension >
    OpenGeodePolyhedralSolidBuilder<
        dimension >::OpenGeodePolyhedralSolidBuilder( VertexSet& vertex_set,
        MeshBuilderFactoryKey )
        : OpenGeodePolyhedralSolidBuilder< dimension >(
              dynamic_cast< OpenGeodePolyhedralSolid< dimension >& >(
                  vertex_set ) )
    {
    }

    template < index_t dimension >
    OpenGeodePolyhedralSolidBuilder< dimension >::
        OpenGeodePolyhedralSolidBuilder(
            OpenGeodePolyhedralSolid< dimension >& mesh )
        : PolyhedralSolidBuilder< dimension >( mesh ),
          geode_polyhedral_solid_( mesh )
    {
    }

    template < index_t dimension >
    OpenGeodePolyhedralSolidBuilder< dimension >::
        OpenGeodePolyhedralSolidBuilder(
            OpenGeodePolyhedralSolidBuilder&& ) noexcept = default;

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_create_vertex()
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_delete_solid_vertices(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void
        OpenGeodePolyhedralSolidBuilder< dimension >::do_permute_solid_vertices(
            absl::Span< const index_t > /*unused*/,
            absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        geode_polyhedral_solid_.set_polyhedron_vertex(
            polyhedron_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_create_polyhedron(
        absl::Span< const index_t > vertices,
        absl::Span< const std::vector< local_index_t > > facets )
    {
        geode_polyhedral_solid_.add_polyhedron( vertices, facets, {} );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::
        do_set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
    {
        geode_polyhedral_solid_.set_polyhedron_adjacent(
            polyhedron_facet, adjacent_id, {} );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::
        do_unset_polyhedron_adjacent( const PolyhedronFacet& polyhedron_facet )
    {
        geode_polyhedral_solid_.set_polyhedron_adjacent(
            polyhedron_facet, NO_ID, {} );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_delete_polyhedra(
        const std::vector< bool >& to_delete,
        absl::Span< const index_t > /*unused*/ )
    {
        geode_polyhedral_solid_.remove_polyhedra( to_delete, {} );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_permute_polyhedra(
        absl::Span< const index_t > permutation,
        absl::Span< const index_t > /*unused*/ )
    {
        geode_polyhedral_solid_.permute_polyhedra( permutation, {} );
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_copy_points(
        const SolidMesh< dimension >& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolyhedralSolidBuilder< dimension >::do_copy_polyhedra(
        const SolidMesh< dimension >& solid_mesh )
    {
        geode_polyhedral_solid_.copy_polyhedra(
            dynamic_cast< const OpenGeodePolyhedralSolid< dimension >& >(
                solid_mesh ),
            {} );
    }

    template class opengeode_mesh_api OpenGeodePolyhedralSolidBuilder< 3 >;
} // namespace geode
