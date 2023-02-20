/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <absl/container/flat_hash_map.h>

#include <bitsery/ext/std_map.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/uuid.h>

namespace geode
{
    namespace detail
    {
        class UuidToIndex
        {
        public:
            index_t nb_mappings() const
            {
                return uuid2index_.size();
            }

            absl::optional< index_t > index( const uuid& id ) const
            {
                const auto index = uuid2index_.find( id );
                if( index != uuid2index_.end() )
                {
                    return index->second;
                }
                return absl::nullopt;
            }

            void erase( const uuid& id )
            {
                uuid2index_.erase( id );
            }

            void erase( index_t index )
            {
                for( auto& it : uuid2index_ )
                {
                    if( it.second == index )
                    {
                        uuid2index_.erase( it.first );
                        return;
                    }
                }
            }

            void set_new_mapping( const uuid& id, const index_t index )
            {
                uuid2index_[id] = index;
            }

            void update( absl::Span< const index_t > old2new )
            {
                for( auto& it : uuid2index_ )
                {
                    const auto new_index = old2new[it.second];
                    OPENGEODE_ASSERT( new_index != NO_ID,
                        "[UuidToIndex::update] no uuid should be removed" );
                    it.second = new_index;
                }
            }

        private:
            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this, DefaultGrowable< Archive, UuidToIndex >{},
                    []( Archive& a, UuidToIndex& uuids ) {
                        a.ext( uuids.uuid2index_,
                            bitsery::ext::StdMap{
                                uuids.uuid2index_.max_size() },
                            []( Archive& a2, uuid& id, index_t& index ) {
                                a2.object( id );
                                a2.value4b( index );
                            } );
                    } );
            }

        private:
            absl::flat_hash_map< uuid, index_t > uuid2index_;
        };
    } // namespace detail
} // namespace geode
