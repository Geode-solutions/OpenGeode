/*
 * Copyright (c) 2019 Geode-solutions
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
        ComponentID( ComponentType component_type, uuid id )
            : type_( std::move( component_type ) ), id_( std::move( id ) )
        {
        }

        uuid id() const
        {
            return id_;
        }

        const ComponentType& type() const
        {
            return type_;
        }

        bool operator!=( const ComponentID& other ) const
        {
            return !( *this == other );
        }

        bool operator==( const ComponentID& other ) const
        {
            return type_.get() == other.type_.get() && id_ == other.id_;
        }

        friend std::ostream& operator<<(
            std::ostream& os, const ComponentID& in )
        {
            os << in.type_.get() << " " << in.id_;
            return os;
        }

    private:
        friend class bitsery::Access;
        ComponentID() : type_( bitsery::Access::create< ComponentType >() ) {}

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.object( type_ );
            archive.object( id_ );
        }

    private:
        ComponentType type_;
        uuid id_;
    };

} // namespace geode
