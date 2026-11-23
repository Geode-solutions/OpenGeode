/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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
#include <optional>

#include <geode/basic/detail/file_content.hpp>

#include <geode/geometry/bitsery_archive.hpp>

#include <geode/image/core/bitsery_archive.hpp>

#include <geode/mesh/core/bitsery_archive.hpp>

#define BITSERY_READ( Mesh )                                                   \
    [[nodiscard]] std::vector< std::unique_ptr< Mesh > > read(                 \
        const MeshImpl& /*impl*/ ) final                                       \
    {                                                                          \
        const auto content =                                                   \
            geode::detail::map_file_content( this->filename() );               \
        geode::OpenGeodeMeshException::check_exception( content.has_value(),   \
            nullptr, geode::OpenGeodeException::TYPE::data,                    \
            "[Bitsery::read] Failed to open file: ", this->filename() );       \
        return read_mesh( content->span() );                                   \
    }                                                                          \
                                                                               \
    [[nodiscard]] std::optional< std::vector< std::unique_ptr< Mesh > > >      \
        read_bytes( geode::ByteSpan content, const MeshImpl& /*impl*/ ) final  \
    {                                                                          \
        return read_mesh( content );                                           \
    }                                                                          \
                                                                               \
private:                                                                       \
    [[nodiscard]] std::vector< std::unique_ptr< Mesh > > read_mesh(            \
        geode::ByteSpan content ) const                                        \
    {                                                                          \
        std::unique_ptr< Mesh > mesh{ new OpenGeode##Mesh{                     \
            BITSERY::constructor } };                                          \
        bool success{ false };                                                 \
        try                                                                    \
        {                                                                      \
            success = geode::detail::deserialize_bytes(                        \
                content, dynamic_cast< OpenGeode##Mesh& >( *mesh ) );          \
        }                                                                      \
        catch( const geode::OpenGeodeException& )                              \
        {                                                                      \
            throw;                                                             \
        }                                                                      \
        catch( const std::exception& exception )                               \
        {                                                                      \
            throw geode::OpenGeodeMeshException{ nullptr,                      \
                geode::OpenGeodeException::TYPE::data,                         \
                "[Bitsery::read] Error while reading file: ",                  \
                this->filename(), " (", exception.what(), ")" };               \
        }                                                                      \
        geode::OpenGeodeMeshException::check_exception( success, nullptr,      \
            geode::OpenGeodeException::TYPE::internal,                         \
            "[Bitsery::read] Error while reading file: ", this->filename() );  \
        std::vector< std::unique_ptr< Mesh > > meshes;                         \
        meshes.emplace_back( std::move( mesh ) );                              \
        return meshes;                                                         \
    }                                                                          \
                                                                               \
public:                                                                        \
    index_t object_priority() const final                                      \
    {                                                                          \
        return 0;                                                              \
    }                                                                          \
                                                                               \
    Percentage is_loadable() const final                                       \
    {                                                                          \
        return Percentage{ 1 };                                                \
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
        [[nodiscard]] AdditionalFiles additional_files() const final           \
        {                                                                      \
            return {};                                                         \
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
        [[nodiscard]] AdditionalFiles additional_files() const final           \
        {                                                                      \
            return {};                                                         \
        }                                                                      \
                                                                               \
        BITSERY_READ( Mesh )                                                   \
    }
