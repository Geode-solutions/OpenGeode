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

#include <geode/geometry/vector.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    template < template < index_t > class Mesh,
        template < index_t >
        class MeshBuilder,
        index_t dimension >
    void translate_mesh( Mesh< dimension >& mesh,
        MeshBuilder< dimension >& builder,
        const Vector< dimension >& translation )
    {
        for( const auto v : Range{ mesh.nb_vertices() } )
        {
            builder.set_point( v, mesh.point( v ) + translation );
        }
    }

    template < template < index_t > class Mesh,
        template < index_t >
        class MeshBuilder,
        index_t dimension >
    void rescale_mesh( Mesh< dimension >& mesh,
        MeshBuilder< dimension >& builder,
        const std::array< double, dimension >& scale )
    {
        for( const auto v : Range{ mesh.nb_vertices() } )
        {
            auto point = mesh.point( v );
            for( const auto d : LRange{ dimension } )
            {
                point.set_value( d, point.value( d ) * scale[d] );
            }
            builder.set_point( v, point );
        }
    }
} // namespace geode