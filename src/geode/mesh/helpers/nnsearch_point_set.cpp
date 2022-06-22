/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/helpers/nnsearch_point_set.h>

#include <geode/geometry/nn_search.h>

#include <geode/mesh/core/point_set.h>

namespace geode
{
    template < index_t dimension >
    NNSearch< dimension > create_nn_search( const PointSet< dimension >& mesh )
    {
        std::vector< Point< dimension > > points;
        points.reserve( mesh.nb_vertices() );
        for( const auto v : Range{ mesh.nb_vertices() } )
        {
            points.push_back( mesh.point( v ) );
        }
        return { std::move( points ) };
    }

    template opengeode_mesh_api NNSearch2D create_nn_search< 2 >(
        const PointSet2D& );
    template opengeode_mesh_api NNSearch3D create_nn_search< 3 >(
        const PointSet3D& );

} // namespace geode
