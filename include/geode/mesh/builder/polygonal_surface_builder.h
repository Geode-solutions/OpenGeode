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

#include <vector>

#include <absl/types/span.h>

#include <geode/basic/passkey.h>

#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurface );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a PolygonalSurface
     */
    template < index_t dimension >
    class PolygonalSurfaceBuilder : public SurfaceMeshBuilder< dimension >
    {
        PASSKEY( PolygonalSurface< dimension >, BuilderKey );

    public:
        /*!
         * Create the builder associated with a PolygonalSurface.
         * @param[in] mesh The PolygonalSurface to build/modify
         */
        static std::unique_ptr< PolygonalSurfaceBuilder< dimension > > create(
            PolygonalSurface< dimension >& mesh );

        void set_mesh(
            PolygonalSurface< dimension >& mesh, MeshBuilderFactoryKey key );

        void copy(
            const PolygonalSurface< dimension >& surface_mesh, BuilderKey );

    protected:
        PolygonalSurfaceBuilder() = default;

    private:
        PolygonalSurface< dimension >* polygonal_surface_;
    };
    ALIAS_2D_AND_3D( PolygonalSurfaceBuilder );
} // namespace geode
