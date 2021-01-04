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

#include <geode/basic/factory.h>

#include <geode/mesh/common.h>
#include <geode/mesh/io/output.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGrid );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a RegularGrid.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] regular_grid RegularGrid to save.
     * @param[in] filename Path to the file where save the RegularGrid.
     */
    template < index_t dimension >
    void save_regular_grid( const RegularGrid< dimension >& regular_grid,
        absl::string_view filename );

    template < index_t dimension >
    class RegularGridOutput : public Output
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( RegularGridOutput );

    protected:
        RegularGridOutput( const RegularGrid< dimension >& regular_grid,
            absl::string_view filename );

        const RegularGrid< dimension >& regular_grid() const
        {
            return regular_grid_;
        }

    private:
        const RegularGrid< dimension >& regular_grid_;
    };

    template < index_t dimension >
    using RegularGridOutputFactory = Factory< std::string,
        RegularGridOutput< dimension >,
        const RegularGrid< dimension >&,
        absl::string_view >;
    ALIAS_2D_AND_3D( RegularGridOutputFactory );
} // namespace geode
