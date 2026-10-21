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

#include <filesystem>
#include <fstream>
#include <memory>

#include <absl/container/flat_hash_map.h>
#include <absl/container/linked_hash_map.h>
#include <absl/strings/match.h>

#include <bitsery/ext/std_map.h>
#include <bitsery/ext/std_smart_ptr.h>

#include <geode/basic/growable.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/bitsery_archive.hpp>

#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/mixin/core/bitsery_archive.hpp>
#include <geode/model/mixin/core/component_type.hpp>

namespace geode::detail
{
    /*!
     * Return the given mesh implementation if it is registered, otherwise
     * the default implementation of the given mesh type (with a warning).
     */
    [[nodiscard]] inline MeshImpl loadable_mesh_impl( const MeshImpl& impl,
        const MeshType& fallback_type,
        std::string_view file )
    {
        if( MeshFactory::has_creator( impl ) )
        {
            return impl;
        }
        const auto& fallback = MeshFactory::default_impl( fallback_type );
        Logger::warning( "[ComponentsStorage] Unknown mesh implementation ",
            impl.get(), " for file ", file, ", loading it as ",
            fallback.get() );
        return fallback;
    }

    /*!
     * Create an empty mesh for a component whose mesh file is missing, using
     * the component mesh implementation if it is registered, otherwise the
     * default implementation of the given mesh type.
     */
    template < typename Component >
    [[nodiscard]] std::unique_ptr< typename Component::Mesh >
        empty_mesh_of_missing_file(
            const Component& component, const MeshType& fallback_type )
    {
        Logger::warning( "[ComponentsStorage] Missing mesh file of ",
            component.component_id().string(), ", an empty mesh is used" );
        if( MeshFactory::has_creator( component.mesh_type() ) )
        {
            return Component::Mesh::create( component.mesh_type() );
        }
        return Component::Mesh::create(
            MeshFactory::default_impl( fallback_type ) );
    }

    template < typename Component >
    class ComponentsStorage
    {
    public:
        using ComponentPtr = std::unique_ptr< Component >;
        using ComponentsStore = absl::linked_hash_map< uuid, ComponentPtr >;
        using Iterator = typename ComponentsStore::const_iterator;

        [[nodiscard]] index_t nb_components() const
        {
            return components_.size();
        }

        [[nodiscard]] bool has_component( const uuid& component_id ) const
        {
            return components_.contains( component_id );
        }

        [[nodiscard]] const Component& component(
            const uuid& component_id ) const
        {
            return find_component( component_id );
        }

        [[nodiscard]] Component& component( const uuid& component_id )
        {
            return find_component( component_id );
        }

        [[nodiscard]] Iterator begin() const
        {
            return components_.begin();
        }

        [[nodiscard]] Iterator end() const
        {
            return components_.end();
        }

        void add_component( ComponentPtr component )
        {
            const auto component_id = component->id();
            const auto [_, new_uuid] =
                components_.emplace( component_id, std::move( component ) );
            OpenGeodeModelException::check_exception( new_uuid, component_id,
                OpenGeodeException::TYPE::data,
                "[ComponentsStorage::add_component] Component with id ",
                component_id.string(), " already exists" );
        }

        void save_components( std::string_view filename ) const
        {
            std::ofstream file{ std::filesystem::u8path(
                                    to_string( filename ) ),
                std::ofstream::binary };
            OpenGeodeModelException::check_exception( file.is_open(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[ComponentsStorage::save_components] Cannot open file: ",
                filename );
            TContext context{};
            BitseryExtensions::register_serialize_pcontext(
                std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OpenGeodeModelException::check_exception(
                file.good() && std::get< 1 >( context ).isValid(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[ComponentsStorage::save_components] Error while writing "
                "file: ",
                filename );
        }

        void delete_component( const uuid& component_id )
        {
            components_.erase( components_.find( component_id ) );
        }

        /*!
         * Load the components stored in the given file.
         * @param[in] required If false, a missing file is expected (e.g. for
         * files written before this component type existed) and silently
         * ignored. Otherwise a warning is emitted.
         */
        void load_components( std::string_view filename, bool required = true )
        {
            const auto path = std::filesystem::u8path( to_string( filename ) );
            if( !std::filesystem::exists( path ) )
            {
                if( required )
                {
                    Logger::warning( "[ComponentsStorage::load_components] "
                                     "Missing file: ",
                        filename, ", no component loaded from it" );
                }
                return;
            }
            std::ifstream file{ path, std::ifstream::binary };
            TContext context{};
            BitseryExtensions::register_deserialize_pcontext(
                std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *this );
            const auto& adapter = archive.adapter();
            OpenGeodeModelException::check_exception(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                nullptr, OpenGeodeException::TYPE::internal,
                "[ComponentsStorage::load_components] Error while reading "
                "file: ",
                filename );
            repair_loaded_components( filename );
        }

        [[nodiscard]] absl::flat_hash_map< std::string, std::string >
            file_mapping( std::string_view directory ) const
        {
            absl::flat_hash_map< std::string, std::string > mapping;
            for( const auto& file : std::filesystem::directory_iterator(
                     std::filesystem::u8path( to_string( directory ) ) ) )
            {
                auto path = file.path();
                auto filename = path.replace_extension( "" ).u8string();
                if( filename.size() > 36 )
                {
                    auto uuid = filename.substr( filename.size() - 36 );
                    mapping.emplace(
                        std::move( uuid ), file.path().u8string() );
                }
            }
            return mapping;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, ComponentsStorage >{
                    { []( Archive& archive, ComponentsStorage& storage ) {
                        archive.ext( storage.components_,
                            bitsery::ext::StdMap{
                                storage.components_.max_size() },
                            []( Archive& archive2, uuid& component_id,
                                ComponentPtr& item ) {
                                archive2.object( component_id );
                                archive2.ext(
                                    item, bitsery::ext::StdSmartPtr{} );
                            } );
                    } } } );
        }

        void repair_loaded_components( std::string_view filename )
        {
            bool has_wrong_component{ false };
            for( const auto& [component_id, component] : components_ )
            {
                if( !component || component->id() != component_id )
                {
                    has_wrong_component = true;
                    break;
                }
            }
            if( !has_wrong_component )
            {
                return;
            }
            ComponentsStore repaired;
            repaired.reserve( components_.size() );
            for( auto& [component_id, component] : components_ )
            {
                if( !component )
                {
                    Logger::warning( "[ComponentsStorage::load_components] "
                                     "Removing null component ",
                        component_id.string(), " from file: ", filename );
                    continue;
                }
                auto id = component->id();
                if( id != component_id )
                {
                    Logger::warning( "[ComponentsStorage::load_components] "
                                     "Component stored with id ",
                        component_id.string(), " has id ", id.string(),
                        " in file: ", filename );
                }
                const auto [_, inserted] =
                    repaired.emplace( id, std::move( component ) );
                if( !inserted )
                {
                    Logger::warning( "[ComponentsStorage::load_components] "
                                     "Removing duplicated component ",
                        id.string(), " from file: ", filename );
                }
            }
            components_ = std::move( repaired );
        }

        [[nodiscard]] Component& find_component(
            const uuid& component_id ) const
        {
            const auto it = components_.find( component_id );
            OpenGeodeModelException::check_exception( it != components_.end(),
                component_id, OpenGeodeException::TYPE::data,
                "[ComponentsStorage::component] Component with id ",
                component_id.string(), " does not exist" );
            auto& component = *it->second;
            OpenGeodeModelException::check_exception(
                component.id() == component_id, component_id,
                OpenGeodeException::TYPE::data,
                "[ComponentsStorage::component] Component with id ",
                component.id().string(), " does not match its stored id ",
                component_id.string() );
            return component;
        }

    private:
        ComponentsStore components_;
    };
} // namespace geode::detail
