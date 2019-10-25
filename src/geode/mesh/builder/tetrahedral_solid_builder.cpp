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

#include <geode/mesh/builder/tetrahedral_solid_builder.h>

#include <algorithm>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/core/tetrahedral_solid.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TetrahedralSolidBuilder< dimension > >
        TetrahedralSolidBuilder< dimension >::create(
            TetrahedralSolid< dimension >& mesh )
    {
        try
        {
            return TetrahedralSolidBuilderFactory< dimension >::create(
                mesh.type_name(), mesh );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException(
                "Could not create TetrahedralSolid builder of data structure: ",
                mesh.type_name().get() );
        }
    }

    template < index_t dimension >
    void TetrahedralSolidBuilder< dimension >::do_create_polyhedron(
        const std::vector< index_t >& vertices,
        const std::vector< std::vector< index_t > >& facets )
    {
        OPENGEODE_EXCEPTION( vertices.size() == 4, "[TetrahedralSolidBuilder::"
                                                   "do_create_polyhedron] Only "
                                                   "tetrahedra are handled" );
        OPENGEODE_EXCEPTION( facets.size() == 4, "[TetrahedralSolidBuilder::do_"
                                                 "create_polyhedron] Only "
                                                 "tetrahedra are handled" );
        std::array< index_t, 4 > tetra_vertices{};
        std::copy_n( vertices.begin(), 4, tetra_vertices.begin() );
        do_create_tetrahedron( tetra_vertices );
    }

    template < index_t dimension >
    std::vector< std::vector< index_t > >
        TetrahedralSolidBuilder< dimension >::get_polyhedron_facet_vertices(
            const std::vector< index_t >& vertices,
            const std::vector< std::vector< index_t > >& facets ) const
    {
        OPENGEODE_EXCEPTION( vertices.size() == 4, "[TetrahedralSolidBuilder::"
                                                   "do_create_polyhedron] Only "
                                                   "tetrahedra are handled" );
        OPENGEODE_EXCEPTION( facets.size() == 4, "[TetrahedralSolidBuilder::do_"
                                                 "create_polyhedron] Only "
                                                 "tetrahedra are handled" );
        std::array< index_t, 4 > tetra_vertices{};
        std::copy_n( vertices.begin(), 4, tetra_vertices.begin() );
        return get_polyhedron_facet_vertices( tetra_vertices );
    }

    template < index_t dimension >
    index_t TetrahedralSolidBuilder< dimension >::create_tetrahedron(
        const std::array< index_t, 4 >& vertices )
    {
        auto added_tetra = tetrahedral_solid_.nb_polyhedra();
        tetrahedral_solid_.polyhedron_attribute_manager().resize(
            added_tetra + 1 );
        index_t vertex_id{ 0 };
        for( const auto& vertex : vertices )
        {
            this->associate_polyhedron_vertex_to_vertex(
                { added_tetra, vertex_id++ }, vertex );
        }
        do_create_tetrahedron( vertices );
        for( auto f : Range{ 4 } )
        {
            std::vector< index_t > facet_vertices( 3 );
            for( auto v : Range{ 3 } )
            {
                facet_vertices[v] = tetrahedral_solid_.polyhedron_facet_vertex(
                    { { added_tetra, f }, v } );
            }
            this->find_or_create_facet( facet_vertices );
        }
        return added_tetra;
    }

    template < index_t dimension >
    void TetrahedralSolidBuilder< dimension >::copy(
        const TetrahedralSolid< dimension >& tetrahedral_solid )
    {
        PolyhedralSolidBuilder< dimension >::copy( tetrahedral_solid );
    }

    template class opengeode_mesh_api TetrahedralSolidBuilder< 3 >;
} // namespace geode
