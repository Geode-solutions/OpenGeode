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

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
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
            for( const auto e : LRange{ 4 } )
            {
                if( const auto adjacent =
                        solid.polyhedron_adjacent( { p, e } ) )
                {
                    builder->set_polyhedron_adjacent(
                        { p, e }, adjacent.value() );
                }
            }
        }
        return absl::optional< std::unique_ptr< TetrahedralSolid3D > >{
            absl::in_place, tet_solid.release()
        };
    }

    std::unique_ptr< SolidMesh3D > merge_solid_meshes(
        absl::Span< const std::reference_wrapper< const SolidMesh3D > > solids )
    {
        detail::SolidMeshMerger3D merger{ solids, global_epsilon };
        return merger.merge();
    }
} // namespace geode
