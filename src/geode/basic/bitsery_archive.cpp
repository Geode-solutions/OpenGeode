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

#include <geode/basic/bitsery_archive.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/uuid.h>

namespace
{
    template < typename Serializer >
    void register_pcontext( geode::PContext& context )
    {
        geode::AttributeManager::register_attribute_type< bool, Serializer >(
            context );
        geode::AttributeManager::register_attribute_type< int, Serializer >(
            context );
        geode::AttributeManager::register_attribute_type< double, Serializer >(
            context );
        geode::AttributeManager::register_attribute_type< geode::index_t,
            Serializer >( context );
        geode::AttributeManager::register_attribute_type<
            std::vector< geode::index_t >, Serializer >( context );
        geode::AttributeManager::register_attribute_type<
            std::array< geode::index_t, 2 >, Serializer >( context );
        geode::AttributeManager::register_attribute_type<
            std::array< geode::index_t, 3 >, Serializer >( context );
        geode::AttributeManager::register_attribute_type<
            std::array< geode::index_t, 4 >, Serializer >( context );
        geode::AttributeManager::register_attribute_type< geode::uuid,
            Serializer >( context );
    }
} // namespace

namespace geode
{
    void register_basic_serialize_pcontext( PContext& context )
    {
        register_pcontext< Serializer >( context );
    }

    void register_basic_deserialize_pcontext( PContext& context )
    {
        register_pcontext< Deserializer >( context );
    }
} // namespace geode
