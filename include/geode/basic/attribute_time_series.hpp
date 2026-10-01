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

#include <memory>
#include <string_view>
#include <vector>

#include <geode/basic/attribute.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/common.hpp>

namespace geode
{
    /*!
     * Read-only access to the steps of a time series stored in an
     * AttributeManager (see AttributeManager::create_time_step_attribute).
     * This is a snapshot taken at construction: steps created later are not
     * seen, and steps deleted later stay readable.
     */
    template < typename T >
    class AttributeTimeSeries
    {
    public:
        AttributeTimeSeries(
            const AttributeManager& manager, std::string_view name )
        {
            const auto steps = manager.time_steps( name );
            times_.reserve( steps.size() );
            attributes_.reserve( steps.size() );
            for( const auto& step : steps )
            {
                OpenGeodeBasicException::check_exception(
                    times_.empty() || times_.back() < step.time, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[AttributeTimeSeries] Time series '", name,
                    "' has several steps at time ", step.time );
                times_.push_back( step.time );
                attributes_.push_back( manager.find_read_only_attribute< T >(
                    step.attribute_id ) );
            }
        }

        [[nodiscard]] index_t nb_time_steps() const
        {
            return static_cast< index_t >( times_.size() );
        }

        [[nodiscard]] double time( index_t step ) const
        {
            return times_.at( step );
        }

        [[nodiscard]] const ReadOnlyAttribute< T >& step_attribute(
            index_t step ) const
        {
            return *attributes_.at( step );
        }

        [[nodiscard]] const T& value( index_t step, index_t element ) const
        {
            return attributes_.at( step )->value( element );
        }

        /*!
         * Get the values of one element at every step, in time order.
         */
        [[nodiscard]] std::vector< T > element_values( index_t element ) const
        {
            std::vector< T > values;
            values.reserve( attributes_.size() );
            for( const auto& attribute : attributes_ )
            {
                values.push_back( attribute->value( element ) );
            }
            return values;
        }

    private:
        std::vector< double > times_;
        std::vector< std::shared_ptr< ReadOnlyAttribute< T > > > attributes_;
    };
} // namespace geode
