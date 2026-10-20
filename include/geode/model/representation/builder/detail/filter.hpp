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

#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component_type.hpp>

namespace geode
{
    namespace internal
    {
        template < typename First, typename... Components >
        [[nodiscard]] bool is_component_supported(
            const ComponentType& component_type )
        {
            if constexpr( sizeof...( Components ) == 0 )
            {
                return component_type == First::Type::component_type_static();
            }
            else
            {
                if( component_type == First::Type::component_type_static() )
                {
                    return true;
                }
                return is_component_supported< Components... >(
                    component_type );
            }
        }

        template < typename >
        struct ComponentsTypesChecker;

        template < typename... Components >
        struct ComponentsTypesChecker< std::tuple< Components... > >
        {
            bool apply( const ComponentType& component_type ) const
            {
                return is_component_supported< Components... >(
                    component_type );
            }
        };

        template < typename >
        struct ComponentsExistenceChecker;

        template < typename... Components >
        struct ComponentsExistenceChecker< std::tuple< Components... > >
        {
            template < typename Model >
            bool apply( const Model& model, const uuid& component_id ) const
            {
                return (
                    model.Components::has_component( component_id ) || ... );
            }
        };
    } // namespace internal

    namespace detail
    {
        template < typename Model >
        void filter_unsupported_components( Model& model )
        {
            typename Model::Builder builder{ model };
            const internal::ComponentsTypesChecker< typename Model::Components >
                checker;
            const internal::ComponentsExistenceChecker<
                typename Model::Components >
                existence_checker;
            std::vector< uuid > components_to_remove;
            for( const auto vertex :
                Range{ model.nb_components_with_relations() } )
            {
                const auto& component_id =
                    model.component_with_relation( vertex );
                if( !checker.apply( component_id.type ) )
                {
                    components_to_remove.push_back( component_id.id );
                }
                else if( !existence_checker.apply( model, component_id.id ) )
                {
                    Logger::warning( "[filter_unsupported_components] "
                                     "Removing relations of missing ",
                        component_id.string() );
                    components_to_remove.push_back( component_id.id );
                }
            }
            for( const auto& component : components_to_remove )
            {
                builder.unregister_component( component );
            }
        }
    } // namespace detail
} // namespace geode
