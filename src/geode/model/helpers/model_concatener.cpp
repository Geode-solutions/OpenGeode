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

#include <geode/model/helpers/model_concatener.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/builder/detail/copy.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    void copy_model_boundaries_relationships2d( geode::Section& model,
        geode::SectionBuilder& builder,
        const geode::Section& other_model,
        const geode::ModelCopyMapping& mapping )
    {
        const auto& line_mapping =
            mapping.at( geode::Line2D::component_type_static() );
        const auto& boundary_mapping =
            mapping.at( geode::ModelBoundary2D::component_type_static() );
        for( const auto& model_boundary : other_model.model_boundaries() )
        {
            const auto& boundary_copy = model.model_boundary(
                boundary_mapping.in2out( model_boundary.id() ) );
            for( const auto& item :
                other_model.model_boundary_items( model_boundary ) )
            {
                const auto& item_copy =
                    model.line( line_mapping.in2out( item.id() ) );
                builder.add_line_in_model_boundary( item_copy, boundary_copy );
            }
        }
    }

    void copy_model_boundaries_relationships3d( geode::BRep& model,
        geode::BRepBuilder& builder,
        const geode::BRep& other_model,
        const geode::ModelCopyMapping& mapping )
    {
        const auto& surface_mapping =
            mapping.at( geode::Surface3D::component_type_static() );
        const auto& boundary_mapping =
            mapping.at( geode::ModelBoundary2D::component_type_static() );
        for( const auto& model_boundary : other_model.model_boundaries() )
        {
            const auto& boundary_copy = model.model_boundary(
                boundary_mapping.in2out( model_boundary.id() ) );
            for( const auto& item :
                other_model.model_boundary_items( model_boundary ) )
            {
                const auto& item_copy =
                    model.surface( surface_mapping.in2out( item.id() ) );
                builder.add_surface_in_model_boundary(
                    item_copy, boundary_copy );
            }
        }
    }

    void copy_block_relationships( geode::BRep& model,
        geode::BRepBuilder& builder,
        const geode::BRep& other_model,
        const geode::ModelCopyMapping& mapping )
    {
        const auto& corner_mapping =
            mapping.at( geode::Corner3D::component_type_static() );
        const auto& line_mapping =
            mapping.at( geode::Line3D::component_type_static() );
        const auto& surface_mapping =
            mapping.at( geode::Surface3D::component_type_static() );
        const auto& block_mapping =
            mapping.at( geode::Block3D::component_type_static() );
        for( const auto& block : other_model.blocks() )
        {
            const auto& block_copy =
                model.block( block_mapping.in2out( block.id() ) );
            for( const auto& surface : other_model.boundaries( block ) )
            {
                const auto& surface_copy =
                    model.surface( surface_mapping.in2out( surface.id() ) );
                builder.add_surface_block_boundary_relationship(
                    surface_copy, block_copy );
            }
            for( const auto& surface : other_model.internal_surfaces( block ) )
            {
                const auto& surface_copy =
                    model.surface( surface_mapping.in2out( surface.id() ) );
                builder.add_surface_block_internal_relationship(
                    surface_copy, block_copy );
            }
            for( const auto& line : other_model.internal_lines( block ) )
            {
                const auto& line_copy =
                    model.line( line_mapping.in2out( line.id() ) );
                builder.add_line_block_internal_relationship(
                    line_copy, block_copy );
            }
            for( const auto& corner : other_model.internal_corners( block ) )
            {
                const auto& corner_copy =
                    model.corner( corner_mapping.in2out( corner.id() ) );
                builder.add_corner_block_internal_relationship(
                    corner_copy, block_copy );
            }
        }
    }
} // namespace

namespace geode
{
    template < typename Model >
    class ModelConcatener< Model >::Impl
    {
        using ModelBuilder = typename Model::Builder;
        static constexpr auto dimension = Model::dim;

    public:
        Impl( Model& model ) : model_( model ), builder_{ model } {}

        ModelCopyMapping concatenate( const Model& other_model )
        {
            const auto mapping = builder_.copy_components( other_model );
            builder_.copy_component_geometry( mapping, other_model );
            copy_relationships( other_model, mapping );
            return mapping;
        }

    private:
        void copy_relationships(
            const Model& other_model, const ModelCopyMapping& mapping );

        void copy_line_relationships(
            const Model& other_model, const ModelCopyMapping& mapping )
        {
            const auto& corner_mapping =
                mapping.at( Corner< dimension >::component_type_static() );
            const auto& line_mapping =
                mapping.at( Line< dimension >::component_type_static() );
            for( const auto& line : other_model.lines() )
            {
                const auto& line_copy =
                    model_.line( line_mapping.in2out( line.id() ) );
                for( const auto& corner : other_model.boundaries( line ) )
                {
                    const auto& corner_copy =
                        model_.corner( corner_mapping.in2out( corner.id() ) );
                    builder_.add_corner_line_boundary_relationship(
                        corner_copy, line_copy );
                }
            }
        }

        void copy_surface_relationships(
            const Model& other_model, const ModelCopyMapping& mapping )
        {
            const auto& corner_mapping =
                mapping.at( Corner< dimension >::component_type_static() );
            const auto& line_mapping =
                mapping.at( Line< dimension >::component_type_static() );
            const auto& surface_mapping =
                mapping.at( Surface< dimension >::component_type_static() );
            for( const auto& surface : other_model.surfaces() )
            {
                const auto& surface_copy =
                    model_.surface( surface_mapping.in2out( surface.id() ) );
                for( const auto& line : other_model.boundaries( surface ) )
                {
                    const auto& line_copy =
                        model_.line( line_mapping.in2out( line.id() ) );
                    builder_.add_line_surface_boundary_relationship(
                        line_copy, surface_copy );
                }
                for( const auto& line : other_model.internal_lines( surface ) )
                {
                    const auto& line_copy =
                        model_.line( line_mapping.in2out( line.id() ) );
                    builder_.add_line_surface_internal_relationship(
                        line_copy, surface_copy );
                }
                for( const auto& corner :
                    other_model.internal_corners( surface ) )
                {
                    const auto& corner_copy =
                        model_.corner( corner_mapping.in2out( corner.id() ) );
                    builder_.add_corner_surface_internal_relationship(
                        corner_copy, surface_copy );
                }
            }
        }

    private:
        Model& model_;
        ModelBuilder builder_;
    };

    template <>
    void ModelConcatener< Section >::Impl::copy_relationships(
        const Section& other_model, const ModelCopyMapping& mapping )
    {
        copy_line_relationships( other_model, mapping );
        copy_surface_relationships( other_model, mapping );
        copy_model_boundaries_relationships2d(
            model_, builder_, other_model, mapping );
    }

    template <>
    void ModelConcatener< BRep >::Impl::copy_relationships(
        const BRep& other_model, const ModelCopyMapping& mapping )
    {
        copy_line_relationships( other_model, mapping );
        copy_surface_relationships( other_model, mapping );
        copy_block_relationships( model_, builder_, other_model, mapping );
        copy_model_boundaries_relationships3d(
            model_, builder_, other_model, mapping );
    }

    template < typename Model >
    ModelConcatener< Model >::ModelConcatener( Model& brep ) : impl_{ brep }
    {
    }

    template < typename Model >
    ModelConcatener< Model >::~ModelConcatener()
    {
    }

    template < typename Model >
    ModelCopyMapping ModelConcatener< Model >::concatenate(
        const Model& other_model )
    {
        return impl_->concatenate( other_model );
    }

    template class opengeode_model_api ModelConcatener< BRep >;
    template class opengeode_model_api ModelConcatener< Section >;
} // namespace geode