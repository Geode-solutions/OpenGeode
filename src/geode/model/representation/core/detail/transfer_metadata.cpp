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

#include <geode/model/representation/core/detail/transfer_metadata.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Model >
    void transfer_model_corner_metadata( const Model& old_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Corner< Model::dim >::component_type_static() ) )
        {
            return;
        }
        for( const auto& out2in_mapping :
            component_mapping
                .at( geode::Corner< Model::dim >::component_type_static() )
                .out2in_map() )
        {
            std::string out_name{ "" };
            bool first{ true };
            for( const auto& in_uuid : out2in_mapping.second )
            {
                if( !first )
                {
                    absl::StrAppend( &out_name, "+" );
                }
                first = false;
                absl::StrAppend(
                    &out_name, old_model.corner( in_uuid ).name() );
            }
            builder.set_corner_name( out2in_mapping.first, out_name );
        }
    }

    template < typename Model >
    void transfer_model_line_metadata( const Model& old_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Line< Model::dim >::component_type_static() ) )
        {
            return;
        }
        for( const auto& out2in_mapping :
            component_mapping
                .at( geode::Line< Model::dim >::component_type_static() )
                .out2in_map() )
        {
            std::string out_name{ "" };
            bool first{ true };
            for( const auto& in_uuid : out2in_mapping.second )
            {
                if( !first )
                {
                    absl::StrAppend( &out_name, "+" );
                }
                first = false;
                absl::StrAppend( &out_name, old_model.line( in_uuid ).name() );
            }
            builder.set_line_name( out2in_mapping.first, out_name );
        }
    }

    template < typename Model >
    void transfer_model_surface_metadata( const Model& old_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Surface< Model::dim >::component_type_static() ) )
        {
            return;
        }
        for( const auto& out2in_mapping :
            component_mapping
                .at( geode::Surface< Model::dim >::component_type_static() )
                .out2in_map() )
        {
            std::string out_name{ "" };
            bool first{ true };
            for( const auto& in_uuid : out2in_mapping.second )
            {
                if( !first )
                {
                    absl::StrAppend( &out_name, "+" );
                }
                first = false;
                absl::StrAppend(
                    &out_name, old_model.surface( in_uuid ).name() );
            }
            builder.set_surface_name( out2in_mapping.first, out_name );
        }
    }

    void transfer_model_block_metadata( const geode::BRep& old_model,
        typename geode::BRepBuilder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Block3D::component_type_static() ) )
        {
            return;
        }
        for( const auto& out2in_mapping :
            component_mapping.at( geode::Block3D::component_type_static() )
                .out2in_map() )
        {
            std::string out_name{ "" };
            bool first{ true };
            for( const auto& in_uuid : out2in_mapping.second )
            {
                if( !first )
                {
                    absl::StrAppend( &out_name, "+" );
                }
                first = false;
                absl::StrAppend( &out_name, old_model.block( in_uuid ).name() );
            }
            builder.set_block_name( out2in_mapping.first, out_name );
        }
    }
} // namespace

namespace geode
{
    namespace detail
    {
        void opengeode_model_api transfer_brep_metadata( const BRep& old_brep,
            BRepBuilder& new_brep_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_model_corner_metadata(
                old_brep, new_brep_builder, component_mapping );
            transfer_model_line_metadata(
                old_brep, new_brep_builder, component_mapping );
            transfer_model_surface_metadata(
                old_brep, new_brep_builder, component_mapping );
            transfer_model_block_metadata(
                old_brep, new_brep_builder, component_mapping );
        }

        void opengeode_model_api transfer_section_metadata(
            const Section& old_section,
            SectionBuilder& new_section_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_model_corner_metadata(
                old_section, new_section_builder, component_mapping );
            transfer_model_line_metadata(
                old_section, new_section_builder, component_mapping );
            transfer_model_surface_metadata(
                old_section, new_section_builder, component_mapping );
        }

        template < typename ModelBuilder >
        void transfer_pointsets_metadata(
            absl::Span< const std::reference_wrapper<
                const PointSet< ModelBuilder::dim > > > pointsets,
            ModelBuilder& model_builder,
            const ModelGenericMapping& component_mapping )
        {
            if( !component_mapping.has_mapping_type( geode::Corner<
                    ModelBuilder::dim >::component_type_static() ) )
            {
                return;
            }
            for( const auto& out2in_mapping :
                component_mapping
                    .at( geode::Corner<
                        ModelBuilder::dim >::component_type_static() )
                    .out2in_map() )
            {
                std::string out_name{ "" };
                bool first{ true };
                for( const auto& in_uuid : out2in_mapping.second )
                {
                    if( !first )
                    {
                        absl::StrAppend( &out_name, "+" );
                    }
                    first = false;
                    const auto in_pointset = absl::c_find_if(
                        pointsets, [&in_uuid]( const std::reference_wrapper<
                                       const PointSet< ModelBuilder::dim > >&
                                           pointset_ref ) {
                            return pointset_ref.get().id() == in_uuid;
                        } );
                    OPENGEODE_EXCEPTION( in_pointset != pointsets.end(),
                        "[transfer_pointsets_metadata] Should have found input "
                        "pointset from mapping" );
                    absl::StrAppend( &out_name, in_pointset->get().name() );
                }
                model_builder.set_corner_name( out2in_mapping.first, out_name );
            }
        }

        template < typename ModelBuilder >
        void transfer_curves_metadata(
            absl::Span< const std::reference_wrapper<
                const EdgedCurve< ModelBuilder::dim > > > curves,
            ModelBuilder& model_builder,
            const ModelGenericMapping& component_mapping )
        {
            if( !component_mapping.has_mapping_type( geode::Line<
                    ModelBuilder::dim >::component_type_static() ) )
            {
                return;
            }
            for( const auto& out2in_mapping :
                component_mapping
                    .at( geode::Line<
                        ModelBuilder::dim >::component_type_static() )
                    .out2in_map() )
            {
                std::string out_name{ "" };
                bool first{ true };
                for( const auto& in_uuid : out2in_mapping.second )
                {
                    if( !first )
                    {
                        absl::StrAppend( &out_name, "+" );
                    }
                    first = false;
                    const auto in_curve = absl::c_find_if(
                        curves, [&in_uuid]( const std::reference_wrapper<
                                    const EdgedCurve< ModelBuilder::dim > >&
                                        curve_ref ) {
                            return curve_ref.get().id() == in_uuid;
                        } );
                    OPENGEODE_EXCEPTION( in_curve != curves.end(),
                        "[transfer_curves_metadata] Should have found input "
                        "curve from mapping" );
                    absl::StrAppend( &out_name, in_curve->get().name() );
                }
                model_builder.set_line_name( out2in_mapping.first, out_name );
            }
        }

        template < typename ModelBuilder >
        void transfer_surfaces_metadata(
            absl::Span< const std::reference_wrapper<
                const SurfaceMesh< ModelBuilder::dim > > > surfaces,
            ModelBuilder& model_builder,
            const ModelGenericMapping& component_mapping )
        {
            if( !component_mapping.has_mapping_type( geode::Surface<
                    ModelBuilder::dim >::component_type_static() ) )
            {
                return;
            }
            for( const auto& out2in_mapping :
                component_mapping
                    .at( geode::Surface<
                        ModelBuilder::dim >::component_type_static() )
                    .out2in_map() )
            {
                std::string out_name{ "" };
                bool first{ true };
                for( const auto& in_uuid : out2in_mapping.second )
                {
                    if( !first )
                    {
                        absl::StrAppend( &out_name, "+" );
                    }
                    first = false;
                    const auto in_surface = absl::c_find_if(
                        surfaces, [&in_uuid]( const std::reference_wrapper<
                                      const SurfaceMesh< ModelBuilder::dim > >&
                                          surface_ref ) {
                            return surface_ref.get().id() == in_uuid;
                        } );
                    OPENGEODE_EXCEPTION( in_surface != surfaces.end(),
                        "[transfer_surfaces_metadata] Should have found input "
                        "surface from mapping" );
                    absl::StrAppend( &out_name, in_surface->get().name() );
                }
                model_builder.set_surface_name(
                    out2in_mapping.first, out_name );
            }
        }

        void transfer_solids_metadata(
            absl::Span< const std::reference_wrapper< const SolidMesh3D > >
                solids,
            BRepBuilder& model_builder,
            const ModelGenericMapping& component_mapping )
        {
            if( !component_mapping.has_mapping_type(
                    geode::Block3D::component_type_static() ) )
            {
                return;
            }
            for( const auto& out2in_mapping :
                component_mapping.at( geode::Block3D::component_type_static() )
                    .out2in_map() )
            {
                std::string out_name{ "" };
                bool first{ true };
                for( const auto& in_uuid : out2in_mapping.second )
                {
                    if( !first )
                    {
                        absl::StrAppend( &out_name, "+" );
                    }
                    first = false;
                    const auto in_solid = absl::c_find_if( solids,
                        [&in_uuid](
                            const std::reference_wrapper< const SolidMesh3D >&
                                solid_ref ) {
                            return solid_ref.get().id() == in_uuid;
                        } );
                    OPENGEODE_EXCEPTION( in_solid != solids.end(),
                        "[transfer_solids_metadata] Should have found input "
                        "solid from mapping" );
                    absl::StrAppend( &out_name, in_solid->get().name() );
                }
                model_builder.set_block_name( out2in_mapping.first, out_name );
            }
        }

        template void opengeode_model_api transfer_pointsets_metadata(
            absl::Span< const std::reference_wrapper< const PointSet2D > >,
            SectionBuilder&,
            const ModelGenericMapping& );

        template void opengeode_model_api transfer_pointsets_metadata(
            absl::Span< const std::reference_wrapper< const PointSet3D > >,
            BRepBuilder&,
            const ModelGenericMapping& );

        template void opengeode_model_api transfer_curves_metadata(
            absl::Span< const std::reference_wrapper< const EdgedCurve2D > >
                curves,
            SectionBuilder& model_builder,
            const ModelGenericMapping& component_mapping );

        template void opengeode_model_api transfer_curves_metadata(
            absl::Span< const std::reference_wrapper< const EdgedCurve3D > >
                curves,
            BRepBuilder& model_builder,
            const ModelGenericMapping& component_mapping );

        template void opengeode_model_api transfer_surfaces_metadata(
            absl::Span< const std::reference_wrapper< const SurfaceMesh2D > >,
            SectionBuilder&,
            const ModelGenericMapping& );

        template void opengeode_model_api transfer_surfaces_metadata(
            absl::Span< const std::reference_wrapper< const SurfaceMesh3D > >,
            BRepBuilder&,
            const ModelGenericMapping& );
    } // namespace detail
} // namespace geode
