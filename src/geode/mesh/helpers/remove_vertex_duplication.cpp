/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/mesh/helpers/remove_vertex_duplication.hpp>

#include <geode/geometry/nn_search.hpp>

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
#include <geode/mesh/helpers/nnsearch_mesh.hpp>

namespace
{
    template < geode::index_t dimension >
    void replace_vertices( const geode::PointSet< dimension >& mesh,
        geode::PointSetBuilder< dimension >& builder,
        const geode::GenericMapping< geode::index_t >& mapping )
    {
        std::vector< bool > to_delete( mesh.nb_vertices(), false );
        for( const auto v : geode::Range{ mesh.nb_vertices() } )
        {
            if( mapping.in2out( v ).front() != v )
            {
                to_delete[v] = true;
            }
        }
        builder.delete_vertices( to_delete );
    }

    template < geode::index_t dimension >
    void replace_vertices( const geode::EdgedCurve< dimension >& mesh,
        geode::EdgedCurveBuilder< dimension >& builder,
        const geode::GenericMapping< geode::index_t >& mapping )
    {
        for( const auto e : geode::Range{ mesh.nb_edges() } )
        {
            const auto vertices = mesh.edge_vertices( e );
            for( const auto v : geode::LRange{ 2 } )
            {
                const auto new_vertex_id =
                    mapping.in2out( vertices[v] ).front();
                if( new_vertex_id != vertices[v] )
                {
                    builder.set_edge_vertex( { e, v }, new_vertex_id );
                }
            }
        }
        builder.delete_isolated_vertices();
    }

    template < geode::index_t dimension >
    void replace_vertices( const geode::SurfaceMesh< dimension >& mesh,
        geode::SurfaceMeshBuilder< dimension >& builder,
        const geode::GenericMapping< geode::index_t >& mapping )
    {
        builder.replace_vertices( mapping );
        if( mesh.are_edges_enabled() )
        {
            builder.edges_builder().delete_isolated_edges();
        }
        builder.delete_isolated_vertices();
    }

    template < geode::index_t dimension >
    void replace_vertices( const geode::SolidMesh< dimension >& mesh,
        geode::SolidMeshBuilder< dimension >& builder,
        const geode::GenericMapping< geode::index_t >& mapping )
    {
        builder.replace_vertices( mapping );
        if( mesh.are_edges_enabled() )
        {
            builder.edges_builder().delete_isolated_edges();
        }
        if( mesh.are_facets_enabled() )
        {
            builder.facets_builder().delete_isolated_facets();
        }
        builder.delete_isolated_vertices();
    }

    template < typename Mesh, typename Builder >
    void remove_vertex_duplication( const Mesh& mesh, Builder& builder )
    {
        const auto nnsearch = create_nn_search( mesh );
        const auto mapping =
            nnsearch.colocated_index_mapping( geode::GLOBAL_EPSILON );
        if( mapping.nb_colocated_points() == mesh.nb_vertices() )
        {
            return;
        }
        replace_vertices( mesh, builder, mapping.vertices_mapping() );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    void remove_vertex_duplication( const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder )
    {
        ::remove_vertex_duplication( mesh, builder );
    }

    template < index_t dimension >
    void remove_vertex_duplication( const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder )
    {
        ::remove_vertex_duplication( mesh, builder );
    }

    template < index_t dimension >
    void remove_vertex_duplication( const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder )
    {
        ::remove_vertex_duplication( mesh, builder );
    }

    void remove_vertex_duplication(
        const SolidMesh3D& mesh, SolidMeshBuilder3D& builder )
    {
        ::remove_vertex_duplication( mesh, builder );
    }

    template void opengeode_mesh_api remove_vertex_duplication(
        const PointSet2D&, PointSetBuilder2D& );
    template void opengeode_mesh_api remove_vertex_duplication(
        const PointSet3D&, PointSetBuilder3D& );

    template void opengeode_mesh_api remove_vertex_duplication(
        const EdgedCurve2D&, EdgedCurveBuilder2D& );
    template void opengeode_mesh_api remove_vertex_duplication(
        const EdgedCurve3D&, EdgedCurveBuilder3D& );

    template void opengeode_mesh_api remove_vertex_duplication(
        const SurfaceMesh2D&, SurfaceMeshBuilder2D& );
    template void opengeode_mesh_api remove_vertex_duplication(
        const SurfaceMesh3D&, SurfaceMeshBuilder3D& );

} // namespace geode
