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

#include <geode/mesh/common.h>
#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurfaceBuilder );
    ALIAS_2D_AND_3D( Vector );
    class AttributeManager;
    struct PolygonEdge;
} // namespace geode

namespace geode
{
    /*!
     * Interface class for Surface made up with polygons (triangles, quads, ...)
     * of arbitrary dimension.
     */
    template < index_t dimension >
    class PolygonalSurface : public SurfaceMesh< dimension >
    {
    public:
        using Builder = PolygonalSurfaceBuilder< dimension >;
        static constexpr auto dim = dimension;

        /*!
         * Create a new PolygonalSurface using default data structure.
         */
        static std::unique_ptr< PolygonalSurface< dimension > > create();

        /*!
         * Create a new PolygonalSurface using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< PolygonalSurface< dimension > > create(
            const MeshImpl& impl );

        static MeshType type_name_static();

        std::unique_ptr< PolygonalSurface< dimension > > clone() const;

    protected:
        PolygonalSurface() = default;
        PolygonalSurface( PolygonalSurface&& other ) noexcept = default;
        PolygonalSurface& operator=(
            PolygonalSurface&& other ) noexcept = default;
    };
    ALIAS_2D_AND_3D( PolygonalSurface );
} // namespace geode
