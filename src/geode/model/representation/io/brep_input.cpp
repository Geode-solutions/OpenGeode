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

#include <geode/model/representation/io/brep_input.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/timer.h>

#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    BRep load_brep( absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION( BRepInputFactory::has_creator( extension ),
                "Unknown extension: ", extension );
            auto input = BRepInputFactory::create( extension, filename );
            auto brep = input->read();
            if( brep.name() == Identifier::DEFAULT_NAME )
            {
                BRepBuilder{ brep }.set_name(
                    filename_without_extension( filename ) );
            }
            Logger::info(
                "BRep loaded from ", filename, " in ", timer.duration() );
            Logger::info( "BRep has: ", brep.nb_blocks(), " Blocks, ",
                brep.nb_surfaces(), " Surfaces, ", brep.nb_lines(), " Lines, ",
                brep.nb_corners(), " Corners, ", brep.nb_model_boundaries(),
                " ModelBoundaries" );
            return brep;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load BRep from file: ",
                filename };
        }
    }
} // namespace geode
