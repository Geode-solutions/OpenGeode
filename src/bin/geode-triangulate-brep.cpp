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

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/solid_mesh.h>

#include <geode/model/helpers/convert_model_meshes.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>
#include <geode/model/representation/io/brep_output.h>

ABSL_FLAG( std::string, input, "/path/my/brep.og_brep", "Input brep" );
ABSL_FLAG(
    std::string, output, "/path/my/remeshed_brep.og_brep", "Output brep" );

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage(
            absl::StrCat( "Geode BRep triangulate from Geode-solutions.\n",
                "Sample usage:\n", argv[0],
                " --input my_brep.og_brep --output remeshed_brep.og_brep" ) );
        absl::ParseCommandLine( argc, argv );

        auto brep = geode::load_brep( absl::GetFlag( FLAGS_input ) );
        for( const auto& block : brep.blocks() )
        {
            if( block.mesh().nb_polyhedra() )
            {
                geode::Logger::warn( "Volumetric meshes are ignored from "
                                     "triangulation process" );
                break;
            }
        }
        geode::triangulate_surface_meshes( brep );
        geode::convert_surface_meshes_into_triangulated_surfaces( brep );
        geode::save_brep( brep, absl::GetFlag( FLAGS_output ) );

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
