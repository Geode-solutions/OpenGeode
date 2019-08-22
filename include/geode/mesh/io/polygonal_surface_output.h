/*
 * Copyright (c) 2019 Geode-solutions
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
#include <geode/mesh/io/vertex_set_output.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurface );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a PolygonalSurface.
     * The adequate saver is called depending on the given filename extension.
        const PolygonalSurface< dimension >& polygonal_surface,
     * @param[in] edged_curve PolygonalSurface to save.
     * @param[in] filename Path to the file where save the PolygonalSurface.
     */
    template < index_t dimension >
    void save_polygonal_surface(
        const PolygonalSurface< dimension >& polygonal_surface,
        const std::string& filename );

    template < index_t dimension >
    class PolygonalSurfaceOutput : public VertexSetOutput
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( PolygonalSurfaceOutput );

    protected:
        PolygonalSurfaceOutput(
            const PolygonalSurface< dimension >& polygonal_surface,
            std::string filename );

        const PolygonalSurface< dimension >& polygonal_surface() const
        {
            return polygonal_surface_;
        }

    private:
        const PolygonalSurface< dimension >& polygonal_surface_;
    };

    template < index_t dimension >
    using PolygonalSurfaceOutputFactory = Factory< std::string,
        PolygonalSurfaceOutput< dimension >,
        const PolygonalSurface< dimension >&,
        std::string >;
    ALIAS_2D_AND_3D( PolygonalSurfaceOutputFactory );
} // namespace geode
