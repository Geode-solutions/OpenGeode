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

#include <string_view>

#include <geode/basic/factory.hpp>
#include <geode/basic/input.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( LightRegularGrid );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an LightRegularGrid.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    LightRegularGrid< dimension > load_light_regular_grid(
        std::string_view filename );

    template < index_t dimension >
    class LightRegularGridInput : public Input< LightRegularGrid< dimension > >
    {
    public:
        using Base = Input< LightRegularGrid< dimension > >;
        using typename Base::InputData;
        using typename Base::MissingFiles;

        explicit LightRegularGridInput( std::string_view filename )
            : Base{ filename }
        {
        }

        LightRegularGrid< dimension > read();
    };
    ALIAS_2D_AND_3D( LightRegularGridInput );

    template < index_t dimension >
    typename LightRegularGridInput< dimension >::MissingFiles
        check_light_regular_grid_missing_files( std::string_view filename );

    template < index_t dimension >
    bool is_light_regular_grid_loadable( std::string_view filename );

    template < index_t dimension >
    using LightRegularGridInputFactory = Factory< std::string,
        LightRegularGridInput< dimension >,
        std::string_view >;
    ALIAS_2D_AND_3D( LightRegularGridInputFactory );
} // namespace geode
