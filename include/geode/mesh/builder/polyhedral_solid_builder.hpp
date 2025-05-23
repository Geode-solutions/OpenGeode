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

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/common.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a PolyhedralSolid
     */
    template < index_t dimension >
    class PolyhedralSolidBuilder : public SolidMeshBuilder< dimension >
    {
    public:
        static constexpr auto dim = dimension;

        PolyhedralSolidBuilder( PolyhedralSolidBuilder&& ) noexcept = default;

        /*!
         * Create the builder associated with a PolyhedralSolid.
         * @param[in] mesh The PolyhedralSolid to build/modify
         */
        [[nodiscard]] static std::unique_ptr<
            PolyhedralSolidBuilder< dimension > >
            create( PolyhedralSolid< dimension >& mesh );

        void copy( const PolyhedralSolid< dimension >& polyhedral_solid );

    protected:
        explicit PolyhedralSolidBuilder( PolyhedralSolid< dimension >& mesh );

    private:
        PolyhedralSolid< dimension >& polyhedral_solid_;
    };
    ALIAS_3D( PolyhedralSolidBuilder );
} // namespace geode
