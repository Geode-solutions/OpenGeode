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

#include <geode/model/representation/core/detail/transfer_metadata.hpp>

#include <absl/container/flat_hash_map.h>
#include <absl/strings/str_join.h>

#include <geode/basic/small_set.hpp>

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
        const Model& new_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Corner< Model::dim >::component_type_static() ) )
        {
            return;
        }
        for( const auto& [out_uuid, in_uuids] : component_mapping
                 .at( geode::Corner< Model::dim >::component_type_static() )
                 .out2in_map() )
        {
            geode::SmallSet< std::string > unique_names;
            for( const auto& in_uuid : in_uuids )
            {
                if( const auto name = old_model.corner( in_uuid ).name() )
                {
                    unique_names.insert( name.value() );
                }
            }
            if( unique_names.empty() )
            {
                continue;
            }
            std::string out_name{ "" };
            for( const auto& corner_name : unique_names )
            {
                absl::StrAppend( &out_name, corner_name, "+" );
            }
            out_name.pop_back();
            builder.set_corner_name( new_model.corner( out_uuid ), out_name );
        }
    }

    template < typename Model >
    void transfer_model_line_metadata( const Model& old_model,
        const Model& new_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Line< Model::dim >::component_type_static() ) )
        {
            return;
        }
        for( const auto& [out_uuid, in_uuids] : component_mapping
                 .at( geode::Line< Model::dim >::component_type_static() )
                 .out2in_map() )
        {
            geode::SmallSet< std::string > unique_names;
            for( const auto& in_uuid : in_uuids )
            {
                if( const auto name = old_model.line( in_uuid ).name() )
                {
                    unique_names.insert( name.value() );
                }
            }
            if( unique_names.empty() )
            {
                continue;
            }
            std::string out_name{ "" };
            for( const auto& line_name : unique_names )
            {
                absl::StrAppend( &out_name, line_name, "+" );
            }
            out_name.pop_back();
            builder.set_line_name( new_model.line( out_uuid ), out_name );
        }
    }

    template < typename Model >
    void transfer_model_surface_metadata( const Model& old_model,
        const Model& new_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Surface< Model::dim >::component_type_static() ) )
        {
            return;
        }
        for( const auto& [out_uuid, in_uuids] : component_mapping
                 .at( geode::Surface< Model::dim >::component_type_static() )
                 .out2in_map() )
        {
            geode::SmallSet< std::string > unique_names;
            for( const auto& in_uuid : in_uuids )
            {
                if( const auto name = old_model.surface( in_uuid ).name() )
                {
                    unique_names.insert( name.value() );
                }
            }
            if( unique_names.empty() )
            {
                continue;
            }
            std::string out_name{ "" };
            for( const auto& surface_name : unique_names )
            {
                absl::StrAppend( &out_name, surface_name, "+" );
            }
            out_name.pop_back();
            builder.set_surface_name( new_model.surface( out_uuid ), out_name );
        }
    }

    void transfer_model_block_metadata( const geode::BRep& old_model,
        const geode::BRep& new_model,
        typename geode::BRepBuilder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Block3D::component_type_static() ) )
        {
            return;
        }
        for( const auto& [out_uuid, in_uuids] :
            component_mapping.at( geode::Block3D::component_type_static() )
                .out2in_map() )
        {
            geode::SmallSet< std::string > unique_names;
            for( const auto& in_uuid : in_uuids )
            {
                if( const auto name = old_model.block( in_uuid ).name() )
                {
                    unique_names.insert( name.value() );
                }
            }
            if( unique_names.empty() )
            {
                continue;
            }
            std::string out_name{ "" };
            for( const auto& surface_name : unique_names )
            {
                absl::StrAppend( &out_name, surface_name, "+" );
            }
            out_name.pop_back();
            builder.set_block_name( new_model.block( out_uuid ), out_name );
        }
    }

    /*!
     * Name each output component with the unique names of its input meshes,
     * joined by '+' (in input order).
     */
    template < typename Mesh, typename NameSetter >
    void transfer_meshes_metadata(
        absl::Span< const std::reference_wrapper< const Mesh > > meshes,
        const geode::ModelGenericMapping& component_mapping,
        const geode::ComponentType& component_type,
        std::string_view function_name,
        NameSetter&& set_name )
    {
        if( !component_mapping.has_mapping_type( component_type ) )
        {
            return;
        }
        absl::flat_hash_map< geode::uuid, const Mesh* > meshes_by_id;
        meshes_by_id.reserve( meshes.size() );
        for( const auto& mesh : meshes )
        {
            meshes_by_id.emplace( mesh.get().id(), &mesh.get() );
        }
        for( const auto& [out_uuid, in_uuids] :
            component_mapping.at( component_type ).out2in_map() )
        {
            geode::SmallSet< std::string > unique_names;
            for( const auto& in_uuid : in_uuids )
            {
                const auto in_mesh = meshes_by_id.find( in_uuid );
                geode::OpenGeodeModelException::check_exception(
                    in_mesh != meshes_by_id.end(), in_uuid,
                    geode::OpenGeodeException::TYPE::data, "[", function_name,
                    "] Should have found input mesh from mapping" );
                if( const auto& name = in_mesh->second->name() )
                {
                    unique_names.insert( name.value() );
                }
            }
            if( unique_names.empty() )
            {
                continue;
            }
            set_name( out_uuid, absl::StrJoin( unique_names, "+" ) );
        }
    }
} // namespace

namespace geode
{
    namespace detail
    {
        void opengeode_model_api transfer_brep_metadata( const BRep& old_brep,
            const BRep& new_brep,
            BRepBuilder& new_brep_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_model_corner_metadata(
                old_brep, new_brep, new_brep_builder, component_mapping );
            transfer_model_line_metadata(
                old_brep, new_brep, new_brep_builder, component_mapping );
            transfer_model_surface_metadata(
                old_brep, new_brep, new_brep_builder, component_mapping );
            transfer_model_block_metadata(
                old_brep, new_brep, new_brep_builder, component_mapping );
        }

        void opengeode_model_api transfer_section_metadata(
            const Section& old_section,
            const Section& new_section,
            SectionBuilder& new_section_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_model_corner_metadata( old_section, new_section,
                new_section_builder, component_mapping );
            transfer_model_line_metadata( old_section, new_section,
                new_section_builder, component_mapping );
            transfer_model_surface_metadata( old_section, new_section,
                new_section_builder, component_mapping );
        }

        template < typename Model >
        void transfer_pointsets_metadata(
            absl::Span< const std::reference_wrapper<
                const PointSet< Model::dim > > > pointsets,
            const Model& model,
            typename Model::Builder& model_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_meshes_metadata( pointsets, component_mapping,
                geode::Corner< Model::dim >::component_type_static(),
                "transfer_pointsets_metadata",
                [&model, &model_builder](
                    const uuid& corner_id, std::string_view name ) {
                    model_builder.set_corner_name(
                        model.corner( corner_id ), name );
                } );
        }

        template < typename Model >
        void transfer_curves_metadata(
            absl::Span< const std::reference_wrapper<
                const EdgedCurve< Model::dim > > > curves,
            const Model& model,
            typename Model::Builder& model_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_meshes_metadata( curves, component_mapping,
                geode::Line< Model::dim >::component_type_static(),
                "transfer_curves_metadata",
                [&model, &model_builder](
                    const uuid& line_id, std::string_view name ) {
                    model_builder.set_line_name( model.line( line_id ), name );
                } );
        }

        template < typename Model >
        void transfer_surfaces_metadata(
            absl::Span< const std::reference_wrapper<
                const SurfaceMesh< Model::dim > > > surfaces,
            const Model& model,
            typename Model::Builder& model_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_meshes_metadata( surfaces, component_mapping,
                geode::Surface< Model::dim >::component_type_static(),
                "transfer_surfaces_metadata",
                [&model, &model_builder](
                    const uuid& surface_id, std::string_view name ) {
                    model_builder.set_surface_name(
                        model.surface( surface_id ), name );
                } );
        }

        void transfer_solids_metadata(
            absl::Span< const std::reference_wrapper< const SolidMesh3D > >
                solids,
            const BRep& model,
            BRepBuilder& model_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_meshes_metadata( solids, component_mapping,
                geode::Block3D::component_type_static(),
                "transfer_solids_metadata",
                [&model, &model_builder](
                    const uuid& block_id, std::string_view name ) {
                    model_builder.set_block_name(
                        model.block( block_id ), name );
                } );
        }

        template void opengeode_model_api transfer_pointsets_metadata(
            absl::Span< const std::reference_wrapper< const PointSet2D > >,
            const Section& model,
            SectionBuilder&,
            const ModelGenericMapping& );

        template void opengeode_model_api transfer_pointsets_metadata(
            absl::Span< const std::reference_wrapper< const PointSet3D > >,
            const BRep&,
            BRepBuilder&,
            const ModelGenericMapping& );

        template void opengeode_model_api transfer_curves_metadata(
            absl::Span< const std::reference_wrapper< const EdgedCurve2D > >
                curves,
            const Section& model,
            SectionBuilder& model_builder,
            const ModelGenericMapping& component_mapping );

        template void opengeode_model_api transfer_curves_metadata(
            absl::Span< const std::reference_wrapper< const EdgedCurve3D > >
                curves,
            const BRep& model,
            BRepBuilder& model_builder,
            const ModelGenericMapping& component_mapping );

        template void opengeode_model_api transfer_surfaces_metadata(
            absl::Span< const std::reference_wrapper< const SurfaceMesh2D > >,
            const Section&,
            SectionBuilder&,
            const ModelGenericMapping& );

        template void opengeode_model_api transfer_surfaces_metadata(
            absl::Span< const std::reference_wrapper< const SurfaceMesh3D > >,
            const BRep&,
            BRepBuilder&,
            const ModelGenericMapping& );
    } // namespace detail
} // namespace geode
