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
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
    template < typename T >
    class GenericMeshAABB;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension > create_aabb_tree(
        const SurfaceMesh< dimension >& mesh );

    template < index_t dimension >
    class DistanceToTriangle
    {
    public:
        DistanceToTriangle( const TriangulatedSurface< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        std::tuple< double, Point< dimension > > operator()(
            const Point< dimension >& query, index_t cur_box ) const;

    private:
        const TriangulatedSurface< dimension >& mesh_;
    };
    ALIAS_2D_AND_3D( DistanceToTriangle );

    template < index_t dimension >
    class GenericMeshAABB< SurfaceMesh< dimension > >
    {
    public:
        GenericMeshAABB( const SurfaceMesh< dimension >& mesh )
            : mesh_( mesh ), elements_tree_{ create_aabb_tree( mesh ) }
        {
        }

    protected:
        const AABBTree< dimension >& elements_aabb() const
        {
            return elements_tree_;
        }

    private:
        const SurfaceMesh< dimension >& mesh_;
        AABBTree< dimension > elements_tree_;
    };

    template < index_t dimension >
    class GenericMeshAABB< TriangulatedSurface< dimension > >
        : public GenericMeshAABB< SurfaceMesh< dimension > >
    {
    public:
        using Base = GenericMeshAABB< SurfaceMesh< dimension > >;

        GenericMeshAABB( const TriangulatedSurface< dimension >& mesh )
            : Base{ mesh }, distance_action_{ mesh }
        {
        }

        std::tuple< index_t, Point< dimension >, double > closest_element(
            const Point< dimension >& query ) const
        {
            return this->elements_aabb().closest_element_box(
                query, distance_action_ );
        }

    private:
        const DistanceToTriangle< dimension > distance_action_;
    };
} // namespace geode