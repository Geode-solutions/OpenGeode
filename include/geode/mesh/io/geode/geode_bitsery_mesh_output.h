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

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <geode/geometry/bitsery_archive.h>

#include <geode/image/core/bitsery_archive.h>

#include <geode/mesh/core/bitsery_archive.h>

#define BITSERY_WRITE( Mesh )                                                  \
    std::vector< std::string > write( const Mesh& mesh ) const final           \
    {                                                                          \
        std::ofstream file{ to_string( this->filename() ),                     \
            std::ofstream::binary };                                           \
        TContext context{};                                                    \
        BitseryExtensions::register_serialize_pcontext(                        \
            std::get< 0 >( context ) );                                        \
        Serializer archive{ context, file };                                   \
        archive.object( dynamic_cast< const OpenGeode##Mesh& >( mesh ) );      \
        archive.adapter().flush();                                             \
        OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),               \
            "[Bitsery::write] Error while writing file: ", this->filename() ); \
        return { to_string( this->filename() ) };                              \
    }

#define BITSERY_OUTPUT_MESH_DIMENSION( Mesh )                                  \
    template < index_t dimension >                                             \
    class OpenGeode##Mesh##Output : public Mesh##Output< dimension >           \
    {                                                                          \
    public:                                                                    \
        OpenGeode##Mesh##Output( absl::string_view filename )                  \
            : Mesh##Output< dimension >( filename )                            \
        {                                                                      \
        }                                                                      \
                                                                               \
        BITSERY_WRITE( Mesh< dimension > )                                     \
    };                                                                         \
    ALIAS_2D_AND_3D( OpenGeode##Mesh##Output )

#define BITSERY_OUTPUT_MESH_NO_DIMENSION( Mesh )                               \
    class OpenGeode##Mesh##Output : public Mesh##Output                        \
    {                                                                          \
    public:                                                                    \
        OpenGeode##Mesh##Output( absl::string_view filename )                  \
            : Mesh##Output( filename )                                         \
        {                                                                      \
        }                                                                      \
                                                                               \
        BITSERY_WRITE( Mesh )                                                  \
    }
