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

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "core/greyscale_color.h"
#include "core/raster_image.h"
#include "core/rgb_color.h"

#include "io/raster_image.h"

namespace pybind11
{
    namespace detail
    {
        template < typename Type >
        struct type_caster< absl::FixedArray< Type > >
            : list_caster< absl::FixedArray< Type >, Type >
        {
        };

        template < typename Type >
        struct type_caster< absl::Span< Type > >
            : list_caster< absl::Span< Type >, Type >
        {
            using value_conv = make_caster< Type >;

            bool load( handle src, bool convert )
            {
                cpp_.clear();
                auto s = reinterpret_borrow< sequence >( src );
                cpp_.reserve( s.size() );
                for( auto it : s )
                {
                    value_conv conv;
                    if( !conv.load( it, convert ) )
                        return false;
                    cpp_.push_back( cast_op< Type&& >( std::move( conv ) ) );
                }
                this->value = absl::MakeConstSpan( cpp_ );
                return true;
            }

            std::vector< typename std::remove_const< Type >::type > cpp_;
        };

        template < typename Type, size_t dimension >
        struct type_caster< absl::InlinedVector< Type, dimension > >
            : list_caster< absl::InlinedVector< Type, dimension >, Type >
        {
        };

        template < typename T >
        struct type_caster< absl::optional< T > >
            : public optional_caster< absl::optional< T > >
        {
        };

        template <>
        struct type_caster< absl::string_view >
            : string_caster< absl::string_view, true >
        {
        };
    } // namespace detail
} // namespace pybind11

PYBIND11_MODULE( opengeode_py_image, module )
{
    pybind11::add_ostream_redirect( module );
    module.doc() = "OpenGeode Python binding for image";
    pybind11::class_< geode::OpenGeodeImage >( module, "OpenGeodeImage" )
        .def( "initialize", &geode::OpenGeodeImage::initialize );
    geode::define_greyscale_color( module );
    geode::define_raster_image( module );
    geode::define_raster_image_io( module );
    geode::define_rgb_color( module );
}