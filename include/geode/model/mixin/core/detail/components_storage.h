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

#include <fstream>
#include <memory>
#include <unordered_map>

#include <bitsery/ext/std_map.h>

#include <ghc/filesystem.hpp>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/bitsery_archive.h>

#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/mesh_type.h>

#include <geode/model/mixin/core/bitsery_archive.h>

namespace geode
{
    namespace detail
    {
        template < typename Component >
        class ComponentsStorage
        {
        public:
            using ComponentPtr = std::unique_ptr< Component >;
            using ComponentsStore = std::unordered_map< uuid, ComponentPtr >;
            using Iterator = typename ComponentsStore::const_iterator;

            index_t nb_components() const
            {
                return components_.size();
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

            virtual void save_components( const std::string& filename ) const
            {
                std::ofstream file{ filename, std::ofstream::binary };
                TContext context{};
                register_librairies_in_serialize_pcontext( context );
                Serializer archive{ context, file };
                archive.object( *this );
                archive.adapter().flush();
                OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                    "[ComponentsStorage::save_components] Error while writing "
                    "file: "
                        + filename );
            }

            void delete_component( const uuid& id )
            {
                components_.erase( components_.find( id ) );
            }

            void load_components( const std::string& filename )
            {
                if( !ghc::filesystem::exists( filename ) )
                {
                    return;
                }
                std::ifstream file{ filename, std::ifstream::binary };
                TContext context{};
                register_librairies_in_deserialize_pcontext( context );
                Deserializer archive{ context, file };
                archive.object( *this );
                auto& adapter = archive.adapter();
                OPENGEODE_EXCEPTION(
                    adapter.error() == bitsery::ReaderError::NoError
                        && adapter.isCompletedSuccessfully()
                        && std::get< 1 >( context ).isValid(),
                    "[ComponentsStorage::load_components] Error while reading "
                    "file: "
                        + filename );
            }

        protected:
            virtual void register_librairies_in_serialize_pcontext(
                TContext& context ) const
            {
                register_basic_serialize_pcontext( std::get< 0 >( context ) );
                register_geometry_serialize_pcontext(
                    std::get< 0 >( context ) );
                register_mesh_serialize_pcontext( std::get< 0 >( context ) );
                register_model_serialize_pcontext( std::get< 0 >( context ) );
            }

            virtual void register_librairies_in_deserialize_pcontext(
                TContext& context ) const
            {
                register_basic_deserialize_pcontext( std::get< 0 >( context ) );
                register_geometry_deserialize_pcontext(
                    std::get< 0 >( context ) );
                register_mesh_deserialize_pcontext( std::get< 0 >( context ) );
                register_model_deserialize_pcontext( std::get< 0 >( context ) );
            }

        private:
            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    DefaultGrowable< Archive, ComponentsStorage >{},
                    []( Archive& archive, ComponentsStorage& storage ) {
                        archive.ext( storage.components_,
                            bitsery::ext::StdMap{
                                storage.components_.max_size() },
                            []( Archive& archive, uuid& id,
                                ComponentPtr& item ) {
                                archive.object( id );
                                archive.ext(
                                    item, bitsery::ext::StdSmartPtr{} );
                            } );
                    } );
            }

        private:
            ComponentsStore components_;
        };
    } // namespace detail
} // namespace geode
