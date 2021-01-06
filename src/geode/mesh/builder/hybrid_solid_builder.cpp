/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/mesh/builder/hybrid_solid_builder.h>

#include <algorithm>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/builder/solid_edges_builder.h>
#include <geode/mesh/builder/solid_facets_builder.h>
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_facets.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< HybridSolidBuilder< dimension > >
        HybridSolidBuilder< dimension >::create(
            HybridSolid< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            HybridSolidBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void HybridSolidBuilder< dimension >::set_mesh(
        HybridSolid< dimension >& mesh, MeshBuilderFactoryKey key )
    {
        hybrid_solid_ = &mesh;
        SolidMeshBuilder< dimension >::set_mesh( mesh, key );
    }

    template < index_t dimension >
    void HybridSolidBuilder< dimension >::do_create_polyhedron(
        absl::Span< const index_t > vertices,
        absl::Span< const std::vector< local_index_t > > facets )
    {
        geode_unused( facets );
        if( vertices.size() == 4 )
        {
            std::array< index_t, 4 > tetra_vertices;
            absl::c_copy_n( vertices, 4, tetra_vertices.begin() );
            do_create_tetrahedron( tetra_vertices );
        }
        else if( vertices.size() == 8 )
        {
            std::array< index_t, 8 > hexa_vertices;
            absl::c_copy_n( vertices, 8, hexa_vertices.begin() );
            do_create_hexahedron( hexa_vertices );
        }
        else if( vertices.size() == 6 )
        {
            std::array< index_t, 6 > prism_vertices;
            absl::c_copy_n( vertices, 6, prism_vertices.begin() );
            do_create_prism( prism_vertices );
        }
        else if( vertices.size() == 5 )
        {
            std::array< index_t, 5 > pyramid_vertices;
            absl::c_copy_n( vertices, 5, pyramid_vertices.begin() );
            do_create_pyramid( pyramid_vertices );
        }
        else
        {
            throw OpenGeodeException(
                "[HybridSolidBuilder::do_create_polyhedron] Only "
                "tetrahedron, hexahedron, prism and pyramid are handled" );
        }
    }

    template < index_t dimension >
    index_t HybridSolidBuilder< dimension >::create_tetrahedron(
        const std::array< index_t, 4 >& vertices )
    {
        const auto added_tetra = hybrid_solid_->nb_polyhedra();
        hybrid_solid_->polyhedron_attribute_manager().resize( added_tetra + 1 );
        do_create_tetrahedron( vertices );
        this->update_polyhedron_info( added_tetra, vertices );
        return added_tetra;
    }

    template < index_t dimension >
    index_t HybridSolidBuilder< dimension >::create_hexahedron(
        const std::array< index_t, 8 >& vertices )
    {
        const auto added_hexa = hybrid_solid_->nb_polyhedra();
        hybrid_solid_->polyhedron_attribute_manager().resize( added_hexa + 1 );
        do_create_hexahedron( vertices );
        this->update_polyhedron_info( added_hexa, vertices );
        return added_hexa;
    }

    template < index_t dimension >
    index_t HybridSolidBuilder< dimension >::create_prism(
        const std::array< index_t, 6 >& vertices )
    {
        const auto added_prism = hybrid_solid_->nb_polyhedra();
        hybrid_solid_->polyhedron_attribute_manager().resize( added_prism + 1 );
        do_create_prism( vertices );
        this->update_polyhedron_info( added_prism, vertices );
        return added_prism;
    }

    template < index_t dimension >
    index_t HybridSolidBuilder< dimension >::create_pyramid(
        const std::array< index_t, 5 >& vertices )
    {
        const auto added_pyramid = hybrid_solid_->nb_polyhedra();
        hybrid_solid_->polyhedron_attribute_manager().resize(
            added_pyramid + 1 );
        do_create_pyramid( vertices );
        this->update_polyhedron_info( added_pyramid, vertices );
        return added_pyramid;
    }

    template < index_t dimension >
    void HybridSolidBuilder< dimension >::copy(
        const HybridSolid< dimension >& hybrid_solid )
    {
        SolidMeshBuilder< dimension >::copy( hybrid_solid );
    }

    template class opengeode_mesh_api HybridSolidBuilder< 3 >;
} // namespace geode
