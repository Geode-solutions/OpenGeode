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

#include <array>
#include <vector>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolid );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolidBuilder : public SolidMeshBuilder< dimension >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        static constexpr auto dim = dimension;

        TetrahedralSolidBuilder( TetrahedralSolidBuilder&& ) noexcept = default;

        [[nodiscard]] static std::unique_ptr<
            TetrahedralSolidBuilder< dimension > >
            create( TetrahedralSolid< dimension >& mesh );

        /*!
         * Create a new tetrahedron from four vertices.
         * @param[in] vertices The four vertices defining the tetrahedron to
         * create
         * @return the index of the created tetrahedron
         */
        index_t create_tetrahedron( const std::array< index_t, 4 >& vertices );

        /*!
         * Create new tetrahedra.
         * @param[in] nb Number of tetrahedra to create
         * @return the index of the first created tetrahedron
         */
        index_t create_tetrahedra( index_t nb );

        /*!
         * Reserve storage for new tetrahedra without creating them.
         * @param[in] nb Number of tetrahedra to reserve
         */
        void reserve_tetrahedra( index_t nb );

        void copy( const TetrahedralSolid< dimension >& tetrahedral_solid );

    protected:
        explicit TetrahedralSolidBuilder( TetrahedralSolid< dimension >& mesh );

    private:
        void do_create_polyhedron( absl::Span< const index_t > vertices,
            absl::Span< const std::vector< local_index_t > > facets ) final;

        virtual void do_create_tetrahedron(
            const std::array< index_t, 4 >& vertices ) = 0;

        virtual void do_create_tetrahedra( index_t nb ) = 0;

    private:
        TetrahedralSolid< dimension >& tetrahedral_solid_;
    };
    ALIAS_3D( TetrahedralSolidBuilder );
} // namespace geode
