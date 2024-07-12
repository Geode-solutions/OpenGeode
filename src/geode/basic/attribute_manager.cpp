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

#include <geode/basic/attribute_manager.h>

#include <algorithm>

#include <absl/container/flat_hash_map.h>

#include <bitsery/traits/string.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class AttributeManager::Impl
    {
        using AttributesMap = absl::flat_hash_map< std::string,
            std::shared_ptr< AttributeBase > >;
        using AttributesMapElement = std::pair< const AttributesMap::key_type,
            AttributesMap::mapped_type >;

    public:
        std::shared_ptr< AttributeBase > find_attribute_base(
            absl::string_view name ) const
        {
            const auto attribute_it = attributes_.find( name );
            if( attribute_it == attributes_.end() )
            {
                return nullptr;
            }
            return attribute_it->second;
        }

        void register_attribute( std::shared_ptr< AttributeBase > &attribute,
            absl::string_view name,
            const AttributeBase::AttributeKey &key )
        {
            attribute->resize( nb_elements_, key );
            attribute->set_name( name, key );
            attributes_.emplace( name, attribute );
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

        absl::FixedArray< absl::string_view > attribute_names() const
        {
            absl::FixedArray< absl::string_view > names( attributes_.size() );
            index_t count{ 0 };
            for( const auto &attribute_it : attributes_ )
            {
                names[count++] = attribute_it.first;
            }
            return names;
        }

        bool attribute_exists( absl::string_view name ) const
        {
            return attributes_.find( name ) != attributes_.end();
        }

        void delete_attribute( absl::string_view name )
        {
            const auto attribute_it = attributes_.find( name );
            if( attribute_it != attributes_.end() )
            {
                attributes_.erase( attribute_it );
            }
        }

        absl::string_view attribute_type( absl::string_view name ) const
        {
            const auto attribute_it = attributes_.find( name );
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

        void copy( const AttributeManager::Impl &attribute_manager,
            const AttributeBase::AttributeKey &key )
        {
            nb_elements_ = attribute_manager.nb_elements_;
            for( const auto &attribute : attribute_manager.attributes_ )
            {
                const auto attribute_it = attributes_.find( attribute.first );
                if( attribute_it != attributes_.end() )
                {
                    try
                    {
                        attribute_it->second->copy(
                            *attribute.second, nb_elements_, key );
                    }
                    catch( const std::bad_cast &e )
                    {
                        Logger::error( "Attribute \"", attribute.first,
                            "\" cannot be copied: ", e.what() );
                    }
                }
                else
                {
                    attributes_.emplace(
                        attribute.first, attribute.second->clone( key ) );
                }
            }
        }

        void import( const AttributeManager::Impl &attribute_manager,
            absl::Span< const index_t > old2new,
            const AttributeBase::AttributeKey &key )
        {
            for( const auto &attribute : attribute_manager.attributes_ )
            {
                if( !attribute_exists( attribute.first ) )
                {
                    attributes_.emplace(
                        attribute.first, attribute.second->extract(
                                             old2new, nb_elements_, key ) );
                }
            }
        }

        void import( const AttributeManager::Impl &attribute_manager,
            const GenericMapping< index_t > &old2new_mapping,
            const AttributeBase::AttributeKey &key )
        {
            for( const auto &attribute : attribute_manager.attributes_ )
            {
                if( !attribute_exists( attribute.first ) )
                {
                    attributes_.emplace( attribute.first,
                        attribute.second->extract(
                            old2new_mapping, nb_elements_, key ) );
                }
            }
        }

        void initialize_attribute_names(
            const AttributeBase::AttributeKey &key )
        {
            for( auto &attribute : attributes_ )
            {
                attribute.second->set_name( attribute.first, key );
            }
        }

        void rename_attribute( absl::string_view old_name,
            absl::string_view new_name,
            const AttributeBase::AttributeKey &key )
        {
            const auto attribute_it = attributes_.find( old_name );
            OPENGEODE_EXCEPTION( attribute_it != attributes_.end(),
                "[AttributeManager::rename_attribute] Attribute ", old_name,
                "does not exist" );
            auto attribute = attribute_it->second;
            attributes_.erase( attribute_it );
            attribute->set_name( new_name, key );
            attributes_.emplace( new_name, attribute );
        }

        void set_attribute_properties( absl::string_view attribute_name,
            const AttributeProperties &new_properties )
        {
            const auto attribute_it = attributes_.find( attribute_name );
            OPENGEODE_EXCEPTION( attribute_it != attributes_.end(),
                "[AttributeManager::rename_attribute] Attribute ",
                attribute_name, "does not exist" );
            attribute_it->second->set_properties( new_properties );
        }

        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext(
                *this, Growable< Archive, Impl >{ { []( Archive &local_archive,
                                                        Impl &impl ) {
                    local_archive.value4b( impl.nb_elements_ );
                    local_archive.ext( impl.attributes_,
                        bitsery::ext::StdMap{ impl.attributes_.max_size() },
                        []( Archive &local_archive2, std::string &name,
                            std::shared_ptr< AttributeBase > &attribute ) {
                            local_archive2.text1b( name, name.max_size() );
                            try
                            {
                                local_archive2.ext(
                                    attribute, bitsery::ext::StdSmartPtr{} );
                            }
                            catch( ... )
                            {
                                throw OpenGeodeException{
                                    "[AttributeManager::serialize] Cannot "
                                    "serialize attribute ",
                                    name, " holding type ", attribute->type()
                                };
                            }
                        } );
                } } } );
        }

    private:
        index_t nb_elements_{ 0 };
        AttributesMap attributes_;
    };

    AttributeManager::AttributeManager() = default;
    AttributeManager::AttributeManager( AttributeManager &&other ) noexcept
        : impl_( std::move( other.impl_ ) )
    {
    }
    AttributeManager::~AttributeManager() = default;

    std::shared_ptr< AttributeBase > AttributeManager::find_attribute_base(
        absl::string_view name ) const
    {
        return impl_->find_attribute_base( name );
    }

    void AttributeManager::register_attribute(
        std::shared_ptr< AttributeBase > attribute, absl::string_view name )
    {
        return impl_->register_attribute( attribute, name, {} );
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

    absl::FixedArray< absl::string_view >
        AttributeManager::attribute_names() const
    {
        return impl_->attribute_names();
    }

    bool AttributeManager::attribute_exists( absl::string_view name ) const
    {
        return impl_->attribute_exists( name );
    }

    void AttributeManager::delete_attribute( absl::string_view name )
    {
        impl_->delete_attribute( name );
    }

    absl::string_view AttributeManager::attribute_type(
        absl::string_view name ) const
    {
        return impl_->attribute_type( name );
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
            OPENGEODE_ASSERT( to_delete.size() == nb_elements(),
                "[AttributeManager::delete_elements] Vector to_delete should "
                "have the same size as the number of "
                "elements" );
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

    void AttributeManager::import( const AttributeManager &attribute_manager,
        absl::Span< const index_t > old2new )
    {
        impl_->import( *attribute_manager.impl_, old2new, {} );
    }

    void AttributeManager::import( const AttributeManager &attribute_manager,
        const GenericMapping< index_t > &old2new_mapping )
    {
        impl_->import( *attribute_manager.impl_, old2new_mapping, {} );
    }

    void AttributeManager::rename_attribute(
        absl::string_view old_name, absl::string_view new_name )
    {
        impl_->rename_attribute( old_name, new_name, {} );
    }

    void AttributeManager::set_attribute_properties(
        absl::string_view attribute_name,
        const AttributeProperties &new_properties )
    {
        impl_->set_attribute_properties( attribute_name, new_properties );
    }

    template < typename Archive >
    void AttributeManager::serialize( Archive &archive )
    {
        archive.ext( *this,
            Growable< Archive, AttributeManager >{
                { []( Archive &local_archive,
                      AttributeManager &attribute_manager ) {
                     local_archive.object( attribute_manager.impl_ );
                     const AttributeBase::AttributeKey key;
                     attribute_manager.impl_->initialize_attribute_names( key );
                 },
                    []( Archive &local_archive,
                        AttributeManager &attribute_manager ) {
                        local_archive.object( attribute_manager.impl_ );
                    } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_basic_api, AttributeManager );
} // namespace geode
