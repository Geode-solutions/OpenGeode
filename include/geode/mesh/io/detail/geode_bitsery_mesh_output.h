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

#pragma once

#include <fstream>

#include <geode/geometry/bitsery_archive.h>

#include <geode/mesh/core/bitsery_archive.h>

#define BITSERY_WRITE()                                                        \
    void write() const final                                                   \
    {                                                                          \
        std::ofstream file{ this->filename().data(), std::ofstream::binary };  \
        TContext context{};                                                    \
        register_basic_serialize_pcontext( std::get< 0 >( context ) );         \
        register_geometry_serialize_pcontext( std::get< 0 >( context ) );      \
        register_mesh_serialize_pcontext( std::get< 0 >( context ) );          \
        Serializer archive{ context, file };                                   \
        archive.object( mesh_ );                                               \
        archive.adapter().flush();                                             \
        OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),               \
            "[Bitsery::write] Error while writing file: ", this->filename() ); \
    }

#define BITSERY_OUTPUT_MESH_DIMENSION_IMPL( Mesh, MeshImpl )                   \
    template < index_t dimension >                                             \
    class OpenGeode##Mesh##Output : public Mesh##Output< dimension >           \
    {                                                                          \
    public:                                                                    \
        OpenGeode##Mesh##Output(                                               \
            const Mesh< dimension >& mesh, absl::string_view filename )        \
            : Mesh##Output< dimension >( mesh, filename ),                     \
              mesh_( dynamic_cast< const MeshImpl< dimension >& >( mesh ) )    \
        {                                                                      \
        }                                                                      \
                                                                               \
        BITSERY_WRITE()                                                        \
                                                                               \
    private:                                                                   \
        const MeshImpl< dimension >& mesh_;                                    \
    };                                                                         \
    ALIAS_2D_AND_3D( OpenGeode##Mesh##Output )

#define BITSERY_OUTPUT_MESH_DIMENSION( Mesh )                                  \
    BITSERY_OUTPUT_MESH_DIMENSION_IMPL( Mesh, OpenGeode##Mesh )

#define BITSERY_OUTPUT_MESH_NO_DIMENSION_IMPL( Mesh, MeshImpl )                \
    class OpenGeode##Mesh##Output : public Mesh##Output                        \
    {                                                                          \
    public:                                                                    \
        OpenGeode##Mesh##Output(                                               \
            const Mesh& mesh, absl::string_view filename )                     \
            : Mesh##Output( mesh, filename ),                                  \
              mesh_( dynamic_cast< const MeshImpl& >( mesh ) )                 \
        {                                                                      \
        }                                                                      \
                                                                               \
        BITSERY_WRITE()                                                        \
                                                                               \
    private:                                                                   \
        const MeshImpl& mesh_;                                                 \
    }

#define BITSERY_OUTPUT_MESH_NO_DIMENSION( Mesh )                               \
    BITSERY_OUTPUT_MESH_NO_DIMENSION_IMPL( Mesh, OpenGeode##Mesh )
