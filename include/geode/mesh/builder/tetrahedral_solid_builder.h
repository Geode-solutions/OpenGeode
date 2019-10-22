/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/mesh/builder/polyhedral_solid_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolid );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolidBuilder : public PolyhedralSolidBuilder< dimension >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        static std::unique_ptr< TetrahedralSolidBuilder< dimension > > create(
            TetrahedralSolid< dimension >& mesh );

        index_t create_tetrahedron( const std::array< index_t, 4 >& vertices );

    protected:
        TetrahedralSolidBuilder(
            TetrahedralSolid< dimension >& tetrahedral_solid )
            : PolyhedralSolidBuilder< dimension >( tetrahedral_solid ),
              tetrahedral_solid_( tetrahedral_solid )
        {
        }

    private:
        void do_create_polyhedron( const std::vector< index_t >& vertices,
            const std::vector< std::vector< index_t > >& facets ) final;

        std::vector< std::vector< index_t > > get_polyhedron_facet_vertices(
            const std::vector< index_t >& vertices,
            const std::vector< std::vector< index_t > >& facets ) const final;

        virtual void do_create_tetrahedron(
            const std::array< index_t, 4 >& vertices ) = 0;

        virtual std::vector< std::vector< index_t > >
            get_polyhedron_facet_vertices(
                const std::array< index_t, 4 >& vertices ) const = 0;

    private:
        TetrahedralSolid< dimension >& tetrahedral_solid_;
    };
    ALIAS_3D( TetrahedralSolidBuilder );

    template < index_t dimension >
    using TetrahedralSolidBuilderFactory = Factory< MeshType,
        TetrahedralSolidBuilder< dimension >,
        TetrahedralSolid< dimension >& >;
    ALIAS_3D( TetrahedralSolidBuilderFactory );
} // namespace geode
