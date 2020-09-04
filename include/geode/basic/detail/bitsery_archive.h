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

#include <geode/basic/bitsery_archive.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/uuid.h>

namespace geode
{
    namespace detail
    {
        template < typename Serializer >
        void register_basic_pcontext( PContext& context )
        {
            AttributeManager::register_attribute_type< bool, Serializer >(
                context );
            AttributeManager::register_attribute_type< int, Serializer >(
                context );
            AttributeManager::register_attribute_type< double, Serializer >(
                context );
            AttributeManager::register_attribute_type< index_t, Serializer >(
                context );
            AttributeManager::register_attribute_type< int8_t, Serializer >(
                context );
            AttributeManager::register_attribute_type< uint8_t, Serializer >(
                context );
            AttributeManager::register_attribute_type< std::vector< index_t >,
                Serializer >( context );
            AttributeManager::register_attribute_type< std::vector< int8_t >,
                Serializer >( context );
            AttributeManager::register_attribute_type< std::vector< uint8_t >,
                Serializer >( context );
            AttributeManager::register_attribute_type< std::array< index_t, 2 >,
                Serializer >( context );
            AttributeManager::register_attribute_type< std::array< index_t, 3 >,
                Serializer >( context );
            AttributeManager::register_attribute_type< std::array< index_t, 4 >,
                Serializer >( context );
            AttributeManager::register_attribute_type< uuid, Serializer >(
                context );
        }
    } // namespace detail
} // namespace geode
