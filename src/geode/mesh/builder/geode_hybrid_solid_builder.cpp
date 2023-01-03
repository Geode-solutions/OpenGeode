/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/builder/geode_hybrid_solid_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/geode_hybrid_solid.h>

namespace geode
{
    template < index_t dimension >
    OpenGeodeHybridSolidBuilder< dimension >::OpenGeodeHybridSolidBuilder(
        VertexSet& vertex_set, MeshBuilderFactoryKey )
        : OpenGeodeHybridSolidBuilder< dimension >(
            dynamic_cast< OpenGeodeHybridSolid< dimension >& >( vertex_set ) )
    {
    }

    template < index_t dimension >
    OpenGeodeHybridSolidBuilder< dimension >::OpenGeodeHybridSolidBuilder(
        OpenGeodeHybridSolid< dimension >& mesh )
        : HybridSolidBuilder< dimension >( mesh ), geode_hybrid_solid_( mesh )
    {
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_set_point(
        index_t vertex_id, Point< dimension > point )
    {
        geode_hybrid_solid_.set_vertex( vertex_id, std::move( point ), {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_create_vertex()
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_delete_solid_vertices(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_permute_solid_vertices(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        geode_hybrid_solid_.set_polyhedron_vertex(
            polyhedron_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_create_tetrahedron(
        const std::array< index_t, 4 >& vertices )
    {
        geode_hybrid_solid_.add_tetrahedron( vertices, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_create_hexahedron(
        const std::array< index_t, 8 >& vertices )
    {
        geode_hybrid_solid_.add_hexahedron( vertices, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_create_prism(
        const std::array< index_t, 6 >& vertices )
    {
        geode_hybrid_solid_.add_prism( vertices, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_create_pyramid(
        const std::array< index_t, 5 >& vertices )
    {
        geode_hybrid_solid_.add_pyramid( vertices, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
    {
        geode_hybrid_solid_.set_polyhedron_adjacent(
            polyhedron_facet, adjacent_id, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_unset_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet )
    {
        geode_hybrid_solid_.set_polyhedron_adjacent(
            polyhedron_facet, NO_ID, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_delete_polyhedra(
        const std::vector< bool >& to_delete,
        absl::Span< const index_t > /*unused*/ )
    {
        geode_hybrid_solid_.remove_polyhedra( to_delete, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_permute_polyhedra(
        absl::Span< const index_t > permutation,
        absl::Span< const index_t > /*unused*/ )
    {
        geode_hybrid_solid_.permute_polyhedra( permutation, {} );
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_copy_points(
        const SolidMesh< dimension >& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeHybridSolidBuilder< dimension >::do_copy_polyhedra(
        const SolidMesh< dimension >& solid_mesh )
    {
        geode_hybrid_solid_.copy_polyhedra(
            dynamic_cast< const OpenGeodeHybridSolid< dimension >& >(
                solid_mesh ),
            {} );
    }

    template class opengeode_mesh_api OpenGeodeHybridSolidBuilder< 3 >;
} // namespace geode
