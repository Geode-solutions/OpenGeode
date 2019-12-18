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

#include <algorithm>
#include <array>
#include <numeric>
#include <utility>
#include <vector>

#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/array.h>
#include <bitsery/brief_syntax/vector.h>

#include <geode/basic/common.h>

namespace geode
{
    namespace detail
    {
        template < typename Container >
        class VertexCycle
        {
        public:
            VertexCycle( Container vertices )
                : vertices_( std::move( vertices ) )
            {
                rotate( vertices_ );
            }

            const Container& vertices() const
            {
                return vertices_;
            }

            bool operator==( const VertexCycle& other ) const
            {
                const auto& other_vertices = other.vertices();
                const auto size = this->vertices_.size();
                if( other_vertices.size() != size )
                {
                    return false;
                }
                if( this->vertices().front() != other_vertices.front() )
                {
                    return false;
                }
                if( this->vertices() == other_vertices )
                {
                    return true;
                }
                for( const auto i : Range{ 1, size } )
                {
                    if( this->vertices()[i] != other_vertices[size - i] )
                    {
                        return false;
                    }
                }
                return true;
            }

            bool operator!=( const VertexCycle& other ) const
            {
                return !operator==( other );
            }

        private:
            friend class bitsery::Access;
            VertexCycle() = default;

            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this, DefaultGrowable< Archive, VertexCycle >{},
                    []( Archive& archive, VertexCycle& storage ) {
                        archive( storage.vertices_ );
                    } );
            }

            static void rotate( std::vector< index_t >& vertices )
            {
                const auto min_itr =
                    std::min_element( vertices.begin(), vertices.end() );
                std::rotate( vertices.begin(), min_itr, vertices.end() );
            }

            static void rotate( std::array< index_t, 2 >& vertices )
            {
                if( vertices.front() > vertices.back() )
                {
                    std::swap( vertices.front(), vertices.back() );
                }
            }

        public:
            Container vertices_;
        };
    } // namespace detail
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::detail::VertexCycle< std::vector< geode::index_t > > >
    {
        std::size_t operator()(
            const geode::detail::VertexCycle< std::vector< geode::index_t > >&
                cycle ) const
        {
            std::size_t hash{ 0 };
            for( const auto v : cycle.vertices() )
            {
                hash = hash ^ std::hash< geode::index_t >()( v );
            }
            return hash;
        }
    };

    template <>
    struct hash< geode::detail::VertexCycle< std::array< geode::index_t, 2 > > >
    {
        std::size_t operator()(
            const geode::detail::VertexCycle< std::array< geode::index_t, 2 > >&
                cycle ) const
        {
            return std::hash< geode::index_t >()( cycle.vertices().front() )
                   ^ std::hash< geode::index_t >()( cycle.vertices().back() );
        }
    };
} // namespace std