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

#include <geode/model/mixin/core/component_type.h>

namespace geode
{
    /*!
     * Identify an element in a model component
     */
    struct ComponentMeshElement
    {
        ComponentMeshElement(
            ComponentID component_id_in, index_t element_id_in )
            : component_id( std::move( component_id_in ) ),
              element_id( element_id_in )
        {
        }

        bool operator==( const ComponentMeshElement& other ) const
        {
            return component_id == other.component_id
                   && element_id == other.element_id;
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext(
                *this, Growable< Archive, ComponentMeshElement >{
                           { []( Archive& a, ComponentMeshElement& cme ) {
                               a.object( cme.component_id );
                               a.value4b( cme.element_id );
                           } } } );
        }

        std::string string() const
        {
            return absl::StrCat(
                "[", component_id.string(), " ", element_id, "]" );
        }

        ComponentID component_id;
        index_t element_id{ NO_ID };
    };
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::ComponentMeshElement >
    {
    public:
        size_t operator()( const geode::ComponentMeshElement& cme ) const
        {
            return absl::Hash< geode::uuid >()( cme.component_id.id() )
                   ^ absl::Hash< geode::index_t >()( cme.element_id );
        }
    };
} // namespace std