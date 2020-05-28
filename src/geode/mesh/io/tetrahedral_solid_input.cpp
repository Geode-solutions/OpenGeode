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

#include <geode/mesh/io/tetrahedral_solid_input.h>

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/tetrahedral_solid.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > > load_tetrahedral_solid(
        const MeshType& type, absl::string_view filename )
    {
        try
        {
            auto tetrahedral_solid =
                TetrahedralSolid< dimension >::create( type );
            auto input = TetrahedralSolidInputFactory< dimension >::create(
                extension_from_filename( filename ).data(), *tetrahedral_solid,
                filename );
            input->read();
            return tetrahedral_solid;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot load TetrahedralSolid from file: ", filename
            };
        }
    }

    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > > load_tetrahedral_solid(
        absl::string_view filename )
    {
        return load_tetrahedral_solid< dimension >(
            MeshFactory::default_mesh(
                TetrahedralSolid< dimension >::kind_name_static() ),
            filename );
    }

    template < index_t dimension >
    TetrahedralSolidInput< dimension >::TetrahedralSolidInput(
        TetrahedralSolid< dimension >& tetrahedral_solid,
        absl::string_view filename )
        : PolyhedralSolidInput< dimension >( tetrahedral_solid, filename ),
          tetrahedral_solid_( tetrahedral_solid )
    {
    }

    template std::unique_ptr< TetrahedralSolid< 3 > > opengeode_mesh_api
        load_tetrahedral_solid( const MeshType&, absl::string_view );

    template std::unique_ptr< TetrahedralSolid< 3 > >
        opengeode_mesh_api load_tetrahedral_solid( absl::string_view );

    template class opengeode_mesh_api TetrahedralSolidInput< 3 >;
} // namespace geode
