/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/attribute.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class AttributeManager::Impl
    {
    public:
        std::shared_ptr< AttributeBase > find_attribute_base(
            absl::string_view name )
        {
            const auto it = attributes_.find( name );
            if( it == attributes_.end() )
            {
                return nullptr;
            }
            return it->second;
        }

        void register_attribute( std::shared_ptr< AttributeBase > &attribute,
            absl::string_view name )
        {
            attribute->resize( nb_elements_ );
            attributes_.emplace( name, attribute );
        }

        void resize( index_t size )
        {
            if( size == nb_elements_ )
            {
                return;
            }
            nb_elements_ = size;
            for( auto &it : attributes_ )
            {
                it.second->resize( size );
            }
        }

        void reserve( index_t capacity )
        {
            if( capacity <= nb_elements_ )
            {
                return;
            }
            for( auto &it : attributes_ )
            {
                it.second->reserve( capacity );
            }
        }

        void assign_attribute_value( index_t from_element, index_t to_element )
        {
            for( auto &it : attributes_ )
            {
                it.second->compute_value( from_element, to_element );
            }
        }

        void interpolate_attribute_value(
            const AttributeLinearInterpolation &interpolation,
            index_t to_element )
        {
            for( auto &it : attributes_ )
            {
                it.second->compute_value( interpolation, to_element );
            }
        }

        absl::FixedArray< absl::string_view > attribute_names() const
        {
            absl::FixedArray< absl::string_view > names( attributes_.size() );
            index_t count{ 0 };
            for( const auto &it : attributes_ )
            {
                names[count++] = it.first;
            }
            return names;
        }

        bool attribute_exists( absl::string_view name ) const
        {
            return attributes_.find( name ) != attributes_.end();
        }

        void delete_attribute( absl::string_view name )
        {
            const auto it = attributes_.find( name );
            if( it != attributes_.end() )
            {
                attributes_.erase( it );
            }
        }

        absl::string_view attribute_type( absl::string_view name ) const
        {
            const auto it = attributes_.find( name );
            if( it == attributes_.end() )
            {
                static constexpr auto undefined = "undefined";
                return undefined;
            }
            return it->second->type();
        }

        void clear()
        {
            attributes_.clear();
            nb_elements_ = 0;
        }

        void clear_attributes()
        {
            for( auto &it : attributes_ )
            {
                it.second->resize( 0 );
            }
            nb_elements_ = 0;
        }

        void delete_elements( const std::vector< bool > &to_delete )
        {
            for( auto &it : attributes_ )
            {
                it.second->delete_elements( to_delete );
            }
            nb_elements_ -=
                static_cast< index_t >( absl::c_count( to_delete, true ) );
        }

        index_t nb_elements() const
        {
            return nb_elements_;
        }

        void copy( const AttributeManager::Impl &attribute_manager )
        {
            nb_elements_ = attribute_manager.nb_elements_;
            for( const auto &attribute : attribute_manager.attributes_ )
            {
                const auto it = attributes_.find( attribute.first );
                if( it != attributes_.end() )
                {
                    try
                    {
                        it->second->copy( *attribute.second, nb_elements_ );
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
                        attribute.first, attribute.second->clone() );
                }
            }
        }

        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive &archive, Impl &impl ) {
                    archive.value4b( impl.nb_elements_ );
                    archive.ext( impl.attributes_,
                        bitsery::ext::StdMap{ impl.attributes_.max_size() },
                        []( Archive &archive, std::string &name,
                            std::shared_ptr< AttributeBase > &attribute ) {
                            archive.text1b( name, name.max_size() );
                            archive.ext(
                                attribute, bitsery::ext::StdSmartPtr{} );
                        } );
                } );
        }

    private:
        index_t nb_elements_{ 0 };
        absl::flat_hash_map< std::string, std::shared_ptr< AttributeBase > >
            attributes_;
    };

    AttributeManager::AttributeManager() {} // NOLINT
    AttributeManager::~AttributeManager() {} // NOLINT

    std::shared_ptr< AttributeBase > AttributeManager::find_attribute_base(
        absl::string_view name )
    {
        return impl_->find_attribute_base( name );
    }

    void AttributeManager::register_attribute(
        std::shared_ptr< AttributeBase > attribute, absl::string_view name )
    {
        return impl_->register_attribute( attribute, name );
    }

    void AttributeManager::resize( index_t size )
    {
        impl_->resize( size );
    }

    void AttributeManager::reserve( index_t capacity )
    {
        impl_->reserve( capacity );
    }

    void AttributeManager::assign_attribute_value(
        index_t from_element, index_t to_element )
    {
        impl_->assign_attribute_value( from_element, to_element );
    }

    void AttributeManager::interpolate_attribute_value(
        const AttributeLinearInterpolation &interpolation, index_t to_element )
    {
        impl_->interpolate_attribute_value( interpolation, to_element );
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
        impl_->clear_attributes();
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
            impl_->delete_elements( to_delete );
        }
    }

    index_t AttributeManager::nb_elements() const
    {
        return impl_->nb_elements();
    }

    void AttributeManager::copy( const AttributeManager &attribute_manager )
    {
        impl_->copy( *attribute_manager.impl_ );
    }

    template < typename Archive >
    void AttributeManager::serialize( Archive &archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, AttributeManager >{},
            []( Archive &archive, AttributeManager &attribute_manager ) {
                archive.object( attribute_manager.impl_ );
            } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_basic_api, AttributeManager );
} // namespace geode
