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

#include <geode/mesh/helpers/remove_element_duplication.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_edges_builder.hpp>
#include <geode/mesh/builder/solid_facets_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/generic_edged_curve_accessor.hpp>
#include <geode/mesh/helpers/generic_solid_accessor.hpp>
#include <geode/mesh/helpers/generic_surface_accessor.hpp>

namespace
{

    template < typename Mesh >
    absl::flat_hash_map< geode::index_t, std::vector< geode::index_t > >
        compute_vertex_to_element_mapping( const Mesh& mesh )
    {
        using Accessor = geode::GenericMeshAccessor< Mesh >;
        const Accessor accessor{ mesh };
        absl::flat_hash_map< geode::index_t, std::vector< geode::index_t > >
            vertex_to_element;
        for( const auto element_id : geode::Range{ accessor.nb_elements() } )
        {
            for( const auto vertex_id :
                accessor.element_vertices( element_id ) )
            {
                vertex_to_element[vertex_id].push_back( element_id );
            }
        }
        return vertex_to_element;
    }

    template < geode::index_t dimension >
    void delete_elements( const std::vector< bool >& to_delete,
        geode::EdgedCurveBuilder< dimension >& builder )
    {
        builder.delete_edges( to_delete );
    }
    template < geode::index_t dimension >
    void delete_elements( const std::vector< bool >& to_delete,
        geode::SurfaceMeshBuilder< dimension >& builder )
    {
        builder.delete_polygons( to_delete );
    }
    template < geode::index_t dimension >
    void delete_elements( const std::vector< bool >& to_delete,
        geode::SolidMeshBuilder< dimension >& builder )
    {
        builder.delete_polyhedra( to_delete );
    }

    template < typename Mesh, typename Builder >
    void remove_element_duplication( const Mesh& mesh, Builder& builder )
    {
        const auto vertex_to_element =
            compute_vertex_to_element_mapping( mesh );
        using Accessor = geode::GenericMeshAccessor< Mesh >;
        const Accessor accessor{ mesh };
        std::vector< bool > to_delete( accessor.nb_elements(), false );
        for( const auto element_id : geode::Range{ accessor.nb_elements() } )
        {
            if( to_delete[element_id] )
            {
                continue;
            }
            const auto element_vertices =
                accessor.element_vertices( element_id );
            std::vector< geode::index_t > common_elements =
                vertex_to_element.at( element_vertices[0] );
            for( const auto v :
                geode::LRange{ 1, accessor.nb_element_vertices( element_id ) } )
            {
                std::vector< geode::index_t > temp;
                absl::c_set_intersection( common_elements,
                    vertex_to_element.at( element_vertices[v] ),
                    std::back_inserter( temp ) );
                common_elements = std::move( temp );
            }
            if( common_elements.size() > 1 )
            {
                for( const auto common_element_id : common_elements )
                {
                    if( element_id == common_element_id )
                    {
                        continue;
                    }
                    to_delete[common_element_id] = true;
                }
            }
        }
        delete_elements( to_delete, builder );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    void remove_edge_duplication( const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder )
    {
        ::remove_element_duplication( mesh, builder );
    }

    template < index_t dimension >
    void remove_polygon_duplication( const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder )
    {
        ::remove_element_duplication( mesh, builder );
    }

    void remove_polyhedron_duplication(
        const SolidMesh3D& mesh, SolidMeshBuilder3D& builder )
    {
        ::remove_element_duplication( mesh, builder );
    }

    template void opengeode_mesh_api remove_edge_duplication(
        const EdgedCurve2D&, EdgedCurveBuilder2D& );
    template void opengeode_mesh_api remove_edge_duplication(
        const EdgedCurve3D&, EdgedCurveBuilder3D& );

    template void opengeode_mesh_api remove_polygon_duplication(
        const SurfaceMesh2D&, SurfaceMeshBuilder2D& );
    template void opengeode_mesh_api remove_polygon_duplication(
        const SurfaceMesh3D&, SurfaceMeshBuilder3D& );

} // namespace geode
