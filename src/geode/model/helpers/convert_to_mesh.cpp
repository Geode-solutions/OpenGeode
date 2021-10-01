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

#include <geode/model/helpers/convert_to_mesh.h>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/attribute_manager.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < typename Model, geode::index_t dimension >
    class FromModel
    {
    public:
        FromModel( const Model& model ) : model_( model ) {}

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

    private:
        const Model& model_;
        absl::flat_hash_map< geode::index_t, geode::index_t > vertices_;
    };

    template < typename SolidType >
    class SolidFromBRep
    {
    public:
        SolidFromBRep( FromModel< geode::BRep, 3 >& model )
            : model_( model ),
              mesh_{ SolidType::create() },
              builder_{ geode::SolidMeshBuilder3D::create( *mesh_ ) },
              attribute_{ mesh_->polyhedron_attribute_manager()
                              .template find_or_create_attribute<
                                  geode::VariableAttribute,
                                  geode::uuid_from_conversion_attribute_type >(
                                  geode::uuid_from_conversion_attribute_name,
                                  {} ) }
        {
        }

        SolidFromBRep( FromModel< geode::BRep, 3 >& model,
            const geode::SurfaceMesh3D& surface )
            : SolidFromBRep( model )
        {
            for( const auto& p : model_.points( surface ) )
            {
                builder_->create_point( p );
            }
        }

        void build_mesh()
        {
            for( const auto& block : model_.model().blocks() )
            {
                const auto polyhedra = build_polyhedra( block );
                set_polyhedra_adjacency( block, polyhedra );
            }
        }

        std::unique_ptr< SolidType > get_result()
        {
            return std::move( mesh_ );
        }

    private:
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
                attribute_->set_value( polyhedra[p], block.id() );
            }
            return polyhedra;
        }

    private:
        FromModel< geode::BRep, 3 >& model_;
        std::unique_ptr< SolidType > mesh_;
        std::unique_ptr< geode::SolidMeshBuilder3D > builder_;
        std::shared_ptr< geode::VariableAttribute<
            geode::uuid_from_conversion_attribute_type > >
            attribute_;
    };

    template < typename SurfaceType, typename Model, geode::index_t dimension >
    class SurfaceFromModel
    {
    public:
        SurfaceFromModel( FromModel< Model, dimension >& model )
            : model_( model ),
              mesh_{ SurfaceType::create() },
              builder_{ geode::SurfaceMeshBuilder< dimension >::create(
                  *mesh_ ) },
              attribute_{ mesh_->polygon_attribute_manager()
                              .template find_or_create_attribute<
                                  geode::VariableAttribute,
                                  geode::uuid_from_conversion_attribute_type >(
                                  geode::uuid_from_conversion_attribute_name,
                                  {} ) }
        {
        }

        SurfaceFromModel( FromModel< Model, dimension >& model,
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
                attribute_->set_value( polygons[p], surface.id() );
            }
            return polygons;
        }

    private:
        FromModel< Model, dimension >& model_;
        std::unique_ptr< SurfaceType > mesh_;
        std::unique_ptr< geode::SurfaceMeshBuilder< dimension > > builder_;
        std::shared_ptr< geode::VariableAttribute<
            geode::uuid_from_conversion_attribute_type > >
            attribute_;
    };

    template < typename SurfaceType >
    using SurfaceFromBRep = SurfaceFromModel< SurfaceType, geode::BRep, 3 >;
    template < typename SurfaceType >
    using SurfaceFromSection =
        SurfaceFromModel< SurfaceType, geode::Section, 2 >;

    template < typename Model, geode::index_t dimension >
    class CurveFromModel
    {
    public:
        CurveFromModel( FromModel< Model, dimension >& model )
            : model_( model ),
              mesh_{ geode::EdgedCurve< dimension >::create() },
              builder_{ geode::EdgedCurveBuilder< dimension >::create(
                  *mesh_ ) },
              attribute_{ mesh_->edge_attribute_manager()
                              .template find_or_create_attribute<
                                  geode::VariableAttribute,
                                  geode::uuid_from_conversion_attribute_type >(
                                  geode::uuid_from_conversion_attribute_name,
                                  {} ) }
        {
        }

        void build_mesh()
        {
            for( const auto& line : model_.model().lines() )
            {
                build_edges( line );
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
                attribute_->set_value( edge, line.id() );
            }
        }

    private:
        FromModel< Model, dimension >& model_;
        std::unique_ptr< geode::EdgedCurve< dimension > > mesh_;
        std::unique_ptr< geode::EdgedCurveBuilder< dimension > > builder_;
        std::shared_ptr< geode::VariableAttribute<
            geode::uuid_from_conversion_attribute_type > >
            attribute_;
    };

    using CurveFromBRep = CurveFromModel< geode::BRep, 3 >;
    using CurveFromSection = CurveFromModel< geode::Section, 2 >;

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
} // namespace

namespace geode
{
    template < typename SolidType >
    std::unique_ptr< SolidType > convert_brep_into_solid( const BRep& brep )
    {
        FromModel< BRep, 3 > model{ brep };
        return build_mesh< SolidFromBRep< SolidType >, SolidType >( model );
    }

    template < typename SurfaceType >
    std::unique_ptr< SurfaceType > convert_brep_into_surface( const BRep& brep )
    {
        FromModel< BRep, 3 > model{ brep };
        return build_mesh< SurfaceFromBRep< SurfaceType >, SurfaceType >(
            model );
    }

    template < typename SurfaceType >
    std::unique_ptr< SurfaceType > convert_section_into_surface(
        const Section& section )
    {
        FromModel< Section, 2 > model{ section };
        return build_mesh< SurfaceFromSection< SurfaceType >, SurfaceType >(
            model );
    }

    std::unique_ptr< EdgedCurve2D > convert_section_into_curve(
        const Section& section )
    {
        FromModel< Section, 2 > model{ section };
        return build_mesh< CurveFromSection, EdgedCurve2D >( model );
    }

    std::unique_ptr< EdgedCurve3D > convert_brep_into_curve( const BRep& brep )
    {
        FromModel< BRep, 3 > model{ brep };
        return build_mesh< CurveFromBRep, EdgedCurve3D >( model );
    }

    template < typename SurfaceType >
    std::tuple< std::unique_ptr< EdgedCurve2D >,
        std::unique_ptr< SurfaceType > >
        convert_section_into_curve_and_surface( const Section& section )
    {
        FromModel< Section, 2 > model{ section };
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
        FromModel< BRep, 3 > model{ brep };
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
        FromModel< BRep, 3 > model{ brep };
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
        FromModel< BRep, 3 > model{ brep };
        auto surface =
            build_mesh< SurfaceFromBRep< SurfaceType >, SurfaceType >( model );
        auto solid = build_mesh< SolidFromBRep< SolidType >, SolidType >(
            model, *surface );
        return std::make_tuple( std::move( surface ), std::move( solid ) );
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
