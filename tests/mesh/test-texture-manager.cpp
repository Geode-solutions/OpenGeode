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

#include <fstream>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/bitsery_archive.h>
#include <geode/geometry/point.h>

#include <geode/image/core/bitsery_archive.h>
#include <geode/image/core/raster_image.h>
#include <geode/image/core/rgb_color.h>

#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/texture2d.h>
#include <geode/mesh/core/texture_manager.h>
#include <geode/mesh/core/texture_storage.h>

#include <geode/tests/common.h>

geode::RasterImage2D create_raster()
{
    geode::RasterImage2D raster{ { 10, 10 } };
    for( const auto i : geode::LRange{ 100 } )
    {
        raster.set_color( i, { i, i, i } );
    }
    return raster;
}

void create_texture(
    geode::AttributeManager& attributes, geode::TextureStorage2D& storage )
{
    geode::TextureManager2D manager{ attributes, storage };
    auto& texture = manager.find_or_create_texture( "texture" );
    for( const auto i : geode::Range{ 42 } )
    {
        texture.set_texture_coordinates(
            { i, 0 }, geode::Point2D{ { i * 2., i * 3. } } );
    }
    texture.set_image( create_raster() );
}

void check_texture(
    geode::AttributeManager& attributes, geode::TextureStorage2D& storage )
{
    const geode::TextureManager2D manager{ attributes, storage };
    const auto& texture = manager.find_texture( "texture" );
    for( const auto i : geode::Range{ 42 } )
    {
        const geode::Point2D coord{ { i * 2., i * 3. } };
        OPENGEODE_EXCEPTION( texture.texture_coordinates( { i, 0 } ) == coord,
            "[Test] Wrong texture coordinates" );
    }
    const auto& image = texture.image();
    for( const auto i : geode::LRange{ 100 } )
    {
        const geode::RGBColor color{ i, i, i };
        OPENGEODE_EXCEPTION(
            image.color( i ) == color, "[Test] Wrong color image" );
    }
}

void save( const geode::TextureStorage2D& storage, std::string_view filename )
{
    std::ofstream file{ geode::to_string( filename ), std::ofstream::binary };
    geode::TContext context{};
    geode::register_basic_serialize_pcontext( std::get< 0 >( context ) );
    geode::register_geometry_serialize_pcontext( std::get< 0 >( context ) );
    geode::register_image_serialize_pcontext( std::get< 0 >( context ) );
    geode::register_mesh_serialize_pcontext( std::get< 0 >( context ) );
    geode::Serializer archive{ context, file };
    archive.object( storage );
    archive.adapter().flush();
    OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
        "[Bitsery::write] Error while writing file: ", filename );
}

geode::TextureStorage2D load( std::string_view filename )
{
    std::ifstream file{ geode::to_string( filename ), std::ifstream::binary };
    geode::TContext context{};
    geode::register_basic_deserialize_pcontext( std::get< 0 >( context ) );
    geode::register_geometry_deserialize_pcontext( std::get< 0 >( context ) );
    geode::register_image_deserialize_pcontext( std::get< 0 >( context ) );
    geode::register_mesh_deserialize_pcontext( std::get< 0 >( context ) );
    geode::Deserializer archive{ context, file };
    geode::TextureStorage2D storage;
    archive.object( storage );
    const auto& adapter = archive.adapter();
    OPENGEODE_EXCEPTION( adapter.error() == bitsery::ReaderError::NoError
                             && adapter.isCompletedSuccessfully()
                             && std::get< 1 >( context ).isValid(),
        "[Bitsery::read] Error while reading file: ", filename );
    return storage;
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    geode::AttributeManager attributes;
    attributes.resize( 42 );
    geode::TextureStorage2D storage;
    create_texture( attributes, storage );
    save( storage, "storage" );
    auto reload = load( "storage" );
    check_texture( attributes, reload );
}

OPENGEODE_TEST( "texture-manager" )
