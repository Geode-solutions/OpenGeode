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
#include <vector>

#include <bitsery/ext/std_smart_ptr.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    namespace detail
    {
        /*!
         * Reads data model storage that older versions saved as hidden
         * attributes, and removes these attributes once loaded.
         */
        class LegacyAttributes
        {
        public:
            /*!
             * Read a legacy attribute and copy its values into storage.
             */
            template < typename Archive, typename T >
            void deserialize( Archive& archive, std::vector< T >& storage )
            {
                const auto attribute = read< T >( archive );
                storage.resize( attribute->size() );
                for( const auto e : Indices{ storage } )
                {
                    storage[e] = attribute->value( e );
                }
            }

            /*!
             * Read a legacy attribute whose values are not needed anymore.
             */
            template < typename T, typename Archive >
            void skip( Archive& archive )
            {
                read< T >( archive );
            }

            /*!
             * Delete every attribute read so far from the given manager.
             */
            void remove_from( AttributeManager& manager )
            {
                for( const auto& attribute_id : attribute_ids_ )
                {
                    manager.delete_attribute( attribute_id );
                }
                attribute_ids_.clear();
            }

        private:
            template < typename T, typename Archive >
            std::shared_ptr< VariableAttribute< T > > read( Archive& archive )
            {
                std::shared_ptr< VariableAttribute< T > > attribute;
                archive.ext( attribute, bitsery::ext::StdSmartPtr{} );
                OpenGeodeMeshException::check_exception( attribute != nullptr,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[LegacyAttributes] Cannot deserialize legacy attribute: "
                    "file may be corrupted" );
                attribute_ids_.push_back( attribute->id() );
                return attribute;
            }

        private:
            std::vector< uuid > attribute_ids_;
        };
    } // namespace detail
} // namespace geode
