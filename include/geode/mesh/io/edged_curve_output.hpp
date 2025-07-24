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

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <geode/basic/factory.hpp>
#include <geode/basic/output.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a EdgedCurve.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] edged_curve EdgedCurve to save.
     * @param[in] filename Path to the file where save the EdgedCurve.
     */
    template < index_t dimension >
    std::vector< std::string > save_edged_curve(
        const EdgedCurve< dimension >& edged_curve, std::string_view filename );

    template < index_t dimension >
    class EdgedCurveOutput : public Output< EdgedCurve< dimension > >
    {
    protected:
        explicit EdgedCurveOutput( std::string_view filename )
            : Output< EdgedCurve< dimension > >{ filename }
        {
        }
    };
    ALIAS_2D_AND_3D( EdgedCurveOutput );

    template < index_t dimension >
    [[nodiscard]] bool is_edged_curve_saveable(
        const EdgedCurve< dimension >& edged_curve, std::string_view filename );

    template < index_t dimension >
    using EdgedCurveOutputFactory =
        Factory< std::string, EdgedCurveOutput< dimension >, std::string_view >;
    ALIAS_2D_AND_3D( EdgedCurveOutputFactory );
} // namespace geode
