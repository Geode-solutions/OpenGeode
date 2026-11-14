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

#include "../../common.hpp"

#include <geode/mesh/core/texture1d.hpp>
#include <geode/mesh/core/texture2d.hpp>
#include <geode/mesh/core/texture3d.hpp>
#include <geode/mesh/core/texture_manager.hpp>

namespace geode
{
    /*!
     * Return the texture to Python, keeping the manager alive, and give it the
     * `_nb_elements` of the manager to check the elements (see texture.cpp)
     */
    template < index_t dimension >
    TypedObject< Texture< dimension > > texture_with_elements(
        const TypedObject< TextureManager< dimension > >& manager,
        const Texture< dimension >& texture )
    {
        auto result =
            pybind11::reinterpret_steal< TypedObject< Texture< dimension > > >(
                pybind11::cast( &texture,
                    pybind11::return_value_policy::reference_internal, manager )
                    .release() );
        if( pybind11::hasattr( manager, "_nb_elements" ) )
        {
            result.attr( "_nb_elements" ) = manager.attr( "_nb_elements" );
        }
        return result;
    }
} // namespace geode

#define PYTHON_TEXTURE_MANAGER( dimension )                                    \
    const auto name##dimension =                                               \
        "TextureManager" + std::to_string( dimension ) + "D";                  \
    pybind11::class_< TextureManager< dimension > >( module,                   \
        name##dimension.c_str(), pybind11::dynamic_attr(),                     \
        "Named textures of a mesh" )                                           \
        .def(                                                                  \
            "find_or_create_texture",                                          \
            []( const TypedObject< TextureManager< dimension > >& self,        \
                std::string_view name ) {                                      \
                return texture_with_elements(                                  \
                    self, self.cast< TextureManager< dimension >& >()          \
                              .find_or_create_texture( name ) );               \
            },                                                                 \
            pybind11::arg( "name" ),                                           \
            "Return the texture with this name, creating it if it does not "   \
            "exist. The texture keeps the manager alive" )                     \
        .def(                                                                  \
            "find_texture",                                                    \
            []( const TypedObject< TextureManager< dimension > >& self,        \
                std::string_view name ) {                                      \
                return texture_with_elements(                                  \
                    self, self.cast< const TextureManager< dimension >& >()    \
                              .find_texture( name ) );                         \
            },                                                                 \
            pybind11::arg( "name" ),                                           \
            "Return the texture with this name; raise an error if there is "   \
            "none. The texture keeps the manager alive" )                      \
        .def( "texture_names", &TextureManager< dimension >::texture_names,    \
            "Return the names of the textures" )                               \
        .def( "texture_exists", &TextureManager< dimension >::texture_exists,  \
            "Return True if a texture has this name",                          \
            pybind11::arg( "name" ) )                                          \
        .def( "delete_texture", &TextureManager< dimension >::delete_texture,  \
            "Delete the texture with this name and its attribute; raise an "   \
            "error if there is none",                                          \
            pybind11::arg( "name" ) )

namespace geode
{
    void define_texture_manager( pybind11::module& module )
    {
        PYTHON_TEXTURE_MANAGER( 1 );
        PYTHON_TEXTURE_MANAGER( 2 );
        PYTHON_TEXTURE_MANAGER( 3 );
    }
} // namespace geode
