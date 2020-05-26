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

#include <geode/mesh/io/polyhedral_solid_input.h>

#include <geode/mesh/core/polyhedral_solid.h>

namespace
{
    template < geode::index_t dimension >
    void load( geode::PolyhedralSolid< dimension >& polyhedral_solid,
        absl::string_view filename )
    {
        auto input = geode::PolyhedralSolidInputFactory< dimension >::create(
            geode::extension_from_filename( filename ).data(), polyhedral_solid,
            filename );
        input->read();
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > > load_polyhedral_solid(
        const MeshType& type, absl::string_view filename )
    {
        try
        {
            auto polyhedral_solid =
                PolyhedralSolid< dimension >::create( type );
            load( *polyhedral_solid, filename );
            return polyhedral_solid;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load PolyhedralSolid from file: ",
                filename };
        }
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > > load_polyhedral_solid(
        absl::string_view filename )
    {
        try
        {
            auto polyhedral_solid = PolyhedralSolid< dimension >::create();
            load( *polyhedral_solid, filename );
            return polyhedral_solid;
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

    template std::unique_ptr< PolyhedralSolid< 3 > > opengeode_mesh_api
        load_polyhedral_solid( const MeshType&, absl::string_view );

    template std::unique_ptr< PolyhedralSolid< 3 > >
        opengeode_mesh_api load_polyhedral_solid( absl::string_view );

    template class opengeode_mesh_api PolyhedralSolidInput< 3 >;
} // namespace geode
