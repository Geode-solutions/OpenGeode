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

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( AABBTree );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolid );
    FORWARD_DECLARATION_DIMENSION_CLASS( CoordinateSystem );
    ALIAS_3D( CoordinateSystem );
    template < typename T >
    class GenericMeshAABB;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    [[nodiscard]] AABBTree< dimension > create_aabb_tree(
        const SolidMesh< dimension >& mesh );

    template < index_t dimension >
    class DistanceToTetrahedron
    {
    public:
        explicit DistanceToTetrahedron(
            const TetrahedralSolid< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        [[nodiscard]] double operator()(
            const Point< dimension >& query, index_t cur_box ) const;

    private:
        const TetrahedralSolid< dimension >& mesh_;
    };
    ALIAS_3D( DistanceToTetrahedron );

    template < index_t dimension >
    class AnisotropicDistanceToTetrahedron
    {
    public:
        explicit AnisotropicDistanceToTetrahedron(
            const TetrahedralSolid< dimension >& mesh,
            const CoordinateSystem< dimension >& coordinate_system )
            : mesh_( mesh ), coordinate_system_( coordinate_system )
        {
        }

        [[nodiscard]] double operator()(
            const Point< dimension >& query, index_t cur_box ) const;

    private:
        const TetrahedralSolid< dimension >& mesh_;
        const CoordinateSystem< dimension >& coordinate_system_;
    };
    ALIAS_3D( AnisotropicDistanceToTetrahedron );

    template < index_t dimension >
    class GenericMeshAABB< SolidMesh< dimension > >
    {
    public:
        explicit GenericMeshAABB( const SolidMesh< dimension >& mesh )
            : elements_tree_{ create_aabb_tree( mesh ) }
        {
        }

    protected:
        [[nodiscard]] const AABBTree< dimension >& elements_aabb() const
        {
            return elements_tree_;
        }

    private:
        AABBTree< dimension > elements_tree_;
    };
    template < index_t dimension >
    using SolidMeshAABB = GenericMeshAABB< SolidMesh< dimension > >;
    ALIAS_3D( SolidMeshAABB );

    template < index_t dimension >
    class GenericMeshAABB< TetrahedralSolid< dimension > >
        : public GenericMeshAABB< SolidMesh< dimension > >
    {
    public:
        using Base = GenericMeshAABB< SolidMesh< dimension > >;

        explicit GenericMeshAABB( const TetrahedralSolid< dimension >& mesh )
            : Base{ mesh }, distance_action_{ mesh }
        {
        }

        [[nodiscard]] std::tuple< index_t, double > closest_element(
            const Point< dimension >& query ) const
        {
            return this->elements_aabb().closest_element_box(
                query, distance_action_ );
        }

    private:
        const DistanceToTetrahedron< dimension > distance_action_;
    };
    template < index_t dimension >
    using TetrahedralSolidAABB =
        GenericMeshAABB< TetrahedralSolid< dimension > >;
    ALIAS_3D( TetrahedralSolidAABB );
} // namespace geode
