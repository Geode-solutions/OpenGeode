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

#include <geode/mesh/io/polyhedral_solid_input.h>

#include <geode/mesh/core/polyhedral_solid.h>

namespace geode
{
    template < index_t dimension >
    void load_polyhedral_solid( PolyhedralSolid< dimension >& polyhedral_solid,
        absl::string_view filename )
    {
        try
        {
            auto input = PolyhedralSolidInputFactory< dimension >::create(
                extension_from_filename( filename ).data(), polyhedral_solid,
                filename );
            input->read();
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load PolyhedralSolid from file: ",
                filename };
        }
    }

    template < index_t dimension >
    PolyhedralSolidInput< dimension >::PolyhedralSolidInput(
        PolyhedralSolid< dimension >& polyhedral_solid,
        absl::string_view filename )
        : VertexSetInput( polyhedral_solid, filename ),
          polyhedral_solid_( polyhedral_solid )
    {
    }

    template void opengeode_mesh_api load_polyhedral_solid(
        PolyhedralSolid< 3 >&, absl::string_view );

    template class opengeode_mesh_api PolyhedralSolidInput< 3 >;
} // namespace geode
