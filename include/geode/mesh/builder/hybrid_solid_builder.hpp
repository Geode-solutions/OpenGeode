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

#include <array>
#include <vector>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( HybridSolid );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class HybridSolidBuilder : public SolidMeshBuilder< dimension >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        static constexpr auto dim = dimension;

        static std::unique_ptr< HybridSolidBuilder< dimension > > create(
            HybridSolid< dimension >& mesh );

        /*!
         * Create a new tetrahedron from four vertices.
         * @param[in] vertices The four vertices defining the tetrahedron to
         * create
         * @return the index of the created tetrahedron
         */
        index_t create_tetrahedron( const std::array< index_t, 4 >& vertices );

        /*!
         * Create a new hexahedron from four vertices.
         * @param[in] vertices The eight vertices defining the hexahedron to
         * create
         * @return the index of the created hexahedron
         */
        index_t create_hexahedron( const std::array< index_t, 8 >& vertices );

        /*!
         * Create a new prism from four vertices.
         * @param[in] vertices The six vertices defining the prism to create
         * @return the index of the created prism
         */
        index_t create_prism( const std::array< index_t, 6 >& vertices );

        /*!
         * Create a new pyramid from four vertices.
         * @param[in] vertices The five vertices defining the pyramid to
         * create
         * @return the index of the created pyramid
         */
        index_t create_pyramid( const std::array< index_t, 5 >& vertices );

        void copy( const HybridSolid< dimension >& hybrid_solid );

    protected:
        explicit HybridSolidBuilder( HybridSolid< dimension >& mesh );

    private:
        void do_create_polyhedron( absl::Span< const index_t > vertices,
            absl::Span< const std::vector< local_index_t > > facets ) final;

        virtual void do_create_tetrahedron(
            const std::array< index_t, 4 >& vertices ) = 0;

        virtual void do_create_hexahedron(
            const std::array< index_t, 8 >& vertices ) = 0;

        virtual void do_create_prism(
            const std::array< index_t, 6 >& vertices ) = 0;

        virtual void do_create_pyramid(
            const std::array< index_t, 5 >& vertices ) = 0;

    private:
        HybridSolid< dimension >& hybrid_solid_;
    };
    ALIAS_3D( HybridSolidBuilder );
} // namespace geode
