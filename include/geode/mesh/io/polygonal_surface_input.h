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

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/mesh_id.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurface );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an PolygonalSurface.
     * The adequate loader is called depending on the filename extension.
     * @param[in] impl Data structure implementation.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > > load_polygonal_surface(
        const MeshImpl& impl, absl::string_view filename );

    /*!
     * API function for loading an PolygonalSurface.
     * The adequate loader is called depending on the filename extension.
     * Default data structure implémentation is used.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > > load_polygonal_surface(
        absl::string_view filename );

    template < index_t dimension >
    class PolygonalSurfaceInput
        : public Input< std::unique_ptr< PolygonalSurface< dimension > >,
              MeshImpl >
    {
    public:
        using Base =
            Input< std::unique_ptr< PolygonalSurface< dimension > >, MeshImpl >;
        using Base::InputData;
        using Base::MissingFiles;

    protected:
        PolygonalSurfaceInput( absl::string_view filename ) : Base{ filename }
        {
        }
    };

    template < index_t dimension >
    typename PolygonalSurfaceInput< dimension >::MissingFiles
        check_polygonal_surface_missing_files( absl::string_view filename );

    template < index_t dimension >
    using PolygonalSurfaceInputFactory = Factory< std::string,
        PolygonalSurfaceInput< dimension >,
        absl::string_view >;
    ALIAS_2D_AND_3D( PolygonalSurfaceInputFactory );
} // namespace geode
