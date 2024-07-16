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

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( HybridSolidBuilder );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class HybridSolid : public SolidMesh< dimension >
    {
        OPENGEODE_DISABLE_COPY( HybridSolid );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        using Builder = HybridSolidBuilder< dimension >;
        static constexpr auto dim = dimension;

        enum class Type : unsigned char
        {
            UNKNOWN,
            TETRAHEDRON,
            HEXAHEDRON,
            PRISM,
            PYRAMID
        };

        /*!
         * Create a new HybridSolid using default data structure.
         */
        static std::unique_ptr< HybridSolid< dimension > > create();

        /*!
         * Create a new HybridSolid using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< HybridSolid< dimension > > create(
            const MeshImpl& impl );

        static MeshType type_name_static();

        std::unique_ptr< HybridSolid< dimension > > clone() const;

        virtual Type polyhedron_type( index_t polyhedron_id ) const = 0;

    protected:
        HybridSolid() = default;
        HybridSolid( HybridSolid&& other ) noexcept = default;
        HybridSolid& operator=( HybridSolid&& other ) noexcept = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
    ALIAS_3D( HybridSolid );
} // namespace geode
