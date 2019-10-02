/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/mesh/core/bitsery_archive.h>

#define BITSERY_DO_READ()                                                      \
    void do_read() final                                                       \
    {                                                                          \
        std::ifstream file{ this->filename(), std::ifstream::binary };         \
        TContext context{};                                                    \
        register_basic_deserialize_pcontext( std::get< 0 >( context ) );       \
        register_mesh_deserialize_pcontext( std::get< 0 >( context ) );        \
        Deserializer archive{ context, file };                                 \
        archive.object( mesh_ );                                               \
        auto& adapter = archive.adapter();                                     \
        OPENGEODE_EXCEPTION( adapter.error() == bitsery::ReaderError::NoError  \
                                 && adapter.isCompletedSuccessfully()          \
                                 && std::get< 1 >( context ).isValid(),        \
            "[Bitsery::read] Error while reading file: " + this->filename() );                 \
    }

#define BITSERY_INPUT_MESH_DIMENSION( Mesh )                                   \
    template < index_t dimension >                                             \
    class OpenGeode##Mesh##Input : public Mesh##Input< dimension >             \
    {                                                                          \
    public:                                                                    \
        OpenGeode##Mesh##Input(                                                \
            Mesh< dimension >& mesh, std::string filename )                    \
            : Mesh##Input< dimension >( mesh, std::move( filename ) ),         \
              mesh_( dynamic_cast< OpenGeode##Mesh< dimension >& >( mesh ) )   \
        {                                                                      \
        }                                                                      \
                                                                               \
        BITSERY_DO_READ()                                                      \
                                                                               \
    private:                                                                   \
        OpenGeode##Mesh< dimension >& mesh_;                                   \
    };                                                                         \
    ALIAS_2D_AND_3D( OpenGeode##Mesh##Input )

#define BITSERY_INPUT_MESH_NO_DIMENSION( Mesh )                                \
    class OpenGeode##Mesh##Input : public Mesh##Input                          \
    {                                                                          \
    public:                                                                    \
        OpenGeode##Mesh##Input( Mesh& mesh, std::string filename )             \
            : Mesh##Input( mesh, std::move( filename ) ),                      \
              mesh_( dynamic_cast< OpenGeode##Mesh& >( mesh ) )                \
        {                                                                      \
        }                                                                      \
                                                                               \
        BITSERY_DO_READ()                                                      \
                                                                               \
    private:                                                                   \
        OpenGeode##Mesh& mesh_;                                                \
    }
