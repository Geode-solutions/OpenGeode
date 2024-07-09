/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#pragma once

#include <memory>

#include <absl/strings/ascii.h>
#include <absl/strings/string_view.h>

#include <geode/basic/filename.h>
#include <geode/basic/identifier.h>
#include <geode/basic/identifier_builder.h>
#include <geode/basic/logger.h>
#include <geode/basic/timer.h>

namespace geode
{
    namespace detail
    {
        template < typename Object >
        void update_default_name( Object& object, absl::string_view filename )
        {
            if( object.name() == Identifier::DEFAULT_NAME )
            {
                IdentifierBuilder{ object }.set_name(
                    filename_without_extension( filename ) );
            }
        }

        template < typename Object >
        void update_default_name(
            std::unique_ptr< Object >& object, absl::string_view filename )
        {
            if( object->name() == Identifier::DEFAULT_NAME )
            {
                IdentifierBuilder{ *object }.set_name(
                    filename_without_extension( filename ) );
            }
        }

        template < typename Factory >
        std::unique_ptr< typename Factory::BaseClass >
            geode_object_input_reader(
                const Factory& factory, absl::string_view& filename )
        {
            filename = absl::StripAsciiWhitespace( filename );
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION( factory.has_creator( extension ),
                "Unknown extension: ", extension );
            return factory.create(
                extension, expand_predefined_folders( filename ) );
        }

        template < typename Factory, typename... Args >
        typename Factory::BaseClass::InputData geode_object_input_impl(
            const Factory& fatory,
            absl::string_view type,
            absl::string_view filename,
            Args... args )
        {
            const Timer timer;
            auto input = geode_object_input_reader( fatory, filename );
            auto object = input->read( std::forward< Args >( args )... );
            update_default_name( object, filename );
            Logger::info(
                type, " loaded from ", filename, " in ", timer.duration() );
            return object;
        }

        inline void add_to_message( std::string& message,
            geode::index_t nb_components,
            absl::string_view component_text )
        {
            if( nb_components > 0 )
            {
                absl::StrAppend( &message, nb_components, component_text );
            }
        }
    } // namespace detail
} // namespace geode
