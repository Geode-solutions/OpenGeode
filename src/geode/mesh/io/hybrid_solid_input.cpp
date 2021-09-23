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

#include <geode/mesh/io/hybrid_solid_input.h>

#include <geode/basic/filename.h>

#include <geode/mesh/builder/hybrid_solid_builder.h>
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< HybridSolid< dimension > > load_hybrid_solid(
        const MeshImpl& impl, absl::string_view filename )
    {
        try
        {
            auto hybrid_solid = HybridSolid< dimension >::create( impl );
            auto input = HybridSolidInputFactory< dimension >::create(
                to_string( extension_from_filename( filename ) ), *hybrid_solid,
                filename );
            input->read();
            if( hybrid_solid->name() == Identifier::DEFAULT_NAME )
            {
                HybridSolidBuilder< dimension >::create( *hybrid_solid )
                    ->set_name( filename_without_extension( filename ) );
            }
            Logger::info(
                "HybridSolid", dimension, "D loaded from ", filename );
            Logger::info( "HybridSolid", dimension,
                "D has: ", hybrid_solid->nb_vertices(), " vertices, ",
                hybrid_solid->nb_polyhedra(), " polyhedra" );
            return hybrid_solid;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load HybridSolid from file: ",
                filename };
        }
    }

    template < index_t dimension >
    std::unique_ptr< HybridSolid< dimension > > load_hybrid_solid(
        absl::string_view filename )
    {
        return load_hybrid_solid< dimension >(
            MeshFactory::default_impl(
                HybridSolid< dimension >::type_name_static() ),
            filename );
    }

    template < index_t dimension >
    HybridSolidInput< dimension >::HybridSolidInput(
        HybridSolid< dimension >& hybrid_solid, absl::string_view filename )
        : VertexSetInput( hybrid_solid, filename ),
          hybrid_solid_( hybrid_solid )
    {
    }

    template std::unique_ptr< HybridSolid< 3 > > opengeode_mesh_api
        load_hybrid_solid( const MeshImpl&, absl::string_view );

    template std::unique_ptr< HybridSolid< 3 > >
        opengeode_mesh_api load_hybrid_solid( absl::string_view );

    template class opengeode_mesh_api HybridSolidInput< 3 >;
} // namespace geode
