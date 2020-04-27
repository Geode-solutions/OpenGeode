/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <absl/container/flat_hash_map.h>

#include <geode/basic/mapping.h>
#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/polyhedral_solid.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>

namespace
{
}

namespace geode
{
    namespace detail
    {
        using Mapping = BijectiveMapping< uuid >;

        class ModelCopyMapping
        {
        public:
            Mapping& at( const ComponentType& type )
            {
                return mappings.at( type );
            }

            const Mapping& at( const ComponentType& type ) const
            {
                return mappings.at( type );
            }

            void emplace( const ComponentType& type, Mapping mapping )
            {
                mappings.emplace( type, std::move( mapping ) );
            }

        private:
            absl::flat_hash_map< ComponentType, Mapping > mappings;
        };

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        Mapping copy_corner_components(
            const ModelFrom& from, const ModelTo& to, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_corners() );
            for( const auto& corner : from.corners() )
            {
                const auto id =
                    builder_to.add_corner( corner.mesh().type_name() );
                mapping.map( corner.id(), id );
                builder_to.register_mesh_component( to.corner( id ) );
            }
            return mapping;
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        Mapping copy_line_components(
            const ModelFrom& from, const ModelTo& to, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_lines() );
            for( const auto& line : from.lines() )
            {
                const auto id = builder_to.add_line( line.mesh().type_name() );
                mapping.map( line.id(), id );
                builder_to.register_mesh_component( to.line( id ) );
            }
            return mapping;
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        Mapping copy_surface_components(
            const ModelFrom& from, const ModelTo& to, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_surfaces() );
            for( const auto& surface : from.surfaces() )
            {
                const auto id =
                    builder_to.add_surface( surface.mesh().type_name() );
                mapping.map( surface.id(), id );
                builder_to.register_mesh_component( to.surface( id ) );
            }
            return mapping;
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        Mapping copy_block_components(
            const ModelFrom& from, const ModelTo& to, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_blocks() );
            for( const auto& block : from.blocks() )
            {
                const auto id =
                    builder_to.add_block( block.mesh().type_name() );
                mapping.map( block.id(), id );
                builder_to.register_mesh_component( to.block( id ) );
            }
            return mapping;
        }

        template < typename Model, typename BuilderTo >
        Mapping copy_model_boundary_components(
            const Model& from, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_model_boundaries() );
            for( const auto& model_boundary : from.model_boundaries() )
            {
                mapping.map(
                    model_boundary.id(), builder_to.add_model_boundary() );
            }
            return mapping;
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_corner_line_relationships( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& corners,
            const Mapping& lines )
        {
            for( const auto& line : from.lines() )
            {
                const auto& new_line = to.line( lines.in2out( line.id() ) );
                for( const auto& corner : from.boundaries( line ) )
                {
                    const auto& new_corner =
                        to.corner( corners.in2out( corner.id() ) );
                    builder_to.add_corner_line_boundary_relationship(
                        new_corner, new_line );
                }
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_corner_surface_relationships( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& corners,
            const Mapping& surfaces )
        {
            for( const auto& surface : from.surfaces() )
            {
                const auto& new_surface =
                    to.surface( surfaces.in2out( surface.id() ) );
                for( const auto& corner : from.internal_corners( surface ) )
                {
                    const auto& new_corner =
                        to.corner( corners.in2out( corner.id() ) );
                    builder_to.add_corner_surface_internal_relationship(
                        new_corner, new_surface );
                }
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_line_surface_relationships( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& lines,
            const Mapping& surfaces )
        {
            for( const auto& surface : from.surfaces() )
            {
                const auto& new_surface =
                    to.surface( surfaces.in2out( surface.id() ) );
                for( const auto& line : from.boundaries( surface ) )
                {
                    const auto& new_line = to.line( lines.in2out( line.id() ) );
                    builder_to.add_line_surface_boundary_relationship(
                        new_line, new_surface );
                }
                for( const auto& line : from.internal_lines( surface ) )
                {
                    const auto& new_line = to.line( lines.in2out( line.id() ) );
                    builder_to.add_line_surface_internal_relationship(
                        new_line, new_surface );
                }
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_corner_block_relationships( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& corners,
            const Mapping& blocks )
        {
            for( const auto& block : from.blocks() )
            {
                const auto& new_block = to.block( blocks.in2out( block.id() ) );
                for( const auto& corner : from.internal_corners( block ) )
                {
                    const auto& new_corner =
                        to.corner( corners.in2out( corner.id() ) );
                    builder_to.add_corner_block_internal_relationship(
                        new_corner, new_block );
                }
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_line_block_relationships( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& lines,
            const Mapping& blocks )
        {
            for( const auto& block : from.blocks() )
            {
                const auto& new_block = to.block( blocks.in2out( block.id() ) );
                for( const auto& line : from.internal_lines( block ) )
                {
                    const auto& new_line = to.line( lines.in2out( line.id() ) );
                    builder_to.add_line_block_internal_relationship(
                        new_line, new_block );
                }
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_surface_block_relationships( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& surfaces,
            const Mapping& blocks )
        {
            for( const auto& block : from.blocks() )
            {
                const auto& new_block = to.block( blocks.in2out( block.id() ) );
                for( const auto& surface : from.boundaries( block ) )
                {
                    const auto& new_surface =
                        to.surface( surfaces.in2out( surface.id() ) );
                    builder_to.add_surface_block_boundary_relationship(
                        new_surface, new_block );
                }
                for( const auto& surface : from.internal_surfaces( block ) )
                {
                    const auto& new_surface =
                        to.surface( surfaces.in2out( surface.id() ) );
                    builder_to.add_surface_block_internal_relationship(
                        new_surface, new_block );
                }
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_corner_geometry( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& corners )
        {
            for( const auto& corner : from.corners() )
            {
                builder_to.update_corner_mesh(
                    to.corner( corners.in2out( corner.id() ) ),
                    corner.mesh().clone() );
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_line_geometry( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& lines )
        {
            for( const auto& line : from.lines() )
            {
                builder_to.update_line_mesh(
                    to.line( lines.in2out( line.id() ) ), line.mesh().clone() );
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_surface_geometry( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& surfaces )
        {
            for( const auto& surface : from.surfaces() )
            {
                builder_to.update_surface_mesh(
                    to.surface( surfaces.in2out( surface.id() ) ),
                    surface.mesh().clone() );
            }
        }

        template < typename ModelFrom, typename ModelTo, typename BuilderTo >
        void copy_block_geometry( const ModelFrom& from,
            const ModelTo& to,
            BuilderTo& builder_to,
            const Mapping& blocks )
        {
            for( const auto& block : from.blocks() )
            {
                builder_to.update_block_mesh(
                    to.block( blocks.in2out( block.id() ) ),
                    block.mesh().clone() );
            }
        }

        template < typename Model, typename BuilderTo >
        void copy_vertex_identifier_components( const Model& from,
            BuilderTo& builder_to,
            const ComponentType& type,
            const Mapping& mapping )
        {
            for( const auto v : Range{ from.nb_unique_vertices() } )
            {
                const auto vertices = from.mesh_component_vertices( v, type );
                for( const auto& mesh_vertex : vertices )
                {
                    builder_to.set_unique_vertex(
                        { { type,
                              mapping.in2out( mesh_vertex.component_id.id() ) },
                            mesh_vertex.vertex },
                        v );
                }
            }
        }
    } // namespace detail
} // namespace geode
