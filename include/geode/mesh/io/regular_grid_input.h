/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/factory.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/mesh_id.h>
#include <geode/mesh/io/input.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGrid );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an RegularGrid.
     * The adequate loader is called depending on the filename extension.
     * @param[in] impl Data structure implementation.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    std::unique_ptr< RegularGrid< dimension > > load_regular_grid(
        const MeshImpl& impl, absl::string_view filename );

    /*!
     * API function for loading a RegularGrid.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    std::unique_ptr< RegularGrid< dimension > > load_regular_grid(
        absl::string_view filename );

    template < index_t dimension >
    class RegularGridInput
        : public Input< std::unique_ptr< RegularGrid< dimension > >, MeshImpl >
    {
    protected:
        RegularGridInput( absl::string_view filename )
            : Input< std::unique_ptr< RegularGrid< dimension > >, MeshImpl >{
                  filename
              }
        {
        }
    };

    template < index_t dimension >
    using RegularGridInputFactory = Factory< std::string,
        RegularGridInput< dimension >,
        absl::string_view >;
    ALIAS_2D_AND_3D( RegularGridInputFactory );
} // namespace geode
