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

#pragma once

#include <geode/basic/bitsery_archive.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/polyhedral_solid.h>

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolid : public PolyhedralSolid< dimension >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        /*!
         * Create a new TetrahedralSolid using default data structure.
         */
        static std::unique_ptr< TetrahedralSolid< dimension > > create();

        /*!
         * Create a new TetrahedralSolid using a specified data structure.
         * @param[in] type Data structure type
         */
        static std::unique_ptr< TetrahedralSolid< dimension > > create(
            const MeshType& type );

        std::unique_ptr< TetrahedralSolid< dimension > > clone() const;

    protected:
        TetrahedralSolid() = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, TetrahedralSolid >{},
                []( Archive& archive, TetrahedralSolid& tetrahedral_solid ) {
                    archive.ext( tetrahedral_solid,
                        bitsery::ext::BaseClass<
                            PolyhedralSolid< dimension > >{} );
                } );
        }

        index_t get_nb_polyhedron_vertices( index_t /*unused*/ ) const final
        {
            return 4;
        }

        index_t get_nb_polyhedron_facets( index_t /*unused*/ ) const final
        {
            return 4;
        }

        index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& /*unused*/ ) const final
        {
            return 3;
        }
    };
    ALIAS_3D( TetrahedralSolid );

    template < index_t dimension >
    using TetrahedralSolidFactory =
        Factory< MeshType, TetrahedralSolid< dimension > >;
    ALIAS_3D( TetrahedralSolidFactory );
} // namespace geode
