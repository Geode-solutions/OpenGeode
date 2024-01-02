/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <bitsery/brief_syntax/string.h>

#include <absl/hash/hash.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/named_type.h>
#include <geode/basic/uuid.h>

namespace geode
{
    struct ComponentTag
    {
    };

    using ComponentType = NamedType< std::string, ComponentTag >;

    /*!
     * Identify a component by its type and a unique index
     */
    class ComponentID
    {
    public:
        ComponentID() : ComponentID( ComponentType{ "undefined" }, uuid{} ) {}

        ComponentID( ComponentType component_type, uuid id )
            : type_( std::move( component_type ) ), id_( std::move( id ) )
        {
        }

        const uuid& id() const&
        {
            return id_;
        }

        uuid&& id() &&
        {
            return std::move( id_ );
        }

        const ComponentType& type() const&
        {
            return type_;
        }

        ComponentType&& type() &&
        {
            return std::move( type_ );
        }

        bool operator!=( const ComponentID& other ) const
        {
            return !( *this == other );
        }

        bool operator==( const ComponentID& other ) const
        {
            return type_.get() == other.type_.get() && id_ == other.id_;
        }

        std::string string() const
        {
            return absl::StrCat( type_.get(), " ", id_.string() );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext(
                *this, Growable< Archive, ComponentID >{
                           { []( Archive& a, ComponentID& component_id ) {
                               a.object( component_id.type_ );
                               a.object( component_id.id_ );
                           } } } );
        }

    private:
        ComponentType type_;
        uuid id_;
    };

} // namespace geode

namespace std
{
    template <>
    struct hash< geode::ComponentType >
    {
    public:
        size_t operator()( const geode::ComponentType& type ) const
        {
            return absl::Hash< string >()( type.get() );
        }
    };

    template <>
    struct hash< geode::ComponentID >
    {
    public:
        size_t operator()( const geode::ComponentID& id ) const
        {
            return absl::Hash< geode::uuid >()( id.id() );
        }
    };
} // namespace std