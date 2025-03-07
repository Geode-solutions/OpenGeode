/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/model/common.hpp>

namespace geode
{
    namespace detail
    {
        template < typename ModelBuilder, typename MeshComponent >
        void add_mesh_component(
            ModelBuilder& builder, const MeshComponent& component )
        {
            builder.register_mesh_component( component );
            builder.add_mesh_component(
                component.component_type(), component.id() );
        }

        template < typename ModelBuilder, typename MeshComponent >
        void remove_mesh_component(
            ModelBuilder& builder, const MeshComponent& component )
        {
            builder.unregister_component( component.id() );
            builder.unregister_mesh_component( component );
            builder.remove_mesh_component(
                component.component_type(), component.id() );
        }

        template < typename ModelBuilder, typename CollectionComponent >
        void add_collection_component(
            ModelBuilder& builder, const CollectionComponent& component )
        {
            builder.add_collection_component(
                component.component_type(), component.id() );
        }

        template < typename ModelBuilder, typename CollectionComponent >
        void remove_collection_component(
            ModelBuilder& builder, const CollectionComponent& component )
        {
            builder.unregister_component( component.id() );
            builder.remove_collection_component(
                component.component_type(), component.id() );
        }
    } // namespace detail

    namespace internal
    {
        template < typename Model, typename First, typename... MeshComponents >
        void register_mesh_components(
            const Model& model, typename Model::Builder& builder )
        {
            if constexpr( sizeof...( MeshComponents ) == 0 )
            {
                for( const auto& component : model.First::components() )
                {
                    detail::add_mesh_component( builder, component );
                }
            }
            else
            {
                for( const auto& component : model.First::components() )
                {
                    detail::add_mesh_component( builder, component );
                }
                register_mesh_components< Model, MeshComponents... >(
                    model, builder );
            }
        }

        template < typename >
        struct MeshComponentsRegistrator;

        template < typename... MeshComponents >
        struct MeshComponentsRegistrator< std::tuple< MeshComponents... > >
        {
            template < typename Model >
            void apply(
                const Model& model, typename Model::Builder& builder ) const
            {
                register_mesh_components< Model, MeshComponents... >(
                    model, builder );
            }
        };

        template < typename Model,
            typename First,
            typename... CollectionComponents >
        void register_collection_components(
            const Model& model, typename Model::Builder& builder )
        {
            if constexpr( sizeof...( CollectionComponents ) == 0 )
            {
                for( const auto& component : model.First::components() )
                {
                    detail::add_collection_component( builder, component );
                }
            }
            else
            {
                for( const auto& component : model.First::components() )
                {
                    detail::add_collection_component( builder, component );
                }
                register_collection_components< Model,
                    CollectionComponents... >( model, builder );
            }
        }

        template < typename >
        struct CollectionComponentsRegistrator;

        template < typename... CollectionComponents >
        struct CollectionComponentsRegistrator<
            std::tuple< CollectionComponents... > >
        {
            template < typename Model >
            void apply(
                const Model& model, typename Model::Builder& builder ) const
            {
                register_collection_components< Model,
                    CollectionComponents... >( model, builder );
            }
        };
    } // namespace internal

    namespace detail
    {
        template < typename Model >
        void register_all_components( Model& model )
        {
            typename Model::Builder builder{ model };
            const internal::MeshComponentsRegistrator<
                typename Model::MeshComponents >
                mesh_registrator;
            mesh_registrator.apply( model, builder );
            const internal::CollectionComponentsRegistrator<
                typename Model::CollectionComponents >
                collection_registrator;
            collection_registrator.apply( model, builder );
        }
    } // namespace detail
} // namespace geode
