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

#include <geode/basic/uuid.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component.hpp>

namespace geode
{
    namespace internal
    {
        template < typename Model, typename First, typename... Components >
        const Component< Model::dim >& get_model_component(
            const Model& model, const uuid& component_id )
        {
            if constexpr( sizeof...( Components ) == 0 )
            {
                if( model.First::has_component( component_id ) )
                {
                    return model.First::component( component_id );
                }
            }
            else
            {
                if( model.First::has_component( component_id ) )
                {
                    return model.First::component( component_id );
                }
                return get_model_component< Model, Components... >(
                    model, component_id );
            }
            throw OpenGeodeException{
                "[ModelComponentGetter::get] Model has no component with id ",
                component_id.string()
            };
        }

        template < typename >
        struct ModelComponentGetter;

        template < typename... Components >
        struct ModelComponentGetter< std::tuple< Components... > >
        {
            template < typename Model >
            const Component< Model::dim >& get(
                const Model& model, const uuid& component_id ) const
            {
                return get_model_component< Model, Components... >(
                    model, component_id );
            }
        };
    } // namespace internal

    namespace detail
    {
        template < typename Model >
        const Component< Model::dim >& model_component(
            Model& model, const uuid& component_id )
        {
            const internal::ModelComponentGetter< typename Model::Components >
                component_getter;
            return component_getter.get( model, component_id );
        }
    } // namespace detail
} // namespace geode
