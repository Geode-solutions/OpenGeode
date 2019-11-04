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

#include <geode/basic/common.h>

namespace geode
{
    namespace detail
    {
        class VertexCycle
        {
        public:
            VertexCycle( std::vector< index_t > vertices )
                : vertices_( std::move( vertices ) )
            {
                auto min_itr =
                    std::min_element( vertices_.begin(), vertices_.end() );
                std::rotate( vertices_.begin(), min_itr, vertices_.end() );
            }

            VertexCycle( const std::array< index_t, 2 >& vertices )
                : vertices_{ vertices[0], vertices[1] }
            {
                auto min_itr =
                    std::min_element( vertices_.begin(), vertices_.end() );
                std::rotate( vertices_.begin(), min_itr, vertices_.end() );
            }

            const std::vector< index_t >& vertices() const
            {
                return vertices_;
            }

            bool operator==( const VertexCycle& other ) const
            {
                auto other_vertices = other.vertices();
                if( other_vertices.size() != this->vertices_.size() )
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
                std::reverse(
                    other_vertices.begin() + 1, other_vertices.end() );
                return this->vertices() == other_vertices;
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
                        archive.container4b(
                            storage.vertices_, storage.vertices_.max_size() );
                    } );
            }

        private:
            std::vector< index_t > vertices_;
        };
    } // namespace detail
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::detail::VertexCycle >
    {
        std::size_t operator()( const geode::detail::VertexCycle& cycle ) const
        {
            return std::accumulate(
                cycle.vertices().begin(), cycle.vertices().end(), 0 );
        }
    };
} // namespace std