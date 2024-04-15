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
    class FromModel
    {
        static constexpr auto dimension = Model::dim;

    public:
        FromModel( const Model& model ) : model_( model )
        {
            OPENGEODE_EXCEPTION( model.nb_unique_vertices() > 0,
                "[Convert Model to Mesh(es)] Given model should have unique "
                "vertices" );
            for( const auto uv : geode::Range{ model.nb_unique_vertices() } )
            {
                OPENGEODE_EXCEPTION(
                    !model.component_mesh_vertices( uv ).empty(),
                    "[Convert Model to Mesh(es)] At least one unique vertex of "
                    "the given model is isolated. You should clean your model, "
                    "before calling conversion functions, using "
                    "BRepBuilder::delete_isolated_vertices() or "
                    "SectionBuilder::::delete_isolated_vertices()" );
            }
        }

        const Model& model() const
        {
            return model_;
        }

        absl::optional< geode::index_t > vertex(
            geode::index_t vertex_id ) const
        {
            const auto it = vertices_.find( vertex_id );
            if( it != vertices_.end() )
            {
                return it->second;
            }
            return absl::nullopt;
        }

        geode::index_t create_vertex( geode::index_t vertex_id )
        {
            OPENGEODE_EXCEPTION( vertex_id != geode::NO_ID,
                "[Convert Model to Mesh(es)] At least one Component Mesh "
                "Vertex is not link to a unique vertex" );
            const geode::index_t new_id = vertices_.size();
            vertices_.emplace( vertex_id, new_id );
            return new_id;
        }

        template < typename Mesh >
        std::vector< geode::Point< dimension > > points(
            const Mesh& mesh ) const
        {
            std::vector< geode::Point< dimension > > points( vertices_.size() );
            for( const auto& v : vertices_ )
            {
                points[v.second] = mesh.point( v.second );
            }
            return points;
        }

        const absl::flat_hash_map< geode::index_t, geode::index_t >&
            vertices() const
        {
            return vertices_;
        }

    private:
        const Model& model_;
        absl::flat_hash_map< geode::index_t, geode::index_t > vertices_;
    };

    template < typename SolidType >
    class SolidFromBRep
    {
    public:
        SolidFromBRep( FromModel< geode::BRep >& model,
            const geode::SurfaceMesh3D& surface )
            : SolidFromBRep( model, true )
        {
            for( const auto& p : model_.points( surface ) )
            {
                builder_->create_point( p );
            }
        }

        SolidFromBRep( FromModel< geode::BRep >& model )
            : SolidFromBRep( model, true )
        {
            const auto& brep = model_.model();
            builder_->create_vertices( brep.nb_unique_vertices() );
            for( const auto& block : brep.blocks() )
            {
                const auto& mesh = block.mesh();
                for( const auto v : geode::Range{ mesh.nb_vertices() } )
                {
                    const auto vertex =
                        brep.unique_vertex( { block.component_id(), v } );
                    builder_->set_point( vertex, mesh.point( v ) );
                }
            }
            for( const auto v : geode::Range{ brep.nb_unique_vertices() } )
            {
                model_.create_vertex( v );
            }
        }

        void build_mesh()
        {
            for( const auto& block : model_.model().blocks() )
            {
                const auto polyhedra = build_polyhedra( block );
                set_polyhedra_adjacency( block, polyhedra );
            }
            for( const auto& uv2v : model_.vertices() )
            {
                attribute_unique_vertex_->set_value( uv2v.second, uv2v.first );
            }
        }

        std::unique_ptr< SolidType > get_result()
        {
            return std::move( mesh_ );
        }

    private:
        SolidFromBRep( FromModel< geode::BRep >& model, bool /*unused*/ )
            : model_( model ),
              mesh_{ SolidType::create() },
              builder_{ geode::SolidMeshBuilder3D::create( *mesh_ ) },
              attribute_uuid_{
                  mesh_->polyhedron_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::uuid_from_conversion_attribute_type >(
                          geode::uuid_from_conversion_attribute_name, {} )
              },
              attribute_unique_vertex_{
                  mesh_->vertex_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::unique_vertex_from_conversion_attribute_type >(
                          geode::unique_vertex_from_conversion_attribute_name,
                          geode::NO_ID )
              },
              mesh_element_mapping_{
                  mesh_->polyhedron_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::mesh_elements_attribute_type >(
                          geode::MESH_ELEMENT_ATTRIBUTE_NAME,
                          { {}, geode::NO_ID } )
              }
        {
        }

        void set_polyhedra_adjacency( const geode::Block3D& block,
            const absl::FixedArray< geode::index_t >& polyhedra )
        {
            const auto& mesh = block.mesh();
            for( const auto p : geode::Range{ mesh.nb_polyhedra() } )
            {
                for( const auto f :
                    geode::LRange{ mesh.nb_polyhedron_facets( p ) } )
                {
                    if( const auto adj = mesh.polyhedron_adjacent( { p, f } ) )
                    {
                        builder_->set_polyhedron_adjacent(
                            { polyhedra[p], f }, polyhedra[adj.value()] );
                    }
                }
            }
        }

        absl::FixedArray< geode::index_t > build_polyhedra(
            const geode::Block3D& block )
        {
            const auto& mesh = block.mesh();
            absl::FixedArray< geode::index_t > polyhedra( mesh.nb_polyhedra() );
            for( const auto p : geode::Range{ mesh.nb_polyhedra() } )
            {
                absl::FixedArray< geode::index_t > polyhedron_vertices(
                    mesh.nb_polyhedron_vertices( p ) );
                for( const auto v :
                    geode::LRange{ mesh.nb_polyhedron_vertices( p ) } )
                {
                    const auto vertex = mesh.polyhedron_vertex( { p, v } );
                    const auto unique_vertex = model_.model().unique_vertex(
                        { block.component_id(), vertex } );
                    if( const auto vertex_id = model_.vertex( unique_vertex ) )
                    {
                        polyhedron_vertices[v] = vertex_id.value();
                    }
                    else
                    {
                        polyhedron_vertices[v] =
                            model_.create_vertex( unique_vertex );
                        builder_->create_point( mesh.point( vertex ) );
                    }
                }
                absl::FixedArray< std::vector< geode::local_index_t > >
                    polyhedron_facet_vertices( mesh.nb_polyhedron_facets( p ) );
                for( const auto f :
                    geode::LRange{ mesh.nb_polyhedron_facets( p ) } )
                {
                    auto& facet_vertices = polyhedron_facet_vertices[f];
                    facet_vertices.resize(
                        mesh.nb_polyhedron_facet_vertices( { p, f } ) );
                    for( const auto v : geode::LRange{
                             mesh.nb_polyhedron_facet_vertices( { p, f } ) } )
                    {
                        const auto vertex =
                            mesh.polyhedron_facet_vertex( { { p, f }, v } );
                        facet_vertices[v] =
                            mesh.vertex_in_polyhedron( p, vertex ).value();
                    }
                }
                polyhedra[p] = builder_->create_polyhedron(
                    polyhedron_vertices, polyhedron_facet_vertices );
                attribute_uuid_->set_value( polyhedra[p], block.id() );
                mesh_element_mapping_->set_value(
                    polyhedra[p], { block.id(), p } );
            }
            return polyhedra;
        }

    private:
        FromModel< geode::BRep >& model_;
        std::unique_ptr< SolidType > mesh_;
        std::unique_ptr< geode::SolidMeshBuilder3D > builder_;
        std::shared_ptr< geode::VariableAttribute<
            geode::uuid_from_conversion_attribute_type > >
            attribute_uuid_;
        std::shared_ptr< geode::VariableAttribute<
            geode::unique_vertex_from_conversion_attribute_type > >
            attribute_unique_vertex_;
        std::shared_ptr<
            geode::VariableAttribute< geode::mesh_elements_attribute_type > >
            mesh_element_mapping_;
    };

    template < typename SurfaceType, typename Model >
    class SurfaceFromModel
    {
        static constexpr auto dimension = Model::dim;

    public:
        SurfaceFromModel( FromModel< Model >& model )
            : model_( model ),
              mesh_{ SurfaceType::create() },
              builder_{ geode::SurfaceMeshBuilder< dimension >::create(
                  *mesh_ ) },
              attribute_uuid_{
                  mesh_->polygon_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::uuid_from_conversion_attribute_type >(
                          geode::uuid_from_conversion_attribute_name, {} )
              },
              attribute_unique_vertex_{
                  mesh_->vertex_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::unique_vertex_from_conversion_attribute_type >(
                          geode::unique_vertex_from_conversion_attribute_name,
                          geode::NO_ID )
              },
              mesh_element_mapping_{
                  mesh_->polygon_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::mesh_elements_attribute_type >(
                          geode::MESH_ELEMENT_ATTRIBUTE_NAME,
                          { {}, geode::NO_ID } )
              }
        {
        }

        SurfaceFromModel( FromModel< Model >& model,
            const geode::EdgedCurve< dimension >& curve )
            : SurfaceFromModel( model )
        {
            for( const auto& p : model_.points( curve ) )
            {
                builder_->create_point( p );
            }
        }

        void build_mesh()
        {
            for( const auto& surface : model_.model().surfaces() )
            {
                const auto polygons = build_polygons( surface );
                set_polygons_adjacency( surface, polygons );
            }
            for( const auto& uv2v : model_.vertices() )
            {
                attribute_unique_vertex_->set_value( uv2v.second, uv2v.first );
            }
        }

        std::unique_ptr< SurfaceType > get_result()
        {
            return std::move( mesh_ );
        }

    private:
        void set_polygons_adjacency( const geode::Surface< dimension >& surface,
            const absl::FixedArray< geode::index_t >& polygons )
        {
            const auto& mesh = surface.mesh();
            for( const auto p : geode::Range{ mesh.nb_polygons() } )
            {
                for( const auto e :
                    geode::LRange{ mesh.nb_polygon_edges( p ) } )
                {
                    if( const auto adj = mesh.polygon_adjacent( { p, e } ) )
                    {
                        builder_->set_polygon_adjacent(
                            { polygons[p], e }, polygons[adj.value()] );
                    }
                }
            }
        }

        absl::FixedArray< geode::index_t > build_polygons(
            const geode::Surface< dimension >& surface )
        {
            const auto& mesh = surface.mesh();
            absl::FixedArray< geode::index_t > polygons( mesh.nb_polygons() );
            for( const auto p : geode::Range{ mesh.nb_polygons() } )
            {
                absl::FixedArray< geode::index_t > polygon(
                    mesh.nb_polygon_vertices( p ) );
                for( const auto v :
                    geode::LRange{ mesh.nb_polygon_vertices( p ) } )
                {
                    const auto vertex = mesh.polygon_vertex( { p, v } );
                    const auto unique_vertex = model_.model().unique_vertex(
                        { surface.component_id(), vertex } );
                    if( const auto vertex_id = model_.vertex( unique_vertex ) )
                    {
                        polygon[v] = vertex_id.value();
                    }
                    else
                    {
                        polygon[v] = model_.create_vertex( unique_vertex );
                        builder_->create_point( mesh.point( vertex ) );
                    }
                }
                polygons[p] = builder_->create_polygon( polygon );
                attribute_uuid_->set_value( polygons[p], surface.id() );
                mesh_element_mapping_->set_value(
                    polygons[p], { surface.id(), p } );
            }
            return polygons;
        }

    private:
        FromModel< Model >& model_;
        std::unique_ptr< SurfaceType > mesh_;
        std::unique_ptr< geode::SurfaceMeshBuilder< dimension > > builder_;
        std::shared_ptr< geode::VariableAttribute<
            geode::uuid_from_conversion_attribute_type > >
            attribute_uuid_;
        std::shared_ptr< geode::VariableAttribute<
            geode::unique_vertex_from_conversion_attribute_type > >
            attribute_unique_vertex_;
        std::shared_ptr<
            geode::VariableAttribute< geode::mesh_elements_attribute_type > >
            mesh_element_mapping_;
    };

    template < typename SurfaceType >
    using SurfaceFromBRep = SurfaceFromModel< SurfaceType, geode::BRep >;
    template < typename SurfaceType >
    using SurfaceFromSection = SurfaceFromModel< SurfaceType, geode::Section >;

    template < typename Model >
    class CurveFromModel
    {
        static constexpr auto dimension = Model::dim;

    public:
        CurveFromModel( FromModel< Model >& model )
            : model_( model ),
              mesh_{ geode::EdgedCurve< dimension >::create() },
              builder_{ geode::EdgedCurveBuilder< dimension >::create(
                  *mesh_ ) },
              attribute_uuid_{
                  mesh_->edge_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::uuid_from_conversion_attribute_type >(
                          geode::uuid_from_conversion_attribute_name, {} )
              },
              attribute_unique_vertex_{
                  mesh_->vertex_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::unique_vertex_from_conversion_attribute_type >(
                          geode::unique_vertex_from_conversion_attribute_name,
                          geode::NO_ID )
              },
              mesh_element_mapping_{
                  mesh_->edge_attribute_manager()
                      .template find_or_create_attribute<
                          geode::VariableAttribute,
                          geode::mesh_elements_attribute_type >(
                          geode::MESH_ELEMENT_ATTRIBUTE_NAME,
                          { {}, geode::NO_ID } )
              }
        {
        }

        void build_mesh()
        {
            for( const auto& line : model_.model().lines() )
            {
                build_edges( line );
            }
            for( const auto& uv2v : model_.vertices() )
            {
                attribute_unique_vertex_->set_value( uv2v.second, uv2v.first );
            }
        }

        std::unique_ptr< geode::EdgedCurve< dimension > > get_result()
        {
            return std::move( mesh_ );
        }

    private:
        void build_edges( const geode::Line< dimension >& line )
        {
            const auto& mesh = line.mesh();
            for( const auto e : geode::Range{ mesh.nb_edges() } )
            {
                std::array< geode::index_t, 2 > vertices;
                for( const auto v : geode::LRange{ 2 } )
                {
                    const auto vertex = mesh.edge_vertex( { e, v } );
                    const auto unique_vertex = model_.model().unique_vertex(
                        { line.component_id(), vertex } );
                    if( const auto vertex_id = model_.vertex( unique_vertex ) )
                    {
                        vertices[v] = vertex_id.value();
                    }
                    else
                    {
                        vertices[v] = model_.create_vertex( unique_vertex );
                        builder_->create_point( mesh.point( vertex ) );
                    }
                }
                const auto edge =
                    builder_->create_edge( vertices[0], vertices[1] );
                attribute_uuid_->set_value( edge, line.id() );
                mesh_element_mapping_->set_value( edge, { line.id(), e } );
            }
        }

    private:
        FromModel< Model >& model_;
        std::unique_ptr< geode::EdgedCurve< dimension > > mesh_;
        std::unique_ptr< geode::EdgedCurveBuilder< dimension > > builder_;
        std::shared_ptr< geode::VariableAttribute<
            geode::uuid_from_conversion_attribute_type > >
            attribute_uuid_;
        std::shared_ptr< geode::VariableAttribute<
            geode::unique_vertex_from_conversion_attribute_type > >
            attribute_unique_vertex_;
        std::shared_ptr<
            geode::VariableAttribute< geode::mesh_elements_attribute_type > >
            mesh_element_mapping_;
    };

    using CurveFromBRep = CurveFromModel< geode::BRep >;
    using CurveFromSection = CurveFromModel< geode::Section >;

    template < typename Builder,
        typename Mesh,
        typename... Args,
        typename Model >
    std::unique_ptr< Mesh > build_mesh( Model& model, const Args&... args )
    {
        Builder builder{ model, args... };
        builder.build_mesh();
        return builder.get_result();
    }

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
            for( const auto& e : geode::Range{ line_mesh.nb_edges() } )
            {
                std::array< geode::index_t, 2 > vertices;
                for( const auto v : geode::LRange{ 2 } )
                {
                    const auto vertex = line_mesh.edge_vertex( { e, v } );
                    const auto unique_vertex =
                        model.unique_vertex( { line.component_id(), vertex } );
                    if( model2mesh.unique_vertices_mapping.has_mapping_input(
                            unique_vertex ) )
                    {
                        vertices[v] = model2mesh.unique_vertices_mapping.in2out(
                            unique_vertex );
                    }
                    else
                    {
                        const auto new_vertex_index = mesh_builder.create_point(
                            line_mesh.point( vertex ) );
                        vertices[v] = new_vertex_index;
                        model2mesh.unique_vertices_mapping.map(
                            unique_vertex, new_vertex_index );
                    }
                }
                const auto edge_index =
                    mesh_builder.create_edge( vertices[0], vertices[1] );
                model2mesh.line_edges_mapping.map(
                    { line.id(), e }, edge_index );
            }
        }
    }

    template < typename Model >
    std::tuple< std::unique_ptr< geode::EdgedCurve< Model::dim > >,
        geode::ModelToMeshMappings >
        new_convert_model_into_curve( const Model& model )
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
                std::move( meshes ) );
        auto mesh_builder =
            geode::EdgedCurveBuilder< Model::dim >::create( *mesh );
        build_edges_from_model( model, model2mesh, *mesh_builder );
        map_corner_vertices( model, model2mesh );
        return std::make_pair( std::move( mesh ), std::move( model2mesh ) );
    }

    template < geode::index_t dim >
    void compute_polygons_surface_adjacencies(
        const absl::FixedArray< geode::index_t >& polygons,
        const geode::SurfaceMesh< dim >& surface_mesh,
        geode::SurfaceMeshBuilder< dim >& mesh_builder )
    {
        for( const auto p : geode::Range{ surface_mesh.nb_polygons() } )
        {
            for( const auto e :
                geode::LRange{ surface_mesh.nb_polygon_edges( p ) } )
            {
                if( const auto adj = surface_mesh.polygon_adjacent( { p, e } ) )
                {
                    mesh_builder.set_polygon_adjacent(
                        { polygons[p], e }, polygons[adj.value()] );
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
            for( const auto p : geode::Range{ surface_mesh.nb_polygons() } )
            {
                absl::FixedArray< geode::index_t > polygon(
                    surface_mesh.nb_polygon_vertices( p ) );
                for( const auto v :
                    geode::LRange{ surface_mesh.nb_polygon_vertices( p ) } )
                {
                    const auto vertex = surface_mesh.polygon_vertex( { p, v } );
                    const auto unique_vertex = model.unique_vertex(
                        { surface.component_id(), vertex } );
                    if( model2mesh.unique_vertices_mapping.has_mapping_input(
                            unique_vertex ) )
                    {
                        polygon[v] = model2mesh.unique_vertices_mapping.in2out(
                            unique_vertex );
                    }
                    else
                    {
                        const auto new_vertex_index = mesh_builder.create_point(
                            surface_mesh.point( vertex ) );
                        polygon[v] = new_vertex_index;
                        model2mesh.unique_vertices_mapping.map(
                            unique_vertex, new_vertex_index );
                    }
                }
                polygons[p] = mesh_builder.create_polygon( polygon );
                model2mesh.surface_polygons_mapping.map(
                    { surface.id(), p }, polygons[p] );
            }
            compute_polygons_surface_adjacencies< Model::dim >(
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
        new_convert_model_into_surface( const Model& model )
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
        map_line_edges( model, model2mesh, *mesh );
        map_corner_vertices( model, model2mesh );
        mesh_builder->compute_polygon_adjacencies();
        return std::make_pair( std::move( mesh ), std::move( model2mesh ) );
    }

    void compute_block_polyhedra_adjacencies(
        const absl::FixedArray< geode::index_t >& polyhedra,
        const geode::SolidMesh3D& block_mesh,
        geode::SolidMeshBuilder3D& mesh_builder )
    {
        for( const auto p : geode::Range{ block_mesh.nb_polyhedra() } )
        {
            for( const auto f :
                geode::LRange{ block_mesh.nb_polyhedron_facets( p ) } )
            {
                if( const auto adj =
                        block_mesh.polyhedron_adjacent( { p, f } ) )
                {
                    mesh_builder.set_polyhedron_adjacent(
                        { polyhedra[p], f }, polyhedra[adj.value()] );
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
            for( const auto p : geode::Range{ block_mesh.nb_polyhedra() } )
            {
                absl::FixedArray< geode::index_t > polyhedron_vertices(
                    block_mesh.nb_polyhedron_vertices( p ) );
                for( const auto v :
                    geode::LRange{ block_mesh.nb_polyhedron_vertices( p ) } )
                {
                    const auto vertex =
                        block_mesh.polyhedron_vertex( { p, v } );
                    const auto unique_vertex =
                        brep.unique_vertex( { block.component_id(), vertex } );
                    if( brep2mesh.unique_vertices_mapping.has_mapping_input(
                            unique_vertex ) )
                    {
                        polyhedron_vertices[v] =
                            brep2mesh.unique_vertices_mapping.in2out(
                                unique_vertex );
                        mesh_builder.set_point( polyhedron_vertices[v],
                            block_mesh.point( vertex ) );
                    }
                    else
                    {
                        auto new_vertex_index = mesh_builder.create_point(
                            block_mesh.point( vertex ) );
                        polyhedron_vertices[v] = new_vertex_index;
                        brep2mesh.unique_vertices_mapping.map(
                            unique_vertex, new_vertex_index );
                    }
                }
                absl::FixedArray< std::vector< geode::local_index_t > >
                    polyhedron_facet_vertices(
                        block_mesh.nb_polyhedron_facets( p ) );
                for( const auto f :
                    geode::LRange{ block_mesh.nb_polyhedron_facets( p ) } )
                {
                    auto& facet_vertices = polyhedron_facet_vertices[f];
                    facet_vertices.resize(
                        block_mesh.nb_polyhedron_facet_vertices( { p, f } ) );
                    for( const auto v :
                        geode::LRange{ block_mesh.nb_polyhedron_facet_vertices(
                            { p, f } ) } )
                    {
                        const auto vertex = block_mesh.polyhedron_facet_vertex(
                            { { p, f }, v } );
                        facet_vertices[v] =
                            block_mesh.vertex_in_polyhedron( p, vertex )
                                .value();
                    }
                }
                polyhedra[p] = mesh_builder.create_polyhedron(
                    polyhedron_vertices, polyhedron_facet_vertices );
                brep2mesh.solid_polyhedra_mapping.map(
                    { block.id(), p }, polyhedra[p] );
            }
            compute_block_polyhedra_adjacencies(
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
    template < typename SolidType >
    std::unique_ptr< SolidType > convert_brep_into_solid( const BRep& brep )
    {
        FromModel< BRep > model{ brep };
        return build_mesh< SolidFromBRep< SolidType >, SolidType >( model );
    }

    template < typename SurfaceType >
    std::unique_ptr< SurfaceType > convert_brep_into_surface( const BRep& brep )
    {
        FromModel< BRep > model{ brep };
        return build_mesh< SurfaceFromBRep< SurfaceType >, SurfaceType >(
            model );
    }

    template < typename SurfaceType >
    std::unique_ptr< SurfaceType > convert_section_into_surface(
        const Section& section )
    {
        FromModel< Section > model{ section };
        return build_mesh< SurfaceFromSection< SurfaceType >, SurfaceType >(
            model );
    }

    std::unique_ptr< EdgedCurve2D > convert_section_into_curve(
        const Section& section )
    {
        FromModel< Section > model{ section };
        return build_mesh< CurveFromSection, EdgedCurve2D >( model );
    }

    std::unique_ptr< EdgedCurve3D > convert_brep_into_curve( const BRep& brep )
    {
        FromModel< BRep > model{ brep };
        return build_mesh< CurveFromBRep, EdgedCurve3D >( model );
    }

    template < typename SurfaceType >
    std::tuple< std::unique_ptr< EdgedCurve2D >,
        std::unique_ptr< SurfaceType > >
        convert_section_into_curve_and_surface( const Section& section )
    {
        FromModel< Section > model{ section };
        auto curve = build_mesh< CurveFromSection, EdgedCurve2D >( model );
        auto surface =
            build_mesh< SurfaceFromSection< SurfaceType >, SurfaceType >(
                model, *curve );
        return std::make_tuple( std::move( curve ), std::move( surface ) );
    }

    template < typename SurfaceType, typename SolidType >
    std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceType >,
        std::unique_ptr< SolidType > >
        convert_brep_into_curve_and_surface_and_solid( const BRep& brep )
    {
        FromModel< BRep > model{ brep };
        auto curve = build_mesh< CurveFromBRep, EdgedCurve3D >( model );
        auto surface =
            build_mesh< SurfaceFromBRep< SurfaceType >, SurfaceType >(
                model, *curve );
        auto solid = build_mesh< SolidFromBRep< SolidType >, SolidType >(
            model, *surface );
        return std::make_tuple(
            std::move( curve ), std::move( surface ), std::move( solid ) );
    }

    template < typename SurfaceType >
    std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceType > >
        convert_brep_into_curve_and_surface( const BRep& brep )
    {
        FromModel< BRep > model{ brep };
        auto curve = build_mesh< CurveFromBRep, EdgedCurve3D >( model );
        auto surface =
            build_mesh< SurfaceFromBRep< SurfaceType >, SurfaceType >(
                model, *curve );
        return std::make_tuple( std::move( curve ), std::move( surface ) );
    }

    template < typename SurfaceType, typename SolidType >
    std::tuple< std::unique_ptr< SurfaceType >, std::unique_ptr< SolidType > >
        convert_brep_into_surface_and_solid( const BRep& brep )
    {
        FromModel< BRep > model{ brep };
        auto surface =
            build_mesh< SurfaceFromBRep< SurfaceType >, SurfaceType >( model );
        auto solid = build_mesh< SolidFromBRep< SolidType >, SolidType >(
            model, *surface );
        return std::make_tuple( std::move( surface ), std::move( solid ) );
    }

    std::tuple< std::unique_ptr< EdgedCurve< 2 > >, ModelToMeshMappings >
        new_convert_section_into_curve( const Section& section )
    {
        return new_convert_model_into_curve( section );
    }

    std::tuple< std::unique_ptr< EdgedCurve< 3 > >, ModelToMeshMappings >
        new_convert_brep_into_curve( const geode::BRep& brep )
    {
        return new_convert_model_into_curve( brep );
    }

    std::tuple< std::unique_ptr< SurfaceMesh3D >, ModelToMeshMappings >
        new_convert_brep_into_surface( const BRep& brep )
    {
        return new_convert_model_into_surface( brep );
    }

    std::tuple< std::unique_ptr< SurfaceMesh2D >, ModelToMeshMappings >
        new_convert_section_into_surface( const Section& section )
    {
        return new_convert_model_into_surface( section );
    }

    std::tuple< std::unique_ptr< SolidMesh3D >, ModelToMeshMappings >
        new_convert_brep_into_solid( const BRep& brep )
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
        map_polygons_to_solid_facets( brep, brep2mesh, *mesh );
        map_line_edges( brep, brep2mesh, *mesh );
        map_corner_vertices( brep, brep2mesh );
        return std::make_pair( std::move( mesh ), std::move( brep2mesh ) );
    }

    template std::unique_ptr< SurfaceMesh2D > opengeode_model_api
        convert_section_into_surface( const Section& section );
    template std::unique_ptr< PolygonalSurface2D > opengeode_model_api
        convert_section_into_surface( const Section& section );
    template std::unique_ptr< TriangulatedSurface2D > opengeode_model_api
        convert_section_into_surface( const Section& section );

    template std::tuple< std::unique_ptr< EdgedCurve2D >,
        std::unique_ptr< SurfaceMesh2D > >
        opengeode_model_api convert_section_into_curve_and_surface(
            const Section& );
    template std::tuple< std::unique_ptr< EdgedCurve2D >,
        std::unique_ptr< PolygonalSurface2D > >
        opengeode_model_api convert_section_into_curve_and_surface(
            const Section& );
    template std::tuple< std::unique_ptr< EdgedCurve2D >,
        std::unique_ptr< TriangulatedSurface2D > >
        opengeode_model_api convert_section_into_curve_and_surface(
            const Section& );

    template std::unique_ptr< SurfaceMesh3D >
        opengeode_model_api convert_brep_into_surface( const BRep& );
    template std::unique_ptr< PolygonalSurface3D >
        opengeode_model_api convert_brep_into_surface( const BRep& );
    template std::unique_ptr< TriangulatedSurface3D >
        opengeode_model_api convert_brep_into_surface( const BRep& );

    template std::unique_ptr< SolidMesh3D >
        opengeode_model_api convert_brep_into_solid( const BRep& );
    template std::unique_ptr< PolyhedralSolid3D >
        opengeode_model_api convert_brep_into_solid( const BRep& );
    template std::unique_ptr< TetrahedralSolid3D >
        opengeode_model_api convert_brep_into_solid( const BRep& );
    template std::unique_ptr< HybridSolid3D >
        opengeode_model_api convert_brep_into_solid( const BRep& );

    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceMesh3D > >
        opengeode_model_api convert_brep_into_curve_and_surface( const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< PolygonalSurface3D > >
        opengeode_model_api convert_brep_into_curve_and_surface( const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< TriangulatedSurface3D > >
        opengeode_model_api convert_brep_into_curve_and_surface( const BRep& );

    template std::tuple< std::unique_ptr< SurfaceMesh3D >,
        std::unique_ptr< SolidMesh3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< SurfaceMesh3D >,
        std::unique_ptr< PolyhedralSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< SurfaceMesh3D >,
        std::unique_ptr< TetrahedralSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< SurfaceMesh3D >,
        std::unique_ptr< HybridSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< PolygonalSurface3D >,
        std::unique_ptr< SolidMesh3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< PolygonalSurface3D >,
        std::unique_ptr< PolyhedralSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< PolygonalSurface3D >,
        std::unique_ptr< TetrahedralSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< PolygonalSurface3D >,
        std::unique_ptr< HybridSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< TriangulatedSurface3D >,
        std::unique_ptr< SolidMesh3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< TriangulatedSurface3D >,
        std::unique_ptr< PolyhedralSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< TriangulatedSurface3D >,
        std::unique_ptr< TetrahedralSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );
    template std::tuple< std::unique_ptr< TriangulatedSurface3D >,
        std::unique_ptr< HybridSolid3D > >
        opengeode_model_api convert_brep_into_surface_and_solid( const BRep& );

    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceMesh3D >,
        std::unique_ptr< SolidMesh3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceMesh3D >,
        std::unique_ptr< PolyhedralSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceMesh3D >,
        std::unique_ptr< TetrahedralSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< SurfaceMesh3D >,
        std::unique_ptr< HybridSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< PolygonalSurface3D >,
        std::unique_ptr< SolidMesh3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< PolygonalSurface3D >,
        std::unique_ptr< PolyhedralSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< PolygonalSurface3D >,
        std::unique_ptr< TetrahedralSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< PolygonalSurface3D >,
        std::unique_ptr< HybridSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< TriangulatedSurface3D >,
        std::unique_ptr< SolidMesh3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< TriangulatedSurface3D >,
        std::unique_ptr< PolyhedralSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< TriangulatedSurface3D >,
        std::unique_ptr< TetrahedralSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
    template std::tuple< std::unique_ptr< EdgedCurve3D >,
        std::unique_ptr< TriangulatedSurface3D >,
        std::unique_ptr< HybridSolid3D > >
        opengeode_model_api convert_brep_into_curve_and_surface_and_solid(
            const BRep& );
} // namespace geode
