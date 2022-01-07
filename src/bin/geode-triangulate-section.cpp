/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/model/helpers/convert_model_meshes.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/section_input.h>
#include <geode/model/representation/io/section_output.h>

ABSL_FLAG( std::string, input, "/path/my/section.og_section", "Input section" );
ABSL_FLAG( std::string,
    output,
    "/path/my/remeshed_section.og_section",
    "Output section" );

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage(
            absl::StrCat( "Geode Section triangulate from Geode-solutions.\n",
                "Sample usage:\n", argv[0],
                " --input my_section.og_section --output "
                "remeshed_section.og_section" ) );
        absl::ParseCommandLine( argc, argv );

        auto section = geode::load_section( absl::GetFlag( FLAGS_input ) );
        geode::triangulate_surface_meshes( section );
        geode::convert_surface_meshes_into_triangulated_surfaces( section );
        geode::save_section( section, absl::GetFlag( FLAGS_output ) );

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
