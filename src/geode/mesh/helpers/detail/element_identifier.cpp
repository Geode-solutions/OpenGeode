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

#include <geode/mesh/helpers/detail/element_identifier.h>

#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    namespace detail
    {
        bool opengeode_mesh_api solid_polyhedron_is_a_tetrahedron(
            const SolidMesh3D &solid, index_t polyhedron_id )
        {
            return ( solid.nb_polyhedron_vertices( polyhedron_id ) == 4 );
        }

        bool opengeode_mesh_api solid_polyhedron_is_a_hexaedron(
            const SolidMesh3D &solid, index_t polyhedron_id )
        {
            if( solid.nb_polyhedron_vertices( polyhedron_id ) != 8 )
            {
                return false;
            }
            const auto facets_vertices =
                solid.polyhedron_facets_vertices( polyhedron_id );
            if( facets_vertices.size() != 6 )
            {
                return false;
            }
            for( const auto &facet_vertices : facets_vertices )
            {
                if( facet_vertices.size() != 4 )
                {
                    return false;
                }
            }
            return true;
        }

        bool opengeode_mesh_api solid_polyhedron_is_a_pyramid(
            const SolidMesh3D &solid, index_t polyhedron_id )
        {
            if( solid.nb_polyhedron_vertices( polyhedron_id ) != 5 )
            {
                return false;
            }
            if( solid.nb_polyhedron_facets( polyhedron_id ) != 5 )
            {
                return false;
            }
            return true;
        }

        bool opengeode_mesh_api solid_polyhedron_is_a_prism(
            const SolidMesh3D &solid, index_t polyhedron_id )
        {
            if( solid.nb_polyhedron_vertices( polyhedron_id ) != 6 )
            {
                return false;
            }
            const auto facets_vertices =
                solid.polyhedron_facets_vertices( polyhedron_id );
            if( facets_vertices.size() != 8 )
            {
                return false;
            }
            for( const auto &facet_vertices : facets_vertices )
            {
                if( facet_vertices.size() != 3 )
                {
                    return false;
                }
            }
            return true;
        }
    } // namespace detail
} // namespace geode
