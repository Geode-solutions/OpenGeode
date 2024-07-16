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

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/common.hpp>

namespace geode
{
    namespace detail
    {
        template < typename Container >
        inline void polygon_sort( Container& vertices )
        {
            const auto min_itr = absl::c_min_element( vertices );
            absl::c_rotate( vertices, min_itr );
        }

        template < typename Container >
        inline void oriented_rotate( Container& vertices )
        {
            if( vertices.size() < 2 )
            {
                return;
            }
            polygon_sort( vertices );
        }

        template <>
        inline void oriented_rotate( std::array< index_t, 2 >& vertices )
        {
            if( vertices.front() > vertices.back() )
            {
                std::swap( vertices.front(), vertices.back() );
            }
        }

        template < typename Container >
        inline void remove_orientation( Container& vertices )
        {
            if( vertices.size() > 1 && vertices[1] > vertices.back() )
            {
                std::reverse( vertices.begin() + 1, vertices.end() );
            }
        }

        template < typename Container >
        class OrientedVertexCycle
        {
            friend class bitsery::Access;

        public:
            explicit OrientedVertexCycle( Container vertices )
                : vertices_( std::move( vertices ) )
            {
                oriented_rotate( vertices_ );
            }

            const Container& vertices() const
            {
                return vertices_;
            }

            bool is_opposite( const OrientedVertexCycle& other ) const
            {
                const auto& other_vertices = other.vertices();
                if( vertices().size() != other_vertices.size()
                    || vertices()[0] != other_vertices[0] )
                {
                    return false;
                }
                auto counter = other_vertices.size() - 1;
                for( const auto vertex_id : Range{ 1, vertices().size() } )
                {
                    if( vertices()[vertex_id] != other_vertices[counter] )
                    {
                        return false;
                    }
                    counter--;
                }
                return true;
            }

            bool operator==( const OrientedVertexCycle& other ) const
            {
                return this->vertices() == other.vertices();
            }

            bool operator!=( const OrientedVertexCycle& other ) const
            {
                return !operator==( other );
            }

            bool operator<( const OrientedVertexCycle& other ) const
            {
                return this->vertices() < other.vertices();
            }

        protected:
            OrientedVertexCycle() = default;

            Container& modifiable_vertices()
            {
                return vertices_;
            }

        private:
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    Growable< Archive, OrientedVertexCycle >{
                        { []( Archive& a, OrientedVertexCycle& storage ) {
                            a( storage.vertices_ );
                        } } } );
            }

        private:
            Container vertices_;
        };

        template < typename Container >
        class VertexCycle : public OrientedVertexCycle< Container >
        {
            friend class bitsery::Access;

        public:
            explicit VertexCycle( Container vertices )
                : OrientedVertexCycle< Container >( std::move( vertices ) )
            {
                remove_orientation( this->modifiable_vertices() );
            }

        private:
            VertexCycle() = default;
        };

        template < typename H, typename Container >
        H AbslHashValue( H h, const VertexCycle< Container >& m )
        {
            return H::combine( std::move( h ), m.vertices() );
        }

        template < typename H, typename Container >
        H AbslHashValue( H h, const OrientedVertexCycle< Container >& m )
        {
            return H::combine( std::move( h ), m.vertices() );
        }
    } // namespace detail
} // namespace geode