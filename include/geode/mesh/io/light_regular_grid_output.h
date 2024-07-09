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

#include <string>
#include <vector>

#include <absl/strings/string_view.h>

#include <geode/basic/factory.h>
#include <geode/basic/output.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/bitsery_archive.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( LightRegularGrid );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a LightRegularGrid.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] light_regular_grid LightRegularGrid to save.
     * @param[in] filename Path to the file where save the LightRegularGrid.
     */
    template < index_t dimension >
    std::vector< std::string > save_light_regular_grid(
        const LightRegularGrid< dimension >& light_regular_grid,
        absl::string_view filename );

    template < index_t dimension >
    class LightRegularGridOutput
        : public Output< LightRegularGrid< dimension > >
    {
    public:
        explicit LightRegularGridOutput( absl::string_view filename )
            : Output< LightRegularGrid< dimension > >{ filename }
        {
        }

        std::vector< std::string > write(
            const LightRegularGrid< dimension >& grid ) const;
    };
    ALIAS_2D_AND_3D( LightRegularGridOutput );

    template < index_t dimension >
    bool is_light_regular_grid_saveable(
        const LightRegularGrid< dimension >& light_regular_grid,
        absl::string_view filename );

    template < index_t dimension >
    using LightRegularGridOutputFactory = Factory< std::string,
        LightRegularGridOutput< dimension >,
        absl::string_view >;
    FACTORY_2D_AND_3D(
        LightRegularGridOutputFactory, light_regular_grid_output_factory );
} // namespace geode
