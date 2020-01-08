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
#include <geode/mesh/io/vertex_set_input.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurface );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an PolygonalSurface.
     * The adequate loader is called depending on the filename extension.
     * @param[out] polygonal_surface Loaded PolygonalSurface.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    void load_polygonal_surface(
        PolygonalSurface< dimension >& polygonal_surface,
        const std::string& filename );

    template < index_t dimension >
    class PolygonalSurfaceInput : public VertexSetInput
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( PolygonalSurfaceInput );

    protected:
        PolygonalSurfaceInput( PolygonalSurface< dimension >& polygonal_surface,
            std::string filename );

        PolygonalSurface< dimension >& polygonal_surface()
        {
            return polygonal_surface_;
        }

    private:
        PolygonalSurface< dimension >& polygonal_surface_;
    };

    template < index_t dimension >
    using PolygonalSurfaceInputFactory = Factory< std::string,
        PolygonalSurfaceInput< dimension >,
        PolygonalSurface< dimension >&,
        std::string >;
    ALIAS_2D_AND_3D( PolygonalSurfaceInputFactory );
} // namespace geode
