/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/mesh/io/hybrid_solid_output.h>

#include <geode/mesh/core/hybrid_solid.h>

namespace geode
{
    template < index_t dimension >
    void save_hybrid_solid( const HybridSolid< dimension >& hybrid_solid,
        absl::string_view filename )
    {
        try
        {
            const auto output = HybridSolidOutputFactory< dimension >::create(
                to_string( extension_from_filename( filename ) ), hybrid_solid,
                filename );
            output->write();
            Logger::info( "HybridSolid", dimension, "D saved in ", filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot save HybridSolid in file: ",
                filename };
        }
    }

    template < index_t dimension >
    HybridSolidOutput< dimension >::HybridSolidOutput(
        const HybridSolid< dimension >& hybrid_solid,
        absl::string_view filename )
        : VertexSetOutput( hybrid_solid, filename ),
          hybrid_solid_( hybrid_solid )
    {
    }

    template void opengeode_mesh_api save_hybrid_solid(
        const HybridSolid< 3 >&, absl::string_view );

    template class opengeode_mesh_api HybridSolidOutput< 3 >;
} // namespace geode