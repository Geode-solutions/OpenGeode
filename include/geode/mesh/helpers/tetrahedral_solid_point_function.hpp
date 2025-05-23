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
    template < index_t dimension, index_t point_dimension >
    class TetrahedralSolidPointFunction
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        TetrahedralSolidPointFunction(
            TetrahedralSolidPointFunction< dimension, point_dimension >&&
                other );
        ~TetrahedralSolidPointFunction();

        /*!
         * Create a new object function from a TetrahedralSolid, a name, and a
         * value.
         * Throws an exception if an attribute with the same name exists.
         */
        [[nodiscard]] static TetrahedralSolidPointFunction< dimension,
            point_dimension >
            create( const TetrahedralSolid< dimension >& solid,
                std::string_view function_name,
                Point< point_dimension > value );

        /*!
         * Finds an object function that already exists in the given
         * TetrahedralSolid, from its given name.
         * Throws an exception if no attribute with the same name exists.
         */
        [[nodiscard]] static TetrahedralSolidPointFunction< dimension,
            point_dimension >
            find( const TetrahedralSolid< dimension >& solid,
                std::string_view function_name );

        void set_value( index_t vertex_index, Point< point_dimension > value );

        [[nodiscard]] const Point< point_dimension >& value(
            index_t vertex_index ) const;

        [[nodiscard]] Point< point_dimension > value(
            const Point< dimension >& point, index_t tetrahedron_id ) const;

    private:
        TetrahedralSolidPointFunction(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name );

        TetrahedralSolidPointFunction(
            const TetrahedralSolid< dimension >& solid,
            std::string_view function_name,
            Point< point_dimension > value );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode