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

#include <absl/types/span.h>

#include <geode/mesh/common.h>
#include <geode/mesh/helpers/detail/vertex_merger.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
} // namespace geode

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class PointSetMerger : public VertexMerger< PointSet< dimension > >
        {
        public:
            PointSetMerger( absl::Span< const std::reference_wrapper<
                                const PointSet< dimension > > > pointsets,
                double epsilon );
            ~PointSetMerger();

            std::unique_ptr< PointSet< dimension > > merge();
        };
        ALIAS_2D_AND_3D( PointSetMerger );
    } // namespace detail
} // namespace geode
