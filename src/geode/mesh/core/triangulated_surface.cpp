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

#include <geode/mesh/core/triangulated_surface.h>

#include <geode/mesh/core/geode_triangulated_surface.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TriangulatedSurface< dimension > >
        TriangulatedSurface< dimension >::create()
    {
        return std::unique_ptr< TriangulatedSurface< dimension > >{
            new OpenGeodeTriangulatedSurface< dimension >
        };
    }

    template < index_t dimension >
    std::unique_ptr< TriangulatedSurface< dimension > >
        TriangulatedSurface< dimension >::create( const MeshType &type )
    {
        try
        {
            return TriangulatedSurfaceFactory< dimension >::create( type );
        }
        catch( const OpenGeodeException &e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException(
                "Could not create TriangulatedSurface data structure: ",
                type.get() );
        }
    }

    template class opengeode_mesh_api TriangulatedSurface< 2 >;
    template class opengeode_mesh_api TriangulatedSurface< 3 >;
} // namespace geode
