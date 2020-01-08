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

#include <geode/mesh/io/triangulated_surface_output.h>

#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    template < index_t dimension >
    void save_triangulated_surface(
        const TriangulatedSurface< dimension >& triangulated_surface,
        const std::string& filename )
    {
        try
        {
            const auto output =
                TriangulatedSurfaceOutputFactory< dimension >::create(
                    extension_from_filename( filename ), triangulated_surface,
                    filename );
            output->write();
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot save TriangulatedSurface in file: ", filename
            };
        }
    }

    template < index_t dimension >
    TriangulatedSurfaceOutput< dimension >::TriangulatedSurfaceOutput(
        const TriangulatedSurface< dimension >& triangulated_surface,
        std::string filename )
        : PolygonalSurfaceOutput< dimension >(
            triangulated_surface, std::move( filename ) ),
          triangulated_surface_( triangulated_surface )
    {
    }

    template void opengeode_mesh_api save_triangulated_surface(
        const TriangulatedSurface< 2 >&, const std::string& );
    template void opengeode_mesh_api save_triangulated_surface(
        const TriangulatedSurface< 3 >&, const std::string& );

    template class opengeode_mesh_api TriangulatedSurfaceOutput< 2 >;
    template class opengeode_mesh_api TriangulatedSurfaceOutput< 3 >;
} // namespace geode
