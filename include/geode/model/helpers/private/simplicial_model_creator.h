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

#include <geode/basic/pimpl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/model/common.h>
#include <geode/model/helpers/simplicial_creator_definitions.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

namespace geode
{
    namespace detail
    {
        template < typename Model, typename ModelBuilder, index_t dimension >
        class SimplicialModelCreator
        {
        public:
            SimplicialModelCreator(
                Model& model, std::vector< Point< dimension > > unique_points )
                : model_( model ),
                  builder_{ model },
                  unique_points_{ std::move( unique_points ) }
            {
                builder_.create_unique_vertices( unique_points_.size() );
            }

            std::vector< uuid > create_corners(
                absl::Span< const CornerDefinition > definitions )
            {
                std::vector< uuid > corners;
                corners.reserve( definitions.size() );
                for( const auto& definition : definitions )
                {
                    const auto& corner_id = builder_.add_corner();
                    const auto& corner = model_.corner( corner_id );
                    auto mesh_builder =
                        builder_.corner_mesh_builder( corner_id );
                    create_point( *mesh_builder, corner, definition.vertex );
                    corners.push_back( corner_id );
                }
                return corners;
            }

            std::vector< uuid > create_lines( absl::Span< const uuid > corners,
                absl::Span< const LineDefinition > definitions )
            {
                std::vector< uuid > lines;
                lines.reserve( definitions.size() );
                for( const auto& definition : definitions )
                {
                    const auto& line_id = builder_.add_line();
                    const auto& line = model_.line( line_id );
                    auto mesh_builder = builder_.line_mesh_builder( line_id );
                    const auto& vertices = definition.vertices;
                    for( const auto vertex : vertices )
                    {
                        create_point( *mesh_builder, line, vertex );
                    }
                    for( const auto e : Range{ vertices.size() - 1 } )
                    {
                        mesh_builder->create_edge( e, e + 1 );
                    }
                    lines.push_back( line_id );
                    builder_.add_corner_line_boundary_relationship(
                        model_.corner( corners[vertices.front()] ), line );
                    if( vertices.front() != vertices.back() )
                    {
                        builder_.add_corner_line_boundary_relationship(
                            model_.corner( corners[vertices.back()] ), line );
                    }
                }
                return lines;
            }

            std::vector< uuid > create_surfaces( absl::Span< const uuid > lines,
                absl::Span< const SurfaceDefinition > definitions )
            {
                std::vector< uuid > surfaces;
                surfaces.reserve( definitions.size() );
                for( const auto& definition : definitions )
                {
                    const auto& surface_id = builder_.add_surface(
                        MeshFactory::default_impl( TriangulatedSurface<
                            dimension >::type_name_static() ) );
                    const auto& surface = model_.surface( surface_id );
                    auto mesh_builder =
                        builder_.surface_mesh_builder( surface_id );
                    for( const auto vertex : definition.vertices )
                    {
                        create_point( *mesh_builder, surface, vertex );
                    }
                    for( index_t v = 0; v < definition.triangles.size();
                         v += 3 )
                    {
                        mesh_builder->create_polygon( { definition.triangles[v],
                            definition.triangles[v + 1],
                            definition.triangles[v + 2] } );
                    }
                    mesh_builder->compute_polygon_adjacencies();
                    surfaces.push_back( surface_id );
                    for( const auto boundary : definition.boundaries )
                    {
                        builder_.add_line_surface_boundary_relationship(
                            model_.line( lines[boundary] ), surface );
                    }
                    for( const auto internal : definition.internals )
                    {
                        builder_.add_line_surface_internal_relationship(
                            model_.line( lines[internal] ), surface );
                    }
                }
                return surfaces;
            }

        protected:
            template < typename MeshBuilder, typename Component >
            void create_point( MeshBuilder& mesh_builder,
                const Component& component,
                index_t vertex )
            {
                const auto id =
                    mesh_builder.create_point( unique_points_[vertex] );
                builder_.set_unique_vertex(
                    { component.component_id(), id }, vertex );
            }

            const Model& model() const
            {
                return model_;
            }

            ModelBuilder& builder()
            {
                return builder_;
            }

        private:
            const Model& model_;
            ModelBuilder builder_;
            std::vector< Point< dimension > > unique_points_;
        };
    } // namespace detail
} // namespace geode