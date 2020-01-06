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
            const std::string &name )
        {
            const auto it = attributes_.find( name );
            if( it == attributes_.end() )
            {
                return nullptr;
            }
            return it->second;
        }

        void register_attribute( std::shared_ptr< AttributeBase > &attribute,
            const std::string &name )
        {
            attribute->resize( nb_elements_ );
            attributes_[name] = attribute;
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

        std::vector< std::string > attribute_names() const
        {
            std::vector< std::string > names;
            names.reserve( attributes_.size() );
            for( const auto &it : attributes_ )
            {
                names.push_back( it.first );
            }
            return names;
        }

        bool attribute_exists( const std::string &name ) const
        {
            return attributes_.find( name ) != attributes_.end();
        }

        void delete_attribute( const std::string &name )
        {
            const auto it = attributes_.find( name );
            if( it != attributes_.end() )
            {
                attributes_.erase( it );
            }
        }

        std::string attribute_type( const std::string &name ) const
        {
            const auto it = attributes_.find( name );
            if( it == attributes_.end() )
            {
                return "undefined";
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
            nb_elements_ -= absl::implicit_cast< index_t >(
                std::count( to_delete.begin(), to_delete.end(), true ) );
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
        const std::string &name )
    {
        return impl_->find_attribute_base( name );
    }

    void AttributeManager::register_attribute(
        std::shared_ptr< AttributeBase > attribute, const std::string &name )
    {
        return impl_->register_attribute( attribute, name );
    }

    void AttributeManager::resize( index_t size )
    {
        impl_->resize( size );
    }

    std::vector< std::string > AttributeManager::attribute_names() const
    {
        return impl_->attribute_names();
    }

    bool AttributeManager::attribute_exists( const std::string &name ) const
    {
        return impl_->attribute_exists( name );
    }

    void AttributeManager::delete_attribute( const std::string &name )
    {
        impl_->delete_attribute( name );
    }

    std::string AttributeManager::attribute_type(
        const std::string &name ) const
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
        if( std::find( to_delete.begin(), to_delete.end(), true )
            != to_delete.end() )
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
