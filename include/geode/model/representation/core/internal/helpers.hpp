/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <optional>
#include <type_traits>
#include <vector>

#include <async++.h>

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
                              .type
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
                           .type
                       != Filter::component_type_static() )
            {
                iterator.Relationships::EmbeddingRangeIterator::operator++();
            }
        }

        /*!
         * Return true if the component has at least the given number of
         * boundaries. Stop iterating as soon as this number is reached.
         */
        template < typename Model >
        [[nodiscard]] bool has_at_least_n_boundaries( const Model& model,
            const uuid& component_id,
            index_t nb_boundaries )
        {
            index_t count{ 0 };
            for( const auto& boundary :
                model.Relationships::boundaries( component_id ) )
            {
                geode_unused( boundary );
                if( ++count >= nb_boundaries )
                {
                    return true;
                }
            }
            return count >= nb_boundaries;
        }

        /*!
         * Return the bounding box of the non-empty meshes of the given
         * components (computed in parallel), std::nullopt if all meshes are
         * empty.
         */
        template < index_t dimension, typename MeshComponentRange >
        [[nodiscard]] std::optional< BoundingBox< dimension > >
            meshes_bounding_box( MeshComponentRange range )
        {
            using Component =
                std::remove_reference_t< decltype( *range.begin() ) >;
            std::vector< const Component* > components;
            for( const auto& component : range )
            {
                if( component.mesh().nb_vertices() > 0 )
                {
                    components.push_back( &component );
                }
            }
            if( components.empty() )
            {
                return std::nullopt;
            }
            return async::parallel_map_reduce(
                async::irange( size_t{ 0 }, components.size() ),
                BoundingBox< dimension >{},
                [&components]( size_t component_id ) {
                    return components[component_id]->mesh().bounding_box();
                },
                []( BoundingBox< dimension > box,
                    const BoundingBox< dimension >& other_box ) {
                    box.add_box( other_box );
                    return box;
                } );
        }
    } // namespace internal
} // namespace geode
