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

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolyhedralSolid );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a PolyhedralSolid.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] polyhedral_solid PolyhedralSolid to save.
     * @param[in] filename Path to the file where save the PolyhedralSolid.
     */
    template < index_t dimension >
    std::vector< std::string > save_polyhedral_solid(
        const PolyhedralSolid< dimension >& polyhedral_solid,
        absl::string_view filename );

    template < index_t dimension >
    class PolyhedralSolidOutput : public Output< PolyhedralSolid< dimension > >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    protected:
        explicit PolyhedralSolidOutput( absl::string_view filename )
            : Output< PolyhedralSolid< dimension > >{ filename }
        {
        }
    };

    template < index_t dimension >
    bool is_polyhedral_solid_saveable(
        const PolyhedralSolid< dimension >& polyhedral_solid,
        absl::string_view filename );

    template < index_t dimension >
    using PolyhedralSolidOutputFactory = Factory< std::string,
        PolyhedralSolidOutput< dimension >,
        absl::string_view >;
    ALIAS_3D( PolyhedralSolidOutputFactory );
} // namespace geode
