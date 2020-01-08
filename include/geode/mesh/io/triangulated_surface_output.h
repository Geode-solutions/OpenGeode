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

#include <geode/basic/factory.h>

#include <geode/mesh/common.h>
#include <geode/mesh/io/polygonal_surface_output.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a TriangulatedSurface.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] triangulated_surface TriangulatedSurface to save.
     * @param[in] filename Path to the file where save the TriangulatedSurface.
     */
    template < index_t dimension >
    void save_triangulated_surface(
        const TriangulatedSurface< dimension >& triangulated_surface,
        const std::string& filename );

    template < index_t dimension >
    class TriangulatedSurfaceOutput : public PolygonalSurfaceOutput< dimension >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( TriangulatedSurfaceOutput );

    protected:
        TriangulatedSurfaceOutput(
            const TriangulatedSurface< dimension >& triangulated_surface,
            std::string filename );

        const TriangulatedSurface< dimension >& triangulated_surface() const
        {
            return triangulated_surface_;
        }

    private:
        const TriangulatedSurface< dimension >& triangulated_surface_;
    };

    template < index_t dimension >
    using TriangulatedSurfaceOutputFactory = Factory< std::string,
        TriangulatedSurfaceOutput< dimension >,
        const TriangulatedSurface< dimension >&,
        std::string >;
    ALIAS_2D_AND_3D( TriangulatedSurfaceOutputFactory );
} // namespace geode
