/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geometry/bitsery_archive.hpp>

#include <geode/image/core/bitsery_archive.hpp>

#include <geode/mesh/core/bitsery_archive.hpp>

#define BITSERY_READ( Mesh )                                                   \
    [[nodiscard]] std::unique_ptr< Mesh > read( const MeshImpl& impl ) final   \
    {                                                                          \
        std::ifstream file{ to_string( this->filename() ),                     \
            std::ifstream::binary };                                           \
        OPENGEODE_EXCEPTION( file, "[Bitsery::read] Failed to open file: ",    \
            to_string( this->filename() ) );                                   \
        TContext context{};                                                    \
        BitseryExtensions::register_deserialize_pcontext(                      \
            std::get< 0 >( context ) );                                        \
        Deserializer archive{ context, file };                                 \
        auto mesh = Mesh::create( impl );                                      \
        archive.object( dynamic_cast< OpenGeode##Mesh& >( *mesh ) );           \
        const auto& adapter = archive.adapter();                               \
        OPENGEODE_EXCEPTION( adapter.error() == bitsery::ReaderError::NoError  \
                                 && adapter.isCompletedSuccessfully()          \
                                 && std::get< 1 >( context ).isValid(),        \
            "[Bitsery::read] Error while reading file: ", this->filename() );  \
        return mesh;                                                           \
    }

#define BITSERY_INPUT_MESH_DIMENSION( Mesh )                                   \
    template < index_t dimension >                                             \
    class OpenGeode##Mesh##Input : public Mesh##Input< dimension >             \
    {                                                                          \
    public:                                                                    \
        explicit OpenGeode##Mesh##Input( std::string_view filename )           \
            : Mesh##Input< dimension >( filename )                             \
        {                                                                      \
        }                                                                      \
                                                                               \
        BITSERY_READ( Mesh< dimension > )                                      \
    };                                                                         \
    ALIAS_2D_AND_3D( OpenGeode##Mesh##Input )

#define BITSERY_INPUT_MESH_NO_DIMENSION( Mesh )                                \
    class OpenGeode##Mesh##Input : public Mesh##Input                          \
    {                                                                          \
    public:                                                                    \
        explicit OpenGeode##Mesh##Input( std::string_view filename )           \
            : Mesh##Input( filename )                                          \
        {                                                                      \
        }                                                                      \
                                                                               \
        BITSERY_READ( Mesh )                                                   \
    }
