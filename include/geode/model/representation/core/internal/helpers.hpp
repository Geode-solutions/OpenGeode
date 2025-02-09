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

#include <geode/geometry/bounding_box.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    namespace internal
    {
        template < typename Filter, typename Iterator >
        void next_filtered_internal_iterator( Iterator& iterator )
        {
            while( iterator.operator!=( iterator )
                   && iterator.Relationships::InternalRangeIterator::operator*()
                              .type()
                          != Filter::component_type_static() )
            {
                iterator.Relationships::InternalRangeIterator::operator++();
            }
        }

        template < typename Filter, typename Iterator >
        void next_filtered_embedding_iterator( Iterator& iterator )
        {
            while(
                iterator.operator!=( iterator )
                && iterator.Relationships::EmbeddingRangeIterator::operator*()
                           .type()
                       != Filter::component_type_static() )
            {
                iterator.Relationships::EmbeddingRangeIterator::operator++();
            }
        }

        template < index_t dimension, typename MeshComponentRange >
        [[nodiscard]] BoundingBox< dimension > meshes_bounding_box(
            MeshComponentRange range )
        {
            BoundingBox< dimension > box;
            for( const auto& component : range )
            {
                box.add_box( component.mesh().bounding_box() );
            }
            return box;
        }
    } // namespace internal
} // namespace geode
