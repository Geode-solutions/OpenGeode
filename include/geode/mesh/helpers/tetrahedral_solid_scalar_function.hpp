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

#include <geode/basic/pimpl.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolid );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolidScalarFunction
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        TetrahedralSolidScalarFunction(
            TetrahedralSolidScalarFunction< dimension >&& other ) noexcept;
        ~TetrahedralSolidScalarFunction();

        /*!
         * Create a new object function from a TetrahedralSolid, a name, and a
         * value.
         * Throws an exception if an attribute with the same name exists.
         */
        [[nodiscard]] static TetrahedralSolidScalarFunction< dimension > create(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name,
            double value );

        /*!
         * Finds an object function that already exists in the given
         * TetrahedralSolid, from its given name.
         * Throws an exception if no attribute with the same name exists.
         */
        [[nodiscard]] static TetrahedralSolidScalarFunction< dimension > find(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name );

        void set_value( index_t vertex_index, double value );

        [[nodiscard]] double value( index_t vertex_index ) const;

        [[nodiscard]] double value(
            const Point< dimension >& point, index_t tetrahedron_id ) const;

    private:
        TetrahedralSolidScalarFunction(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name );

        TetrahedralSolidScalarFunction(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name,
            double value );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( TetrahedralSolidScalarFunction );
} // namespace geode