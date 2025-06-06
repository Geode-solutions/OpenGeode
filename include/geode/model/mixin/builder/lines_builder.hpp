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

#include <memory>

#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Line );
    FORWARD_DECLARATION_DIMENSION_CLASS( Lines );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurveBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class LinesBuilder
    {
    public:
        void load_lines( std::string_view directory );

        /*!
         * Get a pointer to the builder of a Line mesh
         * @param[in] id Unique index of the Line
         */
        [[nodiscard]] std::unique_ptr< EdgedCurveBuilder< dimension > >
            line_mesh_builder( const uuid& id );

        void set_line_name( const uuid& id, std::string_view name );

        void set_line_active( const uuid& id, bool active );

    protected:
        explicit LinesBuilder( Lines< dimension >& lines ) : lines_( lines ) {}

        [[nodiscard]] const uuid& create_line();

        [[nodiscard]] const uuid& create_line( const MeshImpl& impl );

        void create_line( uuid line_id );

        void create_line( uuid line_id, const MeshImpl& impl );

        void delete_line( const Line< dimension >& line );

        void set_line_mesh(
            const uuid& id, std::unique_ptr< EdgedCurve< dimension > > mesh );

        [[nodiscard]] EdgedCurve< dimension >& modifiable_line_mesh(
            const uuid& id );

        [[nodiscard]] std::unique_ptr< EdgedCurve< dimension > >
            steal_line_mesh( const uuid& id );

    private:
        Lines< dimension >& lines_;
    };
    ALIAS_2D_AND_3D( LinesBuilder );
} // namespace geode
