/*
 * Copyright (c) 2019 Geode-solutions
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
#include <geode/mesh/io/graph_input.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an EdgedCurve.
     * The adequate loader is called depending on the filename extension.
     * @param[out] edged_curve Loaded EdgedCurve.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    void load_edged_curve(
        EdgedCurve< dimension >& edged_curve, const std::string& filename );

    template < index_t dimension >
    class EdgedCurveInput : public GraphInput
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( EdgedCurveInput );

    protected:
        EdgedCurveInput(
            EdgedCurve< dimension >& edged_curve, std::string filename );

        EdgedCurve< dimension >& edged_curve()
        {
            return edged_curve_;
        }

    private:
        EdgedCurve< dimension >& edged_curve_;
    };

    template < index_t dimension >
    using EdgedCurveInputFactory = Factory< std::string,
        EdgedCurveInput< dimension >,
        EdgedCurve< dimension >&,
        std::string >;
    ALIAS_2D_AND_3D( EdgedCurveInputFactory );
} // namespace geode
