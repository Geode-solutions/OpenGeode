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

#include <geode/mesh/builder/tetrahedral_solid_builder.h>

#include <algorithm>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/builder/solid_edges_builder.h>
#include <geode/mesh/builder/solid_facets_builder.h>
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_facets.h>
#include <geode/mesh/core/tetrahedral_solid.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TetrahedralSolidBuilder< dimension > >
        TetrahedralSolidBuilder< dimension >::create(
            TetrahedralSolid< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            TetrahedralSolidBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void TetrahedralSolidBuilder< dimension >::set_mesh(
        TetrahedralSolid< dimension >& mesh, MeshBuilderFactoryKey key )
    {
        tetrahedral_solid_ = &mesh;
        SolidMeshBuilder< dimension >::set_mesh( mesh, key );
    }

    template < index_t dimension >
    void TetrahedralSolidBuilder< dimension >::do_create_polyhedron(
        absl::Span< const index_t > vertices,
        absl::Span< const std::vector< local_index_t > > facets )
    {
        geode_unused( facets );
        OPENGEODE_ASSERT( vertices.size() == 4, "[TetrahedralSolidBuilder::"
                                                "do_create_polyhedron] Only "
                                                "tetrahedra are handled" );
        std::array< index_t, 4 > tetra_vertices;
        absl::c_copy_n( vertices, 4, tetra_vertices.begin() );
        do_create_tetrahedron( tetra_vertices );
    }

    template < index_t dimension >
    index_t TetrahedralSolidBuilder< dimension >::create_tetrahedron(
        const std::array< index_t, 4 >& vertices )
    {
        const auto added_tetra = tetrahedral_solid_->nb_polyhedra();
        tetrahedral_solid_->polyhedron_attribute_manager().resize(
            added_tetra + 1 );
        local_index_t vertex_id{ 0 };
        for( const auto& vertex : vertices )
        {
            this->associate_polyhedron_vertex_to_vertex(
                { added_tetra, vertex_id++ }, vertex );
        }
        do_create_tetrahedron( vertices );
        if( tetrahedral_solid_->are_facets_enabled() )

        {
            auto facets = this->facets_builder();
            facets.find_or_create_facet(
                { vertices[1], vertices[3], vertices[2] } );
            facets.find_or_create_facet(
                { vertices[0], vertices[2], vertices[3] } );
            facets.find_or_create_facet(
                { vertices[3], vertices[1], vertices[0] } );
            facets.find_or_create_facet(
                { vertices[0], vertices[1], vertices[2] } );
        }
        if( tetrahedral_solid_->are_edges_enabled() )
        {
            auto edges = this->edges_builder();
            edges.find_or_create_edge( { vertices[0], vertices[1] } );
            edges.find_or_create_edge( { vertices[0], vertices[2] } );
            edges.find_or_create_edge( { vertices[0], vertices[3] } );
            edges.find_or_create_edge( { vertices[1], vertices[2] } );
            edges.find_or_create_edge( { vertices[1], vertices[3] } );
            edges.find_or_create_edge( { vertices[2], vertices[3] } );
        }
        return added_tetra;
    }

    template < index_t dimension >
    void TetrahedralSolidBuilder< dimension >::reserve_tetrahedra( index_t nb )
    {
        const auto nb_tet = tetrahedral_solid_->nb_polyhedra();
        tetrahedral_solid_->polyhedron_attribute_manager().reserve(
            nb_tet + nb );
        if( tetrahedral_solid_->are_facets_enabled() )
        {
            tetrahedral_solid_->facets().facet_attribute_manager().reserve(
                nb_tet + 4 * nb );
        }
        if( tetrahedral_solid_->are_edges_enabled() )
        {
            tetrahedral_solid_->edges().edge_attribute_manager().reserve(
                nb_tet + 6 * nb );
        }
    }

    template < index_t dimension >
    index_t TetrahedralSolidBuilder< dimension >::create_tetrahedra(
        index_t nb )
    {
        const auto added_tetra = tetrahedral_solid_->nb_polyhedra();
        tetrahedral_solid_->polyhedron_attribute_manager().resize(
            added_tetra + nb );
        do_create_tetrahedra( nb );
        return added_tetra;
    }

    template < index_t dimension >
    void TetrahedralSolidBuilder< dimension >::copy(
        const TetrahedralSolid< dimension >& tetrahedral_solid )
    {
        SolidMeshBuilder< dimension >::copy( tetrahedral_solid );
    }

    template class opengeode_mesh_api TetrahedralSolidBuilder< 3 >;
} // namespace geode
