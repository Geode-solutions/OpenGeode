/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#pragma once

#include <geode/geometry/point.h>

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeRegularGrid );
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeRegularGridBuilder );
    class VertexSet;
} // namespace geode

namespace geode
{
    /*!
     * Implementation class for RegularGridBuilder using OpenGeode data
     * structure
     */
    template <>
    class opengeode_mesh_api OpenGeodeRegularGridBuilder< 3 >
        : public RegularGridBuilder< 3 >
    {
    public:
        OpenGeodeRegularGridBuilder(
            VertexSet& vertex_set, MeshBuilderFactoryKey );

        OpenGeodeRegularGridBuilder( OpenGeodeRegularGrid< 3 >& mesh );

    private:
        void update_origin( const Point3D& origin ) final;

        void do_create_vertices( index_t /*unused*/ ) final;

        void do_set_point( index_t /*unused*/, Point3D /*unused*/ )
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use set_point in RegularGrid"
            };
        }

        void do_create_vertex() final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use create_vertex in RegularGrid"
            };
        }

        void do_delete_solid_vertices( const std::vector< bool >& /*unused*/,
            absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "delete_solid_vertices in RegularGrid" };
        }

        void do_permute_solid_vertices( absl::Span< const index_t > /*unused*/,
            absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use _permute_solid_vertices in "
                "RegularGrid"
            };
        }

        void do_set_polyhedron_vertex(
            const PolyhedronVertex& /*unused*/, index_t /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "set_polyhedron_vertex in RegularGrid" };
        }

        void do_create_polyhedron( absl::Span< const index_t > /*unused*/,
            absl::Span< const std::vector< local_index_t > > /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "create_polyhedron in RegularGrid" };
        }

        void do_delete_polyhedra( const std::vector< bool >& /*unused*/,
            absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "delete_polyhedra in RegularGrid" };
        }

        void do_permute_polyhedra( absl::Span< const index_t > /*unused*/,
            absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "permute_polyhedra in RegularGrid" };
        }

        void do_set_polyhedron_adjacent(
            const PolyhedronFacet& /*unused*/, index_t /*unused*/ ) final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use set_polyhedron_adjacent in "
                "RegularGrid"
            };
        }

        void do_unset_polyhedron_adjacent(
            const PolyhedronFacet& /*unused*/ ) final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use unset_polyhedron_adjacent in "
                "RegularGrid"
            };
        }

        void do_copy_points( const SolidMesh< 3 >& solid_mesh ) final;

        void do_copy_polyhedra( const SolidMesh< 3 >& solid_mesh ) final;

    private:
        OpenGeodeRegularGrid< 3 >& geode_regular_grid_;
    };
    ALIAS_3D( OpenGeodeRegularGridBuilder );
} // namespace geode
