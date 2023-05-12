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

#pragma once

#include <async++.h>

#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/mapping.h>

namespace geode
{
    namespace detail
    {
        using Mapping = ModelCopyMapping::Mapping;

        template < typename ModelFrom, typename BuilderTo >
        Mapping copy_corner_components(
            const ModelFrom& from, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_corners() );
            for( const auto& corner : from.corners() )
            {
                const auto& id =
                    ModelFrom::dim == BuilderTo::dim
                        ? builder_to.add_corner( corner.mesh().impl_name() )
                        : builder_to.add_corner();
                builder_to.set_corner_name( id, corner.name() );
                mapping.map( corner.id(), id );
            }
            return mapping;
        }

        template < typename ModelFrom, typename BuilderTo >
        Mapping copy_line_components(
            const ModelFrom& from, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_lines() );
            for( const auto& line : from.lines() )
            {
                const auto& id =
                    ModelFrom::dim == BuilderTo::dim
                        ? builder_to.add_line( line.mesh().impl_name() )
                        : builder_to.add_line();
                builder_to.set_line_name( id, line.name() );
                mapping.map( line.id(), id );
            }
            return mapping;
        }

        template < typename ModelFrom, typename BuilderTo >
        Mapping copy_surface_components(
            const ModelFrom& from, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_surfaces() );
            for( const auto& surface : from.surfaces() )
            {
                const auto& id =
                    ModelFrom::dim == BuilderTo::dim
                        ? builder_to.add_surface( surface.mesh().impl_name() )
                        : builder_to.add_surface();
                builder_to.set_surface_name( id, surface.name() );
                mapping.map( surface.id(), id );
            }
            return mapping;
        }

        template < typename ModelFrom, typename BuilderTo >
        Mapping copy_block_components(
            const ModelFrom& from, BuilderTo& builder_to )
        {
            Mapping mapping;
            mapping.reserve( from.nb_blocks() );
            for( const auto& block : from.blocks() )
            {
                const auto& id =
                    builder_to.add_block( block.mesh().impl_name() );
                builder_to.set_block_name( id, block.name() );
                mapping.map( block.id(), id );
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
                const auto& id = builder_to.add_model_boundary();
                builder_to.set_model_boundary_name( id, model_boundary.name() );
                mapping.map( model_boundary.id(), id );
            }
            return mapping;
        }

        template < typename Mesh, typename Range >
        absl::FixedArray< std::pair< uuid, std::unique_ptr< Mesh > > >
            clone_meshes( Range&& range, index_t nb_components )
        {
            absl::FixedArray< std::pair< uuid, std::unique_ptr< Mesh > > >
                result( nb_components );
            absl::FixedArray< async::task< void > > tasks( nb_components );
            index_t count{ 0 };
            for( const auto& component : range )
            {
                tasks[count] = async::spawn( [&result, count, &component] {
                    result[count] = std::make_pair(
                        component.id(), component.mesh().clone() );
                } );
                count++;
            }
            for( auto& task : async::when_all( tasks ).get() )
            {
                task.get();
            }
            return result;
        }

        template < typename ModelFrom, typename ModelTo >
        void copy_corner_geometry( const ModelFrom& from,
            const ModelTo& to,
            typename ModelTo::Builder& builder_to,
            const Mapping& corners )
        {
            for( auto&& corner : clone_meshes< PointSet< ModelFrom::dim > >(
                     from.corners(), from.nb_corners() ) )
            {
                builder_to.update_corner_mesh(
                    to.corner( corners.in2out( corner.first ) ),
                    std::move( corner.second ) );
            }
        }

        template < typename ModelFrom, typename ModelTo >
        void copy_line_geometry( const ModelFrom& from,
            const ModelTo& to,
            typename ModelTo::Builder& builder_to,
            const Mapping& lines )
        {
            for( auto&& line : clone_meshes< EdgedCurve< ModelFrom::dim > >(
                     from.lines(), from.nb_lines() ) )
            {
                builder_to.update_line_mesh(
                    to.line( lines.in2out( line.first ) ),
                    std::move( line.second ) );
            }
        }

        template < typename ModelFrom, typename ModelTo >
        void copy_surface_geometry( const ModelFrom& from,
            const ModelTo& to,
            typename ModelTo::Builder& builder_to,
            const Mapping& surfaces )
        {
            for( auto&& surface : clone_meshes< SurfaceMesh< ModelFrom::dim > >(
                     from.surfaces(), from.nb_surfaces() ) )
            {
                builder_to.update_surface_mesh(
                    to.surface( surfaces.in2out( surface.first ) ),
                    std::move( surface.second ) );
            }
        }

        template < typename ModelFrom, typename ModelTo >
        void copy_block_geometry( const ModelFrom& from,
            const ModelTo& to,
            typename ModelTo::Builder& builder_to,
            const Mapping& blocks )
        {
            for( auto&& block : clone_meshes< SolidMesh< ModelFrom::dim > >(
                     from.blocks(), from.nb_blocks() ) )
            {
                builder_to.update_block_mesh(
                    to.block( blocks.in2out( block.first ) ),
                    std::move( block.second ) );
            }
        }

        template < typename Model, typename BuilderTo >
        void copy_vertex_identifier_components( const Model& from,
            BuilderTo& builder_to,
            index_t first_new_unique_vertex_id,
            const ModelCopyMapping& mapping )
        {
            async::parallel_for(
                async::irange( index_t{ 0 }, from.nb_unique_vertices() ),
                [&from, &builder_to, &first_new_unique_vertex_id, &mapping](
                    index_t v ) {
                    for( const auto& mesh_vertex :
                        from.component_mesh_vertices( v ) )
                    {
                        const auto& type = mesh_vertex.component_id.type();
                        builder_to.set_unique_vertex(
                            { { type, mapping.at( type ).in2out(
                                          mesh_vertex.component_id.id() ) },
                                mesh_vertex.vertex },
                            first_new_unique_vertex_id + v );
                    }
                } );
        }
    } // namespace detail
} // namespace geode
