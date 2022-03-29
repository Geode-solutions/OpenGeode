/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/passkey.h>

#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a TriangulatedSurface
     */
    template < index_t dimension >
    class TriangulatedSurfaceBuilder : public SurfaceMeshBuilder< dimension >
    {
    public:
        /*!
         * Create the builder associated with a TriangulatedSurface.
         * @param[in] mesh The TriangulatedSurface to build/modify
         */
        static std::unique_ptr< TriangulatedSurfaceBuilder< dimension > >
            create( TriangulatedSurface< dimension >& mesh );

        /*!
         * Create a new triangle from three vertices.
         * @param[in] vertices The three vertices defining the triangle to
         * create
         * @return the index of the created triangle
         */
        index_t create_triangle( const std::array< index_t, 3 >& vertices );

        /*!
         * Create new triangles.
         * @param[in] nb Number of triangles to create
         * @return the index of the first created triangle
         */
        index_t create_triangles( index_t nb );

        /*!
         * Reserve storage for new triangles without creating them.
         * @param[in] nb Number of triangles to reserve
         */
        void reserve_triangles( index_t nb );

        void copy(
            const TriangulatedSurface< dimension >& triangulated_surface );

    protected:
        TriangulatedSurfaceBuilder( TriangulatedSurface< dimension >& mesh );

    private:
        void do_create_polygon( absl::Span< const index_t > vertices ) final;

        virtual void do_create_triangle(
            const std::array< index_t, 3 >& vertices ) = 0;

        virtual void do_create_triangles( index_t nb ) = 0;

    private:
        TriangulatedSurface< dimension >& triangulated_surface_;
    };
    ALIAS_2D_AND_3D( TriangulatedSurfaceBuilder );
} // namespace geode
