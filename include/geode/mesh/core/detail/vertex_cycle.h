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

#include <geode/basic/common.h>
#include <numeric>
#include <utility>
#include <vector>

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
                auto vertices_copy = vertices_;
                auto min_itr = std::min_element(
                    vertices_copy.begin(), vertices_copy.end() );
                auto other_min_itr = std::min_element(
                    other_vertices.begin(), other_vertices.end() );
                if( *min_itr != *other_min_itr )
                {
                    return false;
                }
                std::rotate( other_vertices.begin(), other_min_itr,
                    other_vertices.end() );
                std::rotate(
                    vertices_copy.begin(), min_itr, vertices_copy.end() );
                if( vertices_copy == other_vertices )
                {
                    return true;
                }
                std::reverse(
                    other_vertices.begin() + 1, other_vertices.end() );
                return vertices_copy == other_vertices;
            }

            bool operator!=( const VertexCycle& other ) const
            {
                return !operator==( other );
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