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

#include <geode/basic/attribute_manager.hpp>

#include <algorithm>

#include <absl/container/flat_hash_map.h>
#include <absl/container/linked_hash_map.h>

#include <bitsery/ext/std_map.h>
#include <bitsery/traits/string.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    class AttributeManager::Impl
    {
        using AttributesMap =
            absl::linked_hash_map< uuid, std::shared_ptr< AttributeBase > >;
        using AttributesMapElement = std::pair< const AttributesMap::key_type,
            AttributesMap::mapped_type >;

    public:
        std::shared_ptr< AttributeBase > find_attribute_base(
            const uuid &attribute_id ) const
        {
            const auto attribute_it = attributes_.find( attribute_id );
            if( attribute_it == attributes_.end() )
            {
                return nullptr;
            }
            return attribute_it->second;
        }

        void register_attribute( std::shared_ptr< AttributeBase > &attribute,
            const uuid &attribute_id,
            const AttributeBase::AttributeKey &key )
        {
            attribute->resize( nb_elements_, key );
            attributes_.emplace( attribute_id, attribute );
        }

        void resize( index_t size, const AttributeBase::AttributeKey &key )
        {
            if( size == nb_elements_ )
            {
                return;
            }
            nb_elements_ = size;
            for( auto &attribute_it : attributes_ )
            {
                attribute_it.second->resize( size, key );
            }
        }

        void reserve( index_t capacity, const AttributeBase::AttributeKey &key )
        {
            if( capacity <= nb_elements_ )
            {
                return;
            }
            for( auto &attribute_it : attributes_ )
            {
                attribute_it.second->reserve( capacity, key );
            }
        }

        void assign_attribute_value( index_t from_element,
            index_t to_element,
            const AttributeBase::AttributeKey &key )
        {
            for( auto &attribute_it : attributes_ )
            {
                if( attribute_it.second->properties().assignable )
                {
                    attribute_it.second->compute_value(
                        from_element, to_element, key );
                }
            }
        }

        void copy_attribute_value( index_t from_element,
            index_t to_element,
            const AttributeBase::AttributeKey &key )
        {
            for( auto &attribute_it : attributes_ )
            {
                attribute_it.second->compute_value(
                    from_element, to_element, key );
            }
        }

        void interpolate_attribute_value(
            const AttributeLinearInterpolation &interpolation,
            index_t to_element,
            const AttributeBase::AttributeKey &key )
        {
            for( auto &attribute_it : attributes_ )
            {
                if( attribute_it.second->properties().interpolable )
                {
                    attribute_it.second->compute_value(
                        interpolation, to_element, key );
                }
            }
        }

        bool has_assignable_attributes() const
        {
            return absl::c_any_of(
                attributes_, []( const AttributesMapElement &attribute ) {
                    return attribute.second->properties().assignable;
                } );
        }

        bool has_interpolable_attributes() const
        {
            return absl::c_any_of(
                attributes_, []( const AttributesMapElement &attribute ) {
                    return attribute.second->properties().interpolable;
                } );
        }

        absl::FixedArray< uuid > attribute_ids() const
        {
            absl::FixedArray< uuid > ids( attributes_.size() );
            index_t count{ 0 };
            for( const auto &[attribute_id, _] : attributes_ )
            {
                ids[count++] = attribute_id;
            }
            return ids;
        }

        bool attribute_exists( const uuid &attribute_id ) const
        {
            return attributes_.find( attribute_id ) != attributes_.end();
        }

        void delete_attribute( const uuid &attribute_id )
        {
            absl::MutexLock lock{ mutex_ };
            const auto attribute_it = attributes_.find( attribute_id );
            if( attribute_it != attributes_.end() )
            {
                attributes_.erase( attribute_it );
            }
        }

        std::string_view attribute_type( const uuid &attribute_id ) const
        {
            const auto attribute_it = attributes_.find( attribute_id );
            if( attribute_it == attributes_.end() )
            {
                static constexpr auto UNDEFINED = "undefined";
                return UNDEFINED;
            }
            return attribute_it->second->type();
        }

        void clear()
        {
            attributes_.clear();
            nb_elements_ = 0;
        }

        void clear_attributes( const AttributeBase::AttributeKey &key )
        {
            for( auto &attribute_it : attributes_ )
            {
                attribute_it.second->resize( 0, key );
            }
            nb_elements_ = 0;
        }

        void delete_elements( const std::vector< bool > &to_delete,
            const AttributeBase::AttributeKey &key )
        {
            for( auto &attribute_it : attributes_ )
            {
                attribute_it.second->delete_elements( to_delete, key );
            }
            nb_elements_ -=
                static_cast< index_t >( absl::c_count( to_delete, true ) );
        }

        void permute_elements( absl::Span< const index_t > permutation,
            const AttributeBase::AttributeKey &key )
        {
            for( auto &attribute_it : attributes_ )
            {
                attribute_it.second->permute_elements( permutation, key );
            }
        }

        index_t nb_elements() const
        {
            return nb_elements_;
        }

        std::optional< std::string_view > attribute_name( const uuid &id ) const
        {
            return attributes_.at( id )->name().value_or( "unknown" );
        }

        std::optional< std::vector< uuid > > attribute_ids_matching_name(
            std::string_view name ) const
        {
            std::vector< uuid > ids;
            for( const auto &[attribute_id, attribute] : attributes_ )
            {
                if( !attribute->name().has_value() )
                {
                    continue;
                }
                if( attribute->name().value() == name )
                {
                    ids.push_back( attribute_id );
                }
            }
            if( ids.empty() )
            {
                return std::nullopt;
            }
            return ids;
        }

        void copy( const AttributeManager::Impl &attribute_manager_from,
            const AttributeBase::AttributeKey &key )
        {
            nb_elements_ = attribute_manager_from.nb_elements_;
            for( const auto &[attribute_id_from, attribute_from] :
                attribute_manager_from.attributes_ )
            {
                if( !attribute_from->properties().transferable )
                {
                    continue;
                }
                const auto attribute_it = attributes_.find( attribute_id_from );
                if( attribute_it != attributes_.end() )
                {
                    try
                    {
                        attribute_it->second->copy(
                            *attribute_from, nb_elements_, key );
                    }
                    catch( const std::bad_cast &e )
                    {
                        Logger::error( "Attribute \"",
                            attribute_id_from.string(),
                            "\" cannot be copied: ", e.what() );
                    }
                }
                else
                {
                    attributes_.emplace(
                        attribute_id_from, attribute_from->clone( key ) );
                }
            }
        }

        template < typename T >
        void import( const AttributeManager::Impl &attribute_manager,
            const T &old2new_mapping,
            const AttributeBase::AttributeKey &key )
        {
            for( const auto &[attribute_id, attribute_from] :
                attribute_manager.attributes_ )
            {
                if( !attribute_from->properties().transferable )
                {
                    continue;
                }
                if( attribute_exists( attribute_id ) )
                {
                    if( attribute_from->type()
                        != this->attributes_.at( attribute_id )->type() )
                    {
                        continue;
                    }
                    this->attributes_.at( attribute_id )
                        ->import( old2new_mapping, attribute_from, key );
                }
                else
                {
                    attributes_.emplace(
                        attribute_id, attribute_from->extract( old2new_mapping,
                                          nb_elements_, key ) );
                }
            }
        }

        template < typename T >
        void import( const AttributeManager::Impl &attribute_manager,
            const T &old2new_mapping,
            geode::uuid attribute_id,
            const AttributeBase::AttributeKey &key )
        {
            auto it = attribute_manager.attributes_.find( attribute_id );
            OpenGeodeBasicException::check_exception(
                it != attribute_manager.attributes_.end(), nullptr,
                OpenGeodeException::TYPE::data,
                "[AttributeManager::import] Could not import attribute '",
                attribute_id.string(),
                "'. No attribute with this id exists in the source "
                "AttributeManager." );
            OpenGeodeBasicException::check_exception(
                it->second->properties().transferable, nullptr,
                OpenGeodeException::TYPE::data,
                "[AttributeManager::import] Could not import attribute '",
                attribute_id.string(),
                "'. The attribute is not transferable." );
            if( attribute_exists( attribute_id ) )
            {
                OpenGeodeBasicException::check_exception(
                    it->second->type()
                        == this->attributes_.at( attribute_id )->type(),
                    nullptr, OpenGeodeException::TYPE::data,
                    "[AttributeManager::import] Could not import attribute '",
                    attribute_id.string(),
                    "'. An attribute with the same id but a different type "
                    "already exists in the destination AttributeManager." );
                this->attributes_.at( attribute_id )
                    ->import( old2new_mapping, it->second, key );
            }
            else
            {
                attributes_.emplace( attribute_id,
                    it->second->extract( old2new_mapping, nb_elements_, key ) );
            }
        }

        void set_attribute_properties( geode::uuid attribute_id,
            const AttributeProperties &new_properties )
        {
            const auto attribute_it = attributes_.find( attribute_id );
            OpenGeodeBasicException::check_exception(
                attribute_it != attributes_.end(), nullptr,
                OpenGeodeException::TYPE::data,
                "[AttributeManager::set_attribute_properties] Attribute ",
                attribute_id.string(), "does not exist" );
            attribute_it->second->set_properties( new_properties );
        }

        template < typename Archive >
        void serialize( Archive &serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive &local_archive, Impl &impl ) {
                         local_archive.value4b( impl.nb_elements_ );
                         absl::linked_hash_map< std::string,
                             std::shared_ptr< AttributeBase > >
                             old_map;
                         local_archive.ext( old_map,
                             bitsery::ext::StdMap{ old_map.max_size() },
                             []( Archive &local_archive2, std::string &name,
                                 std::shared_ptr< AttributeBase > &attribute ) {
                                 local_archive2.text1b( name, name.max_size() );
                                 try
                                 {
                                     local_archive2.ext( attribute,
                                         bitsery::ext::StdSmartPtr{} );
                                 }
                                 catch( ... )
                                 {
                                     throw OpenGeodeBasicException{ nullptr,
                                         OpenGeodeException::TYPE::internal,
                                         "[AttributeManager::serialize] Cannot "
                                         "serialize attribute ",
                                         name, " holding type ",
                                         attribute->type() };
                                 }
                             } );
                         for( auto &[attribute_name, attribute] : old_map )
                         {
                             IdentifierBuilder builder{ *attribute };
                             builder.set_name( attribute_name );
                             impl.attributes_.emplace(
                                 attribute->id(), std::move( attribute ) );
                         }
                     },
                        []( Archive &local_archive, Impl &impl ) {
                            local_archive.value4b( impl.nb_elements_ );
                            local_archive.ext( impl.attributes_,
                                bitsery::ext::StdMap{
                                    impl.attributes_.max_size() },
                                []( Archive &local_archive2,
                                    geode::uuid &attribute_id,
                                    std::shared_ptr< AttributeBase >
                                        &attribute ) {
                                    local_archive2.object( attribute_id );
                                    try
                                    {
                                        local_archive2.ext( attribute,
                                            bitsery::ext::StdSmartPtr{} );
                                    }
                                    catch( ... )
                                    {
                                        throw OpenGeodeBasicException{ nullptr,
                                            OpenGeodeException::TYPE::internal,
                                            "[AttributeManager::serialize] "
                                            "Cannot "
                                            "serialize attribute with id ",
                                            attribute_id.string(),
                                            " holding type ",
                                            attribute->type() };
                                    }
                                } );
                        } } } );
        }

        absl::Mutex &mutex() const
        {
            return mutex_;
        }

    private:
        index_t nb_elements_{ 0 };
        AttributesMap attributes_;
        mutable absl::Mutex mutex_;
    };

    AttributeManager::AttributeManager() = default;
    AttributeManager::AttributeManager(
        AttributeManager &&other ) noexcept = default;
    AttributeManager &AttributeManager::operator=(
        AttributeManager &&other ) noexcept = default;
    AttributeManager::~AttributeManager() = default;

    std::shared_ptr< AttributeBase > AttributeManager::find_attribute_base(
        const geode::uuid &attribute_id ) const
    {
        return impl_->find_attribute_base( attribute_id );
    }

    void AttributeManager::register_attribute(
        std::shared_ptr< AttributeBase > attribute,
        const geode::uuid &attribute_id )
    {
        return impl_->register_attribute( attribute, attribute_id, {} );
    }

    void AttributeManager::resize( index_t size )
    {
        impl_->resize( size, {} );
    }

    void AttributeManager::reserve( index_t capacity )
    {
        impl_->reserve( capacity, {} );
    }

    bool AttributeManager::has_assignable_attributes() const
    {
        return impl_->has_assignable_attributes();
    }

    bool AttributeManager::has_interpolable_attributes() const
    {
        return impl_->has_interpolable_attributes();
    }

    void AttributeManager::assign_attribute_value(
        index_t from_element, index_t to_element )
    {
        impl_->assign_attribute_value( from_element, to_element, {} );
    }

    void AttributeManager::copy_attribute_value(
        index_t from_element, index_t to_element )
    {
        impl_->copy_attribute_value( from_element, to_element, {} );
    }

    void AttributeManager::interpolate_attribute_value(
        const AttributeLinearInterpolation &interpolation, index_t to_element )
    {
        impl_->interpolate_attribute_value( interpolation, to_element, {} );
    }

    absl::FixedArray< geode::uuid > AttributeManager::attribute_ids() const
    {
        return impl_->attribute_ids();
    }

    bool AttributeManager::attribute_exists(
        const geode::uuid &attribute_id ) const
    {
        return impl_->attribute_exists( attribute_id );
    }

    void AttributeManager::delete_attribute( const geode::uuid &attribute_id )
    {
        impl_->delete_attribute( attribute_id );
    }

    std::string_view AttributeManager::attribute_type(
        const geode::uuid &attribute_id ) const
    {
        return impl_->attribute_type( attribute_id );
    }

    void AttributeManager::clear()
    {
        impl_->clear();
    }

    void AttributeManager::clear_attributes()
    {
        impl_->clear_attributes( {} );
    }

    void AttributeManager::delete_elements(
        const std::vector< bool > &to_delete )
    {
        if( absl::c_find( to_delete, true ) != to_delete.end() )
        {
            OpenGeodeBasicException::check_assertion(
                to_delete.size() == nb_elements(),
                "[AttributeManager::delete_elements] Vector to_delete should "
                "have the same size as the number of elements" );
            impl_->delete_elements( to_delete, {} );
        }
    }

    void AttributeManager::permute_elements(
        absl::Span< const index_t > permutation )
    {
        impl_->permute_elements( permutation, {} );
    }

    index_t AttributeManager::nb_elements() const
    {
        return impl_->nb_elements();
    }

    void AttributeManager::copy( const AttributeManager &attribute_manager )
    {
        impl_->copy( *attribute_manager.impl_, {} );
    }

    std::optional< std::vector< geode::uuid > >
        AttributeManager::attribute_ids_matching_name(
            std::string_view name ) const
    {
        return impl_->attribute_ids_matching_name( name );
    }

    std::optional< std::string_view > AttributeManager::attribute_name(
        const uuid &id ) const
    {
        return impl_->attribute_name( id );
    }

    void AttributeManager::import( const AttributeManager &attribute_manager,
        absl::Span< const index_t > old2new )
    {
        impl_->import( *attribute_manager.impl_, old2new, {} );
    }

    void AttributeManager::import( const AttributeManager &attribute_manager,
        absl::Span< const index_t > old2new,
        const geode::uuid &attribute_id )
    {
        impl_->import( *attribute_manager.impl_, old2new, attribute_id, {} );
    }

    void AttributeManager::import( const AttributeManager &attribute_manager,
        const GenericMapping< index_t > &old2new_mapping )
    {
        impl_->import( *attribute_manager.impl_, old2new_mapping, {} );
    }

    void AttributeManager::import( const AttributeManager &attribute_manager,
        const GenericMapping< index_t > &old2new_mapping,
        const geode::uuid &attribute_id )
    {
        impl_->import(
            *attribute_manager.impl_, old2new_mapping, attribute_id, {} );
    }

    void AttributeManager::set_attribute_properties(
        geode::uuid attribute_id, const AttributeProperties &new_properties )
    {
        impl_->set_attribute_properties( attribute_id, new_properties );
    }

    absl::Mutex &AttributeManager::mutex() const
    {
        return impl_->mutex();
    }

    template < typename Archive >
    void AttributeManager::serialize( Archive &serializer )
    {
        serializer.ext(
            *this, Growable< Archive, AttributeManager >{
                       { []( Archive &local_archive,
                             AttributeManager &attribute_manager ) {
                            local_archive.object( attribute_manager.impl_ );
                        },
                           []( Archive &local_archive,
                               AttributeManager &attribute_manager ) {
                               local_archive.object( attribute_manager.impl_ );
                           } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_basic_api, AttributeManager );
} // namespace geode
