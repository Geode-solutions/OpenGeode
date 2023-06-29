/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/builder/grid_builder.h>
#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGrid );
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGridBuilder );
    ALIAS_2D( Point );
    ALIAS_2D( Vector );
    ALIAS_2D( RegularGrid );
} // namespace geode

namespace geode
{
    template <>
    class opengeode_mesh_api RegularGridBuilder< 2 >
        : public SurfaceMeshBuilder< 2 >, public GridBuilder< 2 >
    {
    public:
        static constexpr index_t dim{ 2 };

        static std::unique_ptr< RegularGridBuilder< 2 > > create(
            RegularGrid2D& grid );

        void initialize_grid( const Point2D& origin,
            std::array< index_t, 2 > cells_number,
            std::array< double, 2 > cells_length );

        void initialize_grid( const Point2D& origin,
            std::array< index_t, 2 > cells_number,
            double cells_length );

        void initialize_grid( Point2D origin,
            std::array< index_t, 2 > cells_number,
            std::array< Vector2D, 2 > cell_directions );

        virtual void update_origin( const Point2D& origin ) = 0;

        virtual void update_origin_and_directions(
            Point2D origin, std::array< Vector2D, 2 > cell_directions ) = 0;

        void copy( const RegularGrid2D& grid );

    protected:
        RegularGridBuilder( RegularGrid2D& grid );

    private:
        RegularGrid2D& grid_;
    };
    ALIAS_2D( RegularGridBuilder );
} // namespace geode
