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

#include <geode/image/core/bitsery_archive.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_attribute.hpp>

#include <geode/image/core/greyscale_color.hpp>
#include <geode/image/core/rgb_color.hpp>

namespace
{
    template < typename Serializer >
    void register_image_pcontext( geode::PContext& context )
    {
        geode::register_attribute_type< geode::GreyscaleColor, Serializer >(
            context, "GreyScaleColor" );
        geode::register_attribute_type< geode::RGBColor, Serializer >(
            context, "RGBColor" );
    }
} // namespace

namespace geode
{
    void register_image_serialize_pcontext( PContext& context )
    {
        register_image_pcontext< Serializer >( context );
    }

    void register_image_deserialize_pcontext( PContext& context )
    {
        register_image_pcontext< Deserializer >( context );
    }
} // namespace geode
