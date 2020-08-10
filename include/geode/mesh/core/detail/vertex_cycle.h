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

#include <algorithm>
#include <array>
#include <numeric>
#include <utility>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/container/inlined_vector.h>
#include <absl/hash/hash.h>

#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/array.h>
#include <bitsery/brief_syntax/vector.h>

#include <geode/basic/common.h>
#include <geode/basic/logger.h>

namespace geode
{
    namespace detail
    {
        template < typename Container >
        inline void rotate( Container& vertices )
        {
            const auto min_itr = absl::c_min_element( vertices );
            absl::c_rotate( vertices, min_itr );
            if( vertices[1] > vertices.back() )
            {
                std::reverse( vertices.begin() + 1, vertices.end() );
            }
        }

        template <>
        inline void rotate( std::array< index_t, 2 >& vertices )
        {
            if( vertices.front() > vertices.back() )
            {
                std::swap( vertices.front(), vertices.back() );
            }
        }

        template < typename Container >
        class VertexCycle
        {
            friend class bitsery::Access;

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
                return this->vertices() == other.vertices();
            }

            bool operator!=( const VertexCycle& other ) const
            {
                return !operator==( other );
            }

        private:
            VertexCycle() = default;

            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    Growable< Archive, VertexCycle >{
                        { []( Archive& archive, VertexCycle& storage ) {
                             archive( storage.vertices_ );
                         },
                            []( Archive& archive, VertexCycle& storage ) {
                                archive( storage.vertices_ );
                            } },
                        { []( VertexCycle& storage ) {
                            rotate( storage.vertices_ );
                        } } } );
            }

        public:
            Container vertices_;
        };

        template < typename H, typename Container >
        H AbslHashValue( H h, const VertexCycle< Container >& m )
        {
            return H::combine( std::move( h ), m.vertices() );
        }
    } // namespace detail
} // namespace geode