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

#include <memory>

#include <geode/model/helpers/convert_to_mesh.h>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>
#include <geode/basic/mapping.h>
#include <geode/basic/range.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_facets.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_edges.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/detail/create_mesh.h>

#include <geode/model/helpers/component_mesh_edges.h>
#include <geode/model/helpers/component_mesh_polygons.h>
#include <geode/model/helpers/component_mesh_vertices.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < typename Model >
    void map_corner_vertices(
        Model& model, geode::ModelToMeshMappings& model2mesh )
    {
        for( const auto& corner : model.corners() )
        {
            const auto corner_uv =
                model.unique_vertex( { corner.component_id(), 0 } );
            const auto solid_uv =
                model2mesh.unique_vertices_mapping.in2out( corner_uv );
            model2mesh.corner_vertices_mapping.map(
                { corner.id(), 0 }, solid_uv );
        }
    }

    template < typename Model >
    void build_edges_from_model( const Model& model,
        geode::ModelToMeshMappings& model2mesh,
        geode::EdgedCurveBuilder< Model::dim >& mesh_builder )
    {
        for( const auto& line : model.lines() )
        {
            const auto& line_mesh = line.mesh();
            for( const auto& edge_id : geode::Range{ line_mesh.nb_edges() } )
            {
                std::array< geode::index_t, 2 > vertices;
                for( const auto edge_vertex_id : geode::LRange{ 2 } )
                {
                    const auto vertex =
                        line_mesh.edge_vertex( { edge_id, edge_vertex_id } );
                    const auto unique_vertex =
                        model.unique_vertex( { line.component_id(), vertex } );
                    if( model2mesh.unique_vertices_mapping.has_mapping_input(
                            unique_vertex ) )
                    {
                        vertices[edge_vertex_id] =
                            model2mesh.unique_vertices_mapping.in2out(
                                unique_vertex );
                        continue;
                    }
                    const auto new_vertex_index =
                        mesh_builder.create_point( line_mesh.point( vertex ) );
                    vertices[edge_vertex_id] = new_vertex_index;
                    model2mesh.unique_vertices_mapping.map(
                        unique_vertex, new_vertex_index );
                }
                const auto edge_index =
                    mesh_builder.create_edge( vertices[0], vertices[1] );
                model2mesh.line_edges_mapping.map(
                    { line.id(), edge_id }, edge_index );
            }
        }
    }

    template < typename Model >
    std::tuple< std::unique_ptr< geode::EdgedCurve< Model::dim > >,
        geode::ModelToMeshMappings >
        convert_model_into_curve( const Model& model )
    {
        geode::ModelToMeshMappings model2mesh;
        std::vector<
            std::reference_wrapper< const geode::EdgedCurve< Model::dim > > >
            meshes;
        meshes.reserve( model.nb_lines() );
        for( const auto& line : model.lines() )
        {
            meshes.emplace_back( line.mesh() );
        }
        auto mesh =
            geode::detail::create_mesh< geode::EdgedCurve< Model::dim > >(
                meshes );
        auto mesh_builder =
            geode::EdgedCurveBuilder< Model::dim >::create( *mesh );
        build_edges_from_model( model, model2mesh, *mesh_builder );
        map_corner_vertices( model, model2mesh );
        return std::make_pair( std::move( mesh ), std::move( model2mesh ) );
    }

    template < geode::index_t dim >
    void set_polygons_surface_adjacencies(
        const absl::FixedArray< geode::index_t >& polygons,
        const geode::SurfaceMesh< dim >& surface_mesh,
        geode::SurfaceMeshBuilder< dim >& mesh_builder )
    {
        for( const auto polygon_id :
            geode::Range{ surface_mesh.nb_polygons() } )
        {
            for( const auto edge_id :
                geode::LRange{ surface_mesh.nb_polygon_edges( polygon_id ) } )
            {
                if( const auto adj = surface_mesh.polygon_adjacent(
                        { polygon_id, edge_id } ) )
                {
                    mesh_builder.set_polygon_adjacent(
                        { polygons[polygon_id], edge_id },
                        polygons[adj.value()] );
                }
            }
        }
    }

    template < typename Model >
    void build_polygons_from_model( const Model& model,
        geode::SurfaceMeshBuilder< Model::dim >& mesh_builder,
        geode::ModelToMeshMappings& model2mesh )
    {
        for( const auto& surface : model.surfaces() )
        {
            const auto& surface_mesh = surface.mesh();
            absl::FixedArray< geode::index_t > polygons(
                surface_mesh.nb_polygons() );
            for( const auto polygon_id :
                geode::Range{ surface_mesh.nb_polygons() } )
            {
                absl::FixedArray< geode::index_t > polygon_vertices(
                    surface_mesh.nb_polygon_vertices( polygon_id ) );
                for( const auto polygon_vertex :
                    geode::LIndices{ polygon_vertices } )
                {
                    const auto vertex = surface_mesh.polygon_vertex(
                        { polygon_id, polygon_vertex } );
                    const auto unique_vertex = model.unique_vertex(
                        { surface.component_id(), vertex } );
                    if( model2mesh.unique_vertices_mapping.has_mapping_input(
                            unique_vertex ) )
                    {
                        polygon_vertices[polygon_vertex] =
                            model2mesh.unique_vertices_mapping.in2out(
                                unique_vertex );
                        continue;
                    }

                    const auto new_vertex_index = mesh_builder.create_point(
                        surface_mesh.point( vertex ) );
                    polygon_vertices[polygon_vertex] = new_vertex_index;
                    model2mesh.unique_vertices_mapping.map(
                        unique_vertex, new_vertex_index );
                }
                polygons[polygon_id] =
                    mesh_builder.create_polygon( polygon_vertices );
                model2mesh.surface_polygons_mapping.map(
                    { surface.id(), polygon_id }, polygons[polygon_id] );
            }
            set_polygons_surface_adjacencies< Model::dim >(
                polygons, surface_mesh, mesh_builder );
        }
    }

    template < typename Model, typename MeshType >
    void map_line_edges(
        Model& model, geode::ModelToMeshMappings& model2mesh, MeshType& mesh )
    {
        mesh.enable_edges();
        for( const auto& line : model.lines() )
        {
            const auto& line_mesh = line.mesh();
            for( const auto line_edge : geode::Range{ line_mesh.nb_edges() } )
            {
                auto unique_vertices =
                    geode::edge_unique_vertices( model, line, line_edge );
                for( const auto edge_vertex_id :
                    geode::LRange{ unique_vertices.size() } )
                {
                    unique_vertices[edge_vertex_id] =
                        model2mesh.unique_vertices_mapping.in2out(
                            unique_vertices[edge_vertex_id] );
                }
                const auto solid_edge =
                    mesh.edges().edge_from_vertices( unique_vertices ).value();
                model2mesh.line_edges_mapping.map(
                    { line.id(), line_edge }, solid_edge );
            }
        }
    }

    template < typename Model >
    std::tuple< std::unique_ptr< geode::SurfaceMesh< Model::dim > >,
        geode::ModelToMeshMappings >
        convert_model_into_surface( const Model& model )
    {
        std::vector<
            std::reference_wrapper< const geode::SurfaceMesh< Model::dim > > >
            meshes;
        meshes.reserve( model.nb_surfaces() );
        for( const auto& surface : model.surfaces() )
        {
            meshes.emplace_back( surface.mesh() );
        }
        auto mesh =
            geode::detail::create_mesh< geode::SurfaceMesh< Model::dim > >(
                meshes );
        auto mesh_builder =
            geode::SurfaceMeshBuilder< Model::dim >::create( *mesh );
        geode::ModelToMeshMappings model2mesh;
        build_polygons_from_model( model, *mesh_builder, model2mesh );
        mesh_builder->compute_polygon_adjacencies();
        map_line_edges( model, model2mesh, *mesh );
        map_corner_vertices( model, model2mesh );
        return std::make_pair( std::move( mesh ), std::move( model2mesh ) );
    }

    void set_block_polyhedra_adjacencies(
        const absl::FixedArray< geode::index_t >& polyhedra,
        const geode::SolidMesh3D& block_mesh,
        geode::SolidMeshBuilder3D& mesh_builder )
    {
        for( const auto polyhedron_id :
            geode::Range{ block_mesh.nb_polyhedra() } )
        {
            for( const auto polyhedron_facet : geode::LRange{
                     block_mesh.nb_polyhedron_facets( polyhedron_id ) } )
            {
                if( const auto adj = block_mesh.polyhedron_adjacent(
                        { polyhedron_id, polyhedron_facet } ) )
                {
                    mesh_builder.set_polyhedron_adjacent(
                        { polyhedra[polyhedron_id], polyhedron_facet },
                        polyhedra[adj.value()] );
                }
            }
        }
    }

    void build_polyhedra_from_model( const geode::BRep& brep,
        geode::SolidMeshBuilder3D& mesh_builder,
        geode::ModelToMeshMappings& brep2mesh )
    {
        for( const auto& block : brep.blocks() )
        {
            const auto& block_mesh = block.mesh();
            absl::FixedArray< geode::index_t > polyhedra(
                block_mesh.nb_polyhedra() );
            for( const auto polyhedron_id :
                geode::Range{ block_mesh.nb_polyhedra() } )
            {
                absl::FixedArray< geode::index_t > polyhedron_vertices(
                    block_mesh.nb_polyhedron_vertices( polyhedron_id ) );
                for( const auto polyhedron_vertex : geode::LRange{
                         block_mesh.nb_polyhedron_vertices( polyhedron_id ) } )
                {
                    const auto vertex = block_mesh.polyhedron_vertex(
                        { polyhedron_id, polyhedron_vertex } );
                    const auto unique_vertex =
                        brep.unique_vertex( { block.component_id(), vertex } );
                    if( brep2mesh.unique_vertices_mapping.has_mapping_input(
                            unique_vertex ) )
                    {
                        polyhedron_vertices[polyhedron_vertex] =
                            brep2mesh.unique_vertices_mapping.in2out(
                                unique_vertex );
                        mesh_builder.set_point(
                            polyhedron_vertices[polyhedron_vertex],
                            block_mesh.point( vertex ) );
                        continue;
                    }
                    auto new_vertex_index =
                        mesh_builder.create_point( block_mesh.point( vertex ) );
                    polyhedron_vertices[polyhedron_vertex] = new_vertex_index;
                    brep2mesh.unique_vertices_mapping.map(
                        unique_vertex, new_vertex_index );
                }
                absl::FixedArray< std::vector< geode::local_index_t > >
                    polyhedron_facet_vertices(
                        block_mesh.nb_polyhedron_facets( polyhedron_id ) );
                for( const auto polyhedron_facet : geode::LRange{
                         block_mesh.nb_polyhedron_facets( polyhedron_id ) } )
                {
                    auto& facet_vertices =
                        polyhedron_facet_vertices[polyhedron_facet];
                    facet_vertices.resize(
                        block_mesh.nb_polyhedron_facet_vertices(
                            { polyhedron_id, polyhedron_facet } ) );
                    for( const auto polyhedron_facet_vertex :
                        geode::LRange{ block_mesh.nb_polyhedron_facet_vertices(
                            { polyhedron_id, polyhedron_facet } ) } )
                    {
                        const auto vertex = block_mesh.polyhedron_facet_vertex(
                            { { polyhedron_id, polyhedron_facet },
                                polyhedron_facet_vertex } );
                        facet_vertices[polyhedron_facet_vertex] =
                            block_mesh
                                .vertex_in_polyhedron( polyhedron_id, vertex )
                                .value();
                    }
                }
                polyhedra[polyhedron_id] = mesh_builder.create_polyhedron(
                    polyhedron_vertices, polyhedron_facet_vertices );
                brep2mesh.solid_polyhedra_mapping.map(
                    { block.id(), polyhedron_id }, polyhedra[polyhedron_id] );
            }
            set_block_polyhedra_adjacencies(
                polyhedra, block_mesh, mesh_builder );
        }
    }

    void map_polygons_to_solid_facets( const geode::BRep& brep,
        geode::ModelToMeshMappings& brep2mesh,
        geode::SolidMesh3D& mesh )
    {
        mesh.enable_facets();
        for( const auto& surface : brep.surfaces() )
        {
            const auto& surface_mesh = surface.mesh();
            for( const auto surface_polygon :
                geode::Range{ surface_mesh.nb_polygons() } )
            {
                auto unique_vertices = geode::polygon_unique_vertices(
                    brep, surface, surface_polygon );
                for( const auto polygon_vertex_id :
                    geode::LRange{ unique_vertices.size() } )
                {
                    unique_vertices[polygon_vertex_id] =
                        brep2mesh.unique_vertices_mapping.in2out(
                            unique_vertices[polygon_vertex_id] );
                }
                const auto solid_facet =
                    mesh.facets()
                        .facet_from_vertices( unique_vertices )
                        .value();
                brep2mesh.surface_polygons_mapping.map(
                    { surface.id(), surface_polygon }, solid_facet );
            }
        }
    }
} // namespace

namespace geode
{
    std::tuple< std::unique_ptr< EdgedCurve< 2 > >, ModelToMeshMappings >
        convert_section_into_curve( const Section& section )
    {
        return convert_model_into_curve( section );
    }

    std::tuple< std::unique_ptr< EdgedCurve< 3 > >, ModelToMeshMappings >
        convert_brep_into_curve( const geode::BRep& brep )
    {
        return convert_model_into_curve( brep );
    }

    std::tuple< std::unique_ptr< SurfaceMesh2D >, ModelToMeshMappings >
        convert_section_into_surface( const Section& section )
    {
        return convert_model_into_surface( section );
    }

    std::tuple< std::unique_ptr< SurfaceMesh3D >, ModelToMeshMappings >
        convert_brep_into_surface( const BRep& brep )
    {
        return convert_model_into_surface( brep );
    }

    std::tuple< std::unique_ptr< SolidMesh3D >, ModelToMeshMappings >
        convert_brep_into_solid( const BRep& brep )
    {
        std::vector< std::reference_wrapper< const geode::SolidMesh3D > >
            meshes;
        meshes.reserve( brep.nb_blocks() );
        for( const auto& block : brep.blocks() )
        {
            meshes.emplace_back( block.mesh() );
        }
        auto mesh = geode::detail::create_mesh< SolidMesh3D >( meshes );
        auto mesh_builder = geode::SolidMeshBuilder< 3 >::create( *mesh );
        ModelToMeshMappings brep2mesh;
        for( const auto unique_vertex :
            geode::Range{ brep.nb_unique_vertices() } )
        {
            const auto new_vertex_index = mesh_builder->create_vertex();
            brep2mesh.unique_vertices_mapping.map(
                unique_vertex, new_vertex_index );
        }
        build_polyhedra_from_model( brep, *mesh_builder, brep2mesh );
        if( mesh->nb_polyhedra() != 0 )
        {
            mesh_builder->compute_polyhedron_adjacencies();
            map_polygons_to_solid_facets( brep, brep2mesh, *mesh );
            map_line_edges( brep, brep2mesh, *mesh );
            map_corner_vertices( brep, brep2mesh );
        }
        return std::make_pair( std::move( mesh ), std::move( brep2mesh ) );
    }
} // namespace geode
