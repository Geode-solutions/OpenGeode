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

#include <geode/model/representation/io/section_input.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/identifier_builder.h>
#include <geode/basic/timer.h>

#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    Section load_section( absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION( SectionInputFactory::has_creator( extension ),
                "Unknown extension: ", extension );
            auto input = SectionInputFactory::create( extension, filename );
            auto section = input->read();
            if( section.name() == Identifier::DEFAULT_NAME )
            {
                IdentifierBuilder{ section }.set_name(
                    filename_without_extension( filename ) );
            }
            Logger::info(
                "Section loaded from ", filename, " in ", timer.duration() );
            Logger::info( "Section has: ", section.nb_surfaces(), " Surfaces, ",
                section.nb_lines(), " Lines, ", section.nb_corners(),
                " Corners, ", section.nb_model_boundaries(),
                " ModelBoundaries" );
            return section;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load Section from file: ",
                filename };
        }
    }
} // namespace geode
