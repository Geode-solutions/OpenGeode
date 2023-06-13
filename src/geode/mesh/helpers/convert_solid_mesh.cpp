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

#include <geode/mesh/helpers/convert_solid_mesh.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/hybrid_solid_builder.h>
#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/helpers/detail/solid_merger.h>
#include <geode/mesh/helpers/private/copy.h>

namespace
{
    bool all_polyhedra_are_simplex( const geode::SolidMesh3D& solid )
    {
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            if( solid.nb_polyhedron_vertices( p ) > 4 )
            {
                return false;
            }
        }
        return true;
    }

    bool all_polyhedra_are_hybrid( const geode::SolidMesh3D& solid )
    {
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            const auto nb_p_vertices = solid.nb_polyhedron_vertices( p );
            if( nb_p_vertices == 4 )
            {
                continue;
            }
            if( nb_p_vertices == 5 )
            {
                if( solid.nb_polyhedron_facets( p ) != 5 )
                {
                    return false;
                }
                continue;
            }
            if( nb_p_vertices == 6 )
            {
                const auto facets_vertices =
                    solid.polyhedron_facets_vertices( p );
                if( facets_vertices.size() != 8 )
                {
                    return false;
                }
                for( const auto& facet_vertices : facets_vertices )
                {
                    if( facet_vertices.size() != 3 )
                    {
                        return false;
                    }
                }
                continue;
            }
            if( nb_p_vertices == 8 )
            {
                const auto facets_vertices =
                    solid.polyhedron_facets_vertices( p );
                if( facets_vertices.size() != 6 )
                {
                    return false;
                }
                for( const auto& facet_vertices : facets_vertices )
                {
                    if( facet_vertices.size() != 4 )
                    {
                        return false;
                    }
                }
                continue;
            }
            return false;
        }
        return true;
    }
} // namespace

namespace geode
{
    absl::optional< std::unique_ptr< TetrahedralSolid3D > >
        convert_solid_mesh_into_tetrahedral_solid( const SolidMesh3D& solid )
    {
        if( !all_polyhedra_are_simplex( solid ) )
        {
            return absl::nullopt;
        }
        auto tet_solid = TetrahedralSolid3D::create();
        auto builder = TetrahedralSolidBuilder3D::create( *tet_solid );
        detail::copy_meta_info( solid, *builder );
        detail::copy_points( solid, *builder );
        builder->reserve_tetrahedra( solid.nb_polyhedra() );
        for( const auto p : Range{ solid.nb_polyhedra() } )
        {
            const std::array< index_t, 4 > vertices{ solid.polyhedron_vertex(
                                                         { p, 0 } ),
                solid.polyhedron_vertex( { p, 1 } ),
                solid.polyhedron_vertex( { p, 2 } ),
                solid.polyhedron_vertex( { p, 3 } ) };
            builder->create_tetrahedron( vertices );
        }
        for( const auto p : Range{ solid.nb_polyhedra() } )
        {
            for( const auto f : LRange{ 4 } )
            {
                if( const auto adjacent =
                        solid.polyhedron_adjacent( { p, f } ) )
                {
                    builder->set_polyhedron_adjacent(
                        { p, f }, adjacent.value() );
                }
            }
        }
        return absl::optional< std::unique_ptr< TetrahedralSolid3D > >{
            absl::in_place, tet_solid.release()
        };
    }

    absl::optional< std::unique_ptr< HybridSolid3D > >
        convert_solid_mesh_into_hybrid_solid( const SolidMesh3D& solid )
    {
        if( !all_polyhedra_are_hybrid( solid ) )
        {
            return absl::nullopt;
        }
        auto hybrid_solid = HybridSolid3D::create();
        auto builder = HybridSolidBuilder3D::create( *hybrid_solid );
        detail::copy_meta_info( solid, *builder );
        detail::copy_points( solid, *builder );
        for( const auto p : Range{ solid.nb_polyhedra() } )
        {
            const auto vertices = solid.polyhedron_vertices( p );
            if( vertices.size() == 4 )
            {
                builder->create_tetrahedron(
                    { vertices[0], vertices[1], vertices[2], vertices[3] } );
            }
            else if( vertices.size() == 8 )
            {
                builder->create_hexahedron(
                    { vertices[0], vertices[1], vertices[2], vertices[3],
                        vertices[4], vertices[5], vertices[6], vertices[7] } );
            }
            else if( vertices.size() == 5 )
            {
                builder->create_pyramid( { vertices[0], vertices[1],
                    vertices[2], vertices[3], vertices[4] } );
            }
            else if( vertices.size() == 6 )
            {
                builder->create_prism( { vertices[0], vertices[1], vertices[2],
                    vertices[3], vertices[4], vertices[5] } );
            }
        }
        for( const auto p : Range{ solid.nb_polyhedra() } )
        {
            for( const auto f : LRange{ solid.nb_polyhedron_facets( p ) } )
            {
                if( const auto adjacent =
                        solid.polyhedron_adjacent( { p, f } ) )
                {
                    builder->set_polyhedron_adjacent(
                        { p, f }, adjacent.value() );
                }
            }
        }
        return absl::optional< std::unique_ptr< HybridSolid3D > >{
            absl::in_place, hybrid_solid.release()
        };
    }

    std::unique_ptr< SolidMesh3D > merge_solid_meshes(
        absl::Span< const std::reference_wrapper< const SolidMesh3D > > solids )
    {
        detail::SolidMeshMerger3D merger{ solids, global_epsilon };
        return merger.merge();
    }
} // namespace geode
