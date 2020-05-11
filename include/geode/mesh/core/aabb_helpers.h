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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#pragma once

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( AABBTree );
} // namespace geode

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension > create_aabb_tree(
        const EdgedCurve< dimension >& mesh );

    template < index_t dimension >
    class DistanceToEdge
    {
    public:
        explicit DistanceToEdge( const EdgedCurve< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        std::tuple< double, Point< dimension > > operator()(
            const Point< dimension >& query, index_t cur_box ) const;
        double operator()( const Point< dimension >& point1,
            const Point< dimension >& point2 ) const;

    private:
        const EdgedCurve< dimension >& mesh_;
    };

    template < index_t dimension >
    AABBTree< dimension > create_aabb_tree(
        const TriangulatedSurface< dimension >& mesh );

    template < index_t dimension >
    class DistanceToTriangle
    {
    public:
        explicit DistanceToTriangle(
            const TriangulatedSurface< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        std::tuple< double, Point< dimension > > operator()(
            const Point< dimension >& query, index_t cur_box ) const;
        double operator()( const Point< dimension >& point1,
            const Point< dimension >& point2 ) const;

    private:
        const TriangulatedSurface< dimension >& mesh_;
    };

} // namespace geode