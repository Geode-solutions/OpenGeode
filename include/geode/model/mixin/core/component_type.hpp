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

#include <absl/hash/hash.h>

#include <bitsery/brief_syntax/string.h>

#include <geode/basic/growable.hpp>
#include <geode/basic/named_type.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    struct ComponentTag
    {
    };

    using ComponentType = NamedType< std::string, ComponentTag >;

    /*!
     * Identify a component by its type and a unique index
     */
    struct ComponentID
    {
        ComponentID() : ComponentID( ComponentType{ "undefined" }, uuid{} ) {}

        ComponentID( ComponentType component_type, uuid input_id )
            : type{ std::move( component_type ) },
              id{ std::move( input_id ) } {};

        [[nodiscard]] bool operator!=( const ComponentID& other ) const
        {
            return !( *this == other );
        }

        [[nodiscard]] bool operator==( const ComponentID& other ) const
        {
            return type.get() == other.type.get() && id == other.id;
        }

        [[nodiscard]] bool operator<( const ComponentID& other ) const
        {
            if( type.get() != other.type.get() )
            {
                return type.get() < other.type.get();
            }
            return id < other.id;
        }

        [[nodiscard]] std::string string() const
        {
            return absl::StrCat( type.get(), " ", id.string() );
        }

        template < typename H >
        friend H AbslHashValue( H h, const ComponentID& value )
        {
            return H::combine( std::move( h ), value.type, value.id );
        }

        ComponentType type;
        uuid id;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext(
                *this, Growable< Archive, ComponentID >{
                           { []( Archive& archive, ComponentID& component_id ) {
                               archive.object( component_id.type );
                               archive.object( component_id.id );
                           } } } );
        }
    };

} // namespace geode

namespace std
{
    template <>
    struct opengeode_model_api hash< geode::ComponentType >
    {
        size_t operator()( const geode::ComponentType& type ) const;
    };

    template <>
    struct opengeode_model_api hash< geode::ComponentID >
    {
        size_t operator()( const geode::ComponentID& id ) const;
    };
} // namespace std