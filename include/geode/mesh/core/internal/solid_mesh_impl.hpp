/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/basic/growable.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    namespace internal
    {
        struct PolyhedraAroundVertexImpl
        {
            PolyhedraAroundVertexImpl() = default;
            PolyhedraAroundVertexImpl( PolyhedraAroundVertex polyhedra_in,
                bool vertex_is_on_border_in )
                : polyhedra{ std::move( polyhedra_in ) },
                  vertex_is_on_border{ vertex_is_on_border_in }
            {
            }

            [[nodiscard]] bool operator!=(
                const PolyhedraAroundVertexImpl& other ) const
            {
                if( vertex_is_on_border != other.vertex_is_on_border )
                {
                    return true;
                }
                return polyhedra != other.polyhedra;
            }

            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    Growable< Archive, PolyhedraAroundVertexImpl >{
                        { []( Archive& a, PolyhedraAroundVertexImpl& value ) {
                            a.container(
                                value.polyhedra, value.polyhedra.max_size() );
                            a.value1b( value.vertex_is_on_border );
                        } } } );
            }

            PolyhedraAroundVertex polyhedra;
            bool vertex_is_on_border{ true };
        };
    } // namespace internal
} // namespace geode
