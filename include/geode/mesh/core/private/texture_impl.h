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

#include <geode/mesh/core/texture2d.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/image/core/raster_image.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class TextureImpl
        {
            friend class bitsery::Access;
            using ElementTextureCoordinates =
                absl::InlinedVector< Point< dimension >, dimension + 1 >;

            const Point< dimension > DEFAULT_COORD;

        public:
            const RasterImage< dimension >& image() const
            {
                return image_;
            }

            void set_image( RasterImage< dimension >&& image )
            {
                image_ = std::move( image );
            }

        protected:
            const Point< dimension >& texture_coordinates_impl(
                index_t element, local_index_t vertex ) const
            {
                const auto& element_coordinates =
                    coordinates_->value( element );
                if( vertex < element_coordinates.size() )
                {
                    return element_coordinates[vertex];
                }
                return DEFAULT_COORD;
            }

            void set_texture_coordinates_impl( index_t element,
                local_index_t vertex,
                const Point< dimension >& coordinates ) const
            {
                coordinates_->modify_value(
                    element, [this, vertex, &coordinates](
                                 ElementTextureCoordinates& value ) {
                        if( vertex >= value.size() )
                        {
                            value.resize( vertex + 1, DEFAULT_COORD );
                        }
                        value[vertex] = coordinates;
                    } );
            }

            TextureImpl( AttributeManager& manager, absl::string_view name )
                : coordinates_{
                      manager.find_or_create_attribute< VariableAttribute,
                          ElementTextureCoordinates >( name, {} )
                  }
            {
            }

            TextureImpl() = default;

        private:
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this, Growable< Archive, TextureImpl >{
                                        { []( Archive& a, TextureImpl& impl ) {
                                            a.object( impl.image_ );
                                            a.ext( impl.coordinates_,
                                                bitsery::ext::StdSmartPtr{} );
                                        } } } );
            }

        private:
            RasterImage< dimension > image_;
            std::shared_ptr< VariableAttribute< ElementTextureCoordinates > >
                coordinates_;
        };
    } // namespace detail
} // namespace geode