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

#pragma once

#include <absl/container/flat_hash_set.h>

#include <geode/basic/pimpl.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/model/common.hpp>
#include <geode/model/helpers/simplicial_creator_definitions.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>

namespace geode
{
    namespace internal
    {
        template < typename Model >
        class SimplicialModelCreator
        {
            using ModelBuilder = typename Model::Builder;
            static constexpr auto dimension = Model::dim;

        public:
            SimplicialModelCreator(
                Model& model, std::vector< Point< dimension > > unique_points )
                : model_( model ),
                  builder_{ model },
                  unique_points_{ std::move( unique_points ) },
                  first_unique_vertex_{ builder_.create_unique_vertices(
                      unique_points_.size() ) }
            {
            }

            std::vector< uuid > create_corners(
                absl::Span< const CornerDefinition > definitions )
            {
                std::vector< uuid > corners;
                corners.reserve( definitions.size() );
                for( const auto& definition : definitions )
                {
                    check_unique_point( definition.vertex );
                }
                for( const auto& definition : definitions )
                {
                    const auto& corner_id = builder_.add_corner();
                    const auto& corner = model_.corner( corner_id );
                    auto mesh_builder = builder_.corner_mesh_builder( corner );
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
                    const auto& vertices = definition.vertices;
                    OpenGeodeModelException::check_exception(
                        vertices.size() >= 2, nullptr,
                        OpenGeodeException::TYPE::data,
                        "[SimplicialModelCreator::create_lines] A Line "
                        "definition should have at least 2 vertices" );
                    for( const auto vertex : vertices )
                    {
                        check_unique_point( vertex );
                    }
                }
                const absl::flat_hash_set< uuid > corner_set{ corners.begin(),
                    corners.end() };
                std::vector< std::array< uuid, 2 > > line_corners;
                line_corners.reserve( definitions.size() );
                for( const auto& definition : definitions )
                {
                    line_corners.push_back(
                        { find_corner( corner_set, definition.vertices.front() ),
                            find_corner(
                                corner_set, definition.vertices.back() ) } );
                }
                for( const auto d : Indices{ definitions } )
                {
                    const auto& definition = definitions[d];
                    const auto& line_id = builder_.add_line();
                    const auto& line = model_.line( line_id );
                    auto mesh_builder = builder_.line_mesh_builder( line );
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
                        model_.corner( line_corners[d][0] ), line );
                    if( line_corners[d][0] != line_corners[d][1] )
                    {
                        builder_.add_corner_line_boundary_relationship(
                            model_.corner( line_corners[d][1] ), line );
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
                    check_simplices( definition.vertices, definition.triangles,
                        3, "triangles" );
                    for( const auto boundary : definition.boundaries )
                    {
                        check_index( boundary, lines.size(), "Line" );
                    }
                    for( const auto internal : definition.internals )
                    {
                        check_index( internal, lines.size(), "Line" );
                    }
                }
                for( const auto& definition : definitions )
                {
                    const auto& surface_id = builder_.add_surface(
                        MeshFactory::default_impl( TriangulatedSurface<
                            dimension >::type_name_static() ) );
                    const auto& surface = model_.surface( surface_id );
                    auto mesh_builder =
                        builder_.surface_mesh_builder( surface );
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

            std::vector< uuid > create_surfaces(
                absl::Span< const uuid > corners,
                absl::Span< const uuid > lines,
                absl::Span< const SurfaceDefinition > definitions )
            {
                for( const auto& definition : definitions )
                {
                    for( const auto internal : definition.internal_corners )
                    {
                        check_index( internal, corners.size(), "Corner" );
                    }
                }
                const auto surfaces = create_surfaces( lines, definitions );
                for( const auto d : Indices{ definitions } )
                {
                    const auto& definition = definitions[d];
                    for( const auto internal : definition.internal_corners )
                    {
                        builder_.add_corner_surface_internal_relationship(
                            model_.corner( corners[internal] ),
                            model_.surface( surfaces[d] ) );
                    }
                }
                return surfaces;
            }

        protected:
            void check_index( index_t index,
                std::size_t size,
                std::string_view element ) const
            {
                OpenGeodeModelException::check_exception( index < size,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[SimplicialModelCreator] ", element, " index ", index,
                    " is out of range (", size, " elements)" );
            }

            uuid find_corner(
                const absl::flat_hash_set< uuid >& corners,
                index_t vertex ) const
            {
                for( const auto& cmv : model_.component_mesh_vertices(
                         first_unique_vertex_ + vertex ) )
                {
                    if( cmv.component_id.type
                            == Corner< dimension >::component_type_static()
                        && corners.contains( cmv.component_id.id ) )
                    {
                        return cmv.component_id.id;
                    }
                }
                OpenGeodeModelException::check_exception( false, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[SimplicialModelCreator::create_lines] No given Corner "
                    "on Line extremity unique point ",
                    vertex );
                return {};
            }

            void check_unique_point( index_t vertex ) const
            {
                check_index( vertex, unique_points_.size(), "Unique point" );
            }

            void check_simplices( absl::Span< const index_t > vertices,
                absl::Span< const index_t > simplices,
                index_t nb_simplex_vertices,
                std::string_view simplex_name ) const
            {
                for( const auto vertex : vertices )
                {
                    check_unique_point( vertex );
                }
                OpenGeodeModelException::check_exception(
                    simplices.size() % nb_simplex_vertices == 0, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[SimplicialModelCreator] The number of ", simplex_name,
                    " vertices (", simplices.size(),
                    ") should be a multiple of ", nb_simplex_vertices );
                for( const auto vertex : simplices )
                {
                    check_index( vertex, vertices.size(), "Component vertex" );
                }
            }

            template < typename MeshBuilder, typename Component >
            void create_point( MeshBuilder& mesh_builder,
                const Component& component,
                index_t vertex )
            {
                const auto id =
                    mesh_builder.create_point( unique_points_[vertex] );
                builder_.set_unique_vertex( { component.component_id(), id },
                    first_unique_vertex_ + vertex );
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
            index_t first_unique_vertex_;
        };
    } // namespace internal
} // namespace geode