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

#include <list>
#include <memory>
#include <utility>

#include <geode/model/common.hpp>

namespace geode
{
    struct uuid;
} // namespace geode

namespace geode
{
    namespace detail
    {
        /*!
         * Iteration state over the components stored in a
         * ComponentsStorage< Component >.
         * This state is stored by value in component ranges so that copying
         * a range (e.g. in range-based for loops) does not allocate.
         */
        template < typename Component >
        class ComponentRangeIterator
        {
        public:
            /*!
             * Iterator type of ComponentsStorage< Component >, checked with a
             * static_assert where the ranges are constructed.
             */
            using Iterator = typename std::list< std::pair< const uuid,
                std::unique_ptr< Component > > >::const_iterator;

            ComponentRangeIterator( Iterator begin, Iterator end )
                : current_{ begin }, end_{ end }
            {
            }

            [[nodiscard]] bool is_valid() const
            {
                return current_ != end_;
            }

            [[nodiscard]] Component& component() const
            {
                return *current_->second;
            }

            void set_active_only()
            {
                active_only_ = true;
                skip_inactive_components();
            }

            void next()
            {
                ++current_;
                skip_inactive_components();
            }

        private:
            void skip_inactive_components()
            {
                if( !active_only_ )
                {
                    return;
                }
                while( current_ != end_ && !component().is_active() )
                {
                    ++current_;
                }
            }

        private:
            Iterator current_;
            Iterator end_;
            bool active_only_{ false };
        };
    } // namespace detail
} // namespace geode
