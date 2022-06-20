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

#include <geode/basic/bitsery_archive.h>

#include <absl/container/inlined_vector.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/uuid.h>

namespace
{
    template < typename Serializer, typename Type >
    void register_attribute_type_for_all_containers(
        geode::PContext& context, absl::string_view name )
    {
        geode::AttributeManager::register_attribute_type< Type, Serializer >(
            context, name );
        geode::AttributeManager::register_attribute_type< std::array< Type, 2 >,
            Serializer >( context, absl::StrCat( "array_", name, "_2" ) );
        geode::AttributeManager::register_attribute_type< std::array< Type, 3 >,
            Serializer >( context, absl::StrCat( "array_", name, "_3" ) );
        geode::AttributeManager::register_attribute_type< std::array< Type, 4 >,
            Serializer >( context, absl::StrCat( "array_", name, "_4" ) );
    }

    template < typename Serializer >
    void register_inlinedvector( geode::PContext& context )
    {
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 1 >, Serializer >(
            context, "InlinedVector_index_t_1" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 2 >, Serializer >(
            context, "InlinedVector_index_t_2" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 3 >, Serializer >(
            context, "InlinedVector_index_t_3" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 4 >, Serializer >(
            context, "InlinedVector_index_t_4" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 5 >, Serializer >(
            context, "InlinedVector_index_t_5" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 6 >, Serializer >(
            context, "InlinedVector_index_t_6" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 7 >, Serializer >(
            context, "InlinedVector_index_t_7" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 8 >, Serializer >(
            context, "InlinedVector_index_t_8" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 9 >, Serializer >(
            context, "InlinedVector_index_t_9" );
        geode::AttributeManager::register_attribute_type<
            absl::InlinedVector< geode::index_t, 10 >, Serializer >(
            context, "InlinedVector_index_t_10" );
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < typename Serializer >
        void register_basic_pcontext( PContext& context )
        {
            register_attribute_type_for_all_containers< Serializer, bool >(
                context, "bool" );
            register_attribute_type_for_all_containers< Serializer, int >(
                context, "int" );
            register_attribute_type_for_all_containers< Serializer, double >(
                context, "double" );
            register_attribute_type_for_all_containers< Serializer,
                local_index_t >( context, "local_index_t" );
            register_attribute_type_for_all_containers< Serializer, index_t >(
                context, "index_t" );
            AttributeManager::register_attribute_type< std::vector< index_t >,
                Serializer >( context, "vector_index_t" );
            AttributeManager::register_attribute_type< uuid, Serializer >(
                context, "uuid" );
            register_inlinedvector< Serializer >( context );
        }
    } // namespace detail
} // namespace geode
