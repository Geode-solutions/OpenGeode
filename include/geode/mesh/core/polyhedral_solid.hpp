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

#include <vector>

#include <absl/container/inlined_vector.h>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolyhedralSolidBuilder );
    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * This class represents a 3D Solid made up with polyhedra and provides mesh
     * functionnalities.
     */
    template < index_t dimension >
    class PolyhedralSolid : public SolidMesh< dimension >
    {
        OPENGEODE_DISABLE_COPY( PolyhedralSolid );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        using Builder = PolyhedralSolidBuilder< dimension >;
        static constexpr auto dim = dimension;

        /*!
         * Create a new PolyhedralSolid using default data structure.
         */
        [[nodiscard]] static std::unique_ptr< PolyhedralSolid< dimension > >
            create();

        /*!
         * Create a new PolyhedralSolid using a specified data structure.
         * @param[in] impl Data structure implementation.
         */
        [[nodiscard]] static std::unique_ptr< PolyhedralSolid< dimension > >
            create( const MeshImpl& impl );

        [[nodiscard]] static MeshType type_name_static();

        [[nodiscard]] std::unique_ptr< PolyhedralSolid< dimension > >
            clone() const;

    protected:
        PolyhedralSolid() = default;
        PolyhedralSolid( PolyhedralSolid&& other ) noexcept = default;
        PolyhedralSolid& operator=(
            PolyhedralSolid&& other ) noexcept = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
    ALIAS_3D( PolyhedralSolid );
} // namespace geode
