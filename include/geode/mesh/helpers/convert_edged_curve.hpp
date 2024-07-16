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

#include <memory>

#include <absl/types/span.h>

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    ALIAS_2D_AND_3D( EdgedCurve );
} // namespace geode

namespace geode
{
    std::unique_ptr< EdgedCurve2D >
        opengeode_mesh_api convert_edged_curve3d_into_2d(
            const EdgedCurve3D& curve3d, index_t axis_to_remove );

    std::unique_ptr< EdgedCurve3D > opengeode_mesh_api
        convert_edged_curve2d_into_3d( const EdgedCurve2D& curve2d,
            index_t axis_to_add,
            double axis_coordinate );

    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > > merge_edged_curves( absl::Span<
        const std::reference_wrapper< const EdgedCurve< dimension > > >
            curves );
} // namespace geode