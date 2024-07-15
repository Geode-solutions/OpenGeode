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

#include <filesystem>
#include <fstream>
#include <memory>

#include <absl/container/flat_hash_map.h>
#include <absl/strings/match.h>

#include <bitsery/ext/std_map.h>

#include <geode/basic/bitsery_archive.h>

#include <geode/geometry/bitsery_archive.h>

#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/mesh_id.h>

#include <geode/model/mixin/core/bitsery_archive.h>
#include <geode/model/mixin/core/component_type.h>

namespace geode
{
    namespace detail
    {
        template < typename Component >
        class ComponentsStorage
        {
        public:
            using ComponentPtr = std::unique_ptr< Component >;
            using ComponentsStore = absl::flat_hash_map< uuid, ComponentPtr >;
            using Iterator = typename ComponentsStore::const_iterator;

            index_t nb_components() const
            {
                return components_.size();
            }

            bool has_component( const uuid& id ) const
            {
                return components_.contains( id );
            }

            const Component& component( const uuid& id ) const
            {
                return *components_.at( id );
            }

            Component& component( const uuid& id )
            {
                return *components_.at( id );
            }

            Iterator begin() const
            {
                return components_.begin();
            }

            Iterator end() const
            {
                return components_.end();
            }

            void add_component( ComponentPtr component )
            {
                components_.emplace( component->id(), std::move( component ) );
            }

            void save_components( std::string_view filename ) const
            {
                std::ofstream file{ to_string( filename ),
                    std::ofstream::binary };
                TContext context{};
                BitseryExtensions::register_serialize_pcontext(
                    std::get< 0 >( context ) );
                Serializer archive{ context, file };
                archive.object( *this );
                archive.adapter().flush();
                OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                    "[ComponentsStorage::save_components] Error while writing "
                    "file: ",
                    filename );
            }

            void delete_component( const uuid& id )
            {
                components_.erase( components_.find( id ) );
            }

            void load_components( std::string_view filename )
            {
                if( !std::filesystem::exists( to_string( filename ) ) )
                {
                    return;
                }
                std::ifstream file{ to_string( filename ),
                    std::ifstream::binary };
                TContext context{};
                BitseryExtensions::register_deserialize_pcontext(
                    std::get< 0 >( context ) );
                Deserializer archive{ context, file };
                archive.object( *this );
                const auto& adapter = archive.adapter();
                OPENGEODE_EXCEPTION(
                    adapter.error() == bitsery::ReaderError::NoError
                        && adapter.isCompletedSuccessfully()
                        && std::get< 1 >( context ).isValid(),
                    "[ComponentsStorage::load_components] Error while reading "
                    "file: ",
                    filename );
            }

            absl::flat_hash_map< std::string, std::string > file_mapping(
                std::string_view directory ) const
            {
                absl::flat_hash_map< std::string, std::string > mapping;
                for( const auto& file : std::filesystem::directory_iterator(
                         to_string( directory ) ) )
                {
                    auto path = file.path();
                    auto filename = path.replace_extension( "" ).string();
                    if( filename.size() > 36 )
                    {
                        auto uuid = filename.substr( filename.size() - 36 );
                        mapping.emplace(
                            std::move( uuid ), file.path().string() );
                    }
                }
                return mapping;
            }

        private:
            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    Growable< Archive, ComponentsStorage >{
                        { []( Archive& a, ComponentsStorage& storage ) {
                            a.ext( storage.components_,
                                bitsery::ext::StdMap{
                                    storage.components_.max_size() },
                                []( Archive& a2, uuid& id,
                                    ComponentPtr& item ) {
                                    a2.object( id );
                                    a2.ext( item, bitsery::ext::StdSmartPtr{} );
                                } );
                        } } } );
            }

        private:
            ComponentsStore components_;
        };
    } // namespace detail
} // namespace geode
